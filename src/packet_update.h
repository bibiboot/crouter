#include "globals.h"
#include <linux/if_ether.h>

void update_icmp_packet(unsigned char *packet, unsigned char *src_mac,
                        unsigned char *dest_mac);
