#include "globals.h"
#include "print_packet.h"
#include "util.h"

void get_route(unsigned char *dest_ip,
               char *result_if_name, char *dest_mac,
               char *src_mac);

void get_new_route(uint32_t dest_ip,
                   char *result_if_name, char *dest_mac,
                   char *src_mac);
