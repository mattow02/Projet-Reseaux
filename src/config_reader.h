#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include "network.h"

// Structure pour représenter un lien entre deux équipements
typedef struct {
    size_t equip1;
    size_t equip2;
    int cost;
} link_t;

// Structure pour stocker la configuration complète
typedef struct {
    local_network_t network;
    link_t* links;
    size_t nb_links;
} network_config_t;

// Fonction pour lire un fichier de configuration
network_config_t* read_network_config(const char* filename);

// Fonction pour libérer la mémoire d'une configuration
void free_network_config(network_config_t* config);

// Fonction pour afficher une configuration
void print_network_config(const network_config_t* config);

#endif // CONFIG_READER_H 