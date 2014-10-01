#include "print_packet.h"

#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h> //For standard things
#include<stdlib.h>    //malloc
#include<string.h>    //strlen

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

unsigned short csum(unsigned short *buf, int len)
{
        unsigned long sum;
        for(sum=0; len>0; len--)
                sum += *buf++;
        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
}

void print_data (unsigned char* data , int size)
{
    int i;
    for(i=0 ; i < size ; i++)
        fprintf(LOGFILE, "%02hhx ", data[i]);
    fprintf(LOGFILE, "\n");
}

void print_mac(const char *addr)
{
    int i;
    for (i = 0; i < ETH_ALEN - 1; i++)
        printf("%02hhx:", addr[i]);
    printf("%02hhx", addr[ETH_ALEN - 1]);
}

void print_ethernet_header(unsigned char* buffer, int size)
{
    struct ethhdr *eth = (struct ethhdr *)buffer;

    fprintf(LOGFILE , "\n");
    fprintf(LOGFILE , "Ethernet Header\n");
    fprintf(LOGFILE , "   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n",
            eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] ,
            eth->h_dest[4] , eth->h_dest[5] );
    fprintf(LOGFILE , "   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n",
            eth->h_source[0] , eth->h_source[1] , eth->h_source[2] ,
            eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    fprintf(LOGFILE , "   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
}

void print_ip_header(unsigned char* buffer, int size)
{
    struct sockaddr_in source, dest;
    print_ethernet_header(buffer , size);

    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(buffer  + sizeof(struct ethhdr) );
    iphdrlen =iph->ihl*4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    fprintf(LOGFILE , "\n");
    fprintf(LOGFILE , "IP Header\n");
    fprintf(LOGFILE , "   |-IP Version        : %d\n",(unsigned int)iph->version);
    fprintf(LOGFILE , "   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
    fprintf(LOGFILE , "   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
    fprintf(LOGFILE , "   |-IP Total Length   : %d  Bytes(size of Packet)\n",ntohs(iph->tot_len));
    fprintf(LOGFILE , "   |-Identification    : %d\n",ntohs(iph->id));
    //fprintf(LOGFILE , "   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
    //fprintf(LOGFILE , "   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
    //fprintf(LOGFILE , "   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
    fprintf(LOGFILE , "   |-TTL      : %d\n",(unsigned int)iph->ttl);
    fprintf(LOGFILE , "   |-Protocol : %d\n",(unsigned int)iph->protocol);
    fprintf(LOGFILE , "   |-Checksum : %d\n",ntohs(iph->check));
    fprintf(LOGFILE , "   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
    fprintf(LOGFILE , "   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));

    // Calculate the checksum
    unsigned short chk = csum((unsigned short*)buffer, sizeof(struct iphdr) + sizeof(struct udphdr));
    fprintf(LOGFILE , "   |-Calculated   : %d\n", chk);
}

void print_udp_packet(unsigned char *buffer , int size)
{
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(buffer +  sizeof(struct ethhdr));
    iphdrlen = iph->ihl*4;

    struct udphdr *udph = (struct udphdr*)(buffer + iphdrlen  + sizeof(struct ethhdr));

    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;

    fprintf(LOGFILE , "\n\n***********************UDP Packet*************************\n");

    print_ip_header(buffer,size);

    fprintf(LOGFILE , "\nUDP Header\n");
    fprintf(LOGFILE , "   |-Source Port      : %d\n" , ntohs(udph->source));
    fprintf(LOGFILE , "   |-Destination Port : %d\n" , ntohs(udph->dest));
    fprintf(LOGFILE , "   |-UDP Length       : %d\n" , ntohs(udph->len));
    fprintf(LOGFILE , "   |-UDP Checksum     : %d\n" , ntohs(udph->check));

    fprintf(LOGFILE , "\n");
    fprintf(LOGFILE , "IP Header\n");
    //print_data_detail(buffer , iphdrlen);
    //Added TODO
    print_data_detail(buffer+(sizeof (struct ethhdr)) , iphdrlen);
    //unsigned short chk_sum = csum_match((unsigned short*)buffer+(sizeof (struct ethhdr)), iphdrlen);
    print_data_detail(buffer+(sizeof (struct ethhdr)) , iphdrlen);

    fprintf(LOGFILE , "UDP Header\n");
    //print_data_detail(buffer+iphdrlen , sizeof udph);
    //Added TODO
    print_data_detail(buffer+iphdrlen+sizeof(struct ethhdr) , sizeof udph);

    //TODO Calculate checksum
    unsigned short chk = csum((unsigned short*)buffer+iphdrlen+(sizeof (struct ethhdr)), iphdrlen);
    fprintf(LOGFILE , "   |-MineCalculated   : %d\n", chk);

    fprintf(LOGFILE , "Data Payload\n");

    //Move the pointer ahead and reduce the size of string
    print_data_detail(buffer + header_size , size - header_size);

    fprintf(LOGFILE , "\n###########################################################");
}

void print_icmp_packet(unsigned char* buffer , int size)
{
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(buffer  + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;

    struct icmphdr *icmph = (struct icmphdr *)(buffer + iphdrlen  + sizeof(struct ethhdr));

    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;

    fprintf(LOGFILE , "\n\n***********************ICMP Packet*************************\n");

    print_ip_header(buffer , size);

    fprintf(LOGFILE , "\n");

    fprintf(LOGFILE , "ICMP Header\n");
    fprintf(LOGFILE , "   |-Type : %d",(unsigned int)(icmph->type));

    if((unsigned int)(icmph->type) == 11)
    {
        fprintf(LOGFILE , "  (TTL Expired)\n");
    }
    else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
    {
        fprintf(LOGFILE , "  (ICMP Echo Reply)\n");
    }

    fprintf(LOGFILE , "   |-Code : %d\n",(unsigned int)(icmph->code));
    fprintf(LOGFILE , "   |-Checksum : %d\n",ntohs(icmph->checksum));
    //fprintf(LOGFILE , "   |-ID       : %d\n",ntohs(icmph->id));
    //fprintf(LOGFILE , "   |-Sequence : %d\n",ntohs(icmph->sequence));
    fprintf(LOGFILE , "\n");

    fprintf(LOGFILE , "IP Header\n");
    print_data_detail(buffer+sizeof(struct ethhdr) ,iphdrlen);

    fprintf(LOGFILE , "ICMP Header\n");
    print_data_detail(buffer+sizeof(struct ethhdr) + iphdrlen , sizeof icmph);

    fprintf(LOGFILE , "Data Payload\n");

    //Move the pointer ahead and reduce the size of string
    print_data_detail(buffer + header_size , (size - header_size) );

    fprintf(LOGFILE , "\n###########################################################");
}

void print_data_detail (unsigned char* data , int size)
{
    int i , j;
    for(i=0 ; i < size ; i++)
    {
        if( i!=0 && i%16==0)   //if one line of hex printing is complete...
        {
            fprintf(LOGFILE , "         ");
            for(j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    fprintf(LOGFILE , "%c",(unsigned char)data[j]); //if its a number or alphabet

                else fprintf(LOGFILE , "."); //otherwise print a dot
            }
            fprintf(LOGFILE , "\n");
        }

        if(i%16==0) fprintf(LOGFILE , "   ");
            fprintf(LOGFILE , " %02X",(unsigned int)data[i]);

        if( i== size-1)  //print the last spaces
        {
            for(j=0;j<15-i%16;j++)
            {
              fprintf(LOGFILE , "   "); //extra spaces
            }

            fprintf(LOGFILE , "         ");

            for(j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                {
                  fprintf(LOGFILE , "%c",(unsigned char)data[j]);
                }
                else
                {
                  fprintf(LOGFILE , ".");
                }
            }

            fprintf(LOGFILE ,  "\n" );
        }
    }
}
