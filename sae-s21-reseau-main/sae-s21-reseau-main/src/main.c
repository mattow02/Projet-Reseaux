#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "network.h"
#include "addresses.h"
#include "config_reader.h"
#include "ethernet_frame.h"
#include "stp.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    // Lecture de la configuration
    network_config_t* config = read_network_config(argv[1]);
    if (!config) {
        printf("Erreur lors de la lecture de la configuration\n");
        return 1;
    }

    // Affichage de la configuration initiale
    print_network_config(config);

    // Initialisation du réseau STP
    stp_network_t* stp_network = stp_init_network(config);
    if (!stp_network) {
        printf("Erreur lors de l'initialisation du réseau STP\n");
        free_network_config(config);
        return 1;
    }

    // Élection du pont racine
    stp_elect_root_bridge(stp_network);

    // Calcul des chemins
    stp_calculate_paths(stp_network, config);

    // Création d'une trame Ethernet de test
    ethernet_frame_t test_frame;
    memset(&test_frame, 0, sizeof(ethernet_frame_t));
    for (int i = 0; i < 7; i++) {
        test_frame.preambule[i] = 0xAA;
    }
    test_frame.sfd = 0xAB;

    // Adresse MAC source et destination de test
    mac_address_t src_mac = {{0x00, 0x11, 0x22, 0x33, 0x44, 0x55}};
    mac_address_t dest_mac = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    memcpy(&test_frame.src, &src_mac, sizeof(mac_address_t));
    memcpy(&test_frame.dest, &dest_mac, sizeof(mac_address_t));

    // Création et affichage d'une trame de Switch 0 vers Switch 1
    printf("\nTrame de Switch 0 vers Switch 1 :\n");
    ethernet_frame_t frame1;
    memset(&frame1, 0, sizeof(ethernet_frame_t));
    for (int i = 0; i < 7; i++) frame1.preambule[i] = 0xAA;
    frame1.sfd = 0xAB;
    memcpy(&frame1.src, &config->network.switches[0].mac, sizeof(mac_address_t));
    memcpy(&frame1.dest, &config->network.switches[1].mac, sizeof(mac_address_t));
    frame1.type = 0x0800;
    strcpy((char*)frame1.data, "De S0 à S1");
    frame1.data_len = strlen((char*)frame1.data);
    frame1.fcs = 0x11111111;
    print_ethernet_frame(&frame1);
    receive_frame_on_switch(&config->network.switches[1], &frame1, 0); // Switch 1 apprend la MAC de S0 sur port 0

    // Création et affichage d'une trame de Switch 1 vers Switch 0
    printf("\nTrame de Switch 1 vers Switch 0 :\n");
    ethernet_frame_t frame2;
    memset(&frame2, 0, sizeof(ethernet_frame_t));
    for (int i = 0; i < 7; i++) frame2.preambule[i] = 0xAA;
    frame2.sfd = 0xAB;
    memcpy(&frame2.src, &config->network.switches[1].mac, sizeof(mac_address_t));
    memcpy(&frame2.dest, &config->network.switches[0].mac, sizeof(mac_address_t));
    frame2.type = 0x0800;
    strcpy((char*)frame2.data, "De S1 à S0");
    frame2.data_len = strlen((char*)frame2.data);
    frame2.fcs = 0x22222222;
    print_ethernet_frame(&frame2);
    receive_frame_on_switch(&config->network.switches[0], &frame2, 0); // Switch 0 apprend la MAC de S1 sur port 0

    // Affichage des tables de commutation après les échanges
    print_switch_tables(&config->network);

    // Nettoyage
    stp_free_network(stp_network);
    free_network_config(config);

    return 0;
}