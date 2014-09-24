#include <stdio.h>
#include <linux/if_packet.h>

void parse_hex(const char *mac_addr){
    struct sockaddr_ll sa = {0};

    // x for hexdecimal
    // hh for 8 bits
    sscanf(mac_addr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &sa.sll_addr[0],
           &sa.sll_addr[1],
           &sa.sll_addr[2],
           &sa.sll_addr[3],
           &sa.sll_addr[4],
           &sa.sll_addr[5]
    );
}

int main(int argc, char *argv[]){

    const char mac_addr[] = "01:02:03:04:05:ff";
    parse_hex(mac_addr);

    return 0;
}
