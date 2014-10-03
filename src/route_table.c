#include "route_table.h"

uint32_t char_to_uint32(char *network) {
    struct in_addr sock_network;
    memset(&sock_network, 0, sizeof(struct in_addr));

    inet_aton(network, &sock_network);
    return sock_network.s_addr;
}

uint32_t get_route_entry(uint32_t network, char *interface) {
    router_entry *rentry = (router_entry*)find_entry(network);
    if (rentry == NULL) {
        printf("ROUTER: This should never happen\n");
        exit(1);
    }

    strcpy(interface, rentry->interface);
    return rentry->next_hop;
}
/**
 * Print the complete routing table
 *
 * ++++++++++++++++++++++++++++++++++++++++++++++
 * | Network  |   Next-hop |  Interface |       |
 * ++++++++++++++++++++++++++++++++++++++++++++++
 * | 10.1.2.0 |      *     |   inf000   | LAN 1 |
 * ++++++++++++++++++++++++++++++++++++++++++++++
 * | 10.10.3.0|      *     |   inf002   | rtr2  |
 * ++++++++++++++++++++++++++++++++++++++++++++++
 * | 10.10.1.0|      *     |   inf001   | rtr1  |
 * ++++++++++++++++++++++++++++++++++++++++++++++
 * | 10.0.0.0 | 10.10.1.1  |   inf001   | LAN 0 |
 * ++++++++++++++++++++++++++++++++++++++++++++++
 */
void print_route_table() {
    printf("Route table");
    printf("\n++++++++++++++++++++++++++++++++++++++++++++\n");
    char res_interface[100];
    uint32_t next_hop = get_route_entry(globals.sock_network_LAN0.s_addr,
                                        res_interface);
    printf(" LAN0 | ");
    print_ip(globals.sock_network_LAN0.s_addr);
    printf("  | %s |", res_interface);
    print_ip(next_hop);
    printf("\n++++++++++++++++++++++++++++++++++++++++++++\n");

    memset(res_interface, 0, 100);
    next_hop = get_route_entry(globals.sock_network_LAN1.s_addr,
                               res_interface);
    printf(" LAN1 | ");
    print_ip(globals.sock_network_LAN1.s_addr);
    printf("  | %s |", res_interface);
    print_ip(next_hop);
    printf("\n++++++++++++++++++++++++++++++++++++++++++++\n");

    memset(res_interface, 0, 100);
    next_hop = get_route_entry(globals.sock_network_rtr1.s_addr,
                               res_interface);
    printf(" rtr1 | ");
    print_ip(globals.sock_network_rtr1.s_addr);
    printf(" | %s |", res_interface);
    print_ip(next_hop);
    printf("\n++++++++++++++++++++++++++++++++++++++++++++\n");

    memset(res_interface, 0, 100);
    next_hop = get_route_entry(globals.sock_network_rtr2.s_addr,
                               res_interface);
    printf(" rtr2 | ");
    print_ip(globals.sock_network_rtr2.s_addr);
    printf(" | %s |", res_interface);
    print_ip(next_hop);
    printf("\n++++++++++++++++++++++++++++++++++++++++++++\n");
}

void add_entry_char(char *network, char *next_hop,
                    char *interface) {
    /* Create entry */
    router_entry *r_node = malloc(sizeof(router_entry));
    r_node->network = char_to_uint32(network);
    r_node->next_hop = char_to_uint32(next_hop);
    strcpy(r_node->interface, interface);
    /* Add entry */
    add_entry(r_node);
}

void add_entry_uint(uint32_t network, char *next_hop,
                    char *interface) {

    /* Create entry */
    router_entry *r_node = malloc(sizeof(router_entry));
    r_node->network = network;
    r_node->next_hop = char_to_uint32(next_hop);
    strcpy(r_node->interface, interface);
    /* Add entry */
    add_entry(r_node);
}

/**
 * Network | Next Hop | Interface
 *  int         int       string
 */
void init_build_route_table(){
    add_entry_uint(globals.sock_network_LAN0.s_addr, RTR1_IP, INF1);
    add_entry_uint(globals.sock_network_LAN1.s_addr, "0", INF0);
    add_entry_uint(globals.sock_network_rtr1.s_addr, "0", INF1);
    add_entry_uint(globals.sock_network_rtr2.s_addr, "0", INF2);
}
