#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>

// gcc main.c -lpcap
// sudo ./a.out

int default_device(char **dev){
    char errbuf[PCAP_ERRBUF_SIZE];

    *dev = pcap_lookupdev(errbuf);
    if (*dev == NULL) {
        fprintf(stderr, "Can not find device: %s\n", errbuf);
        return 1;
    }

    printf("Device: %s\n", *dev);
    return 0;
}

int main(int argc, char *argv[]){
    char *dev = argv[1];
    printf("Device: %s\n", dev);

    /*
    char *dev;
    if (default_device(&dev) != 0) {
        exit(0);
    }
    */

    bpf_u_int32 mask;
    bpf_u_int32 net;
    char errbuf[PCAP_ERRBUF_SIZE];
    /* Find the properties for the device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
        net = 0;
        mask = 0;
    }

    // Sniff traffic in promiscous mode
    // Get handle to sniffing session
    pcap_t *handle;
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Can not open the device: %s\n", errbuf);
        return 2;
    }

    // Link-header is of type ethernet
    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "Device %s do not provide ethernet headers: %s\n", dev, errbuf);
        return 2;
    }

    // Compiled filter
    struct bpf_program fp;
    char filter_exp[] = "port 80";
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }

    struct pcap_pkthdr header;
    const u_char *packet;
    /* Grab a packet */
    packet = pcap_next(handle, &header);
    /* Print its length */
    printf("Jacked a packet with length of [%d]\n", header.len);
    /* And close the session */
    pcap_close(handle);

    return 0;
}
