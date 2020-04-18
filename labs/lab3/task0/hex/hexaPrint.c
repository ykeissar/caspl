#include<stdio.h>
#include <stdlib.h>

int print_hex(unsigned char* buffer, int length,FILE* f);

int main(int argc, char** argv){
  FILE* file = fopen(argv[1],"r");
  unsigned char* holder = (char*) malloc(sizeof(char));
  FILE* f = fopen("sig","r");

  while(fread(holder,1,1,file) > 0){
    print_hex(holder,1,f);
  }

  puts("");
  free(holder);
  return 0;
}

int print_hex(unsigned char* buffer, int length,FILE* output){
  int i;
  fprintf(output,"print_hexstart:\n");
  for(i = 0 ; i < length ; i++)
    fprintf(output,"%02X ",*buffer);
  return 0;
}