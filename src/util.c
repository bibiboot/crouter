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

bool is_ip_equal(unsigned char *ip1, unsigned char *ip2){
    if ( strcmp(ip1, ip2) == 0 )
        return true;
    return false;
}

bool is_chksum_valid (unsigned char* data , int size) {
    int i;
    unsigned long sum = 0;
    unsigned short *data_t = (unsigned short*)data;

    for(i=0 ; i < size/2 ; i++)
        sum += data_t[i];

    sum = (sum & 0xffff) + (sum >> 16);
    unsigned short chk = (unsigned short)(~sum);
    //printf("\n This value should be zero = %d\n", ntohs(chk));
    if (chk == 0) return true;
    return false;
}

unsigned short cksum (unsigned char* data , int size) {

    int i;
    unsigned long sum = 0;
    unsigned short *data_t = (unsigned short*)data;

    for(i=0 ; i < size/2 ; i++) {
        if(i==5) continue;
        //fprintf(LOGFILE , " %04X",(unsigned int)data_t[i]);
        sum += data_t[i];
        //fprintf(LOGFILE , "(%04lx)", sum);;
    }

    sum = (sum & 0xffff) + (sum >> 16);
    //printf("\nFinal = (%04lx)\n", ~sum);
    unsigned short chk = (unsigned short)(~sum);
    //printf("\nBY HAND CHEKSUM = %d\n", ntohs(chk));
    return chk;
}

/**
 * Calculates difference between two timeval.
 */
unsigned int time_diff_micro(struct timeval end, struct timeval start){
  return ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
}
