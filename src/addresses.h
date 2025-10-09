#ifndef ADDRESSES_H
#define ADDRESSES_H

#include "types.h"

// Fonction pour afficher une adresse MAC en format hexadécimal
void print_mac_address(const mac_address_t* mac);

// Fonction pour afficher une adresse IPv4 en notation décimale pointée
void print_ipv4_address(const ipv4_address_t* ip);

// Fonction pour comparer deux adresses MAC
bool mac_address_equal(const mac_address_t* mac1, const mac_address_t* mac2);

// Fonction pour comparer deux adresses IPv4
bool ipv4_address_equal(const ipv4_address_t* ip1, const ipv4_address_t* ip2);

#endif // ADDRESSES_H 