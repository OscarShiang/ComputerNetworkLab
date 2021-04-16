#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "utils.h"

int read_info(file_info_t *info, char *filename)
{
    // read file info from stat
    struct stat statbuf;
    if (stat(filename, &statbuf) < 0) {
        perror("[Error] stat");
        return -1;
    }

    info->size = statbuf.st_size;
    info->mode = statbuf.st_mode & 0x1ff;  // only the least significant 9 bits

    // trim the path before the filename
    int idx = 0;
    for (int i = strlen(filename) - 1; i >= 0; i--) {
        if (filename[i] == '/') {
            idx = ++i;
            break;
        }
    }
    strncmp(info->name, &filename[idx], 256);

    return 0;
}

static const int storage_shift[] = {
    30,  // GiB
    20,  // MiB
    10,  // KiB
    0,   // B
};

static const char *storage_unit[] = {"GiB", "MiB", "KiB", "B"};

void print_file_size(size_t size)
{
    int pow2 = sizeof(size_t) * 8 - __builtin_clzl(size);

    // choose proper measurement unit
    int idx;
    for (idx = 0; idx < 4; idx++) {
        if (pow2 > storage_shift[idx]) {
            break;
        }
    }

    // print out file size
    size_t quotient = size >> storage_shift[idx];
    printf("File size: %lu %s\n", quotient, storage_unit[idx]);
}
