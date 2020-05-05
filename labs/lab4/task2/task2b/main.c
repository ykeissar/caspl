#include "util.h"


#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_LSEEK 19
#define SYS_CLOSE 6
#define SYS_GETDENTS 141

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_CREATE 64
#define O_DIRECTORY 65536

/* File types for `d_type'.  */

# define DT_UNKNOWN 0
# define DT_FIFO 1
# define DT_CHR 2
# define DT_DIR 4
# define DT_BLK 6
# define DT_REG 8
# define DT_LNK 10
# define DT_SOCK 12


int sys_call_prnt(int sysCallId,int returnValue,int logFd);
int system_call(int,...);

char* getStringType(int d_type);

const int MAX_DATA_SIZE = 8192;

typedef struct linux_dirent {
               unsigned long  d_ino;     /* Inode number (4) */
               unsigned long  d_off;     /* Offset to next linux_dirent (4)*/
               unsigned short d_reclen;  /* Length of this linux_dirent (2) */
               char           d_name[];  /* Filename (null-terminated) */
                                 /* length is actually (d_reclen - 2 -
                                    offsetof(struct linux_dirent, d_name)) */
               /*
               char           pad;       // Zero padding byte
               char           d_type;    // File type (only since Linux
                                         // 2.6.4); offset is (d_reclen - 1)
               */
           }dirent;

int main (int argc , char* argv[], char* envp[]){
  int i,fd,read,offset, prefixSize;
  int logFd = -1, prefixIndex = -1;
  char buffer[MAX_DATA_SIZE];
  char* stringType;
  char fileType;
  dirent* d;
  
  for(i = 1 ; i < argc ; i++){
    if(strcmp("-D",argv[i]) == 0){
      logFd = system_call(SYS_OPEN,"debug_log",O_WRONLY | O_CREATE,0644);
      system_call(SYS_WRITE,logFd,"Start Debugger!\n",16);
    }
      if(strncmp("-p",argv[i],2) == 0){
      prefixIndex = i;
      prefixSize = strlen(argv[i])-2;
      argv[i] = argv[i]+2;
    }
  }
  fd = sys_call_prnt(SYS_OPEN,system_call(SYS_OPEN,".",O_RDONLY | O_DIRECTORY),logFd);
  if(fd < 0)
    return 0x55;
  read = sys_call_prnt(SYS_GETDENTS,system_call(SYS_GETDENTS,fd,buffer,MAX_DATA_SIZE),logFd);
  if(read < 0)
    return 0x55;

  offset = 0;

  while(offset < read){
    d = (dirent*) (buffer+offset);
    if(d->d_ino != 0){
      if(prefixIndex != -1){
        fileType = *(buffer + offset + d->d_reclen - 1);
        if(strncmp(argv[prefixIndex],d->d_name,prefixSize) == 0){
          if(logFd >= 0){
            sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,logFd,d->d_name,strlen(d->d_name)),logFd); 
            sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,logFd,itoa(d->d_reclen),strlen(itoa(d->d_reclen))-1),logFd); 
          }
          sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,STDOUT,d->d_name,strlen(d->d_name)),logFd); 
          sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,STDOUT,"\n",1),logFd);
          sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,STDOUT,&fileType,1),logFd);

          stringType = getStringType(fileType);
          sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,STDOUT,stringType,strlen(stringType)),logFd); 
          sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,STDOUT,"\n",1),logFd);
        }
      }
      else {
        sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,STDOUT,d->d_name,strlen(d->d_name)),logFd); 
        sys_call_prnt(SYS_WRITE,system_call(SYS_WRITE,STDOUT,"\n",1),logFd);

      }
    }
    offset = offset + d->d_reclen;
  }

  sys_call_prnt(SYS_CLOSE,system_call(SYS_CLOSE,fd),logFd);
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

char* getStringType(int d_type){
  switch(d_type){
    case DT_FIFO:
      return "d_type: 'FIFO'";
      break;
    case DT_CHR:
      return "d_type: 'char dev'";
      break;
    case DT_DIR:
      return "d_type: 'directory'";
      break;
    case DT_BLK:
      return "d_type: 'block dev'";
      break;
    case DT_REG:
      return "d_type: 'regular'";
      break;
    case DT_LNK:
      return "d_type: 'symlink'";
      break;
    case DT_SOCK:
      return "d_type: 'socket'";
      break;
    default:
      return "d_type: 'unknown'";
      break;
  }
}