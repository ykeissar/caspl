#include<stdio.h> 
#include <string.h>

int main(int argc,char* argv[]){
    int isAdd = 0,recieveKey = 0,isDebug =0;
    int len=1;
    FILE *input = stdin;
    int enc = 0,inp = 0;

    for(int j=1;j<argc;j++){
        if(argv[j][1] == 'e'){
            enc = j;
            recieveKey = 1;
            if(argv[j][0] == '+')
                isAdd = 1;
            len = strlen(argv[j])-2;
        }
            
        if(argv[j][1] == 'i'){
            size_t inputSize = strlen(argv[j])-2 ;
            char inPath[inputSize];
        
            for(int i = 0;i < strlen(inPath);i++)
                inPath[i] = argv[j][i+2];
            if((input=fopen(inPath,"r")) == NULL){
                fprintf(stderr,"File '%s' does not exist!\n",inPath);
                return 1;
            }
        }
        
        if(argv[j][1] == 'D'){
            if(strncmp(argv[j],"-D",2) == 0){
                isDebug = 1;
                puts(argv[j]);
            }
        }
    }

    int org, mod, i = 0;
    int dif;

    while(1){
        org = getc(input);
        if(org == EOF){
            printf("\n%c\n",org);
            break;
        }
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
        i %= len;
        if(isDebug)
            fprintf(stderr,"%d\t%d\n",org,mod);
        if(org != 10)
            printf("%c",mod);
        else{
            printf("%c",org);
            i = 0;
        }
    }
    fclose(input);
	return 0;
}