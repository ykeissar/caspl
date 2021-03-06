#include "util.h"

#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19

#define STDIN 0
#define STDOUT 1

#define O_WRONLY 1
#define O_RDRW 2
#define O_CREATE 64


extern int system_call(int,...);
int sys_call_print(int sysCallId, int returnValue,int logFd);

int main (int argc , char* argv[], char* envp[]){
  char c, org;
  int i, bytesRead;
  int logFd = -1;

  for (i = 1 ; i < argc ; i++){
    if(strcmp("-D",argv[i]) == 0){
      logFd = system_call(SYS_OPEN,"debug_log",O_CREATE | O_WRONLY, 0644);
    }
  }
  
  while(1){
    bytesRead = sys_call_print(SYS_READ,system_call(SYS_READ,STDIN,&c,1),logFd);
    if(bytesRead <= 0)
      break;
    org = c;
    if(c >= 97 && c <= 122)
      c = c - 32;
    if(logFd >= 0 && c != 10){
      system_call(SYS_WRITE,logFd,"original char: ",15);
      system_call(SYS_WRITE,logFd,&org,1);
      system_call(SYS_WRITE,logFd,"\t",1);
      system_call(SYS_WRITE,logFd,"after change: ",14);
      system_call(SYS_WRITE,logFd,&c,1);
      system_call(SYS_WRITE,logFd,"\n",1);
    }
    sys_call_print(SYS_WRITE,system_call(SYS_WRITE,STDOUT,&c,1),logFd);
  }
  if(logFd >= 0){
    system_call(SYS_CLOSE,logFd);
  }
  return 0;
}

int sys_call_print(int sysCallId, int returnValue,int logFd){
  if(logFd >= 0){
    system_call(SYS_WRITE,logFd,"sys call: ",10);
    system_call(SYS_WRITE,logFd,itoa(sysCallId),1);
    system_call(SYS_WRITE,logFd," return value: ",15);
    system_call(SYS_WRITE,logFd,itoa(returnValue),1);
    system_call(SYS_WRITE,logFd,"\n",1);
  }
  return returnValue;
}