#include "globals.h"
#include "socket_util.h"
#include "util.h"

int main(int argc, char *argv[]){

    /* Create file descriptor to write the packet */
    create_log_file();

    /* Descriptor used for sending packets */
    globals.send_sock_fd = get_socket();

    /**
     * Router's interfaces and its mac addresss
     * Needed to sniff only certain interfaces
     */
    interface_addr(globals.send_sock_fd, "inf001", globals.src_mac);
    interface_addr(globals.send_sock_fd, "inf000", globals.src_mac2);

    sniff();

    return 0;
}
