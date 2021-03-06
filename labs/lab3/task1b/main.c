#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct virus{
  unsigned short SigSize;
  char virusName[16];
  unsigned char* sig;
}virus;

typedef struct link{
  struct link* nextVirus;
  virus* vir;
} link; 

link* list_print(link *virus_list, FILE* ouput); 
     /* Print the data of every link in list to the given stream. Each item followed by a newline character. */
 
link* list_append(link* virus_list, virus* data); 
     /* Add a new link with the given data to the list 
        (either at the end or the beginning, depending on what your TA tells you),
        and return a pointer to the list (i.e., the first link in the list).
        If the list is null - create a new entry and return a pointer to the entry. */
 
void list_free(link *virus_list); /* Free the memory allocated by the list. */

link* create_link(virus* data);
virus* read_virus(FILE* input);
void print_virus(virus* virus, FILE* output);

link* load_signatures(link *virus_list, FILE* ouput);
link* quit(link* virus_list,FILE* output);

struct func_desc{
  char* name;
  link* (*fun)(link*,FILE*);
};

struct func_desc menu[] = {{"Load signatures",load_signatures},{"Print signatures",list_print},{"Quit",quit},{NULL,NULL}};

const size_t MENU_SIZE = 3;

int main(void){
  int i, choosen;
  link* virus_list = NULL;

  while(1){
    for(i = 0 ; i < MENU_SIZE ; i++){
      printf("%d) %s\n",i+1,menu[i].name);
    }
    printf("Option: ");
    scanf("%d",&choosen);
    virus_list = menu[choosen-1].fun(virus_list,stdout);
  }

  return 0;
}

int print_hex(unsigned char* buffer, int length,FILE* output){
  int i;
  for(i = 0 ; i < length ; i++)
    fprintf(stdout,"%02X ",*(buffer+i));
  return 0;
}

virus* read_virus(FILE* input){
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
    free(vir);
    return NULL;
  }
  return vir;
}

void print_virus(virus* virus, FILE* output){
  fprintf(output,"Virus Name: %s\nVirus Size: %d\nsignature:\n",virus->virusName,virus->SigSize);
  print_hex(virus->sig,virus->SigSize,output);
  fprintf(output,"\n\n");
}

void destructVirus(virus * vir){
  free(vir->sig);
  free(vir);
}

link* list_print(link *virus_list, FILE* ouput){
  while(virus_list != NULL){
    print_virus(virus_list->vir,ouput);
    virus_list = virus_list->nextVirus;
  }
  return virus_list;
}

link* list_append(link* virus_list, virus* data){
  if(virus_list == NULL){
    return create_link(data);
  }

  link* saveFirst = virus_list;
  while(virus_list->nextVirus != NULL)
    virus_list = virus_list->nextVirus;
  virus_list->nextVirus = create_link(data);
  return saveFirst;
}

link* create_link(virus* data){
  link* l = (link*) malloc(sizeof(link));
  l->nextVirus = NULL;
  l->vir = data;
  return l;
}

void list_free(link *virus_list){
  link* temp;
  while(virus_list != NULL){
    destructVirus(virus_list->vir);
    temp = virus_list->nextVirus;
    free(virus_list);
    virus_list = temp;
  }
}

const int MAX_FILE_NAME_SIZE = 256;

link* load_signatures(link *virus_list, FILE* ouput){
  char s[MAX_FILE_NAME_SIZE];
  printf("Enter signatures file name: ");
  scanf("%s",s);
  FILE* inputFile = fopen(s,"r");
  virus* v;
  while((v = read_virus(inputFile)) != NULL)
    virus_list = list_append(virus_list,v);
  return virus_list;
}

link* quit(link* virus_list,FILE* output){
  list_free(virus_list);
  exit(0);
}