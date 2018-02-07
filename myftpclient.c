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
		const int port = atoi(argv[2]);
		int sd = socket(AF_INET, SOCK_STREAM, 0);
		char pathname[100] = void;
		strcat("./", pathname);
		strcat(pathname, argv[4]);
		int fd = open(pathname, O_CREAT);
		char* ipstr = argv[1];
		struct sockaddr_in server_addr;
		if (sd < 0){
				errexit("Can not create socket :%s\n", strerror(errno));
		}
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr=inet_addr(ipstr);
		server_addr.sin_port = htons(port);
		if(connect(sd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
       		printf("connection error: %s (Errno:%d)\n", strerror(errno), errno);
      		exit(0);
		}
		printf("connected to the server: %s\n", inet_ntoa((server_addr.sin_addr.s_addr)));
	return 0;
}		



