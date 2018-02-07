#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

int main(int argc,char** argv){
	struct dirent *pStEntry = NULL;
	struct dirent *pStResult = NULL;
	int len = 0;
	DIR *pDir;
	// char lsentry[1025];
	char temp[1024];
	char *lsentry = malloc(sizeof(char) * 1024);
	// lsentry = (char *) malloc(sizeof(char));
	pDir = opendir("./data");
	  if(NULL == pDir)
 	 {
     	printf("Opendir failed!\n");
      	return 0;
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
      			// printf("%s\n", pStEntry->d_name);
  		}
 
  printf("%s",lsentry);
  // free(lsentry);
  free(pStEntry);
  free(pStResult);
  closedir(pDir);


	return 0;
}
// #include <stdio.h>
// #include <sys/types.h>
// #include <dirent.h>
// #include <unistd.h>
// int main(int argc,char** argv){
//     DIR * dir;
//     struct dirent * ptr;
//     int i;
//     dir = opendir("~/data");
//     while((ptr = readdir(dir)) != NULL)
//     {
//         printf("d_name : %s\n", ptr->d_name);
//     }
//     closedir(dir);
//     return 0;
// }