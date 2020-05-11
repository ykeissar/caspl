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
    pid_t pid; 		                      /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	              /* next process in chain */
} process;

/* Receive a process list (process_list), a command (cmd),
   and the process id (pid) of the process running the command.*/
void addProcess(process** process_list, cmdLine* cmd, pid_t pid);

/* Print the processes.*/
void printProcessList(process** process_list);
char* statusStr(int status);

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
    process** process_list;

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
        if(strcmp(cmd->arguments[0],"proc") == 0){
            printProcessList(process_list);
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

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    process* newProc = (process*) malloc(sizeof(process));
    strcpy(newProc->cmd,cmd);
    newProc->pid = pid;
    newProc->status = RUNNING;
    process* temp = NULL;
    process* nextP = *process_list;
    while(nextP != NULL){
        temp = nextP;
        nextP = nextP->next;
    }
    temp->next = newProc;
}

void printProcessList(process** process_list){
    process* temp = *process_list;
    puts("PID       Command     STATUS");
    while(temp != NULL){
        printf("%d      %s      %s\n",temp->pid,temp->cmd->arguments[0],statusStr(temp->status));
    }
}

char* statusStr(int status){
    switch(status){
        case TERMINATED:
            return "Terminated";
        case RUNNING:
            return "Running";
        case SUSPENDED:
            return "Suspended";
    }
}

