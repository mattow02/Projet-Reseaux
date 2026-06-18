#ifndef NETWORK_H
#define NETWORK_H

#include "types.h"
#include "ethernet_frame.h"

/* Network station (end host) */
typedef struct {
    mac_address_t mac;
    ipv4_address_t ip;
} station_t;

/* Entry in a switch's MAC address table */
typedef struct {
    mac_address_t mac;
    size_t port;
} switch_table_entry_t;

/* Layer-2 switch */
typedef struct {
    mac_address_t mac;
    size_t nb_ports;
    switch_table_entry_t* table;
    size_t table_size;
    size_t table_capacity;
} switch_t;

/* Local area network containing stations and switches */
typedef struct {
    graph* topology;
    station_t* stations;
    switch_t* switches;
    size_t nb_stations;
    size_t nb_switches;
} local_network_t;

/* Print the MAC address tables of all switches in the network. */
void print_switch_tables(const local_network_t* network);

/* Process a received Ethernet frame on a switch port (MAC learning). */
void receive_frame_on_switch(switch_t* sw, ethernet_frame_t* frame, size_t port_in);

#endif /* NETWORK_H */
