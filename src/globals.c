#include "globals.h"

// Initializing the global structure with default values
struct globals globals = {
    .config = CONFIG_INIT,
    // This is null as per documentation of uthash
    .rentry = NULL,
    .aentry = NULL,
    .rtable_size = 0
};

