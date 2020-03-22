#include<stdio.h> 
#include <string.h>

FILE* getAddress(char* arg);

int main(int argc,char* argv[]){
    int isAdd = 0,recieveKey = 0, isDebug = 0;
    int len = 1;
    FILE *output = stdout;
    int enc = 0;
    char* DEBUG_FLAG = "-D";
    for(int j=1;j<argc;j++){
        if(argv[j][0] == '-'){
            if(argv[j][1] == 'e'){
                enc = j;
                recieveKey = 1;
                len = strlen(argv[j])-2;
            }
                
            if(argv[j][1] == 'o')
                output = getAddress(argv[j]);
            
            if(strcmp(argv[j],DEBUG_FLAG) == 0){
                isDebug = 1;
                puts(argv[j]);
            }
        }
        if(argv[j][0] == '+' && argv[j][1] == 'e'){
            enc = j;
            isAdd = 1;
            recieveKey = 1;
            len = strlen(argv[j])-2;
        }
    }
    
    int org, mod, i = 0;
    int dif;

    while(1){
        org = getc(stdin);
        mod = org;
        if(org == EOF){
            fprintf(output,"\n%c\n",org);
            break;
        }
        if(recieveKey == 0){
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
        i %= len;
        if(isDebug)
            fprintf(stderr,"%d\t%d\n",org,mod);
        if(org != 10){
            fprintf(output,"%c",mod);
        }
        else{
            fprintf(output,"%c",org);
            i=0;
        }
    }
    fclose(output);
	return 0;
}

FILE* getAddress(char* arg){
    int outputSize = sizeof(arg)-2 ;
    printf("%d\n",outputSize);
    char* out = "abcdefg";
    printf("%d\n",strlen(out));
    for(int i = 0;i < strlen(out);i++)
        out[i] = arg[i+2];
    printf("arg:%d,out:%d, %s\n",strlen(arg),strlen(out),out);
    return fopen(out,"w");
}