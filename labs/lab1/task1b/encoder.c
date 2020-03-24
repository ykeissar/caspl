#include <stdio.h>
#include <string.h>

int main(int argc,char* argv[]){
    int org,mod,i,isDebug;

    for(i = 1;i < argc; i++){
        if(argv[i][0] == '-'){
            if(argv[i][1] == 'D' && strlen(argv[i]) == 2){
                puts(argv[i]);
                isDebug = 1;
            }
        }
    }

    while(1){
        org = getc(stdin);
        mod = org;
        if(org == EOF)
            break;
        
        if(org >= 97 && org<= 122){
            mod = org - 32;
        }
        
        if(org != 10){
            if(isDebug == 1)
                fprintf(stderr,"%d\t%d\n",org,mod);
            fputc(mod,stdout);
        }
        else
            fputc(org,stdout);
    }
    
    return 0;
}