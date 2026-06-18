#ifndef ADDRESSES_H
#define ADDRESSES_H

#include "types.h"

/* Print a MAC address in hexadecimal colon notation. */
void print_mac_address(const mac_address_t* mac);

/* Print an IPv4 address in dotted-decimal notation. */
void print_ipv4_address(const ipv4_address_t* ip);

/* Return true if two MAC addresses are equal. */
bool mac_address_equal(const mac_address_t* mac1, const mac_address_t* mac2);

/* Return true if two IPv4 addresses are equal. */
bool ipv4_address_equal(const ipv4_address_t* ip1, const ipv4_address_t* ip2);

#endif /* ADDRESSES_H */
