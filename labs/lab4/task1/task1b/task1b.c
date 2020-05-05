#include "util.h"

#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19

#define STDIN 0
#define STDOUT 1

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_CREATE 64


extern int system_call(int,...);
int sys_call_print(int sysCallId, int returnValue,int logFd);

int main (int argc , char* argv[], char* envp[]){
  char c, org;
  int i, bytesRead, inIndex, outIndex;
  int logFd = -1, inFd = STDIN, outFd = STDOUT;

  for (i = 1 ; i < argc ; i++){
    if(strcmp("-D",argv[i]) == 0){
      logFd = system_call(SYS_OPEN,"debug_log",O_CREATE | O_WRONLY, 0644);
    }
    if(strncmp("-i",argv[i],2) == 0){
      inFd = system_call(SYS_OPEN,argv[i]+2, O_RDONLY, 0644);
      inIndex = i;
    }
    if(strncmp("-o",argv[i],2) == 0){
      outFd = system_call(SYS_OPEN,argv[i]+2,O_CREATE | O_WRONLY, 0644);
      outIndex = i;
    }
  }
  if(inFd != STDIN){
    system_call(SYS_WRITE,logFd,"input path: ",12);
    system_call(SYS_WRITE,logFd,argv[inIndex]+2,strlen(argv[inIndex]+2));
    system_call(SYS_WRITE,logFd,"\n",1);
  }

  if(outFd != STDOUT){
    system_call(SYS_WRITE,logFd,"output path: ",13);
    system_call(SYS_WRITE,logFd,argv[outIndex]+2,strlen(argv[outIndex]+2));
    system_call(SYS_WRITE,logFd,"\n",1);
  }
  
  while(1){
    bytesRead = sys_call_print(SYS_READ,system_call(SYS_READ,inFd,&c,1),logFd);
    if(bytesRead <= 0)
      break;
    org = c;
    if(c >= 'a' && c <= 'z')
      c = c - ('z' - 'a');
    if(logFd >= 0 && c != '\n'){
      system_call(SYS_WRITE,logFd,"original char: ",15);
      system_call(SYS_WRITE,logFd,&org,1);
      system_call(SYS_WRITE,logFd,"\t",1);
      system_call(SYS_WRITE,logFd,"after change: ",14);
      system_call(SYS_WRITE,logFd,&c,1);
      system_call(SYS_WRITE,logFd,"\n",1);
    }
    sys_call_print(SYS_WRITE,system_call(SYS_WRITE,outFd,&c,1),logFd);
  }
  if(logFd >= 0){
    system_call(SYS_CLOSE,logFd);
  }
  if(inFd != STDIN){
    system_call(SYS_CLOSE,inFd);
  }
  if(outFd != STDOUT){
    system_call(SYS_CLOSE,outFd);
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