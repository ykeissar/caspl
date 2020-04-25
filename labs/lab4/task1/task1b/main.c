#include "util.h"

#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_LSEEK 19
#define SYS_CLOSE 6

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_CREATE 64

int sys_call_prnt(int sysCallId,int returnValue,int logFd);
int system_call(int,...);

int main (int argc , char* argv[], char* envp[]){
  char c,org;
  int i,bytesRead;
  int logFd = -1,inFd = 0,outFd = 1;
  
  for(i = 1 ; i < argc ; i++){
    if(strcmp("-D",argv[i])==0){
      logFd = system_call(SYS_OPEN,"debug_log",O_WRONLY | O_CREATE,0644);
    }
    if(strncmp("-o",argv[i],2)==0){
      outFd = sys_call_prnt(SYS_OPEN,system_call(SYS_OPEN,argv[i]+2,O_WRONLY | O_CREATE,0644),logFd);
    }
    if(strncmp("-i",argv[i],2)==0){
      inFd = sys_call_prnt(SYS_OPEN,system_call(SYS_OPEN,argv[i]+2,O_RDONLY,0644),logFd);
    }
  }

  while(1){      
    bytesRead = sys_call_prnt(SYS_READ,system_call(SYS_READ,inFd,&c,1),logFd);
    if(bytesRead <= 0)
      break;
    org = c;
    if(c >= 97 && c <=122)
       c = c - 32;
    if(logFd >= 0 && c != 10){
      sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,logFd,&org,1),logFd);
      sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,logFd,"\t",1),logFd);
      sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,logFd,&c,1),logFd);
      sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,logFd,"\n",1),logFd);
    }
    sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,outFd,&c,1),logFd);
  }
  if(logFd >= 0)
    sys_call_prnt(SYS_CLOSE,system_call(SYS_CLOSE,logFd),logFd);
  return 0;
}

int sys_call_prnt(int sysCallId,int returnValue,int logFd){
  if(logFd >= 0){
    system_call(SYS_WRITE,logFd,"\nsys call: ",11);
    system_call(SYS_WRITE,logFd,itoa(sysCallId),1);
    system_call(SYS_WRITE,logFd," return value: ",15);
    system_call(SYS_WRITE,logFd,itoa(returnValue),1);
    system_call(SYS_WRITE,logFd,"\n",1);
  }
  return returnValue;
}