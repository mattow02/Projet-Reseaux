#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_reader.h"
#include "addresses.h"

// Fonction pour parser une adresse MAC
static bool parse_mac_address(const char* str, mac_address_t* mac) {
    return sscanf(str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &mac->octets[0], &mac->octets[1], &mac->octets[2],
           &mac->octets[3], &mac->octets[4], &mac->octets[5]) == 6;
}

// Fonction pour parser une adresse IPv4
static bool parse_ipv4_address(const char* str, ipv4_address_t* ip) {
    return sscanf(str, "%hhu.%hhu.%hhu.%hhu",
           &ip->octets[0], &ip->octets[1], &ip->octets[2], &ip->octets[3]) == 4;
}

network_config_t* read_network_config(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    network_config_t* config = malloc(sizeof(network_config_t));
    if (!config) {
        fclose(file);
        return NULL;
    }

    // Lecture de l'en-tête
    size_t nb_equipments, nb_links;
    if (fscanf(file, "%zu %zu", &nb_equipments, &nb_links) != 2) {
        fprintf(stderr, "Erreur: Format d'en-tête invalide\n");
        free(config);
        fclose(file);
        return NULL;
    }
    fgetc(file); // Consomme le retour à la ligne après l'en-tête

    // Initialisation du réseau
    config->network.nb_stations = 0;
    config->network.nb_switches = 0;
    config->network.stations = malloc(nb_equipments * sizeof(station_t));
    config->network.switches = malloc(nb_equipments * sizeof(switch_t));
    config->links = malloc(nb_links * sizeof(link_t));
    config->nb_links = nb_links;

    if (!config->network.stations || !config->network.switches || !config->links) {
        fprintf(stderr, "Erreur: Échec d'allocation mémoire\n");
        free_network_config(config);
        fclose(file);
        return NULL;
    }

    char line[256];
    // Lecture des équipements
    for (size_t i = 0; i < nb_equipments; i++) {
        if (!fgets(line, sizeof(line), file)) {
            fprintf(stderr, "Erreur: Lecture de l'équipement %zu échouée\n", i);
            free_network_config(config);
            fclose(file);
            return NULL;
        }
        line[strcspn(line, "\n")] = 0;

        int type;
        if (sscanf(line, "%d;", &type) != 1) {
            fprintf(stderr, "Erreur: Type d'équipement invalide à la ligne %zu\n", i);
            free_network_config(config);
            fclose(file);
            return NULL;
        }

        if (type == 1) { // Station
            char mac_str[18], ip_str[16];
            if (sscanf(line, "%d;%17[^;];%15[^;\n]", &type, mac_str, ip_str) != 3) {
                fprintf(stderr, "Erreur: Format station invalide à la ligne %zu\n", i);
                free_network_config(config);
                fclose(file);
                return NULL;
            }
            station_t* station = &config->network.stations[config->network.nb_stations];
            if (!parse_mac_address(mac_str, &station->mac) ||
                !parse_ipv4_address(ip_str, &station->ip)) {
                fprintf(stderr, "Erreur: Adresse invalide pour la station %zu\n", i);
                free_network_config(config);
                fclose(file);
                return NULL;
            }
            config->network.nb_stations++;
        } else if (type == 2) { // Switch
            char mac_str[18];
            size_t nb_ports;
            if (sscanf(line, "%d;%17[^;];%zu", &type, mac_str, &nb_ports) != 3) {
                fprintf(stderr, "Erreur: Format switch invalide à la ligne %zu\n", i);
                free_network_config(config);
                fclose(file);
                return NULL;
            }
            switch_t* switch_ = &config->network.switches[config->network.nb_switches];
            if (!parse_mac_address(mac_str, &switch_->mac)) {
                fprintf(stderr, "Erreur: Adresse MAC invalide pour le switch %zu\n", i);
                free_network_config(config);
                fclose(file);
                return NULL;
            }
            switch_->nb_ports = nb_ports;
            switch_->table = NULL;
            switch_->table_size = 0;
            switch_->table_capacity = 0;
            config->network.nb_switches++;
        } else {
            fprintf(stderr, "Erreur: Type d'équipement inconnu à la ligne %zu\n", i);
            free_network_config(config);
            fclose(file);
            return NULL;
        }
    }

    // Lecture des liens
    for (size_t i = 0; i < nb_links; i++) {
        if (!fgets(line, sizeof(line), file)) {
            fprintf(stderr, "Erreur: Lecture du lien %zu échouée\n", i);
            free_network_config(config);
            fclose(file);
            return NULL;
        }
        line[strcspn(line, "\n")] = 0;
        if (sscanf(line, "%zu;%zu;%d", &config->links[i].equip1, 
                   &config->links[i].equip2, &config->links[i].cost) != 3) {
            fprintf(stderr, "Erreur: Format de lien invalide pour le lien %zu\n", i);
            free_network_config(config);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return config;
}

void free_network_config(network_config_t* config) {
    if (!config) return;
    
    if (config->network.stations) free(config->network.stations);
    if (config->network.switches) {
        for (size_t i = 0; i < config->network.nb_switches; i++) {
            if (config->network.switches[i].table) {
                free(config->network.switches[i].table);
            }
        }
        free(config->network.switches);
    }
    if (config->links) free(config->links);
    free(config);
}

void print_network_config(const network_config_t* config) {
    if (!config) return;

    printf("Configuration du réseau :\n");
    printf("Nombre de stations : %zu\n", config->network.nb_stations);
    printf("Nombre de switches : %zu\n", config->network.nb_switches);
    printf("Nombre de liens : %zu\n\n", config->nb_links);

    printf("Switches :\n");
    for (size_t i = 0; i < config->network.nb_switches; i++) {
        printf("Switch %zu :\n", i);
        printf("  MAC : ");
        print_mac_address(&config->network.switches[i].mac);
        printf("\n  Ports : %zu\n", config->network.switches[i].nb_ports);
    }

    printf("\nStations :\n");
    for (size_t i = 0; i < config->network.nb_stations; i++) {
        printf("Station %zu :\n", i);
        printf("  MAC : ");
        print_mac_address(&config->network.stations[i].mac);
        printf("\n  IP : ");
        print_ipv4_address(&config->network.stations[i].ip);
        printf("\n");
    }

    printf("\nLiens :\n");
    for (size_t i = 0; i < config->nb_links; i++) {
        printf("Lien %zu : %zu <-> %zu (coût : %d)\n",
               i, config->links[i].equip1, config->links[i].equip2, config->links[i].cost);
    }
} 