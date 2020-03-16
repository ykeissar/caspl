#include<stdio.h> 

int main(int argc,char* argv[]){
    bool isDebug = flase;
    if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'D'){
        isDebug = true;
        puts(argv[1]);
    }
    int org, mod;
    do{
        org = getc(stdin);
        mod = org;
        if(org >= 97 && org <= 122){
            mod = org - 32;
        }
        if(isDebug)
            fprintf(stderr,"%d\t%d\n",org,mod);
        printf("%c",mod);
    } while(corg != EOF);
	return 0;
}