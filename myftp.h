#ifndef MESSAGE_H
#define MESSAGE_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include "myftp.h"


struct message_s{
    unsigned char protocol[5];          //protocol string(MYFTP)
    unsigned char type;                 //type (1bytes)
    unsigned int length;                //length (header + payload) (4 bytes)
} __attribute__((packed));

typedef struct message_s message;

//common functions shared by server and function 
//header decode encode
long size_of_the_file(FILE *file)
{
	long size = 0;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return size;
}

#endif
