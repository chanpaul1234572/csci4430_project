#include<stdio.h>
#include<stdlib.h>
int main(void){
    FILE* fptr;
    char *s = "a\n\nb";
    fputs(s, stdout);
    return 0;
}