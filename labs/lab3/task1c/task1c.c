#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};

const int MAX_FILE_NAME_SIZE = 256;
const size_t BUFFER_SIZE = 10000;

void list_print(link *virus_list, FILE* output); 
     /* Print the data of every link in list to the given stream. Each item followed by a newline character. */
 
link* list_append(link* virus_list, virus* data); 
     /* Add a new link with the given data to the list 
        (either at the end or the beginning, depending on what your TA tells you),
        and return a pointer to the list (i.e., the first link in the list).
        If the list is null - create a new entry and return a pointer to the entry. */
 
void list_free(link *virus_list); /* Free the memory allocated by the list. */

virus* read_virus(FILE* f);
void print_virus(virus* virus, FILE* output);
int print_hex(unsigned char* buffer, int length,FILE* output);
void destruct_virus(virus* vir);
link* load_signatures(link* virus_list);
void quit(link* virus_list);
link* create_link(virus* data);

char* get_buffer(char* fileName, size_t fileSize);
void detect_virus(char *buffer, unsigned int size, link *virus_list);

int main(int argc, char** argv){
    int choosen;
    link* virus_list = NULL;
    char * fileBuffer;
    
    FILE* fileToDetect = fopen(argv[1],"r");
    //get file size
    fseek(fileToDetect, 0, SEEK_END);
    size_t fileSize = ftell(fileToDetect); 
    fseek(fileToDetect, 0, SEEK_SET);
    fclose(fileToDetect);

    while(1){
        printf("1) Load signatures\n2) Print signatures\n3) Detect virus\n4) Quit\nOption: ");
        scanf("%d",&choosen);
        switch (choosen){
        case 1:
            virus_list = load_signatures(virus_list);
            break;
        case 2:
            list_print(virus_list,stdout);
            break;
        case 3:
            fileBuffer = get_buffer(argv[1],fileSize);
            detect_virus(fileBuffer,fileSize,virus_list);
            free(fileBuffer);
            break;
        case 4:
            quit(virus_list);
            break;
        }   
    }
    return 0;
}

int print_hex(unsigned char* buffer, int length,FILE* output){
    int i;
    for(i = 0 ; i < length ; i++)
        fprintf(output,"%02X ",*(buffer+i));
    return 0;
}

virus* read_virus(FILE* f){
    virus* vir = (virus*) malloc(sizeof(virus));

    if(fread(vir,18,1,f) <= 0){
        free(vir);
        return NULL;
    }

    vir->sig = (unsigned char*) malloc(sizeof(unsigned char)*(vir->SigSize));
    if(fread(vir->sig,vir->SigSize,1,f) <= 0){
        free(vir->sig);
        free(vir);
        return NULL;
    }
    return vir;
}

void print_virus(virus* virus, FILE* output){
    fprintf(output,"Virus name: %s\nVirus size: %d\nsignature:\n",virus->virusName,virus->SigSize);
    print_hex(virus->sig,virus->SigSize,output);
    fprintf(output,"\n\n");
}

void destruct_virus(virus* vir){
    free(vir->sig);
    free(vir);
}

void list_print(link *virus_list, FILE* output){
    while(virus_list != NULL){
        print_virus(virus_list->vir,output);
        virus_list = virus_list->nextVirus;
    }
}

link* list_append(link* virus_list, virus* data){
    if(virus_list == NULL)
        return create_link(data);
    link* saveFirst = virus_list;

    while(virus_list->nextVirus != NULL)
        virus_list = virus_list->nextVirus;
     
    virus_list->nextVirus = create_link(data);
    return saveFirst;
}

link* create_link(virus* data){
    link* l = (link*) malloc(sizeof(link));
    l->nextVirus = NULL;
    l->vir= data;
    return l;
}

void list_free(link* virus_list){
    link* temp;
    while(virus_list != NULL){
        destruct_virus(virus_list->vir);
        temp = virus_list->nextVirus;
        free(virus_list);
        virus_list = temp;
  }
}

link* load_signatures(link* virus_list){
    char s[MAX_FILE_NAME_SIZE];
    if(virus_list != NULL)
        list_free(virus_list);
    virus_list = NULL;
    printf("Enter signatures file name: ");
    scanf("%s",s);
    FILE* inputFile = fopen(s,"r");
    virus* v;
    while((v = read_virus(inputFile)) != NULL)
        virus_list = list_append(virus_list,v);
    fclose(inputFile);
    return virus_list;
}

void quit(link* virus_list){
    list_free(virus_list);
    exit(0);
}


char* get_buffer(char* fileName, size_t fileSize){
    FILE* fileToDetect = fopen(fileName,"r");

    if(BUFFER_SIZE < fileSize)
        fileSize = BUFFER_SIZE;
    
    char* buffer = (char*) malloc(fileSize*sizeof(char));
    fread(buffer,fileSize,1,fileToDetect);
    fclose(fileToDetect);
    return buffer;
}

void detect_virus(char *buffer, unsigned int size, link *virus_list){
    int i;
    link* holder;
    for(i = 0 ; i < size; i++){
        holder = virus_list;
        while(virus_list != NULL){
            if(size-i>virus_list->vir->SigSize){
                if(memcmp(buffer+i,virus_list->vir->sig,virus_list->vir->SigSize)==0)
                    printf("Starting byte: %d, Virus name: %s, Signature size: %d\n",i,virus_list->vir->virusName,virus_list->vir->SigSize);
            }
            virus_list = virus_list->nextVirus;    
        }
        virus_list = holder;
    }
}

