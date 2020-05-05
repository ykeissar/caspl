#include "util.h"

#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SYS_GETDENTS 141

#define STDIN 0
#define STDOUT 1

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_CREATE 64
#define O_DIRECTORY 65536

typedef struct linux_dirent {
               unsigned long  d_ino;     /* Inode number */
               unsigned long  d_off;     /* Offset to next linux_dirent */
               unsigned short d_reclen;  /* Length of this linux_dirent */
               char           d_name[];  /* Filename (null-terminated) */
                                 /* length is actually (d_reclen - 2 -
                                    offsetof(struct linux_dirent, d_name)) */
               /*
               char           pad;       // Zero padding byte
               char           d_type;    // File type (only since Linux
                                         // 2.6.4); offset is (d_reclen - 1)
               */
           }dirent;


const int MAX_DATA_SIZE = 8192;

extern int system_call(int,...);
int sys_call_print(int sysCallId, int returnValue,int logFd);

int main (int argc , char* argv[], char* envp[]){
  char buffer[MAX_DATA_SIZE];
  int i, fd, read;
  int logFd = -1, offset = 0;
  dirent* d;

  for (i = 1 ; i < argc ; i++){
    if(strcmp("-D",argv[i]) == 0){
      logFd = system_call(SYS_OPEN,"debug_log",O_CREATE | O_WRONLY, 0644);
    }
  }

  fd = sys_call_print(SYS_OPEN,system_call(SYS_OPEN,".",O_RDONLY | O_DIRECTORY, 0644),logFd);
  if(fd < 0)
    return 0x55;
  
  read = sys_call_print(SYS_GETDENTS,system_call(SYS_GETDENTS,fd,buffer,MAX_DATA_SIZE),logFd);
  if(read < 0)
    return 0x55;

  while(offset < read){
    d = (dirent*) (buffer+offset);
    if(d->d_ino != 0){
      sys_call_print(SYS_WRITE,system_call(SYS_WRITE,STDOUT,d->d_name,strlen(d->d_name)),logFd);
      sys_call_print(SYS_WRITE,system_call(SYS_WRITE,STDOUT,"\n",1),logFd);
      if(logFd >= 0){
        system_call(SYS_WRITE,logFd,"file: ",6);
        system_call(SYS_WRITE,logFd,d->d_name,strlen(d->d_name));
        system_call(SYS_WRITE,logFd," length: ",9);
        system_call(SYS_WRITE,logFd,itoa(d->d_reclen),strlen(itoa(d->d_reclen)));
        system_call(SYS_WRITE,logFd,"\n",1);
      } 
    }
    offset += d->d_reclen;
  }
  sys_call_print(SYS_CLOSE,system_call(SYS_CLOSE,fd),logFd);

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