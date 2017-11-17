#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG
    #define PRINT_DEBUG(ARGS ...) printf(ARGS); fflush(stdout);
#else
    #define PRINT_DEBUG(ARGS ...) do {} while (0)
#endif

#endif // DEBUG_H