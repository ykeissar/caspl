#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../LineParser.h"
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 2048

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

//task1
void execute(cmdLine* pCmdLine);

//task2
typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;

void addProcess(process** process_list, cmdLine* cmd, pid_t pid);

void printProcessList(process** process_list);

char* strStatus(int status);

int debugMode = 0;
int main(int argc,char** argv){
    char pathBuffer[PATH_MAX];
    char input[MAX_INPUT_SIZE];
    int i, childPid;

    for(i = 1;i < argc;i++){
        if(strcmp(argv[i],"-d") == 0){
            debugMode = 1;
        }
    }

    cmdLine* cmd;
    process* process_list = NULL;

    while(1){
        getcwd(pathBuffer,PATH_MAX);
        printf("%s ",pathBuffer);
        fgets(input,MAX_INPUT_SIZE,stdin);

        if(strcmp(input,"\n") == 0)
            continue;
            
        cmd = parseCmdLines(input);
        if(strcmp(cmd->arguments[0],"quit") == 0){
            free(cmd);
            break;
        }
        if(strcmp(cmd->arguments[0],"cd") == 0){
            chdir(cmd->arguments[1]);
            free(cmd);
            continue;
        }
        if(strcmp(cmd->arguments[0],"proc") == 0){
            printProcessList(&process_list);
            free(cmd);
            continue;
        }
        if((childPid = fork()) == 0){
            execute(cmd);
            continue;
        }
        else {
            if(debugMode == 1){
                fprintf(stderr,"PID: %d, Executed command: %s\n",childPid,cmd->arguments[0]);
            }
        }
        addProcess(&process_list,cmd,childPid);
        if(cmd->blocking == 1){
            waitpid(childPid,NULL,0);
        }
        free(cmd);
    }
    return 0;
}

void execute(cmdLine* pCmdLine){
    if(execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1){
        perror("Error has occured: ");
        freeCmdLines(pCmdLine);
        _exit(1);
    }
    freeCmdLines(pCmdLine);
    _exit(0);
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    process* newProc = (process*) malloc(sizeof(process));
    newProc->cmd = (cmdLine*) malloc(sizeof(cmdLine));
    memcpy(newProc->cmd,cmd,sizeof(cmdLine));
    newProc->pid = pid;
    newProc->status = RUNNING;
    newProc->next = NULL;
    if(*process_list == NULL){
        *process_list = newProc;
        return;
    }
    process* temp;
    process* nextP = *process_list;
    while(nextP != NULL){
        temp = nextP;
        nextP = temp->next;
    }
    temp->next = newProc;
}

void printProcessList(process** process_list){
    process* temp = *process_list;
    puts("PID       Command     Status");
    while(temp != NULL){
        printf("%d      %s      %s\n",temp->pid,temp->cmd->arguments[0],strStatus(temp->status));
        temp = temp->next;
    }
}

char* strStatus(int status){
    switch(status){
        default:
            return "";
        case RUNNING:
            return "Running";
        case SUSPENDED:
            return "Suspended";
        case TERMINATED:
            return "Terminated";
    }
}