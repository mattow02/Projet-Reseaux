#ifndef ETHERNET_FRAME_H
#define ETHERNET_FRAME_H

#include <stdint.h>
#include <stddef.h>
#include "types.h"

#define ETHERNET_MIN_DATA 46
#define ETHERNET_MAX_DATA 1500

/* Ethernet II frame */
typedef struct {
    uint8_t preamble[7];
    uint8_t sfd;
    mac_address_t dest;
    mac_address_t src;
    uint16_t type;
    uint8_t data[ETHERNET_MAX_DATA];
    size_t data_len;
    uint32_t fcs;
} ethernet_frame_t;

/* Print a human-readable representation of an Ethernet frame. */
void print_ethernet_frame(const ethernet_frame_t* frame);

/* Print the raw hexadecimal bytes of an Ethernet frame. */
void print_ethernet_frame_hex(const ethernet_frame_t* frame);

#endif /* ETHERNET_FRAME_H */
