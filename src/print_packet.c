#include "print_packet.h"

void print_data (unsigned char* data , int size)
{
    int i;
    for(i=0 ; i < size ; i++)
        fprintf(LOGFILE, "%02hhx ", data[i]);
    fprintf(LOGFILE, "\n");
}

void print_mac(const char *addr)
{
    int i;
    for (i = 0; i < ETH_ALEN - 1; i++)
        printf("%02hhx:", addr[i]);
    printf("%02hhx\n", addr[ETH_ALEN - 1]);
}
