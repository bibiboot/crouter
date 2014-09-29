#include "globals.h"
#include "socket_util.h"
#include "util.h"

int main(int argc, char *argv[]){

    char if_name[IFNAMSIZ] = "inf001";

    create_log_file();

    globals.send_sock_fd = get_socket();

    interface_addr(globals.send_sock_fd, if_name, globals.src_mac);
    interface_addr(globals.send_sock_fd, "inf000", globals.src_mac2);

    sniff();

    printf("SUCCESS\n");
    return 0;
}

void start(){
    //void *val;
    //pthread_create(&globals.sen_th, 0, reciever, val);
    //pthread_create(&globals.rev_th, 0, sender, val);
}
