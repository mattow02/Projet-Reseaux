#ifndef ETHERNET_FRAME_H
#define ETHERNET_FRAME_H

#include <stdint.h>
#include <stddef.h>
#include "types.h"

#define ETHERNET_MIN_DATA 46
#define ETHERNET_MAX_DATA 1500

// Structure représentant une trame Ethernet
typedef struct {
    uint8_t preambule[7]; // 7 octets
    uint8_t sfd;          // 1 octet
    mac_address_t dest;   // 6 octets
    mac_address_t src;    // 6 octets
    uint16_t type;        // 2 octets
    uint8_t data[ETHERNET_MAX_DATA]; // données (46 à 1500 octets)
    size_t data_len;      // longueur réelle des données
    uint32_t fcs;         // 4 octets
} ethernet_frame_t;

// Affichage lisible
void print_ethernet_frame(const ethernet_frame_t* frame);
// Affichage hexadécimal brut
void print_ethernet_frame_hex(const ethernet_frame_t* frame);

#endif // ETHERNET_FRAME_H 