#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<dirent.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<dirent.h>
#include "myftp.h" 

/*
->error log change later
->listen() max pending N ...?
header 10bytes 514bits
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
	const int header_size = 15;
	if((len = recv(client_sd,buf,sizeof(buf),0)) < 0){
		printf("Receive Error!");
	}else{
		if(buf->type == 0xA1){ //list
				struct dirent *pStEntry = NULL;
				struct dirent *pStResult = NULL;
				pStEntry = malloc(sizeof(struct dirent));
  	 			pStResult = malloc(sizeof(struct dirent));

				DIR *pDir;
				char temp[1024];
				char *lsentry = malloc(sizeof(char) * 1024);
				pDir = opendir("./data");
				if(NULL == pDir){
     				perror("Opendir failed!\n");
      				return 1;
 	 			}
 	 			while(!readdir_r(pDir, pStEntry, &pStResult) && pStResult != NULL){
  					if((strcmp(pStEntry->d_name, ".") != 0) && (strcmp(pStEntry->d_name, "..") != 0)){
  						strcpy(temp,pStEntry->d_name);
  			    		strcat(lsentry,temp);
  			    		strcat(lsentry,"\n");
  					}
  					free(pStEntry);
  					free(pStResult);
  					closedir(pDir);
  					
  				}

		}
		else
			if(buf->type == 0xB1){	//get
				FILE *fp;
				char fname[100];
				char server_path[8] = "./data "; 
				char fpath[108];
				fpath = strcat(server_path,fname);
				//reply GET_REPLY
				if(fp = fopen(fpath ,"r")!=NULL){  /
					printf("File doesn't exists\n");
					message_to_send -> type = 0xB3;
					message_to_send -> length = header_size;
					if(send(client_sd, message_to_send,sizeof(message_to_send),0)==-1){
					perror("sending error");
					}
				}else{
					printf("File:%s exists, prepare for download\n");
					message_to_send -> type = 0xB2;
					message_to_send -> length = header_size;
					if(send(client_sd, message_to_send,sizeof(message_to_send),0)==-1){
						perror("sending error");
					}
					//FILE-DATA
					message_to_send -> type = 0xFF;
					message_to_send -> length = header_size + file_size;
					message_to_send-> payload = file; //?
					if(send(client_sd, message_to_send,sizeof(message_to_send),0)==-1){
					perror("sending file error");
					}

					fclose(fp);
				}
			}
		else
			if(buf->type == 0xC1){	//put

				//reply PUT_REPLY

				//wait for FILE_DATA
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
	struct message* message_to_send;
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
	int connection_count=0;
	//accept 
	while(1){

		if(client_sd = accept(sd, (struct sockaddr*) &client_addr, &addrlen)==-1){
				printf("Accept error: %s (Errno:%d)\n", strerror(errno), errno);
				exit(0);
		}else{
			//open worker thread
			connection_count++;
			printf("count->%d", connection_count);
			buf = ntohl(buf);
			clientsd_p = malloc(sizeof(int)); 
			clientsd_p = client_sd;
			if(pthread_create(&threadid, NULL, workerthread, clientsd_p)==-1){
				printf("create thread error: %s (Errno:%d)\n", strerror(errno), errno);
				free(clientsd_p);
				// free(client_addr);
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
