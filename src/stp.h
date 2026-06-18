#ifndef STP_H
#define STP_H

#include "network.h"
#include "config_reader.h"
#include <stdbool.h>

/* STP port state */
typedef struct {
    size_t port_number;
    bool is_root_port;
    bool is_designated_port;
    bool is_blocked;
    uint32_t path_cost;
    mac_address_t designated_bridge;
    uint16_t designated_port;
} stp_port_t;

/* STP-enabled switch */
typedef struct {
    switch_t* switch_data;
    stp_port_t* ports;
    bool is_root_bridge;
    uint32_t root_path_cost;
    mac_address_t root_bridge_id;
    mac_address_t bridge_id;
} stp_switch_t;

/* STP network state */
typedef struct {
    stp_switch_t* switches;
    size_t nb_switches;
    bool is_converged;
} stp_network_t;

/* Initialize STP state from a parsed network configuration. */
stp_network_t* stp_init_network(const network_config_t* config);

/* Elect the root bridge (lowest MAC address wins). */
void stp_elect_root_bridge(stp_network_t* network);

/* Calculate shortest paths to root using Bellman-Ford. */
void stp_calculate_paths(stp_network_t* network, const network_config_t* config);

/* Free all memory associated with the STP network. */
void stp_free_network(stp_network_t* network);

#endif /* STP_H */
