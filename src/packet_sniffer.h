#pragma once

#include "globals.h"
#include "filter.h"
#include "middleware.h"

#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>

#include<netinet/ip_icmp.h>   //Declarations for icmp header
//#include<netinet/udp.h>   //Declarations for udp header
//#include<netinet/tcp.h>   //Declarations for tcp header
//#include<netinet/ip.h>    //Provides declarations for ip header
//#include<netinet/if_ether.h>  //For ETH_P_ALL
//#include<net/ethernet.h>  //For ether_header
//#include<sys/socket.h>
//#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

void* sniff(void *val);

int process_packet(unsigned char* packet, int size);
