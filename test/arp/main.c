#include "fetch_arp_table.h"

int main(int argc, char *argv[]){
    print_arp_table();

    char *mac_addr = malloc(100);
    const char ip[100] = "192.168.0.2";
    if (get_mac_addr_from_arp_table(ip, &mac_addr) == -1) {
        printf("No entry found in the local arp table\n");
    }
    return 0;
}
