#include "util.h"

void create_log_file(){
    LOGFILE = fopen("log.txt","w");
    if (LOGFILE == NULL)
    {
        printf("Unable to create log.txt file.");
    }
}

bool is_mac_addr_equal(unsigned char *mac_addr1, unsigned char *mac_addr2){
    if (memcmp(mac_addr1, mac_addr2, ETH_ALEN) == 0)
        return true;
    return false;
}

