#include <stdio.h>
#include <string.h>

int main(int argc,char* argv[]){
    int org,mod,i,isDebug,isAdd = 0,encIndex = 0,keyIndex = 0,dif,keyLen = 0;

    for(i = 1;i < argc; i++){
        if(argv[i][0] == '-'){
            if(argv[i][1] == 'D' && strlen(argv[i]) == 2){
                puts(argv[i]);
                isDebug = 1;
            }
            if(argv[i][1] == 'e'){
                encIndex = i;
                keyLen = strlen(argv[i])-2;
            }
        }
         if(argv[i][0] == '+'){
             if(argv[i][1] == 'e'){
                isAdd = 1;
                encIndex = i;
                keyLen = strlen(argv[i])-2;
             }
         }
    }

    while(1){
        org = getc(stdin);
        mod = org;
        if(org == EOF){
            break;
        }
        if(encIndex == 0){
            if(org >= 97 && org<= 122){
                mod = org - 32;
            }
        }
        else{
            dif = argv[encIndex][keyIndex+2] - 48;
            if(isAdd == 1){
                mod = org + dif;
                mod %= 128;
            }
            else{
                mod = org - dif;
                if(mod < 0){
                    mod += 128;
                }
            }
            keyIndex++;
            keyIndex %= keyLen;
        }

        if(org != 10){
            if(isDebug == 1 )
                fprintf(stderr,"%d\t%d\n",org,mod);
            fputc(mod,stdout);
        }
        else{
            fputc(org,stdout);
            keyIndex = 0;
        }
        
    }
    return 0;
}