#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void sighandle(); 

int main(int argc, char **argv){ 

	printf("Starting the program pid: %d\n",getpid());
    while(1) {
        signal(SIGTSTP,sighandle);
        signal(SIGINT,sighandle);
        signal(SIGCONT,sighandle);
	}

	return 0;
}

void sighandle(int sig) {
    signal(sig, SIG_DFL);
    printf("%d: signal - '%s' was recevie.\n",getpid(),strsignal(sig));
    raise(sig);
    if(sig == SIGTSTP)
        signal(SIGCONT,sighandle);
    if(sig == SIGCONT)
        signal(SIGTSTP,sighandle);
} 