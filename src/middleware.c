#include "middleware.h"
#include "packet_send.h"
#include "packet_update.h"
#include "socket_util.h"
#include "route.h"
#include "util.h"

#include<netinet/ip.h>    //Provides declarations for ip header

/**
 * Handle the incoming packet.
 * Either send or append to the queue.
 */
void incoming_packet_handler(unsigned char *packet, int size){
    unsigned char dest_mac[6], src_mac[6];
    char result_if_name[IFNAMSIZ];
    struct sockaddr_in dest;
    struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    // Get this information from routing
    get_route((unsigned char *)inet_ntoa(dest.sin_addr), result_if_name, dest_mac, src_mac);
    printf("Dest ip: %s, send interface: %s, Dest MAC: ",
           (unsigned char *)inet_ntoa(dest.sin_addr),
           result_if_name);
    print_mac(dest_mac);
    printf(" Src MAC: ");
    print_mac(src_mac);
    printf("\n");

    update_ip_packet(packet);

    update_ethernet_packet(packet, src_mac, dest_mac);

    send_packet_on_line(result_if_name, dest_mac, packet);
}

/**
 * Prepare packet
 * Change ip and ethernet header
 * Send packet on line
 */
void prepare_n_send_packet(unsigned char *packet, int size,
                           unsigned char *dest_mac, char *if_name){

    update_ip_packet(packet);

    update_ethernet_packet(packet, globals.src_mac, dest_mac);

    send_packet_on_line(if_name, dest_mac, packet);
}
