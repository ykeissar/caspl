#include<stdio.h> 

int main(int argc,char* argv[]){
    bool isDebug = flase;
    if(argc > 1 && argv[1] == '-D')
        isDebug = true;
    int org, mod, i=0;
    char[] str = fgets(stdin);
    do{
        org = str[i];
        mod = org;
        if(org >= 97 && org <= 122){
            mod = org - 32;
        }
        printf("%d\t%d\n",org,mod);
        i++;
    } while(corg != EOF);
	return 0;
}