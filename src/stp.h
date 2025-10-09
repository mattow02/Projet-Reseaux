#ifndef STP_H
#define STP_H

#include "network.h"
#include "config_reader.h"
#include <stdbool.h>

// Structure pour représenter un port STP
typedef struct {
    size_t port_number;
    bool is_root_port;
    bool is_designated_port;
    bool is_blocked;
    uint32_t path_cost;
    mac_address_t designated_bridge;
    uint16_t designated_port;
} stp_port_t;

// Structure pour représenter un switch STP
typedef struct {
    switch_t* switch_data;
    stp_port_t* ports;
    bool is_root_bridge;
    uint32_t root_path_cost;
    mac_address_t root_bridge_id;
    mac_address_t bridge_id;
} stp_switch_t;

// Structure pour représenter un réseau STP
typedef struct {
    stp_switch_t* switches;
    size_t nb_switches;
    bool is_converged;
} stp_network_t;

// Fonctions STP
stp_network_t* stp_init_network(const network_config_t* config);
void stp_elect_root_bridge(stp_network_t* network);
void stp_calculate_paths(stp_network_t* network, const network_config_t* config);
void stp_free_network(stp_network_t* network);

#endif // STP_H 