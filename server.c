#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<myftp.h>

/*
->error log change later
->listen() max pending N ...?
*/

int main(int argc,char** argv){

	unsigned short port;

	if(argc != 2){
		printf("Port number missing\n");
		exit(1);
	}	
	port = atoi(argv[1]); //port num receive 
	pthread_t 
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	int client_sd, buf; 
	struct sockaddr_in server_addr, client_addr;
	unsigned int addrlen = sizeof(client_addr);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin.family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	if(bind(sd, (struct sockaddr*) &server_addr, sizeof(server_addr))==-1){
		printf("Bind error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(1);
	}

	if(listen(sd, 999) == -1){ //MAXPENDING N later
		printf("Listen error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	//accept 
	while(1){
		if(client_sd = accept(sd, (struct sockaddr*) &client_addr, &addrlen)==-1){
				printf("Accept error: %s (Errno:%d)\n", strerror(errno), errno);
				exit(0);
		}else{
			

		}
	}
	close(sd);
	return 0;
}