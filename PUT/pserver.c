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
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<dirent.h>

int main(){
	int sockfd, new_fd, numbytes, sin_size;
	char buf[1000000];
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	// struct stat filestat;
	FILE *fp;

	//TCP socket
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("socket");
		exit(1);
	}

	//Initail, bind to port 2323
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(2324);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero( &(my_addr.sin_zero), 8 );

	//binding
	if ( bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1 ){
		perror("bind");
		exit(1);
	}

	//Start listening
	if ( listen(sockfd, 10) == -1 ){
		perror("listen");
		exit(1);
	}
    
    	//Connect
	if ( (new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size)) == -1 ){
		perror("accept");
		exit(1);
	}
		//Open file
	if ( (fp = fopen("RECV.MP3", "wb")) == NULL){
		perror("fopen");
		exit(1);
	}
    //Receive file from client
	while(1){
		// numbytes = read(new_fd, buf, sizeof(buf));
		numbytes = recv(new_fd,buf,sizeof(buf),0);
		printf("read %d bytes, ", numbytes);
		if(numbytes == 0){
			break;
		}
		numbytes = fwrite(buf, sizeof(char), numbytes, fp);
		printf("fwrite %d bytes\n", numbytes);
	}


	// //Get file stat
	// if ( lstat("checkmate.mp3", &filestat) < 0){
	// 	exit(1);
	// }
	// printf("The file size is %lun", filestat.st_size);

	// fp = fopen("checkmate.mp3", "rb");

	// //Connect
	// if ( (new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size)) == -1 ){
	// 	perror("accept");
	// 	exit(1);
	// }

	// //Sending file
	// while(!feof(fp)){
	// 	numbytes = fread(buf, sizeof(char), sizeof(buf), fp);
	// 	printf("fread %d bytes, ", numbytes);
	// 	numbytes = write(new_fd, buf, numbytes);
	// 	printf("Sending %d bytesn",numbytes);
	// }
    fclose(fp);
	close(new_fd);
	close(sockfd);
	return 0;
}