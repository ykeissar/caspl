#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char** argv){
    int i;
    int iarray[] = {1,2,3};
    char carray[] = {'a','b','c'};
    int* iarrayPtr = &iarray[0];
    char* carrayPtr = &carray[0];
    int* p;
    for(i=0;i<3;i++){
        printf("iarrayPtr[i]:%d, %p ",*(iarrayPtr+i),iarrayPtr+1);
        printf("carrayPtr[i]:%c, %p\n",*(carrayPtr+i),carray+1);
    }
    printf("p: %p\n",p); //noticed that its in the stack (high address), near iarrayPtr and carrayPtr
    return 0;
}