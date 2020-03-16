#include<stdio.h> 

int main(int argc,char* argv[]){
    int c;
    do{

        c = fgetc(stdin);
        if(c>=97 && c<=122)
            c-= 32;
        printf("%c",c);
    } while(c != EOF);
	return 0;
}