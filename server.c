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
#include<message.h> //remember change name into myftp.h

/*
->error log change later
->listen() max pending N ...?
*/

void *workerthread(int *clientsd_p){
	int client_sd =(int*)clientsd_p; 
	free(clientsd_p);
    pthread_detach(pthread_self());

	clientconnection(client_sd); //handlle client connection 

	return(NULL);
}

//Handling client requests
void clientconnection(int client_sd){

	if((len = recv(client_sd,buf,sizeof(buf),0)) < 0){
		printf("Receive Error!");
	}else{
		if(buf->type == 0xA1){
				//list
			//working..
		}
		else
			if(buf->type == 0xB1){
					//get
			}
		else
			if(buf->type == 0xC1){
					//put
			}
		else
			printf("Wrong request!");
	}
	pthread_exit(NULL);
	close(sd);
	return 0;
}

int main(int argc,char** argv){

	unsigned short port;

	if(argc != 2){
		printf("Port number missing\n");
		exit(1);
	}	
	port = atoi(argv[1]); //port num receive 
	pthread_t threadid; 
	int *clientsd_p; //clinet socket descriptor pointer
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
			//open worker thread
			clientsd_p = malloc(sizeof(int client_sd)); 
			clientsd_p = client_sd;
			if(pthread_create(&threadid, NULL, workerthread, clientsd_p)==-1){
				printf("create thread error: %s (Errno:%d)\n", strerror(errno), errno);
				free(clientsd_p);
				free(client_addr);
				close(sd);
				close(client_sd);
				pthread_exit(NULL);
			}
		}
	}
	close(sd);
	return 0;
	//End of server process
}
