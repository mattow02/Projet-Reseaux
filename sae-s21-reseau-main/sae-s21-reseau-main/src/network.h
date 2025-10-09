#ifndef NETWORK_H
#define NETWORK_H

#include "types.h"
#include "ethernet_frame.h"

// Structure pour représenter une station
typedef struct {
    mac_address_t mac;
    ipv4_address_t ip;
} station_t;

// Structure pour représenter une entrée dans la table de commutation
typedef struct {
    mac_address_t mac;
    size_t port;
} switch_table_entry_t;

// Structure pour représenter un switch
typedef struct {
    mac_address_t mac;
    size_t nb_ports;
    switch_table_entry_t* table;
    size_t table_size;
    size_t table_capacity;
} switch_t;

// Structure pour représenter un réseau local
typedef struct {
    graphe* topology;
    station_t* stations;
    switch_t* switches;
    size_t nb_stations;
    size_t nb_switches;
} local_network_t;

// Fonction pour afficher les tables de commutation
void print_switch_tables(const local_network_t* network);

void receive_frame_on_switch(switch_t* sw, ethernet_frame_t* frame, size_t port_in);

#endif // NETWORK_H 