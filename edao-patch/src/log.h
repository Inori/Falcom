#ifndef VITA_LOG_H
#define VITA_LOG_H


#define DEBUG



void debug_print(const char* format, ...);



#ifdef DEBUG
    #define DEBUG_PRINT(...) debug_print(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif


#endif //VITA_LOG_H