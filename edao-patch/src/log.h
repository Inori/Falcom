#ifndef VITA_LOG_H
#define VITA_LOG_H


void debug_print(const char* text, ...);



#ifdef DEBUG
    #define DEBUG_PRINT(format, ...) debug_print(format, __VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif


#endif //VITA_LOG_H