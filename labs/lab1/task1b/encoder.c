#include<stdio.h> 

int main(int argc,char* argv[]){
    int isDebug = 0;
    
    for(int i=1;i<argc;i++){
        if(argv[i][0] == '-'){
            if(argv[i][1]== 'D'){
                isDebug = 1;
                puts(argv[i]);
            }
        }
    }

    int org, mod;
    while(1){
        org = getc(stdin);
        if(org == EOF){
            printf("\n%c\n",org);
            break;
        }
        mod = org;
        if(org >= 97 && org <= 122){
            mod = org - 32;
        }
        if(isDebug)
            fprintf(stderr,"%d\t%d\n",org,mod);
        printf("%c",mod);
    }
	return 0;
}