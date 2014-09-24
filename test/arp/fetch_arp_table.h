#include <stdio.h>
#include <stdlib.h>

int get_mac_addr_from_arp_table(const char *given_ip,
                                char **mac_addr);

void print_arp_table();
