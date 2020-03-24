#include <stdio.h>

int main(int argc,char* argv[]){
    int org,mod;
    while(1){
        org = getc(stdin);
        mod = org;
        if(org == EOF){
            break;
        }
        if(org >= 97 && org<= 122){
            mod = org - 32;
        }
        if(org != 10)
            fputc(mod,stdout);
        else
            fputc(org,stdout);
        
    }
    return 0;
}