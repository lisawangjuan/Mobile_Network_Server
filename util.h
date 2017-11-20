#ifndef UTIL_H_   /* Include guard */
#define UTIL_H_

#include <assert.h>

const unsigned short int PACKET_IDENTIFIER_START = 0XFFFF;
const unsigned short int PACKET_IDENTIFIER_END = 0XFFFF;

const int BUFF_SIZE = 512;
const int CLIENT_TIMEOUT_SECOND = 3;
const int CLIENT_RETRY_TIMES = 3;

// common offsets
const int OFFSET_IDENTIFIER_START = 0;
const int OFFSET_CLEINT_ID = 2;

// data packet offsets
const int OFFSET_PACK_TYPE = 3;
const int OFFSET_SEGMENT_NO = 5;
const int OFFSET_LENGTH = 6;
const int OFFSET_PAYLOAD = 7;

typedef enum {
    ACC_PER = 0XFFF1, // access permission
    ACK_OK = 0XFFF2,
    NOT_EXIST = 0XFFF3,
    NOT_PAID = 0XFFF4,
} PACK_TYPE;

typedef enum {
    T_2G = 2,
    T_3G = 3,
    T_4G = 4,
    T_5G = 5,
} Technology;

typedef struct {
    int clientId;
    PACK_TYPE packType;
    int segNum;
    int payloadLen;
    Technology tech;
    unsigned long subNo; // subscriber no.
} DATA_PACK;

typedef struct {
    unsigned long subNo; // subscriber no.
    Technology tech;
    short paid;
} SUBSCRIBER;

/**
 * Builds data package.
 * @return length of the whole package.
 */
int buildPack(DATA_PACK dataPack, char *buff) {
    memcpy(buff + OFFSET_IDENTIFIER_START, (char *) &PACKET_IDENTIFIER_START, 2);
    memcpy(buff + OFFSET_CLEINT_ID, (char *) &dataPack.clientId, 1);
    memcpy(buff + OFFSET_PACK_TYPE, (char *) &dataPack.packType, 2);
    memcpy(buff + OFFSET_SEGMENT_NO, (char *) &dataPack.segNum, 1);
    memcpy(buff + OFFSET_LENGTH, (char *) &dataPack.payloadLen, 1);
    memcpy(buff + OFFSET_PAYLOAD, (char *) &dataPack.tech, 1);
    memcpy(buff + OFFSET_PAYLOAD + 1, (char *) &dataPack.subNo, 4);
    int endOffset = OFFSET_PAYLOAD + dataPack.payloadLen;
    memcpy(buff + endOffset, (char *) &PACKET_IDENTIFIER_END, 2);
    return endOffset + 2;
}

/**
 * Unpacks data package.
 * @return Unpacked DATA_PACK.
 */
void unpack(char *buff, int buffLen, DATA_PACK *dataPack) {
    dataPack->clientId = (int) *(buff + OFFSET_CLEINT_ID);
    memcpy((char *) &dataPack->packType, buff + OFFSET_PACK_TYPE, 2);
    dataPack->segNum = (int) *(buff + OFFSET_SEGMENT_NO);
    dataPack->payloadLen = (int) *(buff + OFFSET_LENGTH);
    assert(dataPack->payloadLen == 5); // sanity check
    memcpy(&dataPack->tech, buff + OFFSET_PAYLOAD, 1);
    memcpy(&dataPack->subNo, buff + OFFSET_PAYLOAD + 1, 4);
}

#endif