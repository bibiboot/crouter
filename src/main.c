#include "globals.h"
#include "hashl.h"
#include "print_packet.h"
#include "packet_sniffer.h"
#include "packet_update.h"
#include "packet_send.h"
#include "packet_create.h"
#include "socket_util.h"
#include "socket_addr_create.h"
#include "util.h"

void echo_reply(unsigned char *packet, int size){

    print_icmp_packet( packet , size);

    char if_name[IFNAMSIZ] = "inf000";
    //char if_name[IFNAMSIZ] = "wlan0";
    int s = get_socket();

    /*Get current interface mac address*/
    unsigned char *src_mac = malloc(ETH_ALEN);
    interface_addr(s, if_name, src_mac);

    //eth0 3c:97:0e:9d:53:6e
    unsigned char dest_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x0b};
    //unsigned char dest_mac[6] = {0x3c, 0x97, 0x0e, 0x9d, 0x53, 0x53};

    /*Add the dest and src mac address*/
    update_icmp_packet(packet, src_mac, dest_mac);

    send_icmp_packet(if_name, dest_mac, packet);

    print_icmp_packet( packet , size);
}

void echo_request(unsigned char *packet, int size){

    //print_icmp_packet( packet , size);
    //print_udp_packet( packet , size);

    char if_name[IFNAMSIZ] = "inf001";
    //char if_name[IFNAMSIZ] = "wlan0";
    int s = get_socket();

    /*Get current interface mac address*/
    unsigned char *src_mac = malloc(ETH_ALEN);
    interface_addr(s, if_name, src_mac);

    //eth0 3c:97:0e:9d:53:6e
    unsigned char dest_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x05};
    //unsigned char dest_mac[6] = {0x3c, 0x97, 0x0e, 0x9d, 0x53, 0x53};

    /*Add the dest and src mac address*/
    update_icmp_packet(packet, src_mac, dest_mac);

    send_icmp_packet(if_name, dest_mac, packet);

    //print_icmp_packet( packet , size);
    //print_udp_packet( packet , size);
}

int main(int argc, char *argv[]){

    create_log_file();

    unsigned char *packet = (unsigned char *) malloc(65536); //Its Big!
    /*Sniff will return me a ICMP packet*/
    int size = sniff(packet);

    update_ip_packet(packet);

    echo_request(packet, size);

    /*
    size = sniff(packet);

    echo_reply(packet, size);

    */
    printf("SUCCESS\n");
    return 0;
}

void start(){
    //void *val;
    //pthread_create(&globals.sen_th, 0, reciever, val);
    //pthread_create(&globals.rev_th, 0, sender, val);
}
