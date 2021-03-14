#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "file_trans.h"
#include "utils.h"

ssize_t tcp_transfer(int from, int to, size_t size)
{
    int ret;
    char buf[BUF_SIZE];
    size_t bytes = 0;
    int quarter = 0;

    time_t t;
    struct tm tm;

    struct timeval start, end;

    gettimeofday(&start, NULL);
    while ((ret = read(from, buf, BUF_SIZE))) {
        ret = write(to, buf, ret);
        if (ret < 0)
            return ret;
        bytes += ret;

        // print log
        if (bytes > ((float) size) / 4 * quarter) {
            t = time(NULL);
            tm = *localtime(&t);

            printf("[Info] %-3d%% %d/%02d/%02d %02d:%02d:%02d\n", 25 * quarter,
                   tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
                   tm.tm_min, tm.tm_sec);
            quarter++;
        }
    }
    gettimeofday(&end, NULL);
    printf("[Info] %-3d%% %d/%02d/%02d %02d:%02d:%02d\n", 25 * quarter,
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
           tm.tm_sec);
    DEBUG("transfer completed\n");

    printf("[Info] Total transfer time: %lu ms\n",
           1000 * (end.tv_sec - start.tv_sec) +
               (end.tv_usec - start.tv_usec) / 1000);
    printf("[Info] File size: %lu MB\n", bytes >> 20);

    return bytes;
}

ssize_t udp_transfer(int from,
                     int to,
                     size_t size,
                     int type,
                     struct sockaddr_in addr)
{
    int ret;
    char buf[BUF_SIZE];
    size_t bytes = 0;
    int quarter = 0;

    time_t t;
    struct tm tm;

    char msg[4] = {0};
    int msglen;
    int inlen = 1;

    int packetlen;

    struct timeval start, end;

    if (type == SEND) {
        gettimeofday(&start, NULL);
        while ((ret = read(from, buf, BUF_SIZE))) {
            // wait for ack
            msglen = recvfrom(to, msg, 4, 0, (struct sockaddr *) &addr, &inlen);
            msg[msglen] = '\0';
            if (strcmp(msg, UDP_ACK)) {
                perror("[Error] failed to receive ack from client\n");
                return -1;
            }

            // inform the clinet the length of the data
            sendto(to, &ret, sizeof(ret), 0, (struct sockaddr *) &addr,
                   sizeof(struct sockaddr));

            ret = sendto(to, buf, ret, 0, (struct sockaddr *) &addr,
                         sizeof(struct sockaddr_in));
            if (ret < 0)
                return ret;
            bytes += ret;

            // print log
            if (bytes > ((float) size) / 4 * quarter) {
                t = time(NULL);
                tm = *localtime(&t);

                printf("[Info] %-3d%% %d/%02d/%02d %02d:%02d:%02d\n",
                       25 * quarter, tm.tm_year + 1900, tm.tm_mon + 1,
                       tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                quarter++;
            }
        }
        // send terminate sign
        sendto(to, &ret, sizeof(ret), 0, (struct sockaddr *) &addr,
               sizeof(struct sockaddr_in));
    } else {
        // for RECV case
        gettimeofday(&start, NULL);
        while (1) {
            // send ack to the server
            ret = sendto(from, UDP_ACK, 4, 0, (struct sockaddr *) &addr,
                         sizeof(struct sockaddr_in));
            msglen = recvfrom(from, &packetlen, sizeof(packetlen), 0,
                              (struct sockaddr *) &addr, &inlen);
            if (!packetlen)
                break;

            ret = recvfrom(from, buf, packetlen, 0, (struct sockaddr *) &addr,
                           &inlen);
            ret = write(to, buf, ret);
            if (ret < 0)
                return ret;
            bytes += ret;

            // print log
            if (bytes > ((float) size) / 4 * quarter) {
                t = time(NULL);
                tm = *localtime(&t);

                printf("[Info] %-3d%% %d/%02d/%02d %02d:%02d:%02d\n",
                       25 * quarter, tm.tm_year + 1900, tm.tm_mon + 1,
                       tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                quarter++;
            }
        }
        DEBUG("transfer completed\n");
    }
    gettimeofday(&end, NULL);

    printf("[Info] %-3d%% %d/%02d/%02d %02d:%02d:%02d\n", 25 * quarter,
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
           tm.tm_sec);
    DEBUG("transfer completed\n");

    printf("[Info] Total transfer time: %lu ms\n",
           1000 * (end.tv_sec - start.tv_sec) +
               (end.tv_usec - start.tv_usec) / 1000);
    printf("[Info] File size: %lu MB\n", bytes >> 20);

    return bytes;
}
