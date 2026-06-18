#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include "network.h"

/* Link between two network devices with an associated cost. */
typedef struct {
    size_t equip1;
    size_t equip2;
    int cost;
} link_t;

/* Complete network configuration parsed from a .lan file. */
typedef struct {
    local_network_t network;
    link_t* links;
    size_t nb_links;
} network_config_t;

/* Read and parse a network configuration file. Returns NULL on error. */
network_config_t* read_network_config(const char* filename);

/* Free all memory associated with a network configuration. */
void free_network_config(network_config_t* config);

/* Print the full network configuration to stdout. */
void print_network_config(const network_config_t* config);

#endif /* CONFIG_READER_H */
