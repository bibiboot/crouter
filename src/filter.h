#include "globals.h"
#include<netinet/ip.h>    //Provides declarations for ip header
#include <linux/if_ether.h>

bool is_allowed(unsigned char *packet);
