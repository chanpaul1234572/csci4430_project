#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(int argc, char* argv[]){
	int sockfd, new_fd, numbytes;
	char buf[1000000];
	struct sockaddr_in address;
	struct stat filestat;
	FILE *fp;

	//TCP socket
	if ( ( sockfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1 ){
		perror("socket");
		exit(1);
	}

	//Initial, connect to port 2323
	address.sin_family = AF_INET;
	address.sin_port = htons(2324);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	bzero( &(address.sin_zero), 8 );

	//Connect to server
	if ( connect(sockfd, (struct sockaddr*)&address, sizeof(struct sockaddr)) == -1){
		perror("connect");
		exit(1);
	}

	// //Open file
	// if ( (fp = fopen("GET.MP3", "wb")) == NULL){
	// 	perror("fopen");
	// 	exit(1);
	// }

	// //Receive file from server
	// while(1){
	// 	numbytes = read(sockfd, buf, sizeof(buf));
	// 	printf("read %d bytes, ", numbytes);
	// 	if(numbytes == 0){
	// 		break;
	// 	}
	// 	numbytes = fwrite(buf, sizeof(char), numbytes, fp);
	// 	printf("fwrite %d bytesn", numbytes);
	// }


	//Get file stat
	if ( lstat("checkmate.mp3", &filestat) < 0){
		exit(1);
	}

	printf("The file size is %lu \n", filestat.st_size);

	fp = fopen("checkmate.mp3", "rb");



	// //Sending file
	while(!feof(fp)){
		numbytes = fread(buf, sizeof(char), sizeof(buf), fp);
		printf("fread %d bytes, ", numbytes);
		// numbytes = write(new_fd, buf, numbytes);
		numbytes = send(sockfd,buf,numbytes,0);
		printf("Sending %d bytes\n",numbytes);
	}

	fclose(fp);
	close(new_fd);
	close(sockfd);
	return 0;
}