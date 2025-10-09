#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

// Type pour représenter une adresse MAC (6 octets)
typedef struct {
    uint8_t octets[6];
} mac_address_t;

// Type pour représenter une adresse IPv4 (4 octets)
typedef struct {
    uint8_t octets[4];
} ipv4_address_t;

// Type pour représenter un sommet dans le graphe
typedef size_t sommet;

// Structure pour représenter une arête
typedef struct {
    sommet s1;
    sommet s2;
} arete;

// Structure pour représenter un graphe
typedef struct {
    size_t ordre;
    arete* aretes;
    size_t aretes_capacite;
    size_t nb_aretes;
} graphe;

#endif 