#include "filter.h"
#include "middleware.h"
#include "packet_sniffer.h"

#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>

#include<netinet/ip_icmp.h>   //Provides declarations for icmp header
#include<netinet/udp.h>   //Provides declarations for udp header
#include<netinet/tcp.h>   //Provides declarations for tcp header
#include<netinet/ip.h>    //Provides declarations for ip header
#include<netinet/if_ether.h>  //For ETH_P_ALL
#include<net/ethernet.h>  //For ether_header
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

struct sockaddr_in source,dest;
int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j;

int sniff()
{
    int saddr_size , data_size;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *) malloc(65536); //Its Big!

    printf("Starting...\n");

    int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;
    //setsockopt(sock_raw , SOL_SOCKET , SO_BINDTODEVICE , "eth0" , strlen("eth0")+ 1 );

    if(sock_raw < 0)
    {
        //Print the error with proper message
        perror("Socket Error");
        return 1;
    }
    while(1)
    {
        saddr_size = sizeof saddr;
        //Receive a packet
        data_size = recvfrom(sock_raw , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_size);
        if(data_size <0 )
        {
            printf("Recvfrom error , failed to get packets\n");
            return 1;
        }

        // Filter. Currently allowing all.
        if (!is_allowed(buffer)) {
            continue;
        }

        incoming_packet_handler(buffer, data_size);
        // TODO: Not needed. Remove later.
        // Now process the packet
        int status = process_packet(buffer , data_size);

        // Add to queue after routing
        memset(buffer, '\0', 65536);
    }
    close(sock_raw);
    printf("Finished");
    return data_size;
}

int process_packet(unsigned char* buffer, int size)
{
    //Get the IP Header part of this packet , excluding the ethernet header
    struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    ++total;
    //Check the Protocol and do accordingly...
    switch (iph->protocol)
    {
        case 1:  //ICMP Protocol
            ++icmp;
            break;

        case 2:  //IGMP Protocol
            ++igmp;
            break;

        case 6:  //TCP Protocol
            ++tcp;
            break;

        case 17: //UDP Protocol
            ++udp;
            return 0;
            break;

        default: //Some Other Protocol like ARP etc.
            ++others;
            break;
    }
    fprintf(stdout, "TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r", tcp , udp , icmp , igmp , others , total);
    fflush(stdout);
    return 1;
}
