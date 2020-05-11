#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "../../LineParser.h"
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 2048

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

int main(int argc, char** argv){
    sleep(atoi(argv[1]));
    printf("Done Sleeping!\n");
    fflush(stdout);
    return 0;
}