#include "globals.h"
#include "hashl.h"

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

#define PACKET_SIZE 1000
#define LOGFILE stdout

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
    printf("%02hhx\n", addr[ETH_ALEN - 1]);
}

int interface_addr(int sock, char *ifname, unsigned char *addr)
{
    struct ifreq ifr;

    /* retrieve corresponding source MAC */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        perror("Error in retrieveig mac address");
        printf("Error\n");
        return -EXIT_FAILURE;
    }
    memcpy(addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

    return EXIT_SUCCESS;
}

int interface_index(int sock, char *ifname)
{
    struct ifreq ifr;

    /* retrieve source ethernet interface index */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
        return -EXIT_FAILURE;

    return ifr.ifr_ifindex;
}

int get_socket(){
    int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (s == -1) {
	printf("ERROR: socket\n");
	perror("SOCKET:");
	exit(1);
    }
    return s;
}

void create_socket_address(struct sockaddr_ll *socket_address, int src_index, unsigned char *dest_mac){
    /*prepare sockaddr_ll*/
    /*RAW communication*/
    socket_address->sll_family   = PF_PACKET;
    /*we don't use a protocoll above ethernet layer
    ->just use anything here*/
    socket_address->sll_protocol = htons(ETH_P_IP);

    /*index of the network device
    see full code later how to retrieve it*/
    socket_address->sll_ifindex  = src_index;
    /*ARP hardware identifier is ethernet*/
    socket_address->sll_hatype   = ARPHRD_ETHER;

    /*target is another host*/
    socket_address->sll_pkttype  = PACKET_OTHERHOST;

    // The octet in a ETH_ALEN = 6
    /*address length*/
    socket_address->sll_halen = ETH_ALEN;

    /*Assign the src mac address to the socket address*/
    memcpy(socket_address->sll_addr, dest_mac, ETH_ALEN);
}

void create_packet(void *packet, unsigned char *src_mac, unsigned char *dest_mac){

   /*pointer to ethenet header*/
    unsigned char* etherhead = packet;

    /*userdata in ethernet frame*/
    unsigned char* data = packet + ETH_HLEN;

    /*another pointer to ethernet header*/
    struct ethhdr *eh = (struct ethhdr *)etherhead;

    /*set the frame header*/
    memcpy((void*)packet, (void*)dest_mac, ETH_ALEN);
    memcpy((void*)(packet+ETH_ALEN), (void*)src_mac, ETH_ALEN);
    eh->h_proto = 0x00;
    //eh->h_proto = 0x0806;
    /*fill the frame with some data*/
    /* fill ethernet payload with some data */
    int j;
    for (j = 0; j < PACKET_SIZE - ETH_HLEN; j++)
        data[j] = (unsigned char)(0);
    *(int *)data = htonl(10);
}

void send_packet() {
    int s; /*socketdescriptor*/
    char if_name[IFNAMSIZ] = "inf001";
    struct ifreq ifr;
    /*target address*/
    struct sockaddr_ll socket_address;

    /*other host MAC address*/
    unsigned char dest_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x05};

    /*Create raw socket*/
    s = get_socket();

    /*Get current interface mac address*/
    unsigned char *src_mac = malloc(ETH_ALEN);
    interface_addr(s, if_name, src_mac);

    //Find src interface index
    int src_index = interface_index(s, if_name);

    /*prepare sockaddr_ll*/
    create_socket_address(&socket_address, src_index, dest_mac);

    /*buffer for ethernet frame*/
    void* packet = (void*)malloc(PACKET_SIZE);

    /*Create packet*/
    create_packet(packet, src_mac, dest_mac);

    /*send the packet*/
    int send_result = 0;
    send_result = sendto(s, packet, PACKET_SIZE , 0, (struct sockaddr*)&socket_address, sizeof(socket_address));
    if (send_result < 0){
	printf("ERROR: sendto\n");
	perror("sendto");
	exit(1);
    }

    fprintf(LOGFILE, "Header : ");
    print_data(packet, ETH_HLEN);

    //fprintf(LOGFILE, "Data : ");
    //print_data(packet + ETH_HLEN, PACKET_SIZE- ETH_HLEN);
}

int main(int argc, char *argv[]){

    send_packet();

    printf("SUCCESS\n");
    return 0;
}

void start(){
    //void *val;
    //pthread_create(&globals.sen_th, 0, reciever, val);
    //pthread_create(&globals.rev_th, 0, sender, val);
}
