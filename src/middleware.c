#include "middleware.h"
#include "packet_send.h"
#include "packet_update.h"
#include "print_packet.h"
#include "socket_util.h"
#include "route.h"
#include "util.h"

#include<netinet/ip.h>    //Provides declarations for ip header

void incoming_packet_handler_ttl_zero(unsigned char *packet, int size) {
    print_icmp_packet(packet, size);
    unsigned char dest_mac[6], src_mac[6];
    char result_if_name[IFNAMSIZ];
    struct sockaddr_in dest;
    struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));

    if ( iph->protocol != 1 ) {
        printf("WARN : NOT HANDLED TTL FOR NON ICMP\n");
    }

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

    // Reply back to the source mac address
    update_ethernet_packet_reply(packet);

    update_icmp_packet_reply(packet, size);

    update_ip_packet_reply(packet);


    send_packet_on_line(result_if_name, dest_mac, packet);

    print_icmp_packet(packet, size);
    fflush(LOGFILE);
}

/**
 * Handle the incoming packet whose ttl = 0.
 * Update the ethernet header with corrrect mac address
 *     source mac address
 *     destination mac address
 * Update the ip header with correct ip address
 *     source ip address
 *     destination ip address
 *     ttl = 64
 *     checksum
 * Update the icmp header
 *     change type to 18
 *     checksum
 */
void incoming_packet_handler_self_icmp(unsigned char *packet, int size){
    print_icmp_packet(packet, size);

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

    // Reply back to the source mac address
    update_ethernet_packet_reply(packet);

    update_ip_packet_reply(packet);

    update_icmp_packet_reply(packet, size);

    send_packet_on_line(result_if_name, dest_mac, packet);

    print_icmp_packet(packet, size);
    fflush(LOGFILE);
}

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
