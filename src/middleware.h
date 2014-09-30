#include "globals.h"

void incoming_packet_handler(unsigned char *packet, int size);

void incoming_packet_handler_self_icmp(unsigned char *packet, int size);

void prepare_n_send_packet(unsigned char *packet, int size,
                           unsigned char *dest_mac, char *if_name);

void incoming_packet_handler_ttl_zero(unsigned char *buffer, int data_size) ;
