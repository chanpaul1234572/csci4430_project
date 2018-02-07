CC=gcc
CFLAGS=-Wall
LIB=myftp

all: myftp.c myftpclient.c myftpserver.c 
	gcc -o myftpserver myftpserver.c -lpthread myftpclient $(LIB) 

clean: 
	rm -rf myftpserver myftp myftpclient