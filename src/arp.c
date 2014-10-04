#include "arp.h"
#include <linux/if_ether.h>

void print_arp_cache_table() {

    char res_mac_addr[ETH_ALEN];
    memset(res_mac_addr, 0, ETH_ALEN);
    printf("Arp Table");
    printf("\n+++++++++++++++++++++++++++++++++++++++");
    get_mac_from_arp_cache_print(RTR1_IP, res_mac_addr);
    printf(" %s | ", RTR1_IP);
    print_mac(res_mac_addr);
    printf(" | RTR1 ");
    printf("\n+++++++++++++++++++++++++++++++++++++++");

    memset(res_mac_addr, 0, ETH_ALEN);
    get_mac_from_arp_cache_print(RTR2_IP, res_mac_addr);
    printf(" %s  | ", RTR2_IP);
    print_mac(res_mac_addr);
    printf(" | RTR2 ");
    printf("\n+++++++++++++++++++++++++++++++++++++++");

    memset(res_mac_addr, 0, ETH_ALEN);
    get_mac_from_arp_cache_print(NODE3_IP, res_mac_addr);
    printf(" %s  | ", NODE3_IP);
    print_mac(res_mac_addr);
    printf(" | NODE3 ");
    printf("\n+++++++++++++++++++++++++++++++++++++++");

    memset(res_mac_addr, 0, ETH_ALEN);
    get_mac_from_arp_cache_print(NODE4_IP, res_mac_addr);
    printf(" %s  | ", NODE4_IP);
    print_mac(res_mac_addr);
    printf(" | NODE4 ");
    printf("\n+++++++++++++++++++++++++++++++++++++++\n");

}

void create_n_add_entry(unsigned char *ip, char *mac_addr) {

    //printf("Adding %s ---> ", ip);
    //print_mac(mac_addr);
    //printf("\n");

    struct in_addr *sock_ip = malloc(sizeof(struct in_addr));
    inet_aton(ip, sock_ip);

    arp_entry *a_node = malloc(sizeof(arp_entry));
    unsigned char *mac_addr_store = malloc(ETH_ALEN);
    memcpy(mac_addr_store, mac_addr, ETH_ALEN);

    a_node->ip = sock_ip->s_addr;
    a_node->sock_ip = sock_ip;
    a_node->mac_addr = mac_addr_store;

    add_arp_entry(a_node);

}

/**
 * Fill entries in the hash table.
 * Key = ip and value = structure
 */
void init_build_arp_cache() {
    unsigned char rtr1_ip[] = RTR1_IP;
    unsigned char rtr2_ip[] = RTR2_IP;
    unsigned char node3_ip[] = NODE3_IP;
    unsigned char node4_ip[] = NODE4_IP;

    unsigned char rtr1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
    unsigned char rtr2_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x08};
    unsigned char node3_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    unsigned char node4_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x06};

    create_n_add_entry(rtr1_ip, rtr1_mac);
    create_n_add_entry(rtr2_ip, rtr2_mac);
    create_n_add_entry(node3_ip, node3_mac);
    create_n_add_entry(node4_ip, node4_mac);
}

void get_mac_from_arp_cache(uint32_t ip, char *mac_addr) {

    arp_entry *a_node = (arp_entry*)(find_arp_entry(ip));
    if (!a_node) {
        printf("\nARP: This should never happen. IP : ");
        print_ip(ip);
        printf(" not found in arp cache\n");
        exit(1);
    }
    memcpy(mac_addr, a_node->mac_addr, ETH_ALEN);
    //print_mac(a_node->mac_addr);
    printf("\n");
}

void get_mac_from_arp_cache_print(unsigned char *ip, char *mac_addr) {

    struct in_addr sock_ip;
    memset(&sock_ip, 0, sizeof(struct in_addr));
    inet_aton(ip, &sock_ip);

    arp_entry *a_node = (arp_entry*)(find_arp_entry(sock_ip.s_addr));
    if (!a_node) {
        printf("ARP: This should never happen\n");
        exit(1);
    }
    memcpy(mac_addr, a_node->mac_addr, ETH_ALEN);
    //print_mac(a_node->mac_addr);
    printf("\n");
}
