#include "globals.h"
#include <linux/if_ether.h>

uint32_t char_to_uint32(char *network);

char *ipaddr_string(u_int32_t *ip);

void print_rtable_keys();

void create_log_file();

bool is_mac_addr_equal(unsigned char *mac_addr1,
                       unsigned char *mac_addr2);

bool is_ip_equal(unsigned char *ip1, unsigned char *ip2);

bool is_chksum_valid (unsigned char* data , int size);

unsigned short cksum (unsigned char* data , int size);

unsigned short cksum_icmp (unsigned char* data , int size);

unsigned short in_cksum(unsigned short *ptr, int nbytes);


