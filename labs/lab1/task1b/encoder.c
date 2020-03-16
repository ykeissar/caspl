#include<stdio.h> 

int main(int argc,char* argv[]){
    int isDebug = 0;
    if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'D'){
        isDebug = 1;
        puts(argv[1]);
    }
    int org, mod;
    do{
        org = getc(stdin);
        mod = org;
        if(org >= 97 && org <= 122){
            mod = org - 32;
        }
        if(isDebug && org != EOF)
            fprintf(stderr,"%d\t%d\n",org,mod);
        printf("%c",mod);
    } while(org != EOF);
	return 0;
}