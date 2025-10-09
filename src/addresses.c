#include <stdio.h>
#include <string.h>
#include "types.h"

// Fonction pour afficher une adresse MAC en format hexadécimal
void print_mac_address(const mac_address_t* mac) {
    printf("%02X:%02X:%02X:%02X:%02X:%02X",
           mac->octets[0], mac->octets[1], mac->octets[2],
           mac->octets[3], mac->octets[4], mac->octets[5]);
}

// Fonction pour afficher une adresse IPv4 en notation décimale pointée
void print_ipv4_address(const ipv4_address_t* ip) {
    printf("%d.%d.%d.%d",
           ip->octets[0], ip->octets[1], ip->octets[2], ip->octets[3]);
}

// Fonction pour comparer deux adresses MAC
bool mac_address_equal(const mac_address_t* mac1, const mac_address_t* mac2) {
    return memcmp(mac1->octets, mac2->octets, 6) == 0;
}

// Fonction pour comparer deux adresses IPv4
bool ipv4_address_equal(const ipv4_address_t* ip1, const ipv4_address_t* ip2) {
    return memcmp(ip1->octets, ip2->octets, 4) == 0;
} 