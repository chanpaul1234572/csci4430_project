#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<dirent.h>
#include "myftp.h" 

int sd; //server sd

//long size_of_the_file(FILE *file)
//{
//	long size = 0;
//	fseek(file, 0, SEEK_END);
//	size = ftell(file);
//	fseek(file, 0, SEEK_SET);
//	return size;
//}

void *workerthread(void *x){
	int accept_fd = *(int *)x;
	struct message_s* REQUEST = NULL;
	struct stat filestat;
	char dat[100000];
	FILE *fp;
	int numbytes;
	const int header_size = 10;
	char filename[1000];
	REQUEST = (struct message_s*) malloc(sizeof(struct message_s));
	if(recv(accept_fd,REQUEST,sizeof(struct message_s),0) < 0){
		printf("Receive Error");
		pthread_exit(NULL);
	}
	else{
		fflush(stdout);
	}
	if(REQUEST->type == 0xA1){
		printf("LIST receive!\n");
		struct dirent *pStEntry = NULL;
		struct dirent *pStResult = NULL;
		DIR *pDir;
		char temp[1024];
		char *lsentry = NULL;
		lsentry = (char *)malloc(1000);
		bzero(lsentry ,1000);
		pDir = opendir("./data");

		if(NULL == pDir){
			printf("Opendir failed!\n");
			pthread_exit(NULL);
		}
		pStEntry = malloc(sizeof(struct dirent));
		pStResult = malloc(sizeof(struct dirent));
		while(! readdir_r(pDir, pStEntry, &pStResult) && pStResult != NULL)
		{
			if((strcmp(pStEntry->d_name, ".") != 0) && (strcmp(pStEntry->d_name, "..") != 0)){
				strcpy(temp,pStEntry->d_name);
				strcat(lsentry,temp);
				strcat(lsentry,"\n");
			}
		}
		strcat(lsentry,"\0");
		// printf("%s",lsentry);
		free(pStEntry);
		free(pStResult);
		closedir(pDir);

		struct message_s* LIST_REPLY;
		LIST_REPLY = (struct message_s*) malloc(sizeof(struct message_s));
		strcpy(LIST_REPLY -> protocol , "myftp");
		LIST_REPLY -> type = 0xA2;
		LIST_REPLY -> length = header_size + strlen(lsentry);
		if((send(accept_fd,LIST_REPLY,sizeof(struct message_s),0)) < 0 ){
			printf("Send Error!");
		}	
		else if(send(accept_fd,lsentry,strlen(lsentry)+1,0) < 0){
			printf("Send Error!");
		}
		free(lsentry);
	}

	///////////////////////////////////////////
	//////////////////////////////////////////
	//////////////GET///////////////////////

	else if (REQUEST->type == 0xB1){
		printf("GET Request!\n");
		struct message_s* GET_REPLY;
		struct message_s* FILE_DATA;
		GET_REPLY = (struct message_s*) malloc(10);
		FILE_DATA = (struct message_s*) malloc(10);
		const int header_size = 10;
		strcpy(GET_REPLY -> protocol , "myftp");
		if(recv(accept_fd,filename,sizeof(char)*100,0) < 0){
			printf("filename receive error");
			pthread_exit(NULL);
		}

		if( access( filename, F_OK ) != -1 ) {
			GET_REPLY -> type = 0xB2;
			GET_REPLY -> length = header_size;
			send(accept_fd,GET_REPLY,sizeof(struct message_s),0);
		} else {
			GET_REPLY -> type = 0xB3;
			GET_REPLY -> length = header_size;
			send(accept_fd,GET_REPLY,sizeof(struct message_s),0);
		}
		if ( lstat(filename, &filestat) < 0){
			pthread_exit(NULL);
		}

		fp = fopen(filename, "rb");

		FILE_DATA -> type = 0xFF;
		FILE_DATA -> length = 10 + size_of_the_file(fp);
		send(accept_fd,FILE_DATA,sizeof(struct message_s),0);

		while(!feof(fp)){
			numbytes = fread(dat, sizeof(char), sizeof(dat), fp);
			// printf("fread %d bytes, ", numbytes);
			numbytes = send(accept_fd, dat, numbytes,0);
			// printf("Sending %d bytesn",numbytes);
		}
		fclose(fp);

	}

	///////////////////////////////////////////
	//////////////////////////////////////////
	//////////////PUT///////////////////////

	else if (REQUEST->type == 0xC1){
		printf("PUT request!\n");
		struct message_s* PUT_REPLY;
		struct message_s* FILE_DATA_PUT;
		PUT_REPLY = (struct message_s*) malloc(10);
		FILE_DATA_PUT = (struct message_s*) malloc(10);

		if(recv(accept_fd,filename,sizeof(char)*100,0) < 0){
			printf("filename receive error");
			pthread_exit(NULL);
		}

		PUT_REPLY -> type = 0xC2;
		PUT_REPLY -> length = header_size;
		send(accept_fd,PUT_REPLY,sizeof(struct message_s),0);

		if(recv(accept_fd,FILE_DATA_PUT,sizeof(struct message_s),0) < 0){
			printf("file receive error");
			pthread_exit(NULL);
		}
		char tempo[100];
		memcpy(tempo, filename + 2, strlen(filename)-2);
		tempo[strlen(filename)-2] = '\0';
		char tempo2[100] = "./data/";
		strcat(tempo2,tempo);
		if ((fp = fopen(tempo2, "wb")) == NULL){
			perror("fopen");
			pthread_exit(NULL);
		}
		while(1){
			// numbytes = read(new_fd, buf, sizeof(buf));
			numbytes = recv(accept_fd,dat,sizeof(dat),0);
			printf("numbytes: %d\n",numbytes);
			// printf("read %d bytes, ", numbytes);
			if(numbytes == 0){
				break;
			}
			numbytes = fwrite(dat, sizeof(char), numbytes, fp);
			// printf("fwrite %d bytes\n", numbytes);
		}
		fclose(fp);
	}
	close(accept_fd);
	pthread_exit(NULL);
}

int main(int argc,char** argv){
	long val = 1 ;
	unsigned short port;

	if(argc != 2){
		printf("Port number missing\n");
		exit(1);
	}	
	port = atoi(argv[1]); //port num receive 
	pthread_t threadid[999]; 
	sd = socket(AF_INET, SOCK_STREAM, 0);
	// if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(long))==-1){ //portreusable
	// 	printf("setsockopt error\n");
	// 	exit(1);
	// }
	int *clientsd_p[999]; //clinet socket descriptor pointer
	int client_sd[999], buf; 
	struct sockaddr_in server_addr, client_addr;
	unsigned int addrlen = sizeof(client_addr);
	struct message_s* message_s_to_send;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	if(bind(sd, (struct sockaddr*) &server_addr, sizeof(server_addr))==-1){
		printf("bind error");
		exit(1);
	}

	if(listen(sd, 999) == -1){ //MAXPENDING N later
		printf("listen error");
		exit(1);
	}
	int connection_count=0;
	//accept 
	for(int i=1; i <=999; i++){
		//loop
		printf("--------Press crtl+c to stop the server---------\n");

		if((client_sd[i] = accept(sd, (struct sockaddr*) &client_addr, &addrlen))==-1){
			printf("accept error");
			exit(0);
		}else{
			//open worker thread
			connection_count++;
			// printf("count->%d i=%d\n", connection_count,i);
			// printf("client sd:%d\n", client_sd[i]);
			// buf = ntohl(buf);
			if(pthread_create(&threadid[i], NULL, workerthread,&client_sd[i])!=0){
				printf("create thread error");

			}
		}
	}
	close(sd);
	return 0;
}

