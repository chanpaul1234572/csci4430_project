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
		
		int sd = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in server_addr;
		if (s < 0){
				errexit("Can not create socket :%s\n", strerror(errno));
		}
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.
