#include "filter.h"
#include "util.h"
#include "print_packet.h"
#include "route.h"

/**
 *
 */
bool is_allowed(unsigned char *packet, int data_size){
    struct sockaddr_in source, dest;
    struct ethhdr *eth = (struct ethhdr *)packet;
    struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    // 1 : ICMP
    // 6 : TCP
    // 17: UDP
    if (iph->protocol ==  17 || iph->protocol == 1 || iph->protocol == 6) {
        //fprintf(LOGFILE , "   |-TTL      : %d\n",(unsigned int)iph->ttl);
        //fprintf(LOGFILE , "   |-Protocol : %d\n",(unsigned int)iph->protocol);
        //fprintf(LOGFILE , "   |-Source IP        : %s\n",
         //   (unsigned char *)inet_ntoa(source.sin_addr));
        //fprintf(LOGFILE , "   |-Destination IP   : %s\n",
          //  (unsigned char*)inet_ntoa(dest.sin_addr));
        if (is_mac_addr_equal(eth->h_dest, globals.src_mac) ||
            is_mac_addr_equal(eth->h_dest, globals.src_mac2)){
            //print_icmp_packet(packet, data_size);
            return true;
        }
    }
    return false;
}
