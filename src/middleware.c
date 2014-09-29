#include "middleware.h"
#include "packet_send.h"
#include "packet_update.h"
#include "socket_util.h"
#include "util.h"

/**
 * Handle the incoming packet.
 * Either send or append to the queue.
 */
void incoming_packet_handler(unsigned char *packet, int size){
    //eth0 3c:97:0e:9d:53:6e
    // Get this information from routing
    unsigned char dest_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x05};
    //unsigned car dest_mac[6] = {0x3c, 0x97, 0x0e, 0x9d, 0x53, 0x53};
    char if_name[IFNAMSIZ] = "inf001";

    prepare_n_send_packet(packet, size, dest_mac, if_name);
}

/**
 * Prepare packet
 * Change ip and ethernet header
 * Send packet on line
 */
void prepare_n_send_packet(unsigned char *packet, int size,
                           unsigned char *dest_mac, char *if_name){

    update_ip_packet(packet);

    update_ethernet_packet(packet, globals.src_mac, dest_mac);

    send_packet_on_line(if_name, dest_mac, packet);
}
