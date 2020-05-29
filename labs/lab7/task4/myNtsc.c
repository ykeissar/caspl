#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int countDig(char* str){
    int n=0;
    int i=0;
    while(str[i]){
        if(str[i] >= '0' && str[i] <= '9')
            n++;
        i++;
    }
    return n;
}

int main(int argc,char** argv){
    if(argc >1){
        printf("The number of digits in the string is: %d\n",countDig(argv[1]));
    }
    return 0;
}