#include <linux/if_ether.h>
#include "globals.h"

void print_data (unsigned char* data , int size);
void print_mac(const char *addr);
void print_routed_packet(struct sockaddr_in dest, char *result_if_name, char *src_mac, char *dest_mac);
void print_ip(uint32_t ip);

void print_data_detail (unsigned char* data , int size);
void print_icmp_packet(unsigned char* buffer , int size);
void print_udp_packet(unsigned char *buffer , int size);
void print_ip_header(unsigned char* buffer, int size);
void print_ethernet_header(unsigned char* buffer, int size);
