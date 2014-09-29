#include "packet_update.h"

/**
   0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |     Code      |          Checksum             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                             unused                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      Internet Header + 64 bits of Original Data Datagram      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**/
void update_icmp_packet(unsigned char *packet, unsigned char *src_mac,
                        unsigned char *dest_mac){
   /*pointer to ethenet header*/
    unsigned char* etherhead = packet;

    ///*userdata in ethernet frame*/
    //unsigned char* data = packet + ETH_HLEN;

    /*another pointer to ethernet header*/
    struct ethhdr *eh = (struct ethhdr *)etherhead;

    /*set the frame header*/
    memcpy((void*)packet, (void*)dest_mac, ETH_ALEN);
    memcpy((void*)(packet+ETH_ALEN), (void*)src_mac, ETH_ALEN);
    /*echo request = 8*/
    //eh->h_proto = 0x8;
}


