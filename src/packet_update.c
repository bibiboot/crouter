#include "packet_update.h"

/**
 * Used for forwarding packet
 */
void update_ethernet_packet(unsigned char *packet, unsigned char *src_mac,
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

/**
 * Decrement the TTL and update the checksum
 * If the new checksum is wrong it will be
 * detectec here.
 */
void update_ip_packet(unsigned char *packet){
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(packet  + sizeof(struct ethhdr) );
    unsigned int ttl = (unsigned int)iph->ttl;
    iphdrlen = iph->ihl*4;
    iph->ttl = ttl - 1;

    //printf("\n  Before Calculation Checksum : %d\n",ntohs(iph->check));
    iph->check = cksum(packet + sizeof(struct ethhdr), iphdrlen);
    //printf("\n  After Calculation Checksum : %d\n",ntohs(iph->check));
    if ( is_chksum_valid(packet + sizeof(struct ethhdr), iphdrlen) ) {
        //printf("CHECK SUM CORRECT\n");
    } else {
        printf("CHECK SUM NOT CORRECT\n");
    }
}

/**
 * Swap the source and destination MAC address
 * As you need to send it back.
 */
void update_ethernet_packet_reply(unsigned char *packet){
    /*another pointer to ethernet header*/
    struct ethhdr *eh = (struct ethhdr *)packet;
    char src_mac[ETH_ALEN], dest_mac[ETH_ALEN];

    memcpy(dest_mac, eh->h_source, ETH_ALEN);
    memcpy(src_mac, eh->h_dest, ETH_ALEN);

    /*set the frame header*/
    memcpy((void*)packet, (void*)dest_mac, ETH_ALEN); //0b
    memcpy((void*)(packet+ETH_ALEN), (void*)src_mac, ETH_ALEN); //01
}

/**
 * Update the icmp in case of ping reply
 * Change the icmp type
 * Echo request  = 8
 * Echo reply    = 0
 * Time Exceeded = 11
 * Change the checksum
 */
void update_icmp_packet_reply(unsigned char *packet, int data_size){
    struct iphdr *iph = (struct iphdr *)(packet  + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;
    int header_size = sizeof(struct ethhdr) + iphdrlen + sizeof(struct icmphdr);
    struct icmphdr *icmph = (struct icmphdr *)(packet + iphdrlen  + sizeof(struct ethhdr));

    icmph->type = ICMP_ECHOREPLY;

    int payload_size = data_size - header_size;
    icmph->checksum = 0;
    icmph->checksum = cksum_icmp(packet + sizeof(struct ethhdr) + iphdrlen,
                                 sizeof(struct icmphdr) + payload_size);
    //printf("\n  ICMP: After Calculation Checksum : %d\n",ntohs(icmph->checksum));

    if ( is_chksum_valid(packet + sizeof(struct ethhdr) + iphdrlen,
                         sizeof(struct icmphdr)) + payload_size ) {
        //printf("CHECK SUM CORRECT\n");
    } else {
        printf("CHECK SUM NOT CORRECT\n");
    }
}

// Decrement the ttl
void update_ip_packet_reply(unsigned char *packet){
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(packet  + sizeof(struct ethhdr) );
    unsigned int ttl = (unsigned int)iph->ttl;
    iphdrlen = iph->ihl*4;
    //iph->ttl = iph->ttl - 1;

    // Swap the ip address
    unsigned long src_ip = iph->saddr;
    unsigned long dest_ip = iph->daddr;
    iph->saddr = dest_ip;
    iph->daddr = src_ip;

    // Calculate the checksum
    //printf("\n  Before Calculation Checksum : %d\n",ntohs(iph->check));
    iph->check = cksum(packet + sizeof(struct ethhdr), iphdrlen);
    //printf("\n  After Calculation Checksum : %d\n",ntohs(iph->check));
    if ( is_chksum_valid(packet + sizeof(struct ethhdr), iphdrlen) ) {
        //printf("CHECK SUM CORRECT\n");
    } else {
        printf("CHECK SUM NOT CORRECT\n");
    }
}

/**
 * Calculate the new packet size
 * as the new payload = old payload + icmp header + ip header
 */
int get_resize_icmp_packet_time_exc_reply(unsigned char *packet, int packet_size) {
    struct iphdr *iph = (struct iphdr *)(packet  + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;
    int header_size = sizeof(struct ethhdr) + iphdrlen + sizeof(struct icmphdr);
    struct icmphdr *icmph = (struct icmphdr *)(packet + iphdrlen  + sizeof(struct ethhdr));

    int payload_size = packet_size - header_size;

    /*printf("ETH size = %d, IPH size = %d, ICMPH size = %d, Old payload size = %d\n",
            sizeof(struct ethhdr), iphdrlen, sizeof(struct icmphdr), payload_size);
    */

    int new_payload_size = payload_size + sizeof(struct icmphdr) + iphdrlen;

    return new_payload_size + sizeof(struct icmphdr) + iphdrlen + sizeof(struct ethhdr);
}


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
/**
 * Updae the icmp packet in case of
 * time exceeded packe to be send
 */
void update_icmp_packet_time_exc_reply(unsigned char *packet, int packet_size){

    struct iphdr *iph = (struct iphdr *)(packet  + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;
    int header_size = sizeof(struct ethhdr) + iphdrlen + sizeof(struct icmphdr);
    printf("Header size = %d and ICMPHREAD = %d\n", header_size, sizeof(struct icmphdr));
    struct icmphdr *icmph = (struct icmphdr *)(packet + iphdrlen  + sizeof(struct ethhdr));

    int payload_size = packet_size - header_size;

    // Get ipheader + icmpheader + payload of the echo request
    // Append them after icmpheader
    memcpy(packet + sizeof(struct ethhdr) + iphdrlen + sizeof(struct icmphdr) + iphdrlen,
           packet + sizeof(struct ethhdr) + iphdrlen, sizeof(struct icmphdr));
    memcpy(packet + sizeof(struct ethhdr) + iphdrlen + sizeof(struct icmphdr),
           packet + sizeof(struct ethhdr) , iphdrlen );

    icmph->type = ICMP_TIME_EXCEEDED;

    // Calculate the checksum
    //printf("\n  ICMP: Before Calculation Checksum : %d\n",ntohs(icmph->checksum));
    icmph->checksum = 0;
    //printf("PAYLOAD SIZE = %d\n", payload_size);

    icmph->checksum = in_cksum((unsigned short*)(packet + sizeof(struct ethhdr) + iphdrlen), sizeof(struct icmphdr) + 28);

    if ( is_chksum_valid(packet + sizeof(struct ethhdr) + iphdrlen, sizeof(struct icmphdr) + 28))  {
        //printf("CHECK SUM CORRECT\n");
    } else {
        printf("CHECK SUM NOT CORRECT\n");
    }
}

/**
 * Update the ip in case of ICMP time exceeded
 * Allocate fresh set of TTL i.e. 64
 * Put the routers's ip as source ip
 * Destination ip is the old source ip
 * Recalculate the checksum
 */
void update_ip_packet_time_exc_reply(unsigned char *packet, int packet_size){
    struct iphdr *iph = (struct iphdr *)(packet  + sizeof(struct ethhdr) );
    unsigned int ttl = (unsigned int)iph->ttl;
    unsigned short iphdrlen = iph->ihl*4;
    struct icmphdr *icmph = (struct icmphdr *)(packet + iphdrlen  + sizeof(struct ethhdr));
    int payload_size = packet_size - sizeof(struct ethhdr) + sizeof(iph) + sizeof(icmph);
    iph->ttl = 64;

    unsigned long src_ip = iph->saddr;
    unsigned long dest_ip = iph->daddr;
    iph->saddr = dest_ip;
    iph->daddr = src_ip;

    // Change total length
    // IP Header + ICMP Header + New payload
    //TODO iph->tot_len = iphdrlen + sizeof(icmph) + payload_size;
    iph->tot_len = htons(56);
    //fprintf(LOGFILE , "   |-Got IP Total Length   : %d  Bytes(size of Packet)\n",ntohs(iph->tot_len));

    // Calculate the checksum
    //printf("\n  Before Calculation Checksum : %d\n",ntohs(iph->check));
    iph->check = cksum(packet + sizeof(struct ethhdr), iphdrlen);
    //printf("\n  After Calculation Checksum : %d\n",ntohs(iph->check));
    if ( is_chksum_valid(packet + sizeof(struct ethhdr), iphdrlen) ) {
        //printf("CHECK SUM CORRECT\n");
    } else {
        printf("CHECK SUM NOT CORRECT\n");
    }
}
