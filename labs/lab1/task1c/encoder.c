#include<stdio.h> 
#include <string.h>

int main(int argc,char* argv[]){
    int isAdd = 0, recieveKey = 0;
    int len=1;
    if(argc > 1){
        if(argv[1][0] == '+'){
            isAdd = 1;
        }
        recieveKey = 1;
        len = strlen(argv[1])-2;
    }
    
    int org, mod, i = 0;
    int dif;

    do{
        org = getc(stdin);
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