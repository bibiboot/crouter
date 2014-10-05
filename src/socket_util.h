#pragma once

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>

int interface_addr(int sock, char *ifname, unsigned char *addr);

int interface_index(int sock, char *ifname);

int get_socket();
