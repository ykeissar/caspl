#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../LineParser.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include<fcntl.h> 


#define MAX_INPUT_SIZE 2048

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

#define STDIN 0
#define STDOUT 1


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

void freeProcessList(process* process_list);
void freeProcess(process* proc);
void updateProcessList(process **process_list);
void updateProcessStatus(process* process_list, int pid, int status);

void myKill(int pid,int sig);

int debugMode = 0;
int main(int argc,char** argv){
    char pathBuffer[PATH_MAX];
    char input[MAX_INPUT_SIZE];
    int i, childPid,inFile,outFile,stdoutCopy,stdinCopy;

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

        //task1 - I/O redirects
        if(cmd->inputRedirect != NULL){
            if((inFile = open(cmd->inputRedirect,O_RDONLY)) < 0){
                fprintf(stderr,"Error: Input redirect");
                free(cmd);
                break;
            }
            if((stdinCopy = dup(STDIN)) < 0){
                fprintf(stderr,"Error: Input redirect");
                free(cmd);
                break;
            }
            if(dup2(inFile,STDIN) < 0){
                fprintf(stderr,"Error: Input redirect");
                free(cmd);
                break;
            }
            close(inFile);
        }
        if(cmd->outputRedirect != NULL){
            if((outFile = open(cmd->outputRedirect,O_CREAT | O_WRONLY)) < 0){
                fprintf(stderr,"Error: Output redirect");
                free(cmd);
                break;
            }
            if((stdoutCopy = dup(STDOUT)) < 0){
                fprintf(stderr,"Error: Output redirect");
                free(cmd);
                break;
            }
            if(dup2(outFile,STDOUT) < 0){
                fprintf(stderr,"Error: Output redirect");
                free(cmd);
                break;
            }
            close(outFile);
        }

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
        if(strncmp(cmd->arguments[0],"kill",4) == 0){
            myKill(atoi(cmd->arguments[1]),SIGINT);
            free(cmd);
            continue;
        }
        if(strncmp(cmd->arguments[0],"wake",4) == 0){
            myKill(atoi(cmd->arguments[1]),SIGCONT);
            free(cmd);
            continue;
        }
        if(strncmp(cmd->arguments[0],"suspend",7) == 0){
            myKill(atoi(cmd->arguments[1]),SIGTSTP);
            free(cmd);
            continue;
        }
        if((childPid = fork()) == 0){
            execute(cmd);
            if(cmd->outputRedirect != NULL){
                fflush(stdout);
                dup2(stdoutCopy,STDOUT);
                close(stdoutCopy);
            }
            if(cmd->inputRedirect != NULL){
                fflush(stdin);
                dup2(stdinCopy,STDIN);
                close(stdinCopy);
            }
            exit(0);
        }
        else {
            if(debugMode == 1){
                fprintf(stderr,"PID: %d, Executed command: %s\n",childPid,cmd->arguments[0]);
            }
            addProcess(&process_list,cmd,childPid);
            if(cmd->blocking == 1){
                waitpid(childPid,NULL,0);
            }
        }
        
        if(cmd->inputRedirect != NULL){
            fflush(stdin);
            dup2(stdinCopy,STDIN);
            close(stdinCopy);
        }
        if(cmd->outputRedirect != NULL){
            fflush(stdout);
            dup2(stdoutCopy,STDOUT);
            close(stdoutCopy);
        }

        free(cmd);
    }

    freeProcessList(process_list);
    return 0;
}

void execute(cmdLine* pCmdLine){
    if(execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1){
        perror("Error has occured: ");
        freeCmdLines(pCmdLine);
        _exit(1);
    }
    freeCmdLines(pCmdLine);
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
    updateProcessList(process_list);
    process* temp = *process_list;
    process* prev = NULL;
    puts("PID       Command     Status");
    while(temp != NULL){
        printf("%d      %s      %s\n",temp->pid,temp->cmd->arguments[0],strStatus(temp->status));
        if(temp->status == TERMINATED){
            if(prev == NULL){
                *process_list = temp->next;
                freeProcess(temp);
                temp = *process_list;
            }
            else{
                prev->next = temp->next;
                freeProcessList(temp);
                temp = prev->next;
            }
        }
        else{
            prev = temp;
            temp = temp->next;
        }
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
    int stat,newStat,ret;
    while(temp != NULL){
        ret = waitpid(temp->pid,&stat, WNOHANG | WUNTRACED | WCONTINUED);
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
        else if(ret == -1){
            perror("Erro had occured: ");
        }
        temp = temp->next;
    }
}

void updateProcessStatus(process* process_list, int pid, int status){
    if(debugMode == 1 && process_list->status != status)
        fprintf(stderr,"process: %d status changed from: %s to : %s\n",pid,strStatus(process_list->status),strStatus(status));
    process_list->status = status;
}

void myKill(int pid,int sig){
    if(debugMode == 1)
        fprintf(stderr,"%s signal sent to %d\n",strsignal(sig),pid);
    kill(pid,sig);
}