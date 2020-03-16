#include<stdio.h> 

int main(int argc,char* argv[]){
    int isAdd = 0, recieveKey = 0;
    if(argc > 1)
        if(argv[1][0] == '+'){
            isAdd = 1;
        recieveKey = 1;
    }
    int org, mod, i = 2;
    do{
        org = getc(stdin);
        mod = org;
        if(!recieveKey)
            if(org >= 97 && org <= 122)
                mod = org - 32;
        else{ 
            if(isAdd){
                mod = org + argv[1][i];
                mod = mod % 255; 
            }
            else{
                mod = org - argv[1][i];
                if(mod < 0)
                    mod += 255;  
            }
        }
        
        printf("%c",mod);
    } while(org != EOF);
	return 0;
}