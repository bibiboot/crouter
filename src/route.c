#include "arp.h"
#include "route.h"
#include "route_table.h"

void set_src_mac_address(char *if_name, char *src_mac) {
    if (strcmp(if_name, INF0) == 0) {
        memcpy(src_mac, globals.eth1_mac, 6);
    } else if (strcmp(if_name, INF1) == 0) {
        memcpy(src_mac, globals.eth1_mac, 6);
    } else if (strcmp(if_name, INF2) == 0 ) {
        memcpy(src_mac, globals.eth1_mac, 6);
    } else {
        printf("Interface: This shoukd not happen\n");
    }
}

void get_new_route(uint32_t dest_ip,
                   char *result_if_name, char *dest_mac,
                   char *src_mac){

    // Get Network id
    uint32_t network_type1 = dest_ip & (globals.mask_255_255_255_0).s_addr;
    uint32_t network_type2 = dest_ip & (globals.mask_255_0_0_0).s_addr;

    char rtr1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
    char rtr2_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x08};
    char node3_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    char node4_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x06};

    if (network_type1 == globals.sock_network_LAN1.s_addr) {

        /* Routing table */
        uint32_t next_hop = get_route_entry(globals.sock_network_LAN1.s_addr, result_if_name);

        /**
         * Destination mac address
         * If the next hop is zero then use dest ip
         * for mac
         * Else use mac address of the next hop
         */
        if (next_hop != 0 ) {
            get_mac_from_arp_cache(next_hop, dest_mac);
        } else {
            get_mac_from_arp_cache(dest_ip, dest_mac);
        }

        // Get mac address of inteface recieved from router table
        set_src_mac_address(result_if_name, src_mac);

        //memcpy(src_mac, globals.eth1_mac, 6);
        //strcpy(result_if_name, "inf001");
        //memcpy(dest_mac, node4_mac, 6);

    } else if (network_type1 == globals.sock_network_rtr2.s_addr) {

        /* Routing table */
        uint32_t next_hop = get_route_entry(globals.sock_network_rtr2.s_addr, result_if_name);

        /**
         * Destination mac address
         * If the next hop is zero then use dest ip
         * for mac
         * Else use mac address of the next hop
         */
        if (next_hop != 0 ) {
            get_mac_from_arp_cache(next_hop, dest_mac);
        } else {
            get_mac_from_arp_cache(dest_ip, dest_mac);
        }

        // Get mac address of inteface recieved from router table
        set_src_mac_address(result_if_name, src_mac);

        //strcpy(result_if_name, "inf002");
        //memcpy(dest_mac, rtr2_mac, 6);
        //memcpy(src_mac, globals.eth3_mac, 6);

    } else if (network_type1 == globals.sock_network_rtr1.s_addr) {

        /* Routing table */
        uint32_t next_hop = get_route_entry(globals.sock_network_rtr1.s_addr, result_if_name);

        /**
         * Destination mac address
         * If the next hop is zero then use dest ip
         * for mac
         * Else use mac address of the next hop
         */
        if (next_hop != 0 ) {
            get_mac_from_arp_cache(next_hop, dest_mac);
        } else {
            get_mac_from_arp_cache(dest_ip, dest_mac);
        }

        // Get mac address of inteface recieved from router table
        set_src_mac_address(result_if_name, src_mac);

        //strcpy(result_if_name, "inf002");
        //memcpy(dest_mac, rtr2_mac, 6);
        //memcpy(src_mac, globals.eth3_mac, 6);

    } else if (network_type2 == globals.sock_network_LAN0.s_addr) {

        /* Routing table */
        uint32_t next_hop = get_route_entry(globals.sock_network_LAN0.s_addr, result_if_name);

        /**
         * Destination mac address
         * If the next hop is zero then use dest ip
         * for mac
         * Else use mac address of the next hop
         */
        if (next_hop != 0 ) {
            get_mac_from_arp_cache(next_hop, dest_mac);
        } else {
            get_mac_from_arp_cache(dest_ip, dest_mac);
        }

        // Get mac address of inteface recieved from router table
        set_src_mac_address(result_if_name, src_mac);

        //strcpy(result_if_name, "inf001");
        //memcpy(dest_mac, rtr1_mac, 6);
        //memcpy(src_mac, globals.eth2_mac, 6);

    } else {

        printf("No entry found : network ip:");
        print_ip(dest_ip);
        printf("\n");
    }
}
