#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

int read_info(file_info_t *info, char *filename)
{
    struct stat statbuf;
    int ret = stat(filename, &statbuf);
    if (ret < 0) {
        perror("[Error] File not found\n");
        return -1;
    }

    info->size = statbuf.st_size;

    int idx = 0;
    for (int i = strlen(filename) - 1; i >= 0; i--) {
        if (filename[i] == '/') {
            idx = ++i;
            break;
        }
    }
    strcpy(info->name, &filename[idx]);

    return 0;
}
