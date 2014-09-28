#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <pthread.h>

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

typedef long long unsigned int vlong;

// Hashmap data structure
typedef struct hashl {
    // Sequence number
    vlong seq_num;
    // Address of the node in list.
    UT_hash_handle hh;
} hashed_link;

// Data list is stored as node below
// Both nack and data list have same type of node
struct node {
    vlong seq_num;
    // Its pointing to the start address of data
    char *mem_ptr;
    // Size in bits
    vlong size;
};

struct globals {
    struct config config;
    // Hashmap
    hashed_link *hashl;
};

extern struct globals globals;

unsigned int time_diff_micro(struct timeval end, struct timeval start);
