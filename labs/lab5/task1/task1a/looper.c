#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void sighandle(); 

int main(int argc, char **argv){ 

	printf("Starting the program pid: %d\n",getpid());
	int c=0;
    while(1) {
        signal(SIGTSTP,sighandle);
        signal(SIGINT,sighandle);
        signal(SIGCONT,sighandle);
	}

	return 0;
}

void sighandle(int sig) {
    signal(sig, SIG_DFL);
    printf("signal - '%s' was recevie.\n",strsignal(sig));
    raise(sig);
} 