#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

// ELF utils funcs
Elf32_Phdr* get_phdr(Elf32_Ehdr* hdr){
    return (Elf32_Phdr*)((int)hdr + hdr->e_phoff);
}

Elf32_Phdr* get_phdr_idx(Elf32_Ehdr* hdr,int idx){
    return &get_phdr(hdr)[idx];
}

int main (int argc,char** argv){
    int fd,i;
    struct stat fd_stat;
    void* map_start;
    return 0;
    if((fd = open(argv[1],O_RDWR)) < 0){
        perror("error in open");
        return 1;
    }

    if(fstat(fd, &fd_stat) != 0){
        perror("stat failed");
        close(fd);
        fd = -1;
        return 1;
    }

    if((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED){
        perror("mmap failed");
        close(fd);
        fd = -1;
        return 1;
    }

    Elf32_Ehdr* hdr = (Elf32_Ehdr*) map_start;
    unsigned int phNum = hdr->e_phnum;
    Elf32_Phdr* pHdr;
    printf("Program Headers:\n");
    printf("Type          Offset      VirtAddr    PhysAddr    FileSiz  MemSiz   Flg   Align\n");
 
    for(i = 0 ; i < phNum ; i++){
        pHdr = get_phdr_idx(hdr,i);
        printf("0x%-10d  0x%08x  0x%08x  0x%08x  0x%-5x  0x%-5x  0x%-2x  0x%-4x\n",pHdr->p_type,pHdr->p_offset,pHdr->p_vaddr,pHdr->p_paddr,pHdr->p_filesz,pHdr->p_memsz,pHdr->p_flags,pHdr->p_align);
    }
    printf("\n");                       
    return 0;
}