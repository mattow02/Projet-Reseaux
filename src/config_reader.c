#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_reader.h"
#include "addresses.h"

/* Parse a MAC address string "XX:XX:XX:XX:XX:XX" into a mac_address_t. */
static bool parse_mac_address(const char* str, mac_address_t* mac) {
    return sscanf(str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &mac->octets[0], &mac->octets[1], &mac->octets[2],
           &mac->octets[3], &mac->octets[4], &mac->octets[5]) == 6;
}

/* Parse an IPv4 address string "X.X.X.X" into an ipv4_address_t. */
static bool parse_ipv4_address(const char* str, ipv4_address_t* ip) {
    return sscanf(str, "%hhu.%hhu.%hhu.%hhu",
           &ip->octets[0], &ip->octets[1], &ip->octets[2], &ip->octets[3]) == 4;
}

network_config_t* read_network_config(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        return NULL;
    }

    network_config_t* config = malloc(sizeof(network_config_t));
    if (!config) {
        fclose(file);
        return NULL;
    }

    /* Read header: number of devices and links */
    size_t nb_equipments, nb_links;
    if (fscanf(file, "%zu %zu", &nb_equipments, &nb_links) != 2) {
        fprintf(stderr, "Error: invalid header format\n");
        free(config);
        fclose(file);
        return NULL;
    }
    fgetc(file); /* consume trailing newline */

    /* Initialize network arrays */
    config->network.nb_stations = 0;
    config->network.nb_switches = 0;
    config->network.stations = malloc(nb_equipments * sizeof(station_t));
    config->network.switches = malloc(nb_equipments * sizeof(switch_t));
    config->links = malloc(nb_links * sizeof(link_t));
    config->nb_links = nb_links;

    if (!config->network.stations || !config->network.switches || !config->links) {
        fprintf(stderr, "Error: memory allocation failed\n");
        free_network_config(config);
        fclose(file);
        return NULL;
    }

    char line[256];

    /* Parse each device line */
    for (size_t i = 0; i < nb_equipments; i++) {
        if (!fgets(line, sizeof(line), file)) {
            fprintf(stderr, "Error: failed to read device %zu\n", i);
            free_network_config(config);
            fclose(file);
            return NULL;
        }
        line[strcspn(line, "\n")] = 0;

        int type;
        if (sscanf(line, "%d;", &type) != 1) {
            fprintf(stderr, "Error: invalid device type at line %zu\n", i);
            free_network_config(config);
            fclose(file);
            return NULL;
        }

        if (type == 1) { /* Station */
            char mac_str[18], ip_str[16];
            if (sscanf(line, "%d;%17[^;];%15[^;\n]", &type, mac_str, ip_str) != 3) {
                fprintf(stderr, "Error: invalid station format at line %zu\n", i);
                free_network_config(config);
                fclose(file);
                return NULL;
            }
            station_t* station = &config->network.stations[config->network.nb_stations];
            if (!parse_mac_address(mac_str, &station->mac) ||
                !parse_ipv4_address(ip_str, &station->ip)) {
                fprintf(stderr, "Error: invalid address for station %zu\n", i);
                free_network_config(config);
                fclose(file);
                return NULL;
            }
            config->network.nb_stations++;
        } else if (type == 2) { /* Switch */
            char mac_str[18];
            size_t nb_ports;
            if (sscanf(line, "%d;%17[^;];%zu", &type, mac_str, &nb_ports) != 3) {
                fprintf(stderr, "Error: invalid switch format at line %zu\n", i);
                free_network_config(config);
                fclose(file);
                return NULL;
            }
            switch_t* sw = &config->network.switches[config->network.nb_switches];
            if (!parse_mac_address(mac_str, &sw->mac)) {
                fprintf(stderr, "Error: invalid MAC address for switch %zu\n", i);
                free_network_config(config);
                fclose(file);
                return NULL;
            }
            sw->nb_ports = nb_ports;
            sw->table = NULL;
            sw->table_size = 0;
            sw->table_capacity = 0;
            config->network.nb_switches++;
        } else {
            fprintf(stderr, "Error: unknown device type at line %zu\n", i);
            free_network_config(config);
            fclose(file);
            return NULL;
        }
    }

    /* Parse each link line */
    for (size_t i = 0; i < nb_links; i++) {
        if (!fgets(line, sizeof(line), file)) {
            fprintf(stderr, "Error: failed to read link %zu\n", i);
            free_network_config(config);
            fclose(file);
            return NULL;
        }
        line[strcspn(line, "\n")] = 0;
        if (sscanf(line, "%zu;%zu;%d", &config->links[i].equip1,
                   &config->links[i].equip2, &config->links[i].cost) != 3) {
            fprintf(stderr, "Error: invalid link format for link %zu\n", i);
            free_network_config(config);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return config;
}

void free_network_config(network_config_t* config) {
    if (!config) return;

    if (config->network.stations) free(config->network.stations);
    if (config->network.switches) {
        for (size_t i = 0; i < config->network.nb_switches; i++) {
            free(config->network.switches[i].table);
        }
        free(config->network.switches);
    }
    free(config->links);
    free(config);
}

void print_network_config(const network_config_t* config) {
    if (!config) return;

    printf("Network Configuration:\n");
    printf("Stations: %zu\n", config->network.nb_stations);
    printf("Switches: %zu\n", config->network.nb_switches);
    printf("Links:    %zu\n\n", config->nb_links);

    printf("Switches:\n");
    for (size_t i = 0; i < config->network.nb_switches; i++) {
        printf("  Switch %zu — MAC: ", i);
        print_mac_address(&config->network.switches[i].mac);
        printf(", Ports: %zu\n", config->network.switches[i].nb_ports);
    }

    printf("\nStations:\n");
    for (size_t i = 0; i < config->network.nb_stations; i++) {
        printf("  Station %zu — MAC: ", i);
        print_mac_address(&config->network.stations[i].mac);
        printf(", IP: ");
        print_ipv4_address(&config->network.stations[i].ip);
        printf("\n");
    }

    printf("\nLinks:\n");
    for (size_t i = 0; i < config->nb_links; i++) {
        printf("  Link %zu: %zu <-> %zu (cost: %d)\n",
               i, config->links[i].equip1, config->links[i].equip2, config->links[i].cost);
    }
}
