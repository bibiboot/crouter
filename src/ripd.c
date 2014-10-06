#include "ripd.h"

int create_ripd_packet(char *buffer, char *sender_interface) {
        struct sockaddr_in entry , mask , nexthop;
        struct rip *rph = malloc(sizeof(struct rip));

	rph->rip_cmd = 2;
	rph->rip_vers = 2;

	memcpy(buffer ,rph , sizeof(struct rip));
        free(rph);

        int i;
        for (i = 0; i < globals.rtable_size; i++) {

            struct rip_netinfo *ni  = malloc(sizeof(struct rip_netinfo));

	    ni->rip_family = htons(AF_INET) ;
	    ni->rip_tag = 0;
            char interface[100];
            uint32_t mask, metric;

            uint32_t next_hop = get_route_entry_rip( globals.rtable_keys[i],
                                                     &interface,
                                                     &mask,
                                                     &metric );

            /*
            if (strcmp(interface, sender_interface) == 0) {
                continue;
            }*/

	    ni->rip_dest = globals.rtable_keys[i];
	    ni->rip_dest_mask = mask;
	    ni->rip_router = next_hop;
	    ni->rip_metric = htonl(metric);

            printf("Debug: Sending route: Network:  ");
            print_ip(ni->rip_dest);
            printf("  ,Mask : ");
            print_ip(mask);
            printf("  ,Next hop : ");
            print_ip(next_hop);
            printf("  ,Metric : %d", metric);
            printf("\n");

            /*
	    ni->rip_dest = char_to_uint32("10.1.1.0");
	    ni->rip_dest_mask = char_to_uint32("255.255.255.0");
	    ni->rip_router = char_to_uint32("0.0.0.0");
	    ni->rip_metric = htonl(1);
            */

	    memcpy((buffer + sizeof(struct rip) + sizeof(struct rip_netinfo) * i) ,
                    ni , sizeof(struct rip_netinfo));
            free(ni);
        }

	int datalen = sizeof(struct rip) + sizeof(struct rip_netinfo) * globals.rtable_size ;
	*( buffer + datalen) = '\0';

        return datalen;
}

void send_ripd(int sd, char *buffer, int datalen, struct sockaddr_in *groupSock) {

    if(sendto(sd, buffer, datalen, 0,
       (struct sockaddr*)groupSock,
        sizeof(struct sockaddr_in)) < 0) {
        perror("Error sending RIP Packet");
    }
}

int create_connection(struct sockaddr_in *groupSock,
                      char *sender_interface_ip) {
    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr= htonl(INADDR_ANY);
    //source port for outgoing packets
    cliaddr.sin_port=htons(RIP_PORT);

    /* Create a datagram socket on which to send. */
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("Opening datagram socket error");
  	exit(1);
    }

    memset((char *) groupSock, 0, sizeof(struct sockaddr_in));
    groupSock->sin_family = AF_INET;
    groupSock->sin_addr.s_addr = inet_addr(MULTICAST_IP);
    groupSock->sin_port = htons(RIP_PORT);

    bind(sd,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

    struct in_addr local_interface;
    local_interface.s_addr = inet_addr(sender_interface_ip);

    if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF,
                 (char *)&local_interface,
                  sizeof(local_interface)) < 0) {
        perror("Setting local interface error");
  	exit(1);
    }
    return sd;
}

void* ripd(void *val) {

    char *buffer;

    while(1) {
        buffer = malloc (sizeof(struct rip_netinfo)*globals.rtable_size + sizeof(struct rip) + 1);

        int datalen_inf1 = create_ripd_packet( buffer, INF1 );
        //int datalen_inf2 = create_ripd_packet( buffer, INF2 );

        send_ripd( globals.ripd_eth1_fd, buffer, datalen_inf1, &globals.ripd_eth1_sock );
        //send_ripd( globals.ripd_eth2_fd, buffer, datalen_inf2, &globals.ripd_eth2_sock );
        printf("Sending multicast rip packets\n");
        fflush(stdout);
        sleep(30);
        free(buffer);
    }

}
