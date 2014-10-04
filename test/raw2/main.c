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

#define EXTRACT_16BITS(p) \
	((u_int16_t)((u_int16_t)*((const u_int8_t *)(p) + 0) << 8 | \
		     (u_int16_t)*((const u_int8_t *)(p) + 1)))

#define EXTRACT_32BITS(p) \
	((u_int32_t)((u_int32_t)*((const u_int8_t *)(p) + 0) << 24 | \
		     (u_int32_t)*((const u_int8_t *)(p) + 1) << 16 | \
		     (u_int32_t)*((const u_int8_t *)(p) + 2) << 8 | \
		     (u_int32_t)*((const u_int8_t *)(p) + 3)))

void ProcessPacket(unsigned char* , int);
void print_ip_header(unsigned char* , int);
void print_tcp_packet(unsigned char * , int );
void print_udp_packet(unsigned char * , int );
void print_icmp_packet(unsigned char* , int );
void PrintData (unsigned char* , int);
void PrintData2 (unsigned char* , int);

FILE *logfile;
//#define logfile stdout

struct sockaddr_in source,dest;
int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j;

struct rip {
	u_int8_t rip_cmd;		/* request/response */
	u_int8_t rip_vers;		/* protocol version # */
	u_int8_t unused[2];		/* unused */
};

struct rip_netinfo {
	u_int16_t rip_family;
	u_int16_t rip_tag;
	u_int32_t rip_dest;
	u_int32_t rip_dest_mask;
	u_int32_t rip_router;
	u_int32_t rip_metric;		/* cost of route */
};

void PrintData2 (unsigned char* data , int Size)
{
    unsigned long sum= 0;
    unsigned short *data_t = (unsigned short*)data;
    fprintf(logfile , "Sum = %04X\n",(unsigned int)(data_t[0] + data_t[1]));
    int i , j;
    for(i=0 ; i < Size/2 ; i++)
    {
        if(i==5){

        fprintf(logfile , "(SKIPPING %04X)",(unsigned int)data_t[i]);
        continue;
        }
        if(i%16==0) fprintf(logfile , "   ");
        fprintf(logfile , " %04X",(unsigned int)data_t[i]);
        sum += data_t[i];
        //fprintf(logfile , "(%04lx)", sum);;

        if( i==Size-1)  //print the last spaces
        {
            fprintf(logfile ,  "\n" );
        }

    }
    sum = (sum & 0xffff) + (sum >> 16);
    fprintf(logfile , "Final = (%04lx)", ~sum);
    unsigned short chk = (unsigned short)(~sum);
    fprintf(logfile , "Unsigned = %d\n", ntohs(chk));
}

unsigned short csum(unsigned short *buf, int len)
{
        unsigned long sum;
        for(sum=0; len>0; len--)
                sum += *buf++;
        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
}

int main()
{
    int saddr_size , data_size;
    struct sockaddr saddr;

    unsigned char *buffer = (unsigned char *) malloc(65536); //Its Big!

    logfile=fopen("log.txt","w");
    if(logfile==NULL)
    {
        printf("Unable to create log.txt file.");
    }
    printf("Starting...\n");

    int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;

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
        //Now process the packet
        ProcessPacket(buffer , data_size);
        fflush(logfile);
    }
    close(sock_raw);
    printf("Finished");
    return 0;
}

void ProcessPacket(unsigned char* buffer, int size)
{
    //Get the IP Header part of this packet , excluding the ethernet header
    struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    ++total;
    switch (iph->protocol) //Check the Protocol and do accordingly...
    {
        case 1:  //ICMP Protocol
            ++icmp;
            //print_icmp_packet( buffer , size);
            break;

        case 2:  //IGMP Protocol
            ++igmp;
            break;

        case 6:  //TCP Protocol
            ++tcp;
            //print_tcp_packet(buffer , size);
            break;

        case 17: //UDP Protocol
            ++udp;
            print_udp_packet(buffer , size);
            break;

        default: //Some Other Protocol like ARP etc.
            ++others;
            break;
    }
    printf("TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r", tcp , udp , icmp , igmp , others , total);
}

