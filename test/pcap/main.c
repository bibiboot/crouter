#include "main.h"

// gcc main.c -lpcap
// sudo ./a.out

int main(int argc, char *argv[]){
    char *dev = argv[1];
    printf("Device: %s\n", dev);

    struct bpf_program fp;
    pcap_t *handle;
    packet_capture_config(&fp, &handle, dev);

    // Cleanup
    packet_capture_cleanup(&fp, handle);

    printf("Packet capture complete\n");
    return 0;
}
