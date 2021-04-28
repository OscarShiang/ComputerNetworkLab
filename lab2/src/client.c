#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

int main(void)
{
    // open a datagram socket
    printf("Opening datagram socket...");
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("error\n");
        perror("[Error] fail to open a socket");
        exit(-1);
    }
    printf("OK\n");

    // set socket option
    printf("Setting SO_REUSEADDR...");
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
        0) {
        printf("error\n");
        perror("[Error] fail to set reuse address");
        exit(-1);
    }
    printf("OK\n");


    // bind the proper address to the socket
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(8083);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    printf("Binding datagram socket...");
    if (bind(sockfd, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0) {
        printf("error\n");
        perror("[Error] binding failed");
        exit(-1);
    }

    printf("OK\n");

    // join the group
    struct ip_mreq group = {
        .imr_multiaddr.s_addr = inet_addr("226.1.1.1"),
        .imr_interface.s_addr = inet_addr("127.0.0.1"),
    };
    printf("Adding multicast group...");
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group,
                   sizeof(group)) < 0) {
        printf("error\n");
        perror("[Error] fail to add into the group");
        exit(-1);
    }

    printf("OK\n");

    // receive the file_info
    file_info_t info;
    if (recvfrom(sockfd, &info, sizeof(info), 0, NULL, NULL) < 0) {
        close(sockfd);
        perror("[Error] Fail to receive file info\n");
        exit(-1);
    }

    DEBUG("the file name: %s\n", info.name);

    int fd = open(info.name, O_WRONLY | O_CREAT, info.mode);
    if (fd < 0) {
        perror("[Error] fail to open the specific file");
        exit(-1);
    }

    size_t recv_bytes = 0;
    char buf[BUF_SIZE];
    int ret;

    DEBUG("start to receive data\n");
    // TODO: write the file to the filename
    while (1) {
        ret = recvfrom(sockfd, buf, BUF_SIZE, 0, NULL, NULL);
        if (ret < 0 || !strncmp(buf, UDP_ACK, 4))
            break;

        write(fd, buf, ret);
        recv_bytes += ret;
    }

    print_file_size(recv_bytes);

    close(sockfd);
    close(fd);

    return 0;
}
