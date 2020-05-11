#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../LineParser.h"
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 2048

void execute(cmdLine* pCmdLine);

int main(int argc,char** argv){
    char pathBuffer[PATH_MAX];
    char input[MAX_INPUT_SIZE];
    int i, childPid;
    int debugMode = 0;

    for(i = 1;i < argc;i++){
        if(strcmp(argv[i],"-d") == 0){
            debugMode = 1;
        }
    }

    cmdLine* cmd = (cmdLine*) malloc(sizeof(cmdLine));

    while(1){
        getcwd(pathBuffer,PATH_MAX);
        printf("%s ",pathBuffer);
        fgets(input,MAX_INPUT_SIZE,stdin);
        cmd = parseCmdLines(input);
        if(strcmp(cmd->arguments[0],"quit") == 0)
            break;
        if(strcmp(cmd->arguments[0],"cd") == 0){
            chdir(cmd->arguments[1]);
            continue;
        }
        if((childPid = fork()) == 0){
            execute(cmd);
        }
        else {
            if(debugMode == 1){
                fprintf(stderr,"PID: %d, Executed command: %s\n",childPid,cmd->arguments[0]);
            }
        }
        if(cmd->blocking == 1)
            waitpid(childPid,NULL,0);

    }
    freeCmdLines(cmd);
    return 0;
}

void execute(cmdLine* pCmdLine){
    if(execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1){
        perror("Error has occured: ");
        _exit(1);
    }
}