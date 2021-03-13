#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"
#include "file_trans.h"

static int protocol;
static int func;

void print_usage()
{
    printf(
        "Usage: lab1 [PROTOCOL] [FUNC] [IP] [PORT] [FILE]\n"
        "Options:\n"
        "    Protocol: TCP / UDP\n"
        "    Function: send / recv\n"
        "    IP: IP address\n"
        "    Port: port number\n"
        "    File: file to transfer\n");
    exit(-1);
}

int main(int argc, char *argv[])
{
    // check if the arguments is not sufficient
    if (argc < 5)
        print_usage();

    // parse the protocol and functionality from args
    if (!strcmp(argv[1], "tcp"))
        protocol = TCP;
    else
        protocol = UDP;

    if (!strcmp(argv[2], "send"))
        func = SEND;
    else
        func = RECV;

    DEBUG("Open socket fd\n");
    // open socket fd
    int sockfd;
    if (protocol == TCP)
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    else
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // setting address
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(argv[3]),
        .sin_port = htons(atoi(argv[4])),
        .sin_zero = {0},
    };

    // bind the address and start to send / receive
    if (func == SEND) {
	int clientfd;

        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            perror("[Error] Address binding failed\n");
            exit(-1);
        }

        // read the file size
        struct stat statbuf;
        int ret = stat(argv[5], &statbuf);
        if (ret < 0) {
            perror("[Error] File not found\n");
            exit(-1);
        }
        file_info_t file = {
            .size = statbuf.st_size,
            .mode = statbuf.st_mode,
        };
        strcmp(file.name, argv[5]);

	if (protocol == TCP) {
	    DEBUG("listen from socket\n");
	    if (listen(sockfd, 5) < 0) {
		perror("[Error] Failed to listen\n");
		exit(-1);
	    }

	    // accept the incoming connection
	    struct sockaddr_in clientaddr;
	    socklen_t inlen = 1;
	    clientfd = accept(sockfd, (struct sockaddr *) &clientaddr, &inlen);
	    if (clientfd < 0) {
		perror("[Error] Acception failed\n");
		exit(-1);
	    }
	} else {
	    // for UDP case
	    DEBUG("wait for connction\n");
	    int inlen = sizeof(struct sockaddr);

	    char buf[4];
	    int ret = recvfrom(sockfd, buf, 16, 0, (struct sockaddr *)&addr, &inlen);
	    if (ret < 0) {
		perror("[Error] Wrong starting header\n");
		exit(-1);
	    }
	    buf[ret] = '\0';
	    printf("test: %s\n", buf);
	    if (strcmp(buf, UDP_START)) {
		perror("[Error] Wrong starting header\n");
		exit(-1);
	    }
	    DEBUG("Received client start\n");
	}

	DEBUG("start to send the file\n");
	int fd = open(argv[5], O_RDONLY);

	if (protocol == TCP) {
	    // send file info
	    write(clientfd, &file, sizeof(file));

	    // send the data
	    if (tcp_transfer(fd, clientfd, file.size) < 0)
		perror("[Error] Transfer failed\n");
	} else {
	    // for UDP case
	    DEBUG("start to transfer file by UDP\n");
	    if (udp_transfer(fd, sockfd, file.size, SEND, addr) < 0)
		perror("[Error] Transfer failed\n");
	}
	// close the fds
	close(clientfd);
	close(fd);
    } else {
	// RECV functionality
	DEBUG("connect to the server\n");
	if (protocol == TCP) {
	    // connect to the server
	    int ret = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));
	    if (ret < 0) {
		perror("[Error] Connection failed\n");
		exit(-1);
	    }
	} else {
	    // send UDP starting header
	    if (sendto(sockfd, UDP_START, 2, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("[Error] Failed to send starting header\n");
		exit(-1);
	    }
	    DEBUG("client hello sended\n");
	}
	
        // get file info
        file_info_t file;
	if (protocol == TCP) {
	    if (read(sockfd, &file, sizeof(file)) < 0) {
		perror("[Error] Failed to receive the meta data\n");
		exit(-1);
	    }
	} else {
	    int inlen = sizeof(struct sockaddr);
	    DEBUG("wait for file info\n");
	    if (recvfrom(sockfd, &file, sizeof(file), 0, (struct sockaddr *)&addr, &inlen) < 0) {
		perror("[Error] Failed to receive the meta data\n");
		exit(-1);
	    }
	}

        // open file
        int fd = open(argv[5], O_WRONLY | O_CREAT, 0755);
        if (fd < 0) {
            perror("[Error] Fail to create the file\n");
            exit(-1);
        }
        DEBUG("create the file\n");

        // receive the data
        DEBUG("start to transfer the data\n");
	if (protocol == TCP) {
	    if (tcp_transfer(sockfd, fd, file.size) < 0)
		perror("[Error] Transfer failed\n");
	} else {
	    if (udp_transfer(sockfd, fd, file.size, RECV, addr) < 0)
		perror("[Error] Transfer failed\n");
	}
        close(fd);
    }

    close(sockfd);

    return 0;
}
