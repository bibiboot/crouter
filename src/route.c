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

bool route_logic(uint32_t network_ip, uint32_t dest_ip, char *src_mac,
                     char *dest_mac, char *result_if_name ){
        uint32_t res_mask;
        uint32_t next_hop;
       /* Routing table */
       bool is_matched = get_route_entry(network_ip, dest_ip, result_if_name, &res_mask, &next_hop);

       if ( !is_matched ) {
           return false;
        }

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
        return true;
}

void get_new_route(uint32_t dest_ip,
                   char *result_if_name, char *dest_mac,
                   char *src_mac){

    int i;
    for ( i = 0; i < globals.rtable_size; i++ ) {
        if ( route_logic(globals.rtable_keys[i], dest_ip,
                         src_mac, dest_mac, result_if_name) ) {
            return;
        }
    }

    printf("No entry found : network ip:");
    print_ip(dest_ip);
    printf("\n");
    return ;
}
