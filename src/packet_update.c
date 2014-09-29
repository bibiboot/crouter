#include "packet_update.h"
#include "util.h"
#include<netinet/ip.h>    //Provides declarations for ip header
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

// Decrement the ttl
void update_ip_packet(unsigned char *packet){
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(packet  + sizeof(struct ethhdr) );
    unsigned int ttl = (unsigned int)iph->ttl;
    iphdrlen = iph->ihl*4;
    iph->ttl = ttl - 1;

    if( iph->ttl == 0 ) {
        printf("\n SEND ICMP packet for Time Exceeded \n");
    }

    // Calculate the checksum
    printf("\n  Before Calculation Checksum : %d\n",ntohs(iph->check));
    iph->check = cksum(packet + sizeof(struct ethhdr), iphdrlen);
    printf("\n  After Calculation Checksum : %d\n",ntohs(iph->check));
    if ( is_chksum_valid(packet + sizeof(struct ethhdr), iphdrlen) ) {
        printf("CHECK SUM CORRECT\n");
    } else {
        printf("CHECK SUM NOT CORRECT\n");
    }
}
