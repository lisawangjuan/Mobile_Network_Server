#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "util.h"

#define PORT 3000

const unsigned short int CLIENT_ID = 1;

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0); // create sock
    // check if sock has successfully created
    if (sock == -1) {
        perror("create socket error: ");
        exit(1);
    }
    // set read timeout.
    struct timeval read_timeout;
    read_timeout.tv_sec = CLIENT_TIMEOUT_SECOND;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout)) < 0) {
        perror("Error setting timeout.");
    }

    struct sockaddr_in serv;
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(PORT);
    char buff[BUFF_SIZE];

    int segNo = 0;
    while (1) {
        unsigned long phone_number;
        int tech;
        scanf("%lu %d", &phone_number, &tech);
        segNo++;

        // send one-off request to server
        DATA_PACK dataPack;
        dataPack.clientId = CLIENT_ID;
        dataPack.packType = ACC_PER;
        dataPack.segNum = segNo;
        dataPack.payloadLen = 5; // 1 byte tech, and 4 bytes subno.

        dataPack.tech = (Technology)tech; // subscriber no
        dataPack.subNo = phone_number; // subscriber no

        // generate access request packet, send it to server
        int packLen = buildPack(dataPack, buff);
        sendto(sock, buff, packLen, 0, (struct sockaddr *) &serv, sizeof(serv));
        if (strcmp(buff, "end\n") == 0) {
            printf("client process end.\n");
            return 0;
        }
        int addr_len = sizeof(serv);

        // during valid time period, try to receive response from server
        int retry = CLIENT_RETRY_TIMES;
        int ret;
        while (retry > 0) {
            ret = recvfrom(sock, buff, BUFF_SIZE, 0, (struct sockaddr *) &serv, &addr_len);
            if (ret < 0) {
                perror("recvform error");
                retry -= 1;
                continue;
            } else { break; }
        }
        if (ret < 0) {
            fprintf(stderr, "Server does not respond");
            exit(1);
        }
        DATA_PACK ackDataPack;
        unpack(buff, ret, &ackDataPack);

        // handle response
        switch (ackDataPack.packType) {
            case ACK_OK:
                printf("receive ACK_OK from server for segment %d\n", ackDataPack.segNum);
                break;
            case NOT_EXIST:
                printf("receive NOT_EXIST from server for segment %d\n", ackDataPack.segNum);
                break;
            case NOT_PAID:
                printf("receive NOT_PAID from server for segment %d\n", ackDataPack.segNum);
                break;
        }
    }

    close(sock);
    return 0;
}
