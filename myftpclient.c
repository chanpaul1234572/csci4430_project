#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include "myftp.h"

long size_of_the_file(FILE* file){
	long size = 0;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return size;
}

int sendMsg(int sd, char *buff, int len)
{
	int recvLen = 0;
	while (recvLen != len)
	{
		int rLen = send(sd, buff + recvLen, len - recvLen, 0);
		if (rLen <= 0)
		{
			fprintf(stderr, "error sending msg\n");
			return 1;
		}
		recvLen += rLen;
	}
	return 0;
}

void request_prepare(struct message* message_to_send, char* cmd, char* file, int payload_size){
	const int header_size = 15;
	strcpy(message_to_send -> protocol , "myftp");
	if(strcmp("list", cmd) == 0){
		message_to_send -> type = 0xA1;
		message_to_send -> length = header_size;
	}
	else if(strcmp("get", cmd) == 0){
		message_to_send -> type = 0xB1;
		message_to_send -> length = header_size + payload_size;
	}
	else if(strcmp("PUT", cmd) == 0){
		message_to_send -> type = 0xC1;
		messgae_to_send -> length = header_size + payload_size;
	}
}


int main(int argc, char** argv){
		if(argc != 5 &&  argc != 4){
				printf("The number of argument are not correct\n");
				exit(0);
		}
		const int port = atoi(argv[2]);
		int sd = socket(AF_INET, SOCK_STREAM, 0);
		if(strcmp(argv[3], "list") != 0){
				char pathname[100] = "\0";
				strcat("./", pathname);
				strcat(pathname, argv[4]);
				FILE* file = NULL;		
				file = fopen(pathname, "r");
				if(!file){
						printf("Can not open file: %s\n", strerror(errno));
						exit(0);
				}
		}
		else{
				pathname[0] = "\0";
		}
		char *cmd = argv[3];
		char* ipstr = argv[1];
		printf("%s", ipstr);
		struct sockaddr_in server_addr;
		if (sd < 0){
				printf("Can not create socket :%s\n", strerror(errno));
				exit(0);
		}
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr=inet_addr(ipstr);
		server_addr.sin_port = htons(port);
		if(connect(sd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
       		printf("connection error: %s (Errno:%d)\n", strerror(errno), errno);
      		exit(0);
		}
		printf("connected to the server: %s\n", inet_ntoa((server_addr.sin_addr)));
		while(1){
			struct message_s message_to_send;
			long lsize = size_of_the_file(file);
			request_prepare(&message_to_send, cmd, file, lsize);
			char* buf = NULL;
			char end[4] = "\0";
			buf = (char*) malloc(sizeof(message));
			long hsize = sizeof(message);
			bzero(buf, sizeof(message));
			memcpy(buf, message_to_send.protocol, sizeof(message_to_send.protocol));
			memcpy(buf + sizeof(message_to_send.protocol), message_to_send.type, sizeof(message_to_send.type));
			memcpy(buf + (sizeof(message_to_send.protocol) + sizeof(message_to_send.type), message_to_send.length, sizeof(message_to_send.length)));
			printf("Size of buf : %d\n", sizeof(buf));
			getchar();
			long sent_size = 0;
			if(hsize > (sent_size = send(sd, buf, hsize, 0))){
					if(sent_size < 0){
						printf("Send failed: %s\n", (strerror(errno)));
					}
					else{
							printf("Send incomplete\n");
					}
			}
			else
			{
					printf("Send successfully!\n");
			}
		fgets(end, 4, stdin);
		if(strcmp(end. "end") == 0){
				printf("end\n");
				close(sd);
				printf("Connection closed\n");
		}
	}
	return 0;
}		



