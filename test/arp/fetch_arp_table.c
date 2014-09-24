#include "fetch_arp_table.h"

// Format
// 192.168.0.1      0x1         0x2         00:00:ca:00:00:03     *        wlan0

int get_mac_addr_from_arp_table(const char *given_ip, char **mac_addr){
    const char arp_filename[100] = "/proc/net/arp";
    char *line;
    ssize_t len = 0;
    ssize_t read;
    char ip[100], hw_type[10], flag[10], mask[10], dev[10];
    int line_num = 0;

    FILE *fp = fopen(arp_filename, "r");

    while ((read = getline(&line, &len, fp)) != -1) {
        // Ignore the heading
        if ( line_num++ == 0 ) continue;
        //printf("Recv: %s", line);
        sscanf(line, "%[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %s",
                     ip, hw_type, flag, *mac_addr, mask, dev);
        if (strcmp(ip, given_ip) == 0)
            return 0;

        //printf("IP: %s -> MAC: %s\n", ip, *mac_addr);
    }
    return -1;
}

void print_arp_table(){
    const char arp_filename[100] = "/proc/net/arp";
    char *line;
    ssize_t len = 0;
    ssize_t read;
    char ip[100], mac[100], hw_type[10], flag[10], mask[10], dev[10];
    int line_num = 0;

    FILE *fp = fopen(arp_filename, "r");

    while ((read = getline(&line, &len, fp)) != -1) {
        // Ignore the heading
        if ( line_num++ == 0 ) continue;
        //printf("Recv: %s", line);
        sscanf(line, "%[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %s", ip, hw_type, flag, mac, mask, dev);
        printf("IP: %s -> MAC: %s\n", ip, mac);
    }
}
