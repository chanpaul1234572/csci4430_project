#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>


int main(int argc, char** argv){
		const int port = atoi(argv[2]);
		int sd = socket(AF_INET, SOCK_STREAM, 0);
		char* ipstr = argv[1];
		struct sockaddr_in server_addr;
		if (sd < 0){
				errexit("Can not create socket :%s\n", strerror(errno));
		}
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr=inet_addr(ipstr);
		server_addr.sin.port = htons(port);
		if(connect(sd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
       		printf("connection error: %s (Errno:%d)\n", strerror(errno), errno);
      		exit(0);
		}
		printf("connected to the server: %s\n", inet_ntoa(server_addr.sin_addr.s_addr));
		

