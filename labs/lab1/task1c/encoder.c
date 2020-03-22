#include<stdio.h> 
#include <string.h>

int main(int argc,char* argv[]){
    int isAdd = 0, recieveKey = 0, isDebug = 0;
    int len=1,encIndex = 0;
    char* DEBUG_FLAG = "-D";
    
    for(int j=1;j<argc;j++){
        if(argv[j][0] == '-'){
            if(argv[j][1] == 'e'){
                recieveKey = 1;
                encIndex = j;
                len = strlen(argv[j])-2;
            }
            if(strcmp(argv[j],DEBUG_FLAG) == 0){
                isDebug = 1;
                puts(argv[j]);
            }
        }
        if(argv[j][0] == '+' && argv[j][1] == 'e'){
            isAdd = 1;
            recieveKey = 1;
            encIndex = j;
            len = strlen(argv[j])-2;
        }
    }
    
    int org, mod, i = 0;
    int dif;

    while(1){
        org = getc(stdin);
        if(org == EOF){
            printf("\n%c\n",org);
            break;
        }
        mod = org;
        if(recieveKey == 0){
            if(org >= 97 && org <= 122)
                mod = org - 32;
        }
        else{ 
            dif = argv[encIndex][i+2];
            dif -= 48;
            if(isAdd){
                mod = org + dif;
                mod %= 255; 
            }
            else{
                mod = org - dif;
                if(mod < 0)
                    mod += 255;  
            }
        }
        i++;
        i %= len;
        if(isDebug)
            fprintf(stderr,"%d\t%d\n",org,mod);
        if(org != 10){
            printf("%c",mod);
        }
        else{
            printf("%c",org);
            i = 0;
        }
    }
	return 0;
}