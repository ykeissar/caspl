#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STDIN 0
#define STDOUT 1 

int main(int argc,char** argv){
    int childPid;
    int pipefd[2];
    pipefd[0] = STDOUT;
    pipefd[1] = STDIN;
    pipe(pipefd);
    char msg[7];
    if((childPid = fork()) == 0){
        printf("hello");
        exit(0);
    }
    else{
        scanf("%s",msg);
        printf("%s\n",msg);
        exit(0);
    }
    
    return 0;
}