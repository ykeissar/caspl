#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../LineParser.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

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

/* Free all memory allocated for the process list.*/
void freeProcessList(process* process_list);
void freeProcess(process* proc);

/* Go over the process list, and for each process check if it is done, 
   you can use waitpid with the option WNOHANG. WNOHANG does not block the calling process,
   the process returns from the call to waitpid immediately.
   If no process with the given process id exists, then waitpid returns -1.*/
void updateProcessList(process **process_list);

/* Find the process with the given id in the process_list and change
   its status to the received status*/
void updateProcessStatus(process* process_list, int pid, int status);

void myKill(int pid,int sig);

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
            freeCmdLines(cmd);
            break;
        }
        if(strcmp(cmd->arguments[0],"cd") == 0){
            chdir(cmd->arguments[1]);
            freeCmdLines(cmd);
            continue;
        }
        if(strcmp(cmd->arguments[0],"proc") == 0){
            printProcessList(&process_list);
            freeCmdLines(cmd);
            continue;
        }
        if(strncmp(cmd->arguments[0],"kill",4) == 0){
            myKill(atoi(cmd->arguments[1]),SIGINT);
            freeCmdLines(cmd);
            continue;
        }
        if(strncmp(cmd->arguments[0],"suspend",7) == 0){
            myKill(atoi(cmd->arguments[1]),SIGTSTP);
            freeCmdLines(cmd);
            continue;
        }
        if(strncmp(cmd->arguments[0],"wake",4) == 0){
            myKill(atoi(cmd->arguments[1]),SIGCONT);
            freeCmdLines(cmd);
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

        if(cmd->blocking == 1)
            waitpid(childPid,NULL,0);
        freeCmdLines(cmd);
    }
    freeProcessList(process_list);
    return 0;
}

void execute(cmdLine* pCmdLine){
    if(execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1){
        perror("Error has occured: ");
        free(pCmdLine);
        _exit(1);
    }
    free(pCmdLine);
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
        nextP = nextP->next;
    }

    temp->next = newProc;
}

void printProcessList(process** process_list){
    updateProcessList(process_list);
    process* temp = *process_list;
    process* prev = NULL;
    puts("PID       Command     STATUS");
    while(temp != NULL){
        printf("%d      %s      %s\n",temp->pid,temp->cmd->arguments[0],statusStr(temp->status));
        if(temp->status == TERMINATED){
            if(prev == NULL){
                *process_list = temp->next;
                freeProcess(temp);
                temp = *process_list;
            }
            else{
                prev->next = temp->next;
                freeProcess(temp);
                temp = prev->next;
            }
        }
        else{
            prev = temp;
            temp = temp->next;
        }
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
        default:
            return "";

    }
}

void freeProcessList(process* process_list){
    process* temp;
    while(process_list != NULL){
        temp = process_list->next;
        freeProcess(process_list);
        process_list = temp;
    }
}

void freeProcess(process* proc){
    freeCmdLines(proc->cmd);
    free(proc);
}
void updateProcessList(process **process_list){
    process* temp = *process_list;
    int stat; 
    int newStat, ret;
    while(temp != NULL){
        ret = waitpid(temp->pid,&stat,WNOHANG|WUNTRACED);
        if(ret != 0){
            if(WIFEXITED(stat) || WIFSIGNALED(stat)){
                newStat = TERMINATED;
            } else if(WIFSTOPPED(stat)){
                newStat = SUSPENDED;
            } else if(WIFCONTINUED(stat)){
                newStat = RUNNING;
            }
            updateProcessStatus(temp,temp->pid,newStat);
        }
        temp = temp->next;
    }
        
}

void updateProcessStatus(process* process_list, int pid, int status){
    if(debugMode == 1 && process_list->status != status)
        fprintf(stderr,"process: %d status changed from: %s to:%s\n",pid,statusStr(process_list->status),statusStr(status));
    process_list->status = status;
}

void myKill(int pid,int sig){
    if(debugMode == 1)
        fprintf(stderr,"%s signal sent to %d\n",strsignal(sig),pid);
    kill(pid,sig);
}