#include "globals.h"
#include<netinet/ip.h>    //Provides declarations for ip header
#include <linux/if_ether.h>

bool is_allowed(unsigned char *packet, int size);
bool is_packet_reply(unsigned char *packet, int data_size);
bool is_ttl_zero(unsigned char *packet, int data_size);
