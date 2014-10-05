#pragma once

#include "globals.h"
#include <linux/if_ether.h>
#include <sys/socket.h>
#include <linux/if_arp.h>

void create_socket_address(struct sockaddr_ll *socket_address, int src_index, unsigned char *dest_mac);
