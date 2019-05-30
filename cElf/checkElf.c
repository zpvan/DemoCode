#include <stdio.h>
// open; O_RDONLY
#include <fcntl.h>
// errno
#include <errno.h>
// strerror
#include <string.h>
// struct stat
#include <sys/stat.h>
// calloc
#include <stdlib.h>
// Elf32_Ehdr
#include <elf.h>

static char *elf_types[] = {
    "ET_NONE",
    "ET_REL",
    "ET_EXEC",
    "ET_DYN",
    "ET_CORE",
    "ET_NUM"
};

int elf_is_valid(Elf32_Ehdr *elf_hdr);
int print_elf_header(Elf32_Ehdr *elf_hdr);
char *get_elf_type(Elf32_Ehdr *elf_hdr);

int main(const int argc, const char **argv) {
    int fd_elf = -1;
    struct stat elf_stat;
    u_char *p_base = NULL;
    Elf32_Ehdr *p_ehdr = NULL;

    if (argc < 2) {
        printf("Usage: %s \n", argv[0]);
        return 1;
    }

    fd_elf = open(argv[1], O_RDONLY);
    if (fd_elf == -1) {
        fprintf(stderr, "Couldn't open %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    if (fstat(fd_elf, &elf_stat) == -1) {
        fprintf(stderr, "Couldn't stat %s: %s\n", argv[1], strerror(errno));
        close(fd_elf);
        return 1;
    }

    p_base = (u_char *)calloc(sizeof(u_char), elf_stat.st_size);
    if (!p_base) {
        fprintf(stderr, "Not enough memory\n");
        close(fd_elf);
        return 1;
    }

    if (read(fd_elf, p_base, elf_stat.st_size) != elf_stat.st_size) {
        fprintf(stderr, "Error while reading file: %s\n", strerror(errno));
        free(p_base);
        close(fd_elf);
        return 1;
    }

    close(fd_elf);

    p_ehdr = (Elf32_Ehdr *)p_base;
    if (elf_is_valid(p_ehdr)) {
        print_elf_header(p_ehdr);
    } else {
        fprintf(stderr, "Invalid ELF file\n");
    }

    free(p_base);

    return 0;
}

int elf_is_valid(Elf32_Ehdr *elf_hdr) {
    if ( (elf_hdr->e_ident[EI_MAG0] != 0x7F) ||
         (elf_hdr->e_ident[EI_MAG1] != 'E') ||
         (elf_hdr->e_ident[EI_MAG2] != 'L') ||
         (elf_hdr->e_ident[EI_MAG3] != 'F') ) {
        return 0;
    }

    if (elf_hdr->e_ident[EI_CLASS] != ELFCLASS32) {
        return 0;
    }

    if (elf_hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
        return 0;
    }
    return 1;
}

int print_elf_header(Elf32_Ehdr *elf_hdr) {
    char *sz_elf_type = NULL;

    if (!elf_hdr) {
        return 0;
    }

    printf("ELF header information\n");

    sz_elf_type = get_elf_type(elf_hdr);
    if (sz_elf_type) {
        printf("- Type: %s\n", sz_elf_type);
    } else {
        printf("- Type: %04x\n", elf_hdr->e_type);
    }

    printf("- Version: %d\n", elf_hdr->e_version);
    printf("- Entrypoint: 0x%08x\n", elf_hdr->e_entry);
    printf("- Program header table offset: 0x%08x\n", elf_hdr->e_phoff);
    printf("- Section header table offset: 0x%08x\n", elf_hdr->e_shoff);
    printf("- Flags: 0x%08x\n", elf_hdr->e_flags);
    printf("- ELF header size: %d\n", elf_hdr->e_ehsize);
    printf("- Program header size: %d\n", elf_hdr->e_phentsize);
    printf("- Program header entries: %d\n", elf_hdr->e_phnum);
    printf("- Section header size: %d\n", elf_hdr->e_shentsize);
    printf("- Section header entries: %d\n", elf_hdr->e_shnum);
    printf("- Section string table index: %d\n", elf_hdr->e_shstrndx);

    return 1;
}

char *get_elf_type(Elf32_Ehdr *elf_hdr) {
    if (elf_hdr->e_type > 5) {
        return NULL;
    }

    return elf_types[elf_hdr->e_type];
}