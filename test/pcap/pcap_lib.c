#include "pcap_lib.h"

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


void print_icmp_packet(u_char *packet) {

    struct icmphdr *icmph = (struct icmphdr *)(packet + iphdrlen  + sizeof(struct ethhdr));

    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;

    fprintf(stdout , "\n\n***********************ICMP Packet*************************\n");

    //print_ip_header(Buffer , Size);

    fprintf(stdout , "\n");

    fprintf(stdout , "ICMP Header\n");
    fprintf(stdout , "   |-Type : %d",(unsigned int)(icmph->type));

    if((unsigned int)(icmph->type) == 11)
    {
        fprintf(stdout , "  (TTL Expired)\n");
    }
    else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
    {
        fprintf(stdout , "  (ICMP Echo Reply)\n");
    }

    fprintf(stdout , "   |-Code : %d\n",(unsigned int)(icmph->code));
    fprintf(stdout , "   |-Checksum : %d\n",ntohs(icmph->checksum));
    //fprintf(stdout , "   |-ID       : %d\n",ntohs(icmph->id));
    //fprintf(logfile , "   |-Sequence : %d\n",ntohs(icmph->sequence));
    fprintf(stdout   , "\n");

    fprintf(stdout , "IP Header\n");
    //PrintData(Buffer,iphdrlen);

    fprintf(stdout , "UDP Header\n");
    //PrintData(Buffer + iphdrlen , sizeof icmph);

    fprintf(stdout , "Data Payload\n");

    //Move the pointer ahead and reduce the size of string
    //PrintData(Buffer + header_size , (Size - header_size) );

    fprintf(stdout , "\n###########################################################");
}



}

/*
 * dissect/print packet
 */
void
got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
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

        print_icmp_packet(packet,0 );

	/* define ethernet header */
	ethernet = (struct sniff_ethernet*)(packet);

	/* define/compute ip header offset */
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET_1);
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
		printf("   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

	/* print source and destination IP addresses */
	printf("       From: %s\n", inet_ntoa(ip->ip_src));
	printf("         To: %s\n", inet_ntoa(ip->ip_dst));

	/* determine protocol */
	switch(ip->ip_p) {
		case IPPROTO_TCP:
			printf("   Protocol: TCP\n");
			break;
		case IPPROTO_UDP:
			printf("   Protocol: UDP\n");
			return;
		case IPPROTO_ICMP:
			printf("   Protocol: ICMP\n");
                        print_icmp_packet(packet);
			return;
		case IPPROTO_IP:
			printf("   Protocol: IP\n");
			return;
		default:
			printf("   Protocol: unknown\n");
			return;
	}

	/*
	 *  OK, this packet is TCP.
	 */

	/* define/compute tcp header offset */
	tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET_1 + size_ip);
	size_tcp = TH_OFF(tcp)*4;
	if (size_tcp < 20) {
		printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
		return;
	}

	printf("   Src port: %d\n", ntohs(tcp->th_sport));
	printf("   Dst port: %d\n", ntohs(tcp->th_dport));

	/* define/compute tcp payload (segment) offset */
	payload = (u_char *)(packet + SIZE_ETHERNET_1 + size_ip + size_tcp);

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
    return;
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
    //char filter_exp[] = "port 80";
    char filter_exp[] = "dst 127.0.0.1";
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
