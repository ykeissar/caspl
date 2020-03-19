#include<stdio.h> 
#include <string.h>

FILE* getAddress(char* arg);

int main(int argc,char* argv[]){
    int isAdd = 0;
    int len = 1;
    FILE *output = stdout;
    int enc = 0,outIndex = 0;
    if(argc > 1){
        if(argv[1][1] == 'e')
            enc = 1;
        else
            outIndex = 1;
        if(argc > 2){
            if(argv[2][1] == 'e')
                enc = 2;
            else
                outIndex = 2;
        }
    }
    
    if(enc > 0){
        if(argv[enc][0] == '+')
                isAdd = 1;
        len = strlen(argv[enc])-2;
    }
    if(outIndex > 0){
        output = getAddress(argv[outIndex]);
    }
    int org, mod, i = 0;
    int dif;

    do{
        org = getc(stdin);
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
            fprintf(output,"%c",mod);
        }
        else{
            fprintf(output,"%c",org);
            i=0;
        }
    } while(org != EOF);
	return 0;
}

FILE* getAddress(char* arg){
    char out[strlen(arg)-2];
    for(int i = 0;i < strlen(out);i++)
        out[i] = arg[i+2];
    return fopen(out,"w");
}