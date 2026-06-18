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

    /* Parse the network configuration file */
    network_config_t* config = read_network_config(argv[1]);
    if (!config) {
        fprintf(stderr, "Error: failed to read network configuration\n");
        return 1;
    }

    print_network_config(config);

    /* Initialize and run STP */
    stp_network_t* stp_network = stp_init_network(config);
    if (!stp_network) {
        fprintf(stderr, "Error: failed to initialize STP network\n");
        free_network_config(config);
        return 1;
    }

    stp_elect_root_bridge(stp_network);
    stp_calculate_paths(stp_network, config);

    /* Demonstrate MAC learning with test frames */
    printf("\nFrame from Switch 0 to Switch 1:\n");
    ethernet_frame_t frame1;
    memset(&frame1, 0, sizeof(ethernet_frame_t));
    for (int i = 0; i < 7; i++) frame1.preamble[i] = 0xAA;
    frame1.sfd = 0xAB;
    memcpy(&frame1.src, &config->network.switches[0].mac, sizeof(mac_address_t));
    memcpy(&frame1.dest, &config->network.switches[1].mac, sizeof(mac_address_t));
    frame1.type = 0x0800;
    strcpy((char*)frame1.data, "S0 -> S1");
    frame1.data_len = strlen((char*)frame1.data);
    frame1.fcs = 0x11111111;
    print_ethernet_frame(&frame1);
    receive_frame_on_switch(&config->network.switches[1], &frame1, 0);

    printf("\nFrame from Switch 1 to Switch 0:\n");
    ethernet_frame_t frame2;
    memset(&frame2, 0, sizeof(ethernet_frame_t));
    for (int i = 0; i < 7; i++) frame2.preamble[i] = 0xAA;
    frame2.sfd = 0xAB;
    memcpy(&frame2.src, &config->network.switches[1].mac, sizeof(mac_address_t));
    memcpy(&frame2.dest, &config->network.switches[0].mac, sizeof(mac_address_t));
    frame2.type = 0x0800;
    strcpy((char*)frame2.data, "S1 -> S0");
    frame2.data_len = strlen((char*)frame2.data);
    frame2.fcs = 0x22222222;
    print_ethernet_frame(&frame2);
    receive_frame_on_switch(&config->network.switches[0], &frame2, 0);

    /* Show MAC address tables after the exchange */
    print_switch_tables(&config->network);

    /* Cleanup */
    stp_free_network(stp_network);
    free_network_config(config);

    return 0;
}
