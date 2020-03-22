#include<stdio.h> 

int main(int argc,char* argv[]){
    int c;
    while(1){
        c = fgetc(stdin);
        if(c == EOF){
            printf("%c\n",c);
            break;
        }
        if(c>=97 && c<=122)
            c-= 32;
        printf("%c",c);
    } 
	return 0;
}