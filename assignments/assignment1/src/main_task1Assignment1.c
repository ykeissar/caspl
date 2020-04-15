#include <stdio.h>
 #include <string.h>
 #include <stdlib.h>

 extern void assFunc(int x, int y);
 char c_check_validity(int x, int y);

 int main(int argc, char** argv){
     printf("%s", "Please enter two numbers : ");
     int x,y;
    scanf("%d",&x);
    scanf("%d",&y);
    assFunc(x,y);
     return 0;
 }

 char c_check_validity(int x, int y){
    if(x>=y)
        return 1;
    return 0 ;
 }