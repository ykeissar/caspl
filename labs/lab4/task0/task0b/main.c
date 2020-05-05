#include "util.h"

#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19

#define O_RDRW 2

extern int system_call(int,...);

int main (int argc , char* argv[], char* envp[]){
  int fd = system_call(SYS_OPEN,argv[1], O_RDRW,0777);
  if(fd <= 0)
    return 0x55;
  
  if(system_call(SYS_LSEEK,fd,0x291,0) <= 0)
    return 0x55;
  
  if(system_call(SYS_WRITE,fd,argv[2],strlen(argv[2])) <= 0)
    return 0x55;

  system_call(SYS_CLOSE,fd);
  
  return 0;
}