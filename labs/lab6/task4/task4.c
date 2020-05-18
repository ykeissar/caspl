#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../LineParser.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h> 


#define MAX_INPUT_SIZE 2048

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

#define STDIN 0
#define STDOUT 1

int execute(cmdLine* pCmdLine);
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

typedef struct intVarLink{// Shell internal variable
    char* var;              //internal variable name
    char* val;             
    struct intVarLink* next;
}intVarLink;

void addIntVar(intVarLink** var_list,char* var,char* val);
void freeIntVarLink(intVarLink* link);
void freeVarList(intVarLink* var_list);
void printVarList(intVarLink* var_list);
char* getVarVal(intVarLink* var_list,char* wanted_var);

int debugMode = 0;
int main(int argc,char** argv){
    char pathBuffer[PATH_MAX];
    char input[MAX_INPUT_SIZE];
    int i, childPid,childPid2,inFile,outFile,stdoutCopy,stdinCopy;
    char* varBuffer;
    char hadError = 0;
    intVarLink* var_list = NULL;
    for(i = 1;i < argc;i++){
        if(strcmp(argv[i],"-d") == 0){
            debugMode = 1;
        }
    }

    cmdLine* cmd;
    process* process_list = NULL;

    int pipefd[2];

    addIntVar(&var_list,"~",getenv("HOME"));

    while(1){
        hadError = 0;
        getcwd(pathBuffer,PATH_MAX);
        printf("%s ",pathBuffer);
        fgets(input,MAX_INPUT_SIZE,stdin);

        if(strcmp(input,"\n") == 0)
            continue;

        cmd = parseCmdLines(input);

        //replace vars
        for(i = 0;i<cmd->argCount;i++){
            if(strncmp(cmd->arguments[i],"$",1) == 0){
                varBuffer = getVarVal(var_list,cmd->arguments[i]+1);
                if(varBuffer)
                    replaceCmdArg(cmd,i,varBuffer);
                else{
                    fprintf(stderr,"Variable '%s' is not defined!\n",cmd->arguments[i]+1);
                    hadError = 1;
                    free(cmd);
                    break;
                }
            }
            if(strncmp(cmd->arguments[i],"~",1) == 0){
                if((varBuffer = getVarVal(var_list,cmd->arguments[i])))
                    replaceCmdArg(cmd,i,varBuffer);
                else{
                    fprintf(stderr,"Home Variable is not defined!\n");
                    hadError = 1;
                    free(cmd);
                    break;
                }
            }
        }

        
        
        if(hadError == 0){
            if(strcmp(cmd->arguments[0],"set") == 0){
                if(cmd->argCount > 2)
                    addIntVar(&var_list,cmd->arguments[1],cmd->arguments[2]);
                else{
                    fprintf(stderr,"Error: Not enough arguments to 'set'.\n");
                }
                free(cmd);
                continue;
            }

            if(strcmp(cmd->arguments[0],"vars") == 0){
                printVarList(var_list);
                free(cmd);
                continue;
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
            if(cmd->next){
                if(pipe(pipefd) == -1){
                    perror("Error:Pipe failed");
                    exit(1);
                }
            }
            if((childPid = fork()) == 0){
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
                        fprintf(stderr,"Error: Input redirect\n");
                        free(cmd);
                        break;
                    }
                    close(inFile);
                }
                if(cmd->outputRedirect != NULL){
                    if((outFile = open(cmd->outputRedirect,O_CREAT | O_WRONLY)) < 0){
                        fprintf(stderr,"Error: Output redirect\n");
                        free(cmd);
                        break;
                    }
                    if((stdoutCopy = dup(STDOUT)) < 0){
                        fprintf(stderr,"Error: Output redirect\n");
                        free(cmd);
                        break;
                    }
                    if(dup2(outFile,STDOUT) < 0){
                        fprintf(stderr,"Error: Output redirect\n");
                        free(cmd);
                        break;
                    }
                    close(outFile);
                }
                if(cmd->next){
                    close(STDOUT);
                    dup(pipefd[1]);
                    close(pipefd[1]);
                }
                
                if((execute(cmd))==-1){
                    perror("Error has occured: ");
                    free(cmd);
                    if(cmd->next)
                        free(cmd->next);
                    _exit(1);
                }

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
                break;
            }
            else {
                if(cmd->next){
                    close(pipefd[1]);
                    if((childPid2 = fork()) == 0){
                        close(STDIN);
                        dup(pipefd[0]);
                        close(pipefd[0]);
                        if(execute(cmd->next)==-1){
                            perror("Error has occured: ");
                            free(cmd->next);
                            _exit(1);
                        }
                    }
                    else{
                        close(pipefd[0]);
                        waitpid(childPid2,NULL,0);
                        
                    }
                }
                else{
                    if(debugMode == 1){
                        fprintf(stderr,"PID: %d, Executed command: %s\n",childPid,cmd->arguments[0]);
                    }
                    addProcess(&process_list,cmd,childPid);
                    if(cmd->blocking == 1){
                        waitpid(childPid,NULL,0);
                    }
                }
            }

            if(cmd->next)
                free(cmd->next);
            free(cmd);
        }
    }
    freeVarList(var_list);
    freeProcessList(process_list);
    return 0;
}

int execute(cmdLine* pCmdLine){
    return execvp(pCmdLine->arguments[0],pCmdLine->arguments);
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

void addIntVar(intVarLink** var_list,char* var,char* val){
    if(debugMode == 1){
        fprintf(stderr,"Setting internal variable: %s = %s\n",var,val);
    }
    intVarLink* temp = *var_list;
    intVarLink* prev = NULL;
    while(temp != NULL){
        if(strcmp(temp->var,var)==0)
            break;
        prev = temp;
        temp = temp->next;
    }
    intVarLink* newLink = (intVarLink*) malloc(sizeof(intVarLink));
    newLink->var = (char*)calloc(sizeof(char),strlen(var)+1);
    strcpy(newLink->var,var);
    newLink->val = (char*)calloc(sizeof(char),strlen(val)+1);
    strcpy(newLink->val,val);
    newLink->next = NULL;

    if(temp != NULL){
        newLink->next = temp->next;
        freeIntVarLink(temp);
    }
    
    if(prev != NULL)
        prev->next = newLink;
    else
        *var_list = newLink;
}

void freeIntVarLink(intVarLink* link){
    if(debugMode == 1){
        fprintf(stderr,"Deleting internal variable: %s = %s\n",link->var,link->val);
    }
    free(link->var);
    free(link->val);
    free(link);
}

void freeVarList(intVarLink* var_list){
    intVarLink* temp;
    while(var_list != NULL){
        temp = var_list->next;
        freeIntVarLink(var_list);
        var_list = temp;
    }
}

void printVarList(intVarLink* var_list){
    printf("Internal Variables:\n");
    while(var_list != NULL){
        printf("%s = %s\n",var_list->var,var_list->val);
        var_list = var_list->next;
    }
}

char* getVarVal(intVarLink* var_list,char* wanted_var){
    while(var_list){
        if(strcmp(wanted_var,var_list->var) == 0)
            return var_list->val;
        var_list = var_list->next;
    }
    return NULL;
}