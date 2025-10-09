#include "ethernet_frame.h"
#include <stdio.h>
#include <string.h>
#include "addresses.h"

void print_ethernet_frame(const ethernet_frame_t* frame) {
    printf("Trame Ethernet :\n");
    printf("  Préambule : ");
    for (int i = 0; i < 7; i++) printf("%02X ", frame->preambule[i]);
    printf("\n  SFD : %02X\n", frame->sfd);
    printf("  Destination : ");
    print_mac_address(&frame->dest);
    printf("\n  Source      : ");
    print_mac_address(&frame->src);
    printf("\n  Type        : 0x%04X\n", frame->type);
    printf("  Données    : ");
    for (size_t i = 0; i < frame->data_len; i++) printf("%02X ", frame->data[i]);
    printf("\n  Longueur    : %zu\n", frame->data_len);
    printf("  FCS         : %08X\n", frame->fcs);
}

void print_ethernet_frame_hex(const ethernet_frame_t* frame) {
    // Affichage brut de tous les octets de la trame
    printf("Trame brute : ");
    for (int i = 0; i < 7; i++) printf("%02X ", frame->preambule[i]);
    printf("%02X ", frame->sfd);
    for (int i = 0; i < 6; i++) printf("%02X ", frame->dest.octets[i]);
    for (int i = 0; i < 6; i++) printf("%02X ", frame->src.octets[i]);
    printf("%02X %02X ", (frame->type >> 8) & 0xFF, frame->type & 0xFF);
    for (size_t i = 0; i < frame->data_len; i++) printf("%02X ", frame->data[i]);
    // Padding si data_len < ETHERNET_MIN_DATA
    for (size_t i = frame->data_len; i < ETHERNET_MIN_DATA; i++) printf("00 ");
    printf("%02X %02X %02X %02X", (frame->fcs >> 24) & 0xFF, (frame->fcs >> 16) & 0xFF, (frame->fcs >> 8) & 0xFF, frame->fcs & 0xFF);
    printf("\n");
} 