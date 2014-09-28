#include "socket_util.h"

int interface_addr(int sock, char *ifname, unsigned char *addr)
{
    struct ifreq ifr;

    /* retrieve corresponding source MAC */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        perror("Error in retrieveig mac address");
        printf("Error\n");
        return -EXIT_FAILURE;
    }
    memcpy(addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

    return EXIT_SUCCESS;
}

int interface_index(int sock, char *ifname)
{
    struct ifreq ifr;

    /* retrieve source ethernet interface index */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
        return -EXIT_FAILURE;

    return ifr.ifr_ifindex;
}

int get_socket(){
    int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (s == -1) {
	printf("ERROR: socket\n");
	perror("SOCKET:");
	exit(1);
    }
    return s;
}
