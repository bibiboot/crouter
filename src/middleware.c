#include "middleware.h"
#include "packet_send.h"
#include "packet_update.h"
#include "print_packet.h"
#include "route.h"
#include "socket_util.h"
#include "route_table.h"
#include "util.h"
#include<netinet/udp.h>   //Provides declarations for udp header

/**
 * Based on the source ip,
 * it can tell us where it was recieved,
 * that is which interface
 */
void get_recived_interface(uint32_t dest_ip, char *res_interface) {
    // Ip of the sending router with RIP packet
    if ( dest_ip == char_to_uint32(RTR1_IP) ) {
       strcpy(res_interface, INF0);
    } else {
       printf("RIP: This should never happen, do not know the interface recoeved\n");
       exit(1);
    }
}

void incoming_packet_handler_rip(unsigned char *buffer, int data_size) {

    struct iphdr *iph = (struct iphdr *)(buffer +  sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl*4;
    struct rip *rph = (struct rip*)(buffer + sizeof(struct ethhdr) + iphdrlen + sizeof(struct udphdr));
    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof(struct udphdr);

    struct rip_netinfo *ni;
    int rip_packet_length = data_size - header_size;
    u_int i, j;
    j = rip_packet_length / sizeof(*ni);
    ni = (struct rip_netinfo *)(rph + 1);

    uint32_t network = ni->rip_dest;
    uint32_t next_hop = EXTRACT_32BITS(&ni->rip_router);;
    uint32_t mask = ni->rip_dest_mask;
    uint32_t metric = EXTRACT_32BITS(&ni->rip_metric);
    uint32_t source_ip = iph->saddr;

    char interface[IFNAMSIZ];
    get_recived_interface(iph->saddr, interface);

    // Print the details here

    // Update the routing table
    printf("\nRIP Packet Found\n");
    //print_udp_packet(buffer, data_size);

    update_or_add_entry(network, source_ip, next_hop, interface,
                        mask, metric);

    print_route_table();

    fflush(LOGFILE);
}

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

    // Routing
    get_new_route(iph->saddr, result_if_name, dest_mac, src_mac);

    print_routed_packet(dest, result_if_name, src_mac, dest_mac);

    int new_packet_size = get_resize_icmp_packet_time_exc_reply(packet,size);
    printf("Old size = %d and New size = %d\n", size, new_packet_size);

    unsigned char *new_packet = malloc(new_packet_size);

    //Copy the whole old packet to the new packet
    memcpy(new_packet, packet, size);

    // Reply back to the source mac address
    update_ethernet_packet_reply(new_packet);

    update_icmp_packet_time_exc_reply(new_packet, size);

    update_ip_packet_time_exc_reply(new_packet);

    send_packet_on_line(result_if_name, dest_mac, new_packet, new_packet_size);

    print_icmp_packet(new_packet, new_packet_size);

    free(new_packet);
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

    unsigned char dest_mac[6], src_mac[6];
    char result_if_name[IFNAMSIZ];
    struct sockaddr_in dest;
    struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    // Routing
    get_new_route(iph->saddr, result_if_name, dest_mac, src_mac);

    print_routed_packet(dest, result_if_name, src_mac, dest_mac);

    // Reply back to the source mac address
    update_ethernet_packet_reply(packet);

    update_ip_packet_reply(packet);

    update_icmp_packet_reply(packet, size);

    send_packet_on_line(result_if_name, dest_mac, packet, size);

    fflush(LOGFILE);
}

/**
 * Handle the incoming packet.
 * Either send or append to the queue.
 */
void incoming_packet_handler(unsigned char *packet, int size){
    print_udp_packet(packet, size);

    unsigned char dest_mac[6], src_mac[6];
    char result_if_name[IFNAMSIZ];
    struct sockaddr_in dest;
    struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    // Routing
    get_new_route(iph->daddr, result_if_name, dest_mac, src_mac);

    print_routed_packet(dest, result_if_name, src_mac, dest_mac);

    update_ip_packet(packet);

    update_ethernet_packet(packet, src_mac, dest_mac);

    send_packet_on_line(result_if_name, dest_mac, packet, size);

    print_udp_packet(packet, size);
}

/**
 * Prepare packet
 * Change ip and ethernet header
 * Send packet on line
 * Deprecated ********************************* Deprecated
 */
void prepare_n_send_packet(unsigned char *packet, int size,
                           unsigned char *dest_mac, char *if_name){

    update_ip_packet(packet);

    update_ethernet_packet(packet, globals.eth1_mac, dest_mac);

    send_packet_on_line(if_name, dest_mac, packet, size);
}
