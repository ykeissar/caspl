#include<stdio.h> 
#include <string.h>

FILE* getAddress(char* arg);

int main(int argc,char* argv[]){
    int isAdd = 0, recieveKey = 0;
    int len=1;
    FILE *input = stdin;
    int enc = 0,inp = 0;
    if(argc > 1){
        if(argv[1][1] == 'e')
            enc = 1;
        else
            inp = 1;
        if(argc > 2){
            if(argv[2][1] == 'e')
                inp = 2;
            else
                enc = 2;
        }
    }
    
    if(enc > 0){
        if(argv[enc][0] == '+'){
                isAdd = 1;
            }
        recieveKey = 1;
        len = strlen(argv[enc])-2;
    }
    if(inp > 0){
        input = getAddress(argv[inp]);
    }

    int org, mod, i = 0;
    int dif;

    do{
        org = getc(input);
        mod = org;
        if(recieveKey == 0){
            if(org >= 97 && org <= 122)
                mod = org - 32;
        }
        else{ 
            dif = argv[1][i+2];
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
            printf("%c",org);
        }
    } while(org != EOF);
	return 0;
}

FILE* getAddress(char* arg){
    char out[strlen(arg)-2];
    for(int i = 0;i < strlen(out);i++)
        out[i] = arg[i+2];
    return fopen(out,"r");
}