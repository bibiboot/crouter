#include "route.h"

/**
 * Rules
 *     If dest_ip is node0 ( 10.10.0.1 )
 *         Interface = inf000
 *         dest_mac  = 00:00:00:00:0B
 *         src_mac   = 00:00:00:00:01
 *     If dest_ip is node5 ( 10.1.2.3 )
 *         Interface = inf001
 *         dest_mac  = 00:00:00:00:05
 *         src_mac   = 00:00:00:00:02
 *     If dest_ip is rtr2 ( 10.99.0.2 )
 *         Interface = inf001
 *         dest_mac  = 00:00:00:00:05
 *         src_mac   = 00:00:00:00:02
 *     If dest_ip is usRTR ( 10.10.0.2 )
 *         Interface = inf000
 *         dest_mac  = 00:00:00:00:0B
 *         src_mac   = 00:00:00:00:01
 *     If dest_ip is usRTR ( 10.99.0.3 ) NOT USED
 *         Interface = inf001
 *         dest_mac  = 00:00:00:00:05
 *         src_mac   = 00:00:00:00:02
 *
 */
void get_route(unsigned char *dest_ip,
               char *result_if_name, char *dest_mac,
               char *src_mac){
    if (is_ip_equal(dest_ip, "10.10.0.1") ||
        is_ip_equal(dest_ip, "10.10.0.2")) {

        strcpy(result_if_name, "inf000");
        char test_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x0B};
        char test_mac2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
        memcpy(dest_mac, test_mac, 6);
        memcpy(src_mac, test_mac2, 6);

    } else if (is_ip_equal(dest_ip, "10.1.2.3")){

        strcpy(result_if_name, "inf001");
        char test_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x05};
        char test_mac2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
        memcpy(dest_mac, test_mac, 6);
        memcpy(src_mac, test_mac2, 6);

    } else if (is_ip_equal(dest_ip, "10.99.0.2")){

        strcpy(result_if_name, "inf001");
        char test_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x05};
        char test_mac2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
        memcpy(dest_mac, test_mac, 6);
        memcpy(src_mac, test_mac2, 6);

    } else {
        printf("No entry found for dest: %s\n", dest_ip);
    }
}
