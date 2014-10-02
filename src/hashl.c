#include "hashl.h"
#include "globals.h"

/**
 * @brief Add structure to the hashtable
 *        Key is the sequence number
 */
void add_entry(router_entry *r){
    HASH_ADD_INT( globals.rentry, network, r);
}

/**
 * @brief Find a structure based on the key
 *        which is a sequence number
 */
router_entry *find_entry(uint32_t network){
    router_entry *r;
    HASH_FIND_INT( globals.rentry, &network, r);
    return r;
}
