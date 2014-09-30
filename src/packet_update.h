#include "globals.h"
#include <linux/if_ether.h>
#include <arpa/inet.h>

void update_ethernet_packet(unsigned char *packet,
                            unsigned char *src_mac,
                            unsigned char *dest_mac);

void update_ethernet_packet_reply(unsigned char *packet);

void update_ip_packet(unsigned char *packet);

void update_ip_packet_reply(unsigned char *packet);

void update_icmp_packet_reply(unsigned char *packet);
