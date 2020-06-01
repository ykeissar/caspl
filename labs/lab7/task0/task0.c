#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

struct fun_desc {
  char *name;
  void (*fun)(state*);
};
void toggleDebugMode(state* s);
void setFileName(state* s);
void setUnitSize(state* s);
void quit(state* s);

state* initState(void);

struct fun_desc menu[] ={
    {"Toggle Debug Mode",toggleDebugMode},
    {"Set File Name",setFileName},
    {"Set Unit Size",setUnitSize},
    {"Quit",quit},
    {NULL,NULL}
};

int main(int argc,char** argv){
    int i,choosen;
    unsigned int menuSize = sizeof menu / sizeof menu[0]-1;
    state* s = initState();
    while(1){
        if(s->debug_mode)
            fprintf(stderr,"Unit size:%d\nFile name:%s\nMem count:%d\n",s->unit_size,s->file_name,s->mem_count);
        //print menu
        for(i = 0 ; i<menuSize;i++){
            printf("%d-%s\n",i,menu[i].name);
        }
        scanf("%d",&choosen);
        if(choosen >= 0 && choosen <menuSize)
            menu[choosen].fun(s);
        else
            fprintf(stdout,"Error: %d is an illegal choice\n",choosen);
        

    }
    free(s);
    return 0;
}

void toggleDebugMode(state* s){
    if(s->debug_mode){
        s->debug_mode = 0;
        printf("Debug flag now off\n");
    }
    else{
        s->debug_mode = 1;
        printf("Debug flag now on\n");
    }
}

void setFileName(state* s){
    printf("Enter new file name: ");
    scanf("%s",s->file_name);
    if(s->debug_mode)
        fprintf(stderr,"Debug: file name set to '%s'\n",s->file_name);
    
}

void setUnitSize(state* s){
    int newUnit;
    printf("Enter new unit size: ");
    scanf("%d",&newUnit);
    if(newUnit == 1 || newUnit == 2 || newUnit == 4){
        s->unit_size = newUnit;
        if(s->debug_mode)
            fprintf(stderr,"Debug: set size to %d\n",s->unit_size);
   
    }
    else{
        fprintf(stderr,"Error: %d is illegal, use 1, 2 or 4 only\n",newUnit);
    }
}

void quit(state* s){
    if(s->debug_mode)
        fprintf(stderr,"quitting\n");
    exit(0);
}

state* initState(void){
    state* s  = (state*)malloc(sizeof(state));
    strncpy(s->file_name,"",sizeof(s->file_name));
    s->debug_mode = 0;
    s->unit_size = 1;
    s->mem_count = 0;
    return s;
}
