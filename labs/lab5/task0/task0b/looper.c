#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void sighandle(); 

int main(int argc, char **argv){ 

	printf("Starting the program pid: %d\n",getpid());
	int c=0;

    signal(SIGTSTP,sighandle);
    signal(SIGINT,sighandle);
    signal(SIGCONT,sighandle);

    while(1) {
       sleep(2);
	}
    
	return 0;
}

void sighandle(int sig) {
    printf("signal - '%s' was recevie.\n",strsignal(sig));
    
    if(sig == SIGTSTP){
        signal(SIGCONT,sighandle);
    }

    if(sig == SIGCONT){
        signal(SIGSTOP,sighandle);
        signal(SIGINT,sighandle);
    }

    signal(sig, SIG_DFL);
    raise(sig);
} 