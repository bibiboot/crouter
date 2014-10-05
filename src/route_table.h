#include "globals.h"

void init_build_route_table();

void init_build_route_table_dynamic();

void update_or_add_entry(uint32_t network, uint32_t source_ip, uint32_t next_hop,
                         char *interface, uint32_t mask, uint32_t metric) ;

void print_route_table();

uint32_t get_route_entry_rip(uint32_t network, char *interface, 
                             uint32_t *mask, uint32_t *metric);
