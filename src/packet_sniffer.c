#include "packet_sniffer.h"

int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j;

void sniff()
{
    int saddr_size , data_size;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *) malloc(65536); //Its Big!
    memset(buffer, '\0', 65536);

    printf("Starting...\n");

    int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;

    if (sock_raw < 0) {
        perror("Socket Error");
        return;
    }

    while(1) {
        saddr_size = sizeof saddr;
        // Receive a packet
        data_size = recvfrom(sock_raw , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_size);
        if(data_size <0 )
        {
            printf("Recvfrom error , failed to get packets\n");
            return ;
        }

        // Filter. Currently allowing all.
        if (!is_allowed(buffer, data_size)) {
            continue;
        }

        if( is_rip_packet(buffer, data_size) ) {
            // RIP Handler
            incoming_packet_handler_rip(buffer, data_size);
            continue;
        }

        /**
         * Handle Time Exceeded ICMP case
         * Handle ping to the user router
         * Handle packet forwarding
         */
        if ( is_ttl_zero(buffer, data_size) ) {
            incoming_packet_handler_ttl_zero(buffer, data_size);
        } else if ( is_packet_reply(buffer, data_size) ){
            incoming_packet_handler_self_icmp(buffer, data_size);
        } else {
            incoming_packet_handler(buffer, data_size);
        }

        /* Track count of the packet type */
        int status = process_packet(buffer , data_size);

        //TODO Check if needed
        memset(buffer, '\0', 65536);
        fflush(LOGFILE);
    }

    close(sock_raw);
}

int process_packet(unsigned char* buffer, int size)
{
    struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    ++total;
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
    //fprintf(stdout, "TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r",
     //       tcp , udp , icmp , igmp , others , total);

    //fflush(stdout);
    //fflush(LOGFILE);
    return 1;
}
