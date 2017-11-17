#ifdef DEBUG
    #define PRINT_DEBUG(ARGS ...) printf(ARGS); fflush(stdout);
#else
    #define PRINT_DEBUG(ARGS ...) do {} while (0)
#endif