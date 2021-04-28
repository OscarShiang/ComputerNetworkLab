#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <sys/types.h>

/* Debug tool */
#define debug_flag 0
#define DEBUG(txt, ...)                            \
    do {                                           \
        if (debug_flag) {                          \
            printf("[Debug] " txt, ##__VA_ARGS__); \
        }                                          \
    } while (0)

/* Tool for printing progress */
#define PROGRESS_CTX "[%3d%%] %d/%02d/%02d %02d:%02d:%02d\n"
#define PROGRESS(...)                        \
    do {                                     \
        printf(PROGRESS_CTX, ##__VA_ARGS__); \
    } while (0)

/* Information of a file */
typedef struct {
    size_t size;
    char name[256];
    mode_t mode;
} file_info_t;

int read_info(file_info_t *info, char *filename);
void print_file_size(size_t size);

/* Buffer size */
#define BUF_SIZE 512

#define UDP_ACK "ack"

#endif /* UTILS_H */
