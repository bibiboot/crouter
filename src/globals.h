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

//#define PACKET_SIZE 1000
#define PACKET_SIZE 65536
//#define LOGFILE stdout
// Print function name, filename and line number in print
#define DEBUG(fmt, ...) printf("%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__);
#define DBG(...) do{fprintf(stderr, "%s (%s, line %d): ", __func__, __FILE__, __LINE__); \
                         fprintf(stderr, __VA_ARGS__);           \
                         fprintf(stderr, "\n");} while(0)

FILE *LOGFILE;

// Hashmap data structure
typedef struct hashl {
    // Address of the node in list.
    uint32_t network;
    uint32_t dest_ip;
    char *interface;
    UT_hash_handle hh;
} router_entry;

struct globals {
    struct config config;
    // Hashmap
    router_entry *rentry;
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
