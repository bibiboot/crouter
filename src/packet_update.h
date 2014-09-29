#include "globals.h"
#include <linux/if_ether.h>
#include <arpa/inet.h>

void update_icmp_packet(unsigned char *packet, unsigned char *src_mac,
                        unsigned char *dest_mac);

void update_ip_packet(unsigned char *packet);
