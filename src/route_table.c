#include "route_table.h"

#define DEF_MASK_255_255_255_0 "255.255.255.0"
#define DEF_MASK_255_0_0_0 "255.0.0.0"

#define LAN0_NETWORK "10.0.0.0"
#define LAN1_NETWORK "10.1.2.0"
#define RTR1_NETWORK "10.10.1.0"
#define RTR2_NETWORK "10.10.3.0"

uint32_t get_route_entry_print(uint32_t network, char *interface, 
                               uint32_t *mask, uint32_t *metric) {

    router_entry *rentry = (router_entry*)find_entry(network);
    if (rentry == NULL) {
        printf("ROUTER: This should never happen\n");
        exit(1);
    }

    *mask = rentry->mask;
    *metric = rentry->metric;
    strcpy(interface, rentry->interface);
    return rentry->next_hop;
}

uint32_t get_route_entry_rip(uint32_t network, char *interface, 
                             uint32_t *mask, uint32_t *metric) {

    router_entry *rentry = (router_entry*)find_entry(network);
    if (rentry == NULL) {
        printf("ROUTER: This should never happen\n");
        exit(1);
    }

    *mask = rentry->mask;
    *metric = rentry->metric;
    strcpy(interface, rentry->interface);
    return rentry->next_hop;
}

bool get_route_entry(uint32_t network, uint32_t dest_ip,
                     char *interface, uint32_t *mask, uint32_t *next_hop,
                     uint32_t *metric) {

    router_entry *rentry = (router_entry*)find_entry(network);
    if (rentry == NULL) {
        printf("ROUTER: This should never happen\n");
        exit(1);
    }

    if ( (dest_ip & rentry->mask) != rentry->network ) {
        return false;
    }
    /*
    uint32_t result_and = dest_ip & rentry->mask;
    printf("\n ANDED = ");
    print_ip(result_and);

    printf("\n Debug route: Dest ip:  ");
    print_ip(dest_ip);
    printf("  Network ip : ");
    print_ip(network);
    printf("\n");
    */

    *mask = rentry->mask;
    *next_hop = rentry->next_hop;
    *metric = rentry->metric;
    strcpy(interface, rentry->interface);

    return true;
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
    printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    char res_interface[100];
    uint32_t res_mask, metric;

    int i;
    for (i = 0; i < globals.rtable_size; i++) {
        memset(res_interface, 0, 100);
        uint32_t network_ip = globals.rtable_keys[i];
        uint32_t next_hop = get_route_entry_print(network_ip,
                                                  res_interface, &res_mask,
                                                  &metric);
        print_ip(network_ip);
        printf("  | %s |", res_interface);
        print_ip(next_hop);
        printf(" | ");
        print_ip(res_mask);
        printf(" | ");
        printf( " %d ", metric);
        printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    }
}

void add_entry_char(char *network, char *next_hop,
                    char *interface, char *mask) {
    /* Create entry */
    router_entry *r_node = malloc(sizeof(router_entry));
    r_node->network = char_to_uint32(network);
    r_node->next_hop = char_to_uint32(next_hop);
    r_node->mask = char_to_uint32(mask);
    strcpy(r_node->interface, interface);
    /* Add entry */
    add_entry(r_node);
}

void add_entry_uint(uint32_t network, char *next_hop,
                    char *interface, char *mask, uint32_t metric) {

    /* Create entry */
    router_entry *r_node = malloc(sizeof(router_entry));
    r_node->network = network;
    r_node->next_hop = char_to_uint32(next_hop);
    r_node->mask = char_to_uint32(mask);
    r_node->metric = metric;
    strcpy(r_node->interface, interface);
    /* Add entry */
    add_entry(r_node);

    globals.rtable_keys[globals.rtable_size] = network;
    globals.rtable_size++;

}

void add_entry_rip(uint32_t network, uint32_t next_hop,
                    char *interface, uint32_t mask, uint32_t metric) {
    printf("\n Debug route: Mask:  ");
    print_ip(mask);
    printf("  Network ip : ");
    print_ip(network);
    printf("\n");

    /* Create entry */
    router_entry *r_node = malloc(sizeof(router_entry));
    r_node->network = network;
    r_node->next_hop = next_hop;
    r_node->mask = mask;;
    strcpy(r_node->interface, interface);
    /* Add entry */
    add_entry(r_node);

    globals.rtable_keys[globals.rtable_size] = network;
    globals.rtable_size++;
}

/**
 * This will add or update the entry in the routing table
 * as per the RIP entry recieved.
 */
void update_or_add_entry(uint32_t network, uint32_t source_ip, uint32_t next_hop,
                         char *interface, uint32_t mask, uint32_t metric) {
    /** Look for entry in the table
     *  If the entry is not their, then add
     *  entry otherwise compare and create
     */
    router_entry *rentry = (router_entry*)find_entry(network);
    if( rentry == NULL ) {
        printf("RIP: Adding new entry in the routing table\n");
        add_entry_rip(network,source_ip , interface, mask, metric);
    }
}

void init_network_id() {
    char *network_LAN0 = LAN0_NETWORK;
    char *network_LAN1 = LAN1_NETWORK;
    char *network_rtr1 = RTR1_NETWORK;
    char *network_rtr2 = RTR2_NETWORK;

    memset(&globals.sock_network_LAN0, 0, sizeof(struct in_addr));
    memset(&globals.sock_network_LAN1, 0, sizeof(struct in_addr));
    memset(&globals.sock_network_rtr1, 0, sizeof(struct in_addr));
    memset(&globals.sock_network_rtr2, 0, sizeof(struct in_addr));

    inet_aton(network_LAN0, &(globals.sock_network_LAN0));
    inet_aton(network_LAN1, &(globals.sock_network_LAN1));
    inet_aton(network_rtr1, &(globals.sock_network_rtr1));
    inet_aton(network_rtr2, &(globals.sock_network_rtr2));
}

/**
 * Network | Next Hop | Interface
 *  int         int       string
 */
void init_build_route_table(){
    init_network_id();

    add_entry_uint(globals.sock_network_LAN1.s_addr, "0", INF0, DEF_MASK_255_255_255_0, 1);
    add_entry_uint(globals.sock_network_rtr1.s_addr, "0", INF1, DEF_MASK_255_255_255_0, 1);
    add_entry_uint(globals.sock_network_rtr2.s_addr, "0", INF2, DEF_MASK_255_255_255_0, 1);
    add_entry_uint(globals.sock_network_LAN0.s_addr, RTR1_IP, INF1, DEF_MASK_255_0_0_0, 1);
}

void init_build_route_table_dynamic(){

    add_entry_uint(char_to_uint32("10.1.1.0"), "0", INF1, DEF_MASK_255_255_255_0, 1);
}