void print_ethernet_header(unsigned char* Buffer, int Size)
{
    struct ethhdr *eth = (struct ethhdr *)Buffer;

    fprintf(logfile , "\n");
    fprintf(logfile , "Ethernet Header\n");
    fprintf(logfile , "   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    fprintf(logfile , "   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    fprintf(logfile , "   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
}

void print_ip_header(unsigned char* Buffer, int Size)
{
    print_ethernet_header(Buffer , Size);

    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr) );
    iphdrlen =iph->ihl*4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    fprintf(logfile , "\n");
    fprintf(logfile , "IP Header\n");
    fprintf(logfile , "   |-IP Version        : %d\n",(unsigned int)iph->version);
    fprintf(logfile , "   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
    fprintf(logfile , "   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
    fprintf(logfile , "   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
    fprintf(logfile , "   |-Identification    : %d\n",ntohs(iph->id));
    //fprintf(logfile , "   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
    //fprintf(logfile , "   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
    //fprintf(logfile , "   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
    fprintf(logfile , "   |-TTL      : %d\n",(unsigned int)iph->ttl);
    fprintf(logfile , "   |-Protocol : %d\n",(unsigned int)iph->protocol);
    fprintf(logfile , "   |-Checksum : %d\n",ntohs(iph->check));
    fprintf(logfile , "   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
    fprintf(logfile , "   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
}

/*
void print_tcp_packet(unsigned char* Buffer, int Size)
{
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );
    iphdrlen = iph->ihl*4;

    struct tcphdr *tcph=(struct tcphdr*)(Buffer + iphdrlen + sizeof(struct ethhdr));

    int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;

    fprintf(logfile , "\n\n***********************TCP Packet*************************\n");

    print_ip_header(Buffer,Size);

    fprintf(logfile , "\n");
    fprintf(logfile , "TCP Header\n");
    fprintf(logfile , "   |-Source Port      : %u\n",ntohs(tcph->source));
    fprintf(logfile , "   |-Destination Port : %u\n",ntohs(tcph->dest));
    fprintf(logfile , "   |-Sequence Number    : %u\n",ntohl(tcph->seq));
    fprintf(logfile , "   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
    fprintf(logfile , "   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
    //fprintf(logfile , "   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
    //fprintf(logfile , "   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
    fprintf(logfile , "   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
    fprintf(logfile , "   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    fprintf(logfile , "   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
    fprintf(logfile , "   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
    fprintf(logfile , "   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
    fprintf(logfile , "   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
    fprintf(logfile , "   |-Window         : %d\n",ntohs(tcph->window));
    fprintf(logfile , "   |-Checksum       : %d\n",ntohs(tcph->check));
    fprintf(logfile , "   |-Urgent Pointer : %d\n",tcph->urg_ptr);
    fprintf(logfile , "\n");
    fprintf(logfile , "                        DATA Dump                         ");
    fprintf(logfile , "\n");

    fprintf(logfile , "IP Header\n");
    PrintData(Buffer,iphdrlen);

    fprintf(logfile , "TCP Header\n");
    PrintData(Buffer+iphdrlen,tcph->doff*4);

    fprintf(logfile , "Data Payload\n");
    PrintData(Buffer + header_size , Size - header_size );

    fprintf(logfile , "\n###########################################################");
}
*/

char *ipaddr_string(u_int32_t *ip){
  struct in_addr sock_addr;
  memset(&sock_addr, 0, sizeof(struct in_addr));
  sock_addr.s_addr = *ip;

  return inet_ntoa(sock_addr);
}

int mask2plen( u_int32_t mask ){
       u_int32_t bitmasks[33] = {
              0x00000000,
              0x80000000, 0xc0000000, 0xe0000000, 0xf0000000,
              0xf8000000, 0xfc000000, 0xfe000000, 0xff000000,
              0xff800000, 0xffc00000, 0xffe00000, 0xfff00000,
              0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000,
              0xffff8000, 0xffffc000, 0xffffe000, 0xfffff000,
              0xfffff800, 0xfffffc00, 0xfffffe00, 0xffffff00,
              0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0,
              0xfffffff8, 0xfffffffc, 0xfffffffe, 0xffffffff
       };
       int prefix_len = 32;

       /* let's see if we can transform the mask into a prefixlen */
       while (prefix_len >= 0) {
              if (bitmasks[prefix_len] == mask)
                     break;
              prefix_len--;
       }
       return (prefix_len);
}

void rip_entry_print(struct rip_netinfo *ni) {

   u_short family = EXTRACT_16BITS(&(ni->rip_family));

   fprintf(logfile , "   |-Family      : %u\n" , family);
   fprintf(logfile , "   |-Address     : %s\n", ipaddr_string(&ni->rip_dest));
   fprintf(logfile , "   |-Mask        : %d\n",
           mask2plen(EXTRACT_32BITS(&ni->rip_dest_mask)));
   fprintf(logfile , "   |-Mask        : %s\n",
           ipaddr_string(&ni->rip_dest_mask));
   fprintf(logfile , "   |-Tag         : 0x%04x\n", EXTRACT_16BITS(&ni->rip_tag));
   fprintf(logfile , "   |-Metric      : %u\n", EXTRACT_32BITS(&ni->rip_metric));
   fprintf(logfile , "   |-Next hop    : ");

   if (EXTRACT_32BITS(&ni->rip_router))
	fprintf(logfile, "%s\n", ipaddr_string(&ni->rip_router));
   else
        fprintf(logfile, "self\n");

   /*
   fprintf(logfile, "\n\t  AFI: IPv4: %15s, Mask: %15d, tag 0x%04x, metric: %u, next-hop: ",
                       ipaddr_string(&ni->rip_dest),
		       mask2plen(EXTRACT_32BITS(&ni->rip_dest_mask)),
                       EXTRACT_16BITS(&ni->rip_tag),
                       EXTRACT_32BITS(&ni->rip_metric));
    */

}

void print_rip_packet(unsigned char *Buffer, int Size) {
   struct iphdr *iph = (struct iphdr *)(Buffer +  sizeof(struct ethhdr));
   unsigned short iphdrlen = iph->ihl*4;
    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof(struct udphdr);

   struct rip *rph = (struct rip*)(Buffer + sizeof(struct ethhdr) + iphdrlen + sizeof(struct udphdr));
   fprintf(logfile , "\nRIP Header\n");
   fprintf(logfile , "   |-Command      : %u\n" , (rph->rip_cmd));
   fprintf(logfile , "   |-Version      : %u\n" , (rph->rip_vers));

   struct rip_netinfo *ni;
   int rip_packet_length = Size - header_size;
   u_int i, j;
   j = rip_packet_length / sizeof(*ni);
   ni = (struct rip_netinfo *)(rph + 1);

   rip_entry_print(ni);

   fprintf(logfile , "   |-Size        : %u\n" , rip_packet_length);
   fprintf(logfile , "   |-Routes      : %u\n" , j);
}

void print_udp_packet(unsigned char *Buffer , int Size)
{
    struct iphdr *iph = (struct iphdr *)(Buffer +  sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl*4;

    struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen  + sizeof(struct ethhdr));
    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof(struct udphdr);

    fprintf(logfile , "\n\n***********************UDP Packet*************************\n");

    print_ip_header(Buffer,Size);

    fprintf(logfile , "\nUDP Header\n");
    fprintf(logfile , "   |-Source Port      : %d\n" , ntohs(udph->source));
    fprintf(logfile , "   |-Destination Port : %d\n" , ntohs(udph->dest));
    fprintf(logfile , "   |-UDP Length       : %d\n" , ntohs(udph->len));
    fprintf(logfile , "   |-UDP Checksum     : %d\n" , ntohs(udph->check));

    if (ntohs(udph->source) == 520 ) {
        print_rip_packet(Buffer, Size);
    }

    fprintf(logfile , "\n");
    fprintf(logfile , "IP Header\n");
    PrintData(Buffer+(sizeof (struct ethhdr)) , iphdrlen);
    fprintf(logfile , "\n");

    fprintf(logfile , "UDP Header\n");
    PrintData(Buffer+iphdrlen+sizeof(struct ethhdr) , sizeof(struct udphdr));
    fprintf(logfile , "\n");

    fprintf(logfile , "Data Payload\n");
    PrintData(Buffer + header_size , Size - header_size);

    fprintf(logfile , "\n###########################################################");
}

void print_icmp_packet(unsigned char* Buffer , int Size)
{
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;

    struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen  + sizeof(struct ethhdr));

    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;

    fprintf(logfile , "\n\n***********************ICMP Packet*************************\n");

    print_ip_header(Buffer , Size);

    fprintf(logfile , "\n");

    fprintf(logfile , "ICMP Header\n");
    fprintf(logfile , "   |-Type : %d",(unsigned int)(icmph->type));

    if((unsigned int)(icmph->type) == 11)
    {
        fprintf(logfile , "  (TTL Expired)\n");
    }
    else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
    {
        fprintf(logfile , "  (ICMP Echo Reply)\n");
    }

    fprintf(logfile , "   |-Code : %d\n",(unsigned int)(icmph->code));
    fprintf(logfile , "   |-Checksum : %d\n",ntohs(icmph->checksum));
    //fprintf(logfile , "   |-ID       : %d\n",ntohs(icmph->id));
    //fprintf(logfile , "   |-Sequence : %d\n",ntohs(icmph->sequence));
    fprintf(logfile , "\n");

    fprintf(logfile , "IP Header\n");
    PrintData(Buffer,iphdrlen);

    fprintf(logfile , "ICMP Header\n");
    PrintData(Buffer + sizeof(struct ethhdr) + iphdrlen , sizeof icmph);

    fprintf(logfile , "Data Payload\n");

    //Move the pointer ahead and reduce the size of string
    PrintData(Buffer + header_size , (Size - header_size) );

    fprintf(logfile , "\n###########################################################");
}

void PrintData (unsigned char* data , int Size)
{
    int i , j;
    for(i=0 ; i < Size ; i++)
    {
        if( i!=0 && i%16==0)   //if one line of hex printing is complete...
        {
            fprintf(logfile , "         ");
            for(j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    fprintf(logfile , "%c",(unsigned char)data[j]); //if its a number or alphabet

                else fprintf(logfile , "."); //otherwise print a dot
            }
            fprintf(logfile , "\n");
        }

        if(i%16==0) fprintf(logfile , "   ");
            fprintf(logfile , " %02X",(unsigned int)data[i]);

        if( i==Size-1)  //print the last spaces
        {
            for(j=0;j<15-i%16;j++)
            {
              fprintf(logfile , "   "); //extra spaces
            }

            fprintf(logfile , "         ");

            for(j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                {
                  fprintf(logfile , "%c",(unsigned char)data[j]);
                }
                else
                {
                  fprintf(logfile , ".");
                }
            }

            fprintf(logfile ,  "\n" );
        }
    }
}
