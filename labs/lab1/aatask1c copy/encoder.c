#include<stdio.h> 
#include <string.h>

int main(int argc,char* argv[]){
    FILE* a;
    if(fopen("input","r"))
    {
        a = fopen("input","r");
        char s[5];
        fgets(s,3,a);
        printf("%s\n",s);
    }
    else
    {
        printf("Not exist\n");
    }
    
	return 0;
}