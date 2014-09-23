#pragma once

#define CONFIG_INIT { \
    .packet_size = 1400, \
    .a_recv_port = 3005, \
    .b_recv_port = 2005, \
    .a_main_send_port = 4005, \
    .b_main_recv_port = 5005, \
    .queue_len = 10000000, \
    .read_buffer_size = 1500,\
    .nack_timer = 1,}

struct config {
    // Port to recieve data at NodeA
    uint16_t a_recv_port;
    uint16_t a_main_send_port;
    uint16_t b_main_recv_port;
    // Port to recieve data at NodeB
    uint16_t b_recv_port;
    // Packet size to be send
    long long unsigned int packet_size;
    // Queue length at the listen socket
    long long unsigned int queue_len;
    long long unsigned int read_buffer_size;;
    int nack_timer;
};


