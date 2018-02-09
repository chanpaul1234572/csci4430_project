#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include "myftp.h"


//common functions shared by server and function 
//header decode encode
long size_of_the_file(FILE *file)
{
	long size = 0;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return size;
}


