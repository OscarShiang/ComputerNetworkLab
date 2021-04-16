#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

static void print_usage()
{
    printf(
        "Usage: server [FILE]\n"
        "    File: the file to be transfered\n");
    exit(-1);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
	print_usage();
    }

    // TODO: implement multicast server to handle several clients

    return 0;
}
