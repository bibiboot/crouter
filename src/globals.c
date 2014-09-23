#include "globals.h"

// Initializing the global structure with default values
struct globals globals = {
    .config = CONFIG_INIT,
    // This is null as per documentation of uthash
    .hashl = NULL
};

/**
 * Calculates difference between two timeval.
 */
unsigned int time_diff_micro(struct timeval end, struct timeval start){
  return ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
}
