#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>    //Declarations for ip header
#include <netinet/ip_icmp.h>   //Declarations for icmp header

#include "globals.h"
#include "print_packet.h"
#include "util.h"

void update_ethernet_packet(unsigned char *packet,
                            unsigned char *src_mac,
                            unsigned char *dest_mac);

void update_ip_packet(unsigned char *packet);

void update_ethernet_packet_reply(unsigned char *packet);

void update_ip_packet_reply(unsigned char *packet);

void update_icmp_packet_reply(unsigned char *packet,
                              int data_size);

void update_icmp_packet_time_exc_reply(unsigned char *packet,
                                       int data_size);

int get_resize_icmp_packet_time_exc_reply(unsigned char *packet, int packet_size);
