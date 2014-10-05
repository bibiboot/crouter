#include "arp.h"
#include <linux/if_ether.h>

void print_arp_cache_table_list(uint32_t *lan_node, int size) {

    int i;
    char res_mac_addr[ETH_ALEN];
    memset(res_mac_addr, 0, ETH_ALEN);
    printf("Arp Table");
    printf("\n++++++++++++++++++++++++++++++++");

    for (i = 0; i < size; i++) {
        get_mac_from_arp_cache(lan_node[i], res_mac_addr);
        print_ip(lan_node[i]);
        printf(" | ");
        print_mac(res_mac_addr);
        printf("\n++++++++++++++++++++++++++++++++");

    }
    printf("\n");
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


/**
 * Fill entries in the hash table.
 * Key = ip and value = structure
 */
void init_build_arp_cache_dynamic() {
    unsigned char rtr1_ip[] = RTR1_IP;
    unsigned char node3_ip[] = NODE3_IP;

    unsigned char rtr1_mac[6] = {0x00, 0x04, 0x23, 0xc5, 0xe1, 0x29};
    unsigned char node3_mac[6] = {0x00, 0x04, 0x23, 0xc7, 0xa7, 0x7c};

    create_n_add_entry(rtr1_ip, rtr1_mac);
    create_n_add_entry(node3_ip, node3_mac);
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
    printf("\n");
}
