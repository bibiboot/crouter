#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <linux/if_ether.h>

#include "uthash.h"
#include "config.h"

#define PACKET_SIZE 1000
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
    int seq_num;
    UT_hash_handle hh;
} hashed_link;

struct node {
    // Its pointing to the start address of data
    char *mem_ptr;
};

struct globals {
    struct config config;
    // Hashmap
    hashed_link *hashl;
    int send_sock_fd;
    unsigned char src_mac[ETH_ALEN];
    unsigned char src_mac2[ETH_ALEN];
};

extern struct globals globals;
