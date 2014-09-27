#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>

void sniff_packet(){
    int fd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(fd == -1) {
        perror("Socket failed: ");
        exit(1);
    }
    char buffer[8192]; /* single packets are usually not bigger than 8192 bytes */
    while (read (fd, buffer, 8192) > 0) {
         printf ("Caught tcp packet: %s\n",
                 buffer+sizeof(struct iphdr)+sizeof(struct tcphdr));
    }

}

int main(int argc, char *argv[]){
    sniff_packet();

    return 0;
}
