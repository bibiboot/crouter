#pragma once

#include "globals.h"

void add_entry( router_entry *r );

router_entry *find_entry( uint32_t network );

void add_arp_entry( arp_entry *a );

arp_entry *find_arp_entry( uint32_t ip );
