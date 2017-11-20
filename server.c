#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "util.h"

#define PORT 3000

int main() {


    // read DB.
    FILE *fp;
    if ((fp = fopen("/home/juan/Downloads/assignment_2/DB.txt", "r")) == NULL) {
        printf("Cannot open file, press any key exit!");
        exit(1);
    }
    int dbSize;
    fscanf(fp, "%d", &dbSize);
    printf("Read %d records from DB\n", dbSize);
    SUBSCRIBER db[dbSize];
    unsigned long subNo;
    int tech;
    short paid;
    for (int i = 0; i < dbSize; ++i) {
        fscanf(fp, "%lu %d %hd", &subNo, &tech, &paid);
        printf("subNo: %lu\n", subNo);
        db[i].subNo = subNo;
        db[i].tech = (Technology) tech;
        db[i].paid = paid;
    }
    fclose(fp);


    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        perror("create socket error: ");
        exit(1);
    }
    struct sockaddr_in serv, client;
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(PORT);
    if (bind(s, (struct sockaddr *) &serv, sizeof(serv)) == -1) {
        perror("bind error: ");
        exit(1);
    } else {
        printf("bind server successfully\n");
    }

    // start listening, receive access request from client
    while (1) {
        char buff[BUFF_SIZE];
        int addr_len = sizeof(client);
        int ret = recvfrom(s, buff, BUFF_SIZE, 0, (struct sockaddr *) &client,
                           &addr_len); // once success, we get client.
        if (ret == -1) {
            perror("recvfrom error: ");
            break;
        }
        buff[ret] = 0;
        if (strcmp(buff, "end\n") == 0) {
            printf("server process end.\n");
            break;
        }

        // unpack received request, store in DATA_PACK receivedDataPack
        DATA_PACK receivedDataPack;
        unpack(buff, ret, &receivedDataPack);
        printf("receive message from client: %d, with seq num: %d, request tech: %d for subscriber no %lu\n",
               receivedDataPack.clientId,
               receivedDataPack.segNum, receivedDataPack.tech, receivedDataPack.subNo);


        // check database, update paid info if find matched record
        int paid = -1;
        for (int i = 0; i < dbSize; ++i) {
            SUBSCRIBER subscriber = db[i];
            if (subscriber.subNo == receivedDataPack.subNo && subscriber.tech == receivedDataPack.tech) {
                paid = subscriber.paid;
                break;
            }
        }

        // update packType info according to paid info
        switch (paid) {
            case 0:
                receivedDataPack.packType = NOT_PAID;
                break;
            case 1:
                receivedDataPack.packType = ACK_OK;
                break;
            default:
                receivedDataPack.packType = NOT_EXIST;
        }

        // call builPack to generate response and send back to client
        int packLen = buildPack(receivedDataPack, buff);

        if (sendto(s, buff, packLen, 0, (struct sockaddr *) &client, addr_len) == -1) {
            perror("sendto error: ");
            break;
        }
    }
    close(s);
    return 0;
}
