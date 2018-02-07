CC=gcc
CFLAGS=-Wall

all: myftp.c myftpclient.c myftpserver.c 
	gcc -o myftpserver myftpserver.c -lpthread  myftp myftpclient

clean: 
	rm -rf myftpserver myftp myftpclient