#include "socket_addr_create.h"

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


