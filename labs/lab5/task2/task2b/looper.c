#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void sighandle(); 

int main(int argc, char **argv){ 

	printf("Starting the program pid: %d\n",getpid());
    fflush(stdout);
	int c=0;

    signal(SIGTSTP,sighandle);
    signal(SIGTSTP, SIG_DFL);
    
    signal(SIGINT,sighandle);
    signal(SIGINT, SIG_DFL);

    signal(SIGCONT,sighandle);
    signal(SIGCONT, SIG_DFL);

    while(1) {
        sleep(2);
	}
	return 0;
}

void sighandle(int sig) {
    printf("signal - '%s' was recevie.\n",strsignal(sig));
    fflush(stdout);
    raise(sig);
} 