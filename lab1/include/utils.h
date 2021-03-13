#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <sys/types.h>

/* Debug tool */
#define debug_flag 1
#define DEBUG(txt)                  \
    do {                            \
        if (debug_flag) {           \
            printf("[Debug] " txt); \
        }                           \
    } while (0)

/* Enumerate for protocols and functionalities */
enum { TCP, UDP, SEND, RECV };

/* Information of a file */
typedef struct {
    size_t size;
    mode_t mode;
    char name[256];
} file_info_t;

/* Buffer size */
#define BUF_SIZE 512

/* UDP starting header */
#define UDP_START "hi"

#endif /* UTILS_H */
