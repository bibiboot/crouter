#pragma once

#include <pcap.h>

#include<netinet/ip_icmp.h>   //Provides declarations for icmp header
#include<netinet/udp.h>   //Provides declarations for udp header
#include<netinet/tcp.h>   //Provides declarations for tcp header
#include<netinet/ip.h>    //Provides declarations for ip header
#include<netinet/if_ether.h>  //For ETH_P_ALL
#include<net/ethernet.h>  //For ether_header

void print_payload(const u_char *payload, int len);

void print_icmp_packet(const u_char* Buffer , int Size);

void print_mac(const u_char *host);
