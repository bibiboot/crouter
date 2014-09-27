#include "pcap_lib.h"

void icmp_handler(const u_char *packet){
    print_icmp_packet(packet, 0);
    // TODO: Add code
}

void udp_handler(const u_char *packet){
    /* define/compute tcp header offset */
    int size_ip;
    int size_udp;
    int size_payload;
    const struct sniff_ethernet *ethernet;
    const struct sniff_udp *udp;            /* The TCP header */
    const struct sniff_ip *ip;              /* The IP header */
    const char *payload;                    /* Packet payload */

    ethernet = (struct sniff_ethernet*)(packet);

    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
	printf("   * Invalid IP header length: %u bytes\n", size_ip);
	return;
    }

    udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + size_ip);
    //size_udp = TH_OFF(udp)*4;
    //printf("SIZE = %d\n", size_udp);
    //if (size_udp < 20) {
//	printf("   * Invalid TCP header length: %u bytes\n", size_udp);
//	return;
 //   }

    printf("   Src port: %d\n", ntohs(udp->sport));
    printf("   Dst port: %d\n", ntohs(udp->dport));
    print_mac(ethernet->ether_dhost);
    print_mac(ethernet->ether_shost);

    /* define/compute tcp payload (segment) offset */
    payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_udp);

    /* compute tcp payload (segment) size */
    size_payload = ntohs(ip->ip_len) - (size_ip + size_udp);

    /*
     * Print payload data; it might be binary, so don't just
     * treat it as a string.
     */
    if (size_payload > 0) {
	printf("   Payload (%d bytes):\n", size_payload);
	print_payload(payload, size_payload);
    }
}

void tcp_handler(const u_char *packet){
    /* define/compute tcp header offset */
    int size_ip;
    int size_tcp;
    int size_payload;
    const struct sniff_ethernet *ethernet;
    const struct sniff_tcp *tcp;            /* The TCP header */
    const struct sniff_ip *ip;              /* The IP header */
    const char *payload;                    /* Packet payload */

    ethernet = (struct sniff_ethernet*)(packet);

    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
	printf("   * Invalid IP header length: %u bytes\n", size_ip);
	return;
    }

    tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
	printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
	return;
    }

    printf("   Src port: %d\n", ntohs(tcp->th_sport));
    printf("   Dst port: %d\n", ntohs(tcp->th_dport));
    print_mac(ethernet->ether_dhost);
    print_mac(ethernet->ether_shost);

    /* define/compute tcp payload (segment) offset */
    payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);

    /* compute tcp payload (segment) size */
    size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

    /*
     * Print payload data; it might be binary, so don't just
     * treat it as a string.
     */
    if (size_payload > 0) {
	printf("   Payload (%d bytes):\n", size_payload);
	print_payload(payload, size_payload);
    }
}

/*
 * dissect/print packet
 */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    static int count = 1;                   /* packet counter */

    /* declare pointers to packet headers */
    const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
    const struct sniff_ip *ip;              /* The IP header */
    const struct sniff_tcp *tcp;            /* The TCP header */
    const char *payload;                    /* Packet payload */

    int size_ip;
    int size_tcp;
    int size_payload;

    printf("\nPacket number %d:\n", count);
    count++;

    /* define ethernet header */
    ethernet = (struct sniff_ethernet*)(packet);

    // Try sending the packet
    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
	printf("   * Invalid IP header length: %u bytes\n", size_ip);
	return;
    }

    /* print source and destination IP addresses */
    printf("       From: %s\n", inet_ntoa(ip->ip_src));
    printf("         To: %s\n", inet_ntoa(ip->ip_dst));
    printf("%d: %d\n", IPPROTO_TCP, ip->ip_p);

    /* determine protocol */
    switch(ip->ip_p) {
	case IPPROTO_TCP:
	    printf("   Protocol: TCP\n");
            tcp_handler(packet);
	    break;
	case IPPROTO_UDP:
	    printf("   Protocol: UDP\n");
            udp_handler(packet);
	    return;
	case IPPROTO_ICMP:
	    printf("   Protocol: ICMP\n");
            icmp_handler(packet);
	    return;
	case IPPROTO_IP:
	    printf("   Protocol: IP\n");
	    return;
	default:
	    printf("   Protocol: unknown\n");
	    return;
    }
}

void packet_capture_cleanup(struct bpf_program *fp, pcap_t *handle){
    pcap_freecode(fp);
    pcap_close(handle);
}

int packet_capture_config(struct bpf_program *fp, pcap_t **handle, char *dev){
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
    *handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (*handle == NULL) {
        fprintf(stderr, "Can not open the device: %s\n", errbuf);
        return 2;
    }

    // Link-header is of type ethernet
    if (pcap_datalink(*handle) != DLT_EN10MB) {
        fprintf(stderr, "Device %s do not provide ethernet headers: %s\n", dev, errbuf);
        return 2;
    }

    // Compiled filter
    char filter_exp[] = SNIFF_FILTER;
    if (pcap_compile(*handle, fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(*handle));
        return(2);
    }

    // Apply filter
    if (pcap_setfilter(*handle, fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(*handle));
        return(2);
    }

    int num_packets = 10;
    printf("Number of packets: %d\n", num_packets);
    /* now we can set our callback function */
    pcap_loop(*handle, num_packets, got_packet, NULL);

    return 0;
}
