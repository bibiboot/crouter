#include "globals.h"

void init_build_arp_cache();

void get_mac_from_arp_cache_print(unsigned char *ip, char *mac_addr);

void get_mac_from_arp_cache(uint32_t ip, char *mac_addr);

void init_build_arp_cache_dynamic();

void print_arp_cache_table_list(uint32_t *lan_node, int size);
