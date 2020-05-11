#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../LineParser.h"

#define MAX_INPUT_SIZE 2048

void execute(cmdLine* pCmdLine);

int main(int argc,char** argv){
    char pathBuffer[PATH_MAX];
    char input[MAX_INPUT_SIZE];
    cmdLine* cmd = (cmdLine*) malloc(sizeof(cmdLine));

    while(1){
        getcwd(pathBuffer,PATH_MAX);
        printf("%s ",pathBuffer);
        fgets(input,MAX_INPUT_SIZE,stdin);
        cmd = parseCmdLines(input);
        if(strcmp(cmd->arguments[0],"quit") == 0)
            break;
        execute(cmd);
    }
    
    freeCmdLines(cmd);
    return 0;
}

void execute(cmdLine* pCmdLine){
    execvp(pCmdLine->arguments[0],pCmdLine->arguments);
    perror("Erro has occured: ");
}