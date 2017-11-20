Name Juan Wang
Student ID: W1271878

COEN 233 Computer Networks
Assignment 2


Solution explanation
1. create database text file DB.txt, which stores 3 subscriber users information

2. make a headfile util.h. 
 1) Set const variables and offsets, define enum type PACK_TYPE, Technology, struct type DATA_PACK and SUBSCRIBER.
 2) Define two functions, buildPack() and unpack(). 
 3) buildPack() takes a data packet (DATA_PACK) and a char pointer to a buff as parameters. Calling it copy info from the data packet to buff and return the length of data packet.
 4) unpack() takes three parameters, a char pointer to buff, buff length and a data packet. It copies info from buff to data packet.

3. For server:
 1) open the DB.txt to get subscribers information and store them in an subscribers array.
 2) make a socekt and bind to a server, check binding error.
 3) start listening. When received packet from client, call unpack() to store recevied info to a packet variable receivedDataPack. Check in DB.txt to see if receivedDataPack matches any record in the file and update the paid info of the packet accordingly. 
 4) Based on paid info, update packet packtype correspondently to NOT_PAID, AC_OK and NOT_EXIST.
 5) call buildPack() to generate ack packet, send it back to client.

4. For client
 1) create socket.
 2) set read timeout, initialize server info.
 3) get subscriber number and tech info from user input, generate access request packet dataPack.
 4) call buildPack() to pass dataPack info to buff.
 5) send buff info to server.
 6) during valid retry period, receive ack packet from server.
 7) call unpack() to store received ack packet in buff to a new DATA_PACK ackDataPack.
 8) print server response according to packtype.


Assumptions:
1. default paid value is set to -1, which means NOT_EXIST.
2. when server checks record in DB.txt, both subscriber number and tech info are checked. If both info machtes, update paid value of access request packet to paid value of record. Otherwise do not update paid info.


Running results:
Results of defferent cases are shown in the snapshot file.

Thanks for reading!