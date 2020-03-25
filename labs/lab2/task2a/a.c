#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}
 
char* map(char *array, int array_length, char (*f) (char)){
    int i;
    char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
    for(i=0;i<array_length;i++){
        *(mapped_array+i)= f(*(array+i)); //*(mapped_array+i) = f(array+i);
    }
    return mapped_array;
}
 
int main(int argc, char **argv){
    char* ar = "yoav!!!";
    char* ar2 = map(ar,7,censor);
    puts(ar2);
    free(ar2);
    return 0;
}