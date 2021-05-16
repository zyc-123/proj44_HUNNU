#ifndef MACRO_DEF
#define MACRO_DEF

#define RHINO_NO_WAIT      0u
#define RHINO_WAIT_FOREVER ((uint64_t)-1)

#ifndef NULL
    #ifdef __cplusplus
        #define NULL 0
    #else
        #define NULL ((void *)0)
    #endif
#endif

#define NULL_PARA_CHK(para)        \
    do {                           \
        if (para == NULL) {        \
            return RHINO_NULL_PTR; \
        }                          \
    } while (0)


#endif
