#include "packet_create.h"

void create_packet(void *packet, unsigned char *src_mac, unsigned char *dest_mac){

   /*pointer to ethenet header*/
    unsigned char* etherhead = packet;

    /*userdata in ethernet frame*/
    unsigned char* data = packet + ETH_HLEN;

    /*another pointer to ethernet header*/
    struct ethhdr *eh = (struct ethhdr *)etherhead;

    /*set the frame header*/
    memcpy((void*)packet, (void*)dest_mac, ETH_ALEN);
    memcpy((void*)(packet+ETH_ALEN), (void*)src_mac, ETH_ALEN);
    eh->h_proto = 0x00;
    /*fill the frame with some data*/
    /* fill ethernet payload with some data */
    int j;
    for (j = 0; j < PACKET_SIZE - ETH_HLEN; j++)
        data[j] = (unsigned char)(0);
    *(int *)data = htonl(10);
}


