#include<netinet/ip.h>    //declarations for ip header
#include <linux/if_ether.h>
#include<netinet/ip_icmp.h>   //declarations for icmp header

#include "globals.h"
#include "print_packet.h"
#include "route.h"
#include "util.h"

bool is_allowed(unsigned char *packet, int size);

bool is_packet_reply(unsigned char *packet, int data_size);

bool is_ttl_zero(unsigned char *packet, int data_size);
