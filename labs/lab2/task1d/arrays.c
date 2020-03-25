#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char** argv){
    int i;
    int iarray[] = {1,2,3};
    char carray[] = {'a','b','c'};
    int* iarrayPtr = iarray;
    char* carrayPtr = carray;
    int* p;
    for(i=0;i<3;i++){
        printf("iarrayPtr[i]:%d ",*(iarrayPtr+i));
        printf("carrayPtr[i]:%c\n",*(carrayPtr+i));
    }
    printf("p: %p\n",p); //noticed that its in the stack (high address), near iarrayPtr and carrayPtr
    return 0;
}