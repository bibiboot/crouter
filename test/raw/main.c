#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define PCKT_LEN 8192



struct ipheader {
     unsigned char      iph_ihl:5, iph_ver:4;
     unsigned char      iph_tos;
     unsigned short int iph_len;
     unsigned short int iph_ident;
     unsigned char      iph_flag;
     unsigned short int iph_offset;
     unsigned char      iph_ttl;
     unsigned char      iph_protocol;
     unsigned short int iph_chksum;
     unsigned int       iph_sourceip;
     unsigned int       iph_destip;
};

struct udpheader {
     unsigned short int udph_srcport;
     unsigned short int udph_destport;
     unsigned short int udph_len;
     unsigned short int udph_chksum;
};

// Simple checksum function, may use others such as Cyclic Redundancy Check, CRC
unsigned short csum(unsigned short *buf, int len)
{
        unsigned long sum;
        for(sum=0; len>0; len--)
                sum += *buf++;
        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
}


void create_udp_packet(char **buffer, int *sd, struct sockaddr_in *sin,
                       char *src_port, char *dest_port,
                       char *src_ip, char *dest_ip)
{
    *buffer = malloc(PCKT_LEN);

    struct ipheader *ip = (struct ipheader *) *buffer;
    struct udpheader *udp = (struct udpheader *) (*buffer + sizeof(struct ipheader));
    struct sockaddr_in din;

    memset(*buffer, 0, PCKT_LEN);

    *sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
    if (*sd < 0) {
        perror("Socket() error");
    }
    else {
        printf("Scoket() - USING SOCK_RAW socket and UDP\n");
    }

    sin->sin_family = AF_INET;
    din.sin_family = AF_INET;

    sin->sin_port = htons(atoi(src_port));
    din.sin_port = htons(atoi(dest_port));

    sin->sin_addr.s_addr = inet_addr(src_ip);
    din.sin_addr.s_addr = inet_addr(dest_ip);

    ip->iph_ihl = 5;
    ip->iph_ver = 4;
    // Low delay
    ip->iph_tos = 16;
    ip->iph_len = sizeof(struct ipheader) + sizeof(struct udpheader);
    ip->iph_ident = htons(54321);
    // Hops
    ip->iph_ttl = 64;
    // UDP
    ip->iph_protocol = 17;
    // Source address
    ip->iph_sourceip = inet_addr(src_ip);
    ip->iph_destip = inet_addr(dest_ip);

    // Fabricate the UDP header. Source port number
    udp->udph_srcport = htons(atoi(src_port));
    // Destination port number
    udp->udph_destport = htons(atoi(dest_port));
    udp->udph_len = htons(sizeof(struct udpheader));
    // Calculate checksum
    ip->iph_chksum = csum((unsigned short*)buffer, sizeof(struct ipheader) + sizeof(struct udpheader));
}

int main(int argc, char *argv[]){
    int sd;
    char *buffer;
    int one = 1;
    const int *val = &one;
    struct sockaddr_in sin;

    if (argc != 5) {
        printf("Invalid parameter!!!\n");
        printf("- Usage: %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);
        exit(-1);
    }

    create_udp_packet(&buffer, &sd, &sin, argv[2], argv[4], argv[1], argv[3]);

    // Inform the kernel not to fill up the packet
    if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
    {
        perror("setsockopt() error");
        exit(-1);
    }

    // Send loop
    printf("Using Source IP: %s port: %u, Target IP: %s port: %u.\n",
            argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));

    struct ipheader *ip = (struct ipheader *)&buffer;
    int count = 1;
    for (; count <= 20; count++) {
        if (sendto(sd, buffer, ip->iph_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            perror("sendto() error");
            exit(-1);
        }
    }
    return 0;
}
