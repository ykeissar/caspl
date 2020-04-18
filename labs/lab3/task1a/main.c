#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct virus{
  unsigned short SigSize;
  char virusName[16];
  unsigned char* sig;
}virus;

void destructVirus(virus * vir);

virus* readVirus(FILE* input);
void printVirus(virus* virus, FILE* output);

int main(void){
  FILE* f = fopen("signatures","r");
  virus* v;
  while((v=readVirus(f))!= NULL){
    printVirus(v,stdout);
    destructVirus(v);
  }
  fclose(f);
  return 0;
}

int print_hex(unsigned char* buffer, int length,FILE* output){
  int i;
  for(i = 0 ; i < length ; i++)
    fprintf(stdout,"%02X ",*(buffer+i));
  return 0;
}

virus* readVirus(FILE* input){
  virus* vir = (virus*) malloc(sizeof(virus));
  char* buffer= (char*) malloc(sizeof(char)*18);

  if(fread(buffer,18,1,input) <= 0){
    free(vir);
    free(buffer);
    return NULL;
  }

  char size[2];
  memcpy(size,buffer,2);
  vir->SigSize = size[0] + size[1]*16*16;
  memcpy(vir->virusName,buffer+2,16);
  free(buffer);

  vir->sig = (unsigned char*) malloc(sizeof(unsigned char)*(vir->SigSize));
  if(fread(vir->sig,vir->SigSize,1,input) <= 0){
    fprintf(stderr,"Virus's format in the file broken.\n");
    free(vir);
    return NULL;
  }
  return vir;
}

void printVirus(virus* virus, FILE* output){
  fprintf(output,"Virus name: %s\nVirus size: %d\nsignature:\n",virus->virusName,virus->SigSize);
  print_hex(virus->sig,virus->SigSize,output);
  fprintf(output,"\n\n");
}

void destructVirus(virus * vir){
  free(vir->sig);
  free(vir);
}