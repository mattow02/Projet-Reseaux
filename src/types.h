#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* MAC address (6 bytes) */
typedef struct {
    uint8_t octets[6];
} mac_address_t;

/* IPv4 address (4 bytes) */
typedef struct {
    uint8_t octets[4];
} ipv4_address_t;

/* Graph vertex index */
typedef size_t vertex;

/* Graph edge connecting two vertices */
typedef struct {
    vertex v1;
    vertex v2;
} edge;

/* Adjacency-list graph */
typedef struct {
    size_t order;
    edge* edges;
    size_t edges_capacity;
    size_t nb_edges;
} graph;

#endif
