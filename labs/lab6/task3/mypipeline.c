#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int debugMode = 0;

int main(int argc,char** argv){
    int pipefd[2];
    int childPid1,childPid2,i;

    for(i = 1;i < argc;i++){
        if(strcmp(argv[i],"-d") == 0){
            debugMode = 1;
        }
    }

    char* myArgv[4];

    if(pipe(pipefd) == -1) {
        perror("Pipe failed");
        exit(1);
    }

    if(debugMode == 1)
        fprintf(stderr,"(parent_process>forking…)\n");
    if((childPid1 = fork()) == 0){
        close(STDOUT_FILENO);
        if(debugMode == 1){
            fprintf(stderr,"(child1>redirecting stdout to the write end of the pipe…)\n");
        }
        dup(pipefd[1]);
        close(pipefd[1]);
        myArgv[0] = "ls";
        myArgv[1] = "-l";
        myArgv[2] = 0;
        if(debugMode == 1){
            fprintf(stderr,"(child1>going to execute cmd: 'ls -l')\n");
        }
        if(execvp("ls",myArgv)==-1){
            perror("Error: ls failed");
            exit(1);
        }
    }
    else{
        if(debugMode == 1){
            fprintf(stderr,"(parent_process>created process with id: %d)\n",childPid1);
            fprintf(stderr,"(parent_process>closing the write end of the pipe…)\n");
        }
        
        close(pipefd[1]);
        if((childPid2 = fork()) == 0){
            close(STDIN_FILENO);
            if(debugMode == 1){
                fprintf(stderr,"(child2>redirecting stdin to the read end of the pipe…)\n");
            }
            dup(pipefd[0]);
            close(pipefd[0]);
            myArgv[0] = "tail";
            myArgv[1] = "-n";
            myArgv[2] = "2";
            myArgv[3] = 0;
            if(debugMode == 1){
                fprintf(stderr,"(child2>going to execute cmd: 'tail -n 2')\n");
            }
            if((execvp("tail",myArgv)) == -1){
                perror("Error: ls failed");
                exit(1);
            }
        }
        else{
            if(debugMode == 1){
                fprintf(stderr,"(parent_process>created process with id: %d)\n",childPid2);
                fprintf(stderr,"(parent_process>closing the read end of the pipe…)\n");
            }
            close(pipefd[0]);
            if(debugMode == 1){
                fprintf(stderr,"(parent_process>waiting for child processes to terminate…)\n");
            }
            waitpid(childPid1,NULL,0);
            if(debugMode == 1){
                fprintf(stderr,"(parent_process>waiting for child processes to terminate…)\n");
            }
            waitpid(childPid2,NULL,0);
            if(debugMode == 1){
                fprintf(stderr,"(parent_process>exiting…)\n");
            }
        }
    }
}