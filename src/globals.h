#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <linux/if_ether.h>
#include<netinet/ip.h>    //Provides declarations for ip header

#include "uthash.h"
#include "config.h"

#define PACKET_SIZE 65536
// Print function name, filename and line number in print
#define DEBUG(fmt, ...) printf("%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__);
#define DBG(...) do{fprintf(stderr, "%s (%s, line %d): ", __func__, __FILE__, __LINE__); \
                         fprintf(stderr, __VA_ARGS__);           \
                         fprintf(stderr, "\n");} while(0)

#define RTR1_IP "10.10.1.1"
#define RTR2_IP "10.10.3.2"
#define NODE3_IP "10.1.2.3"
#define NODE4_IP "10.1.2.4"

#define LAN0_NETWORK "10.0.0.0"
#define LAN1_NETWORK "10.1.2.0"
#define RTR1_NETWORK "10.10.1.0"
#define RTR2_NETWORK "10.10.3.0"

#define INF0 "inf000"
#define INF1 "inf001"
#define INF2 "inf002"

#define DEF_MASK_255_255_255_0 "255.255.255.0"
#define DEF_MASK_255_0_0_0 "255.0.0.0"

FILE *LOGFILE;

// Hashmap data structure
typedef struct hashl {
    // Address of the node in list.
    uint32_t network;
    uint32_t next_hop;
    uint32_t mask;
    char interface[100];
    UT_hash_handle hh;
} router_entry;

// Hashmap data structure
typedef struct arp_hashl {
    uint32_t ip;
    struct in_addr *sock_ip;
    char *mac_addr;
    UT_hash_handle hh;
} arp_entry;

struct globals {
    struct config config;
    // Hashmap
    router_entry *rentry;
    arp_entry *aentry;
    int send_sock_fd;
    unsigned char eth1_mac[ETH_ALEN];
    unsigned char eth2_mac[ETH_ALEN];
    unsigned char eth3_mac[ETH_ALEN];
    struct in_addr mask_255_255_255_0;
    struct in_addr mask_255_0_0_0;
    struct in_addr sock_network_LAN0;
    struct in_addr sock_network_LAN1;
    struct in_addr sock_network_rtr1;
    struct in_addr sock_network_rtr2;
};

extern struct globals globals;
