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

//long size_of_the_file(FILE *file)
//{
//	long size = 0;
//	fseek(file, 0, SEEK_END);
//	size = ftell(file);
//	fseek(file, 0, SEEK_SET);
//	return size;
//}

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
	char pathname[100] = "./data/";
	FILE *file = NULL;
	FILE *GETFILE = NULL;
	if (strcmp(argv[3], "put") == 0)
	{
		bzero(pathname, 100);
		strcat(pathname, "./");
		strcat(pathname, argv[4]);
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
	//char *header_buf = NULL;
	message* header_buf = NULL;
	header_buf = (message *)malloc(sizeof(message));
	long header_size = sizeof(message);
	bzero(header_buf, sizeof(message));
	memcpy(header_buf, &message_to_send, header_size);
	//prepare header buf
	//memcpy(header_buf, &(message_to_send.protocol), sizeof(message_to_send.protocol));
	//memcpy(header_buf + sizeof(message_to_send.protocol), &(message_to_send.type), sizeof(message_to_send.type));
	//memcpy(header_buf + (sizeof(message_to_send.protocol) + sizeof(message_to_send.type)), &(message_to_send.length), sizeof(message_to_send.length));
	//prepare payload buf
	long sent_size = 0;
	char *payload_buf = NULL;
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
			close(sd);
			exit(1);
		}
		else
		{
			printf("Send header incomplete\n");
			close(sd);
			exit(1);

		}
	}
	//send payload
	if (mode != 0)
	{
		if (payload_size > (sent_size = send(sd, payload_buf, payload_size, 0)))
		{
			if (sent_size < 0)
			{
				printf("Send payload failed: %s\n", (strerror(errno)));
				close(sd);
				exit(1);
			}
			else
			{
				printf("Send payload incomplete\n");
				close(sd);
				exit(1);
			}
		}
	}
	free(payload_buf);
	message *reply_buf = NULL;
	char protocol_check[6];
	reply_buf = (message *)malloc(header_size);
	bzero(reply_buf, header_size);
	long revlen = 0;
	long length_of_payload = 0;
	if ((revlen = recv(sd, reply_buf, header_size, 0)) < 0)
	{
		printf("recevie header failed\n");
		close(sd);
		exit(1);

	}
	else
	{
		if (reply_buf -> type == 0xA2 && mode == 0)
		{
			length_of_payload = reply_buf -> length - 10;
			payload_buf = (char*)malloc(length_of_payload + 1);
			if((revlen = recv(sd, payload_buf, length_of_payload, 0)) < 0){
				printf("recevie payload failed\n");
			}	
			else{
				printf("%s", payload_buf);
			}
		}
		if (reply_buf -> type == 0xB2 && mode == 1)
		{
			if((revlen = recv(sd, reply_buf, header_size, 0)) < 0){
				printf("recevie file data header failed\n"); 		
			}
			else{
				length_of_payload = reply_buf -> length - 10;
				payload_buf = (char*)malloc(length_of_payload);
				if((revlen = recv(sd, payload_buf, length_of_payload, 0)) < 0){
					printf("recevie file data failed\n");
				}	
				else{
					GETFILE = fopen(argv[4], "wb");
					if(GETFILE == NULL){
						perror("failed to create file");
					}
					else{
						long filesize = 0;
						filesize = fwrite(payload_buf, sizeof(char), revlen, GETFILE);
						if(filesize != revlen){
							printf("write failed\n");
						}
					}
					fclose(GETFILE);
				}
				free(payload_buf);
			}
		}
		if (reply_buf -> type == 0xB3 && mode == 1){
			printf("File not exist\n");
		}
		if (reply_buf -> type == 0xC2 && mode == 2)
		{
			message file_data;
			file_data.protocol[0] = 'm';
			file_data.protocol[1] = 'y';
			file_data.protocol[2] = 'f';
			file_data.protocol[3] = 't';
			file_data.protocol[4] = 'p';
			file_data.type = 0xFF;
			file_data.length = htonl(10 + size_of_the_file(file));
			if(send(sd, &file_data, 10, 0) == 10){
				file = fopen(argv[4], "rb");
				long numbytes = 0;
				payload_buf = (char*)malloc(10000000);
				while(!feof(file)){
					numbytes = fread(payload_buf, 1, 10000000, file);
					send(sd, payload_buf, numbytes, 0);
				}
			}
			free(payload_buf);
		}
	}
	free(reply_buf);
	free(header_buf);
	close(sd);
	return 0;
}
