#include "filter.h"

#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h> //For standard things
#include<stdlib.h>    //malloc
#include<string.h>    //strlen

#include<netinet/ip_icmp.h>   //Provides declarations for icmp header
#include<netinet/udp.h>   //Provides declarations for udp header
#include<netinet/tcp.h>   //Provides declarations for tcp header
#include<netinet/ip.h>    //Provides declarations for ip header
#include<netinet/if_ether.h>  //For ETH_P_ALL
#include<net/ethernet.h>  //For ether_header
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

/**
 * The source ip is not of his own
 */
bool is_src_not_own_ip(uint32_t ip) {
    if ( ip != globals.eth0_ip && 
         ip != globals.eth1_ip &&
         ip != globals.eth2_ip ) {

        return true;
    }
    return false;
}

bool is_rip_packet(unsigned char *packet, int data_size) {
    struct ethhdr *eth = (struct ethhdr *)packet;
    struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;
    struct udphdr *udph = (struct udphdr*)(packet + iphdrlen  + sizeof(struct ethhdr));

    //printf("PROT: %d, PORT: %d, DEST IP: ", iph->protocol, ntohs(udph->dest));
    //print_ip(iph->daddr);
    //printf("\n");

    if (iph->protocol == 17 && ntohs(udph->dest) == RIP_PORT) {
        return true;
    }
    return false;
}

bool is_same_topology(uint32_t dest_ip) {
    uint32_t value = dest_ip & char_to_uint32("255.0.0.0");
    //print_ip(value);
    //printf("\n");
    uint32_t topo_network = char_to_uint32("10.0.0.0");
    if (value == topo_network) {
        //printf("Same network\n");
        return true;
    }
    return false;
}

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


        if (( is_mac_addr_equal(eth->h_dest, globals.eth1_mac) ||
            is_mac_addr_equal(eth->h_dest, globals.eth2_mac) ||
            is_mac_addr_equal(eth->h_dest, globals.eth3_mac) ||
            ( is_ip_equal(inet_ntoa(dest.sin_addr), MULTICAST_IP)) ) &&
            ( is_same_topology( iph->daddr ) ||
            ( ( is_ip_equal(inet_ntoa(dest.sin_addr), MULTICAST_IP)) && is_src_not_own_ip(source.sin_addr.s_addr) ))) {
             
             if (DEBUG) {
                 printf("\nFilter: Allowed : Source ip : ");
                 print_ip(source.sin_addr.s_addr);
                 printf(" Destination ip : ");
                 print_ip(dest.sin_addr.s_addr);
                 printf("\n");
             }
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
         (is_ip_equal((unsigned char *)inet_ntoa(dest.sin_addr), ETH0_IP) ||
         is_ip_equal((unsigned char *)inet_ntoa(dest.sin_addr), ETH1_IP) ||
         is_ip_equal((unsigned char *)inet_ntoa(dest.sin_addr), ETH2_IP) ) &&
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
