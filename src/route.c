#include "route.h"

/**
 * Network | Dest ip | Interface |
 * Key value pair system
 * Key   = Network
 * Value = Complete router entry
 */
void init_route_table() {


}

void lookup_router_table() {

}

void get_new_route(uint32_t dest_ip,
                   char *result_if_name, char *dest_mac,
                   char *src_mac){

    // MAC Address required
    // eth1, eth2, eth3, rtr1, rtr2, node3, node4

    // Get Network id
    uint32_t network_type1 = dest_ip & (globals.mask_255_255_255_0).s_addr;
    uint32_t network_type2 = dest_ip & (globals.mask_255_0_0_0).s_addr;

    char rtr1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
    char rtr2_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x08};
    char node3_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    char node4_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x06};

    /*
    printf("Network -1 :");
    print_ip(network_type1);
    printf("Network -2 :");
    print_ip(network_type2);
    printf("\n");
    */

    if (network_type1 == globals.sock_network_LAN1.s_addr) {

        strcpy(result_if_name, "inf000");
        // Check dest ip and choose mac address
        // TODO This works only for node4
        memcpy(dest_mac, node4_mac, 6);
        memcpy(src_mac, globals.eth1_mac, 6);

    } else if (network_type1 == globals.sock_network_rtr2.s_addr) {

        strcpy(result_if_name, "inf002");
        memcpy(dest_mac, rtr2_mac, 6);
        memcpy(src_mac, globals.eth3_mac, 6);

    } else if (network_type2 == globals.sock_network_LAN0.s_addr) {

        strcpy(result_if_name, "inf001");
        memcpy(dest_mac, rtr1_mac, 6);
        memcpy(src_mac, globals.eth2_mac, 6);

    } else {

        printf("No entry found : network ip:");
        print_ip(dest_ip);
        printf("\n");
    }
}
