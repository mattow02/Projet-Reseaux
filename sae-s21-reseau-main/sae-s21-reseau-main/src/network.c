#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "network.h"
#include "addresses.h"

void print_switch_tables(const local_network_t* network) {
    if (!network) return;

    printf("\nTables de Commutation\n");
    
    for (size_t i = 0; i < network->nb_switches; i++) {
        const switch_t* sw = &network->switches[i];
        
        printf("\nSwitch %zu (MAC: ", i);
        print_mac_address(&sw->mac);
        printf(")\n");
        
        if (sw->table_size == 0) {
            printf("  Table vide\n");
            continue;
        }

        // En-tête du tableau
        printf("  +------------------+------+\n");
        printf("  | Adresse MAC      | Port |\n");
        printf("  +------------------+------+\n");

        // Affichage des entrées
        for (size_t j = 0; j < sw->table_size; j++) {
            const switch_table_entry_t* entry = &sw->table[j];
            printf("  | ");
            print_mac_address(&entry->mac);
            printf(" | %4zu |\n", entry->port);
        }

        printf("  +------------------+------+\n");
        printf("  Entrées: %zu/%zu\n", sw->table_size, sw->table_capacity);
    }
    printf("\n");
} 

void learn_mac(switch_t* sw, const mac_address_t* mac_src, size_t port_in) {
    // Vérifie si la table existe, sinon alloue-la
    if (!sw->table) {
        sw->table_capacity = 16;
        sw->table = malloc(sw->table_capacity * sizeof(switch_table_entry_t));
        sw->table_size = 0;
    }
    // Cherche si la MAC existe déjà
    for (size_t i = 0; i < sw->table_size; i++) {
        if (mac_address_equal(&sw->table[i].mac, mac_src)) {
            sw->table[i].port = port_in;
            return;
        }
    }
    // Ajoute une nouvelle entrée si capacité suffisante
    if (sw->table_size < sw->table_capacity) {
        sw->table[sw->table_size].mac = *mac_src;
        sw->table[sw->table_size].port = port_in;
        sw->table_size++;
    }
}

void receive_frame_on_switch(switch_t* sw, ethernet_frame_t* frame, size_t port_in) {
    // Apprentissage MAC automatique
    learn_mac(sw, &frame->src, port_in);
}