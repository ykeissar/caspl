#include<stdio.h>
#include <stdlib.h>

int printHex(char* buffer, int length,FILE* f);

int main(int argc, char** argv){
  FILE* file = fopen(argv[1],"r");
  char* holder = (char*) malloc(sizeof(char));
  FILE* f = fopen("sig","w");

  while(fread(holder,1,1,file) > 0){
    printHex(holder,1,f);
  }

  puts("");
  free(holder);
  return 0;
}

int printHex(char* buffer, int length,FILE* output){
  int i;
  fprintf(output,"printhexstart:\n");
  for(i = 0 ; i < length ; i++)
    fprintf(output,"%02hhX ",*buffer);
  return 0;
}