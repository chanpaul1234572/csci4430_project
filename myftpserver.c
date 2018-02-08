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
#include<dirent.h>
// #include "myftp.h" 

int sd;
void clientconnection(void *x, int y);

void *workerthread(void *x){
	int accept_fd = *(int *)x;

	// int **client_sd = &clientsd_p;
	 // printf("hello handling client - %p \n", clientsd_pp);
	// free(clientsd_p);
    // pthread_detach(pthread_self());
	 printf("server_sd=%d\n", accept_fd);
	// clientconnection((void*)clientsd_pp, sd); //handlle client connection 
}

//Handling client requests
void clientconnection(void *clientsd_pp, int sd){
	printf("blocking session for %p\n", clientsd_pp);
	const int header_size = 15;
	int recedata;
	char buf[10000];
	int clientsd = *(int*)clientsd_pp;
	if((recedata = recv(clientsd,buf,sizeof(buf),0)) < 0){
		printf("Receive Error!\n"); 
	}
	printf("%s", buf);

	close(clientsd);
	free(clientsd_pp);
	pthread_exit(NULL);
}

int main(int argc,char** argv){

	unsigned short port;

	if(argc != 2){
		printf("Port number missing\n");
		exit(1);
	}	
	port = atoi(argv[1]); //port num receive 
	pthread_t threadid[999]; 
	sd = socket(AF_INET, SOCK_STREAM, 0);
	int *clientsd_p[999]; //clinet socket descriptor pointer
	int client_sd[999], buf; 
	struct sockaddr_in server_addr, client_addr;
	unsigned int addrlen = sizeof(client_addr);
	struct message* message_to_send;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
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
	int connection_count=0;
	//accept 
	for(int i=1; i <=999; i++){
		//loop
		printf("--------Press crtl+c to stop the server---------\n");

		if((client_sd[i] = accept(sd, (struct sockaddr*) &client_addr, &addrlen))==-1){
			printf("Accept error: %s (Errno:%d)\n", strerror(errno), errno);
			exit(0);
		}else{
			//open worker thread
			connection_count++;
			printf("count->%d i=%d\n", connection_count,i);
			// printf("server sd:%d\n", sd );
			// clientsd_p[i] = malloc(sizeof(client_sd));
			printf("client sd:%d\n", client_sd[i]);
			// *clientsd_p[i] = client_sd[i];
			// *clientsd_p[i] = client_sd[i];
			// printf("pointer - %p\n", (void *)clientsd_p[i]); 
			// printf("pointer stored value - %d\n", clientsd_p[i]);

			// buf = ntohl(buf);
			if(pthread_create(&threadid[i], NULL, workerthread,&client_sd[i])!=0){
				printf("create thread error: %s (Errno:%d)\n", strerror(errno), errno);

			}
		}
	}
	close(sd);
	return 0;
	}
	