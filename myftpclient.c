#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "myftp.h"

long size_of_the_file(FILE *file)
{
	long size = 0;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return size;
}

int sendMsg(int sd, char *buff, int len)
{
	int recvLen = 0;
	while (recvLen != len)
	{
		int rLen = send(sd, buff + recvLen, len - recvLen, 0);
		if (rLen <= 0)
		{
			fprintf(stderr, "error sending msg\n");
			return 1;
		}
		recvLen += rLen;
	}
	return 0;
}

void request_prepare(struct message_s *message_to_send, char *cmd, int payload_size)
{
	const int header_size = 10;
	bzero(message_to_send->protocol, 5);
	strcpy(message_to_send->protocol, "myftp");
	if (strcmp("list", cmd) == 0)
	{
		message_to_send->type = 0xA1;
		message_to_send->length = header_size;
	}
	else if (strcmp("get", cmd) == 0)
	{
		message_to_send->type = 0xB1;
		message_to_send->length = header_size + payload_size;
	}
	else if (strcmp("put", cmd) == 0)
	{
		message_to_send->type = 0xC1;
		message_to_send->length = header_size + payload_size;
	}
}

int main(int argc, char **argv)
{
	int mode = 0;
	if (argc != 5 && argc != 4)
	{
		printf("The number of argument are not correct\n");
		exit(0);
	}
	const int port = atoi(argv[2]);
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	char pathname[100] = "./";
	FILE *file = NULL;
	if (strcmp(argv[3], "put") == 0)
	{
		strcat(pathname, argv[4]);
		printf("%s\n", pathname);
		file = fopen(pathname, "r");
		if (!file)
		{
			printf("Can not open file: %s\n", strerror(errno));
			exit(0);
		}
	}
	else if (strcmp(argv[3], "get") == 0)
	{
		strcat(pathname, argv[4]);
		printf("%s\n", pathname);
	}
	else
	{
		pathname[0] = '\0';
	}
	char *cmd = argv[3];
	char *ipstr = argv[1];
	printf("%s", ipstr);
	struct sockaddr_in server_addr;
	if (sd < 0)
	{
		printf("Can not create socket :%s\n", strerror(errno));
		exit(0);
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ipstr);
	server_addr.sin_port = htons(port);
	if (connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("connection error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	printf("connected to the server: %s\n", inet_ntoa((server_addr.sin_addr)));
	struct message_s message_to_send;
	struct message_s file_data;
	long payload_size = 0;
	if (strcmp(argv[3], "put") == 0)
	{
		mode = 2;
		payload_size = strlen(pathname);
		request_prepare(&message_to_send, cmd, payload_size);
	}
	else if (strcmp(argv[3], "get") == 0)
	{
		mode = 1;
		payload_size = strlen(pathname);
		request_prepare(&message_to_send, cmd, payload_size);
	}
	else if (strcmp(argv[3], "list") == 0)
	{
		mode = 0;
		payload_size = 0;
		request_prepare(&message_to_send, cmd, payload_size);
	}
	char *header_buf = NULL;
	header_buf = (char *)malloc(sizeof(message));
	long header_size = sizeof(message);
	bzero(header_buf, sizeof(message));
	//prepare header buf
	memcpy(header_buf, &(message_to_send.protocol), sizeof(message_to_send.protocol));
	memcpy(header_buf + sizeof(message_to_send.protocol), &(message_to_send.type), sizeof(message_to_send.type));
	memcpy(header_buf + (sizeof(message_to_send.protocol) + sizeof(message_to_send.type)), &(message_to_send.length), sizeof(message_to_send.length));
	printf("buf = %s\n", header_buf);
	//prepare payload buf
	long sent_size = 0;
	char *payload_buf = NULL;
	printf("%d\n", payload_size);
	if (mode != 0)
	{
		payload_buf = (char *)malloc(payload_size);
		memcpy(payload_buf, &pathname, payload_size);
	}
	else
	{
		payload_buf = NULL;
	}
	//send header
	if (header_size > (sent_size = send(sd, header_buf, header_size, 0)))
	{
		if (sent_size < 0)
		{
			printf("Send header failed: %s\n", (strerror(errno)));
		}
		else
		{
			printf("Send header incomplete\n");
		}
	}
	else
	{
		printf("Send header successfully!\n");
	}
	//send payload
	if (mode != 0)
	{
		if (payload_size > (sent_size = send(sd, payload_buf, payload_size, 0)))
		{
			if (sent_size < 0)
			{
				printf("Send payload failed: %s\n", (strerror(errno)));
			}
			else
			{
				printf("Send payload incomplete\n");
			}
		}
		else
		{
			printf("Send payload successfully! payload_size = %ld\n", payload_size);
		}
	}
	int check = 1;
	message *reply_buf = NULL;
	char protocol_check[6];
	char type_check = '\0';
	reply_buf = (message *)malloc(header_size);
	bzero(reply_buf, header_size);
	long revlen = 0;
	long length_of_payload = 0;
	if ((revlen = recv(sd, reply_buf, sizeof(reply_buf), 0)) < 0)
	{
		printf("recevie header failed\n");
	}
	else
	{
		type_check = reply_buf -> type;
		if (type_check == 0xA2 && mode == 0)
		{
			check = 1;
		}
		if (type_check == 0xB2 && mode == 1)
		{
			check = 1;
		}
		if (type_check == 0xC3 && mode == 2)
		{
			check = 1;
		}
		if (check)
		{
			int i = 0;
			length_of_payload = reply_buf->length;
			//extract the reply_buf[6]
			length_of_payload -= 10;
			printf("%d", length_of_payload);

		}
	}

	//	char* reply_buf = NULL;
	//	reply_buf = (char* ) malloc(sizeof(message))
	//	if((reclen = recv(sd, buf, sizeof(buff), 0)) < 0){
	//			printf("recvced reply message")
	//		switch(mode) {
	//			case 0:
	//					{
		free(payload_buf);
		free(reply_buf);
		free(header_buf);
	close(sd);
	return 0;
}
