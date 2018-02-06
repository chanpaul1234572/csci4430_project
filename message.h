#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct message_s{
    unsigned char protocol[5];          //protocol string(MYFTP)
    unsigned char type;                 //type (1bytes)
    unsigned int length;                //length (header + payload) (4 bytes)
} message __attribute__((packed));


