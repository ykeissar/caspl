#include<stdio.h> 
#include <string.h>

int getAddress(char* arg,FILE* fiile);

int main(int argc,char* argv[]){
    int isAdd = 0;
    int len=1;
    FILE *input =stdin;
    int enc = 0,inp = 0;
    if(argc > 1){
        if(argv[1][1] == 'e')
            enc = 1;
        else
            inp = 1;
        if(argc > 2){
            if(argv[2][1] == 'e')
                enc = 2;
            else
                inp = 2;
        }
    }
    
    if(enc > 0){
        if(argv[enc][0] == '+')
            isAdd = 1;
        len = strlen(argv[enc])-2;
    }

    if(inp > 0){
        if(getAddress(argv[inp],input) == 1){
            fprintf(stderr,"File '%s' does not exist!\n",argv[inp]);
            return 1;
        }
        //getAddress(argv[inp],input);
    } 
    
    int org, mod, i = 0;
    int dif;

    do{
        org = getc(input);
        mod = org;
        if(enc == 0){
            if(org >= 97 && org <= 122)
                mod = org - 32;
        }
        else{ 
            dif = argv[enc][i+2];
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
        i = i % len;
        if(org != 10 && org != -1){
            printf("%c",mod);
        }
        else{
            if(org == 10){
                printf("%c",org);
                i = 0;
            }
            else
            {
                printf(" -1: %d",org);
                break;    
            }
            

        }
    } while(org != EOF);
	return 0;
}

int getAddress(char* arg,FILE* file){
    char inp[strlen(arg)-2];
    for(int i = 0;i < strlen(inp);i++)
        inp[i] = arg[i+2];
    if(fopen(inp,"r")){
        *file = *fopen(inp,"r");
        return 0;
    }
    else
        return 1;
}