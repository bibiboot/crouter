#include "globals.h"
#include <linux/if_ether.h>

void create_log_file();

bool is_mac_addr_equal(unsigned char *mac_addr1, unsigned char *mac_addr2);

unsigned short cksum (unsigned char* data , int size);

bool is_chksum_valid (unsigned char* data , int size);
