#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>



typedef struct {
    char debug_mode; // 0 - off, 1 - on
    int fd;
    void* map_start;
    struct stat fd_stat;    
} state;

struct fun_desc {
    char *name;
    void (*fun)(state*);
};

void toggleDebugMode(state* s);
void examineELFFile(state* s);
void printSectionNames(state* s);
void quit(state* s);

struct fun_desc menu[] ={
    {"Toggle Debug Mode",toggleDebugMode},
    {"Examine ELF File",examineELFFile},
    {"Print Section Names",printSectionNames},
    {"Quit",quit},
    {NULL,NULL}
};

state* initState(void);

// ELF utils funcs
Elf32_Shdr* get_shdr(Elf32_Ehdr* hdr){
    return (Elf32_Shdr*)((int)hdr + hdr->e_shoff);
}

Elf32_Shdr* get_shdr_idx(Elf32_Ehdr* hdr,int idx){
    return &get_shdr(hdr)[idx];
}

char* str_table(Elf32_Ehdr* hdr){
    if(hdr->e_shstrndx == SHN_UNDEF)
        return NULL;
    return (char*)hdr+get_shdr_idx(hdr,hdr->e_shstrndx)->sh_offset;
}

char* get_shdr_name(Elf32_Ehdr* hdr, Elf32_Shdr* secHdr){
    char* strtab = str_table(hdr);
    if(!strtab)
        return NULL;
    return strtab + (secHdr->sh_name);
}

int main(int argc,char** argv){
    int i,choosen;
    unsigned int menuSize = sizeof menu / sizeof menu[0]-1;
    state* s = initState();
    while(1){
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

state* initState(void){
    state* s = (state*)malloc(sizeof(state));
    s->debug_mode = 0;
    s->fd = -1;
    s->map_start = NULL;
    return s;
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

void examineELFFile(state* s){
    char file_name[1000];
    printf("Enter new ELF file name: ");
    scanf("%s",file_name);
    
    //closing old fd
    if(s->fd > -1)
        close(s->fd);

    if(s->map_start)
        munmap(s->map_start,s->fd_stat.st_size);

    if((s->fd = open(file_name,O_RDWR)) < 0){
        perror("error in open");
        return;
    }

    if(fstat(s->fd, &s->fd_stat) != 0){
        perror("stat failed");
        close(s->fd);
        s->fd = -1;
        return;
    }

    if((s->map_start = mmap(0, s->fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, s->fd, 0)) == MAP_FAILED){
        perror("mmap failed");
        close(s->fd);
        s->fd = -1;
        return;
    }

    Elf32_Ehdr* header = (Elf32_Ehdr*) s->map_start;

    if(strncmp((char*)header->e_ident+1,"ELF",3) != 0){
        fprintf(stderr,"Error: File is not ELF file!\n");
        close(s->fd);
        s->fd = -1;
        munmap(s->map_start,s->fd_stat.st_size);
        s->map_start = NULL;
        return;
    }

    printf("%c %c %c\n",header->e_ident[1],header->e_ident[2],header->e_ident[3]);
    switch(header->e_ident[EI_DATA]){
        case 1:
            printf("Data encoded scheme of object file: 2's complement, little endian\n");
            break;
        case 2:
            printf("Data encoded scheme of object file: 2's complement, big endian\n");
            break;
    }
    printf("Entry point: %x\n",header->e_entry);
    printf("Section header table offset: %d\n",header->e_shoff);
    printf("Number of section header entries: %d\n",header->e_shnum);
    printf("Size of each section header entry: %d\n",header->e_shentsize);
    printf("Program header table offset: %d\n",header->e_phoff);
    printf("Number of Program header enteries: %d\n",header->e_phnum);
    printf("Size of each section header entry: %d\n",header->e_phentsize);
    printf("\n");
}

void quit(state* s){
    if(s->fd > -1)
        close(s->fd);
    if(s->map_start)
        munmap(s->map_start,s->fd_stat.st_size);
    exit(0);
}

void printSectionNames(state* s){
    int i;
    if(!s->map_start){
        fprintf(stderr,"Error: file is not defined.\n");
        return;
    }

    Elf32_Ehdr* hdr = (Elf32_Ehdr*) s->map_start;
    unsigned int scNum = hdr->e_shnum;
    Elf32_Shdr* secHdr;
    printf("Section Headers:\n");
    printf("[Nr] Name                 Addr      Off     Size    Type");
    if(s->debug_mode)
        printf("      Name offset");
    printf("\n");

    for(i = 0 ; i < scNum ; i++){
        secHdr = get_shdr_idx(hdr,i);
        printf("[%2d] %-20s %08x  %06x  %06x  %-8x",i,get_shdr_name(hdr,secHdr),secHdr->sh_addr,            secHdr->sh_offset,secHdr->sh_size,secHdr->sh_type);
        if(s->debug_mode)
            printf("  %06x",secHdr->sh_name);
        printf("\n");
    }
    printf("\n");
}