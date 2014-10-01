#include "filter.h"

/**
 * Allow a packet to be sniffed or not.
 * First filter to incoming packet.
 * TCP = 6 | ICMP = 1 | UDP = 17
 */
bool is_allowed(unsigned char *packet, int data_size){
    struct sockaddr_in source, dest;
    struct ethhdr *eth = (struct ethhdr *)packet;
    struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));

    /* Get src and dest ip of the incoming packet */
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    /*
     * Only allow TCP or UDP or ICMP , and matches the given
     * interface MAC address
     */
    if (iph->protocol ==  17 || iph->protocol == 1 || iph->protocol == 6) {

        if (is_mac_addr_equal(eth->h_dest, globals.src_mac) ||
            is_mac_addr_equal(eth->h_dest, globals.src_mac2)){
            return true;
        }
    }
    return false;
}

/**
 * Check if the packet is ICMP echo request and
 * destination ip equals to the user router
 */
bool is_packet_reply(unsigned char *packet, int data_size) {
    struct ethhdr *eth = (struct ethhdr *)packet;
    struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;
    struct icmphdr *icmph = (struct icmphdr *)(packet + iphdrlen  + sizeof(struct ethhdr));

    struct sockaddr_in source, dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    /* Check ICMP packet, Echo request and destination ip */
    if ( iph->protocol == 1 &&
         is_ip_equal((unsigned char *)inet_ntoa(dest.sin_addr), "10.10.0.2") &&
         icmph->type == 8 ) {
        return true;
    }
    return false;
}

/**
 * If the current TTL is 1
 */
bool is_ttl_zero(unsigned char *packet, int data_size) {
    struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));
    return (iph->ttl == 1) ? true : false ;
}
