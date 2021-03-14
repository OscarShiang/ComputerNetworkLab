#ifndef FILE_TRANS_H
#define FILE_TRANS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stddef.h>

/* UDP starting header */
#define UDP_START "hi"
#define UDP_ACK "ack"

ssize_t tcp_transfer(int from, int to, size_t size);
ssize_t udp_transfer(int from, int to, size_t size, int type, struct sockaddr_in addr);

#endif /* FILE_TRANS_H */
