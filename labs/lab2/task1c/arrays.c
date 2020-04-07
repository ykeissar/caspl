#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char** argv){
    int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3];
    
    printf("- &iarray: %p\n",iarray);
        printf("- &iarray+1: %p\n",iarray+1); //dif - 12, 3 int * 4, the +1 operator is giving the next free address?

    printf("- &farray: %p\n",farray);
    printf("- &farray+1: %p\n",farray+1);//dif - 12, 3 float * 4
    
    printf("- &darray: %p\n",darray);
    printf("- &darray+1: %p\n",darray+1);//dif - 24, 3 double * 8
    
    printf("- &carray: %p\n",carray);
    printf("- &carray+1: %p\n",carray+1);//dif - 3, 3 char * 1
    
    return 0;
}