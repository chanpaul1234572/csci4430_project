#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "myftp.h"

#define PORT 12345

int main(int argc, char **argv)
{
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	int client_sd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	message header;
	header.protocol[0] = 'm';
	header.protocol[1] = 'y';
	header.protocol[2] = 'f';
	header.protocol[3] = 't';
	header.protocol[4] = 'p';
	header.length = 3000;
	header.type = 0xC2;
	if (bind(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("Bind error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	if (listen(sd, 3) < 0)
	{
		printf("Listen error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	int addr_len = sizeof(client_addr);
	if ((client_sd = accept(sd, (struct sockaddr *)&client_addr, &addr_len)) < 0)
	{
		printf("Accept error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	else
	{
		char ip4[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(client_addr.sin_addr), ip4, INET_ADDRSTRLEN);
		printf("Connected with %s, length : %d\n", ip4, INET_ADDRSTRLEN);
	}
	char buff[100];
	char buff1[100];
	int len;
	if ((len = recv(client_sd, buff, sizeof(buff), 0)) < 0)
	{
		printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	//if(send(client_sd, "0", sizeof("0"), 0) < 0){
	//		printf("ended\n");
	//		close(client_sd);
	//}
	buff[len] = '\0';
	printf("Received: ");
	if (strlen(buff) != 0)
	{
		printf("%s\n", buff);
		printf("len = %d\n", len);
		printf("buff[6 -7] = %d %d\n", buff[6], buff[7]);
	}
	if (strcmp("exit", buff) == 0)
	{
		close(client_sd);
	}
	printf("recevied header\n");
	message *header1 = (message *)(buff);
	printf("%x\n", header1 -> type);
	if ((header1->type != 0xA1))
	{
		if ((len = recv(client_sd, buff1, sizeof(buff1), 0)) < 0)
		{
			printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
			exit(0);
		}
		printf("payload_len = %d\n", len);
		buff1[len] = '\0';
		printf("Received: ");
		if (strlen(buff1) != 0)
		{
			printf("%s\n", buff1);
			printf("len = %d\n", len);
			printf("buff[6 -7] = %s\n", buff1);
		}
		if (strcmp("exit", buff) == 0)
		{
			close(client_sd);
		}
	}
	if (sizeof(header) > send(sd, &header, sizeof(header), 0))
	{
		printf("send successfully\n");
	}
	else
	{
		printf("failed\n");
	}
	close(client_sd);
	close(sd);
	return 0;
}
