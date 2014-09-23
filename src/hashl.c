#include "hashl.h"
#include "globals.h"

/**
 * @brief Add structure to the hashtable
 *        Key is the sequence number
 */
void add_hashl(hashed_link *h){
    HASH_ADD_INT( globals.hashl, seq_num, h);
}

/**
 * @brief Find a structure based on the key
 *        which is a sequence number
 */
hashed_link *find_hashl(vlong seq_num){
    hashed_link *h;
    HASH_FIND_INT( globals.hashl, &seq_num, h);
    return h;
}
