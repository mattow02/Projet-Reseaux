#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stp.h"
#include "addresses.h"

// Fonction pour comparer deux adresses MAC
static int compare_mac_addresses(const mac_address_t* mac1, const mac_address_t* mac2) {
    return memcmp(mac1->octets, mac2->octets, sizeof(mac_address_t));
}

// Fonction pour trouver le coût d'un lien entre deux équipements
static uint32_t find_link_cost(const network_config_t* config, size_t equip1, size_t equip2) {
    for (size_t i = 0; i < config->nb_links; i++) {
        if ((config->links[i].equip1 == equip1 && config->links[i].equip2 == equip2) ||
            (config->links[i].equip1 == equip2 && config->links[i].equip2 == equip1)) {
            return config->links[i].cost;
        }
    }
    return UINT32_MAX; // Lien inexistant
}

// Fonction pour calculer les chemins
void stp_calculate_paths(stp_network_t* network, const network_config_t* config) {
    if (!network || !config) return;

    // Trouver le pont racine
    stp_switch_t* root_bridge = NULL;
    for (size_t i = 0; i < network->nb_switches; i++) {
        if (network->switches[i].is_root_bridge) {
            root_bridge = &network->switches[i];
            break;
        }
    }
    if (!root_bridge) return;

    // Initialiser les coûts de chemin
    for (size_t i = 0; i < network->nb_switches; i++) {
        stp_switch_t* sw = &network->switches[i];
        if (sw != root_bridge) {
            sw->root_path_cost = UINT32_MAX;
            for (size_t j = 0; j < sw->switch_data->nb_ports; j++) {
                sw->ports[j].path_cost = UINT32_MAX;
            }
        }
    }

    // Calculer les chemins en utilisant l'algorithme de Bellman-Ford
    bool changed;
    do {
        changed = false;
        for (size_t i = 0; i < network->nb_switches; i++) {
            stp_switch_t* sw = &network->switches[i];
            if (sw == root_bridge) continue;

            // Pour chaque port du switch
            for (size_t j = 0; j < sw->switch_data->nb_ports; j++) {
                // Trouver le switch connecté à ce port
                size_t connected_equip = 0;
                for (size_t k = 0; k < config->nb_links; k++) {
                    if (config->links[k].equip1 == i && config->links[k].equip2 < config->network.nb_switches) {
                        connected_equip = config->links[k].equip2;
                        break;
                    }
                    if (config->links[k].equip2 == i && config->links[k].equip1 < config->network.nb_switches) {
                        connected_equip = config->links[k].equip1;
                        break;
                    }
                }

                stp_switch_t* neighbor = &network->switches[connected_equip];
                uint32_t link_cost = find_link_cost(config, i, connected_equip);
                
                if (link_cost != UINT32_MAX) {
                    uint32_t new_cost = neighbor->root_path_cost + link_cost;
                    if (new_cost < sw->root_path_cost) {
                        sw->root_path_cost = new_cost;
                        sw->ports[j].path_cost = new_cost;
                        memcpy(&sw->root_bridge_id, &root_bridge->bridge_id, sizeof(mac_address_t));
                        changed = true;
                    }
                }
            }
        }
    } while (changed);

    // Afficher les résultats
    printf("\nCalcul des Chemins\n");
    for (size_t i = 0; i < network->nb_switches; i++) {
        stp_switch_t* sw = &network->switches[i];
        printf("Switch %zu (MAC: ", i);
        print_mac_address(&sw->bridge_id);
        printf(")\n");
        printf("  Coût vers la racine : %u\n", sw->root_path_cost);
        
        for (size_t j = 0; j < sw->switch_data->nb_ports; j++) {
            printf("  Port %zu : Coût = %u\n", j, sw->ports[j].path_cost);
        }
        printf("\n");
    }
}

// Fonction pour initialiser le réseau STP
stp_network_t* stp_init_network(const network_config_t* config) {
    if (!config) return NULL;

    stp_network_t* network = malloc(sizeof(stp_network_t));
    if (!network) return NULL;

    network->nb_switches = config->network.nb_switches;
    network->switches = malloc(network->nb_switches * sizeof(stp_switch_t));
    network->is_converged = false;

    if (!network->switches) {
        free(network);
        return NULL;
    }

    // Initialisation des switches
    for (size_t i = 0; i < network->nb_switches; i++) {
        stp_switch_t* stp_sw = &network->switches[i];
        switch_t* sw = &config->network.switches[i];

        stp_sw->switch_data = sw;
        stp_sw->is_root_bridge = false;
        stp_sw->root_path_cost = 0;
        memcpy(&stp_sw->bridge_id, &sw->mac, sizeof(mac_address_t));
        memcpy(&stp_sw->root_bridge_id, &sw->mac, sizeof(mac_address_t));

        // Initialisation des ports
        stp_sw->ports = malloc(sw->nb_ports * sizeof(stp_port_t));
        if (!stp_sw->ports) {
            stp_free_network(network);
            return NULL;
        }

        for (size_t j = 0; j < sw->nb_ports; j++) {
            stp_port_t* port = &stp_sw->ports[j];
            port->port_number = j;
            port->is_root_port = false;
            port->is_designated_port = false;
            port->is_blocked = false;
            port->path_cost = 0;
            memcpy(&port->designated_bridge, &sw->mac, sizeof(mac_address_t));
            port->designated_port = j;
        }
    }

    return network;
}

// Fonction pour élire le pont racine
void stp_elect_root_bridge(stp_network_t* network) {
    if (!network || network->nb_switches == 0) return;

    // Trouver le switch avec la plus petite adresse MAC
    stp_switch_t* root_bridge = &network->switches[0];
    for (size_t i = 1; i < network->nb_switches; i++) {
        if (compare_mac_addresses(&network->switches[i].bridge_id, &root_bridge->bridge_id) < 0) {
            root_bridge = &network->switches[i];
        }
    }

    // Définir le pont racine
    root_bridge->is_root_bridge = true;
    root_bridge->root_path_cost = 0;
    memcpy(&root_bridge->root_bridge_id, &root_bridge->bridge_id, sizeof(mac_address_t));

    // Afficher le résultat
    printf("\nÉlection du Pont Racine\n");
    printf("Pont racine : Switch %zu\n", root_bridge - network->switches);
    printf("Adresse MAC : ");
    print_mac_address(&root_bridge->bridge_id);
    printf("\n");
}

// Fonction pour libérer la mémoire du réseau STP
void stp_free_network(stp_network_t* network) {
    if (!network) return;

    if (network->switches) {
        for (size_t i = 0; i < network->nb_switches; i++) {
            if (network->switches[i].ports) {
                free(network->switches[i].ports);
            }
        }
        free(network->switches);
    }
    free(network);
} 