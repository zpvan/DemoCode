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

static char *ptypes[] = {
    "PT_NULL",
    "PT_LOAD",
    "PT_DYNAMIC",
    "PT_INTERP",
    "PT_NOTE",
    "PT_SHLIB",
    "PT_PHDR"
};

static char *stypes[] = {
    "SHT_NULL",
    "SHT_PROGBITS",
    "SHT_SYMTAB",
    "SHT_STRTAB",
    "SHT_RELA",
    "SHT_HASH",
    "SHT_DYNAMIC",
    "SHT_NOTE",
    "SHT_NOBITS",
    "SHT_REL",
    "SHT_SHLIB",
    "SHT_DYNSYM"
};

static char *btypes[] = {
    "STB_LOCAL",
    "STB_GLOBAL",
    "STB_WEAK"
};

static char *symtypes[] = {
    "STT_NOTYPE",
    "STT_OBJECT",
    "STT_FUNC",
    "STT_SECTION",
    "STT_FILE"
};

int elf_is_valid(Elf32_Ehdr *elf_hdr);
int print_elf_header(Elf32_Ehdr *elf_hdr);
char *get_elf_type(Elf32_Ehdr *elf_hdr);
int print_program_header(Elf32_Phdr *phdr, uint index);
int print_section_header(Elf32_Shdr *shdr, uint index, char *strtable);
int print_sym_table(u_char *filebase, Elf32_Shdr *section, char *strtable);
void print_bind_type(u_char info);
void print_sym_type(u_char info);

int main(const int argc, const char **argv) {
    int fd_elf = -1;
    struct stat elf_stat;
    u_char *p_base = NULL;
    Elf32_Ehdr *p_ehdr = NULL;
    Elf32_Phdr *p_phdr = NULL;
    Elf32_Shdr *p_shdr = NULL;
    char *p_strtable = NULL;
    int i;

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

        printf("\n");

        // to reach the section header table and the program header table
        // we simply add the offset of these table to the base address
        p_phdr = (Elf32_Phdr *)(p_base + p_ehdr->e_phoff);
        p_shdr = (Elf32_Shdr *)(p_base + p_ehdr->e_shoff);

        // this is the first example of string table usage: 
        // the e_shstrndx field holds an index into the section header table, 
        // which is address by p_shdr.
        // The section's sh_offset field will hold the offset of the string table,
        // to get the actual pointer we have just to sum it to the base address.
        p_strtable = (char *)(p_base + p_shdr[p_ehdr->e_shstrndx].sh_offset);

        for (i = 0; i < p_ehdr->e_phnum; i++) {
            print_program_header(&p_phdr[i], i);
        }

        for (i = 0; i < p_ehdr->e_shnum; i++) {
            print_section_header(&p_shdr[i], i, p_strtable);
            if (p_shdr[i].sh_type == SHT_SYMTAB || p_shdr[i].sh_type == SHT_DYNSYM) {
                printf("This section holds a symbol table...\n");

                // being a symbol table, the field sh_link of the section header
                // will hold an index into the section table which gives the
                // section containing the string table
                print_sym_table(p_base, &p_shdr[i], (char *)(p_base + p_shdr[p_shdr[i].sh_link].sh_offset));
            }
        }

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

int print_program_header(Elf32_Phdr *phdr, uint index) {
    if (!phdr) {
        return 0;
    }

    printf("Program header %d\n", index);
    if (phdr->p_type <= 6) {
        printf("- Type: %s\n", ptypes[phdr->p_type]);
    } else {
        printf("- Type: %08x\n", phdr->p_type);
    }

    printf("- Offset: %08x\n", phdr->p_offset);
    printf("- Virtual Address: %08x\n", phdr->p_vaddr);
    printf("- Physical Address: %08x\n", phdr->p_paddr);
    printf("- File size: %d\n", phdr->p_filesz);
    printf("- Memory size: %d\n", phdr->p_memsz);
    printf("- Flags: %08x\n", phdr->p_flags);
    printf("- Aligments: %08x\n", phdr->p_align);
    return 1;
}

int print_section_header(Elf32_Shdr *shdr, uint index, char *strtable) {
    if (!shdr) {
        return 0;
    }

    printf("Section header: %d\n", index);
    printf("- Name index: %d\n", shdr->sh_name);

    // as you can see, we're using sh_name as an index into the string table
    if (shdr->sh_type <= 11) {
        printf("- Type: %s\n", stypes[shdr->sh_type]);
    } else {
        printf("- Type: %04x\n", shdr->sh_type);
    }

    printf("- Flags: %08x\n", shdr->sh_flags);
    printf("- Address: %08x\n", shdr->sh_addr);
    printf("- Offset: %08x\n", shdr->sh_offset);
    printf("- Size: %08x\n", shdr->sh_size);
    printf("- Link: %08x\n", shdr->sh_link);
    printf("- Info: %08x\n", shdr->sh_info);
    printf("- Address alignment: %08x\n", shdr->sh_addralign);
    printf("- Entry size: %08x\n", shdr->sh_entsize);
    return 1;
}

int print_sym_table(u_char *filebase, Elf32_Shdr *section, char *strtable) {
    Elf32_Sym *symbols;
    size_t sym_size = section->sh_entsize;
    size_t cur_size = 0;

    if (section->sh_type == SHT_SYMTAB) {
        printf("Symbol table\n");
    } else {
        printf("Dynamic symbol table\n");
    }

    if (sym_size != sizeof(Elf32_Sym)) {
        printf("There's something evil with symbol table...\n");
        return 0;
    }

    symbols = (Elf32_Sym *)(filebase + section->sh_offset);
    symbols++;
    cur_size = sym_size;

    do {
        printf("- Name index: %d\n", symbols->st_name);
        printf("- Name: %s\n", strtable + symbols->st_name);
        printf("- Value: 0x%08x\n", symbols->st_value);
        printf("- Size: 0x%08x\n", symbols->st_size);

        print_bind_type(symbols->st_info);
        print_sym_type(symbols->st_info);

        printf("- Section index: %d\n", symbols->st_shndx);
        cur_size += sym_size;
        symbols++;
    } while (cur_size < section->sh_size);

    return 1;
}

void print_bind_type(u_char info) {
    u_char bind = ELF32_ST_BIND(info);
    if (bind <= 2) {
        printf("- Bind type: %s\n", btypes[bind]);
    } else {
        printf("- Bind type: %d\n", bind);
    }
}

void print_sym_type(u_char info) {
    u_char type = ELF32_ST_TYPE(info);

    if (type <= 4) {
        printf("- Symbol type: %s\n", symtypes[type]);
    } else {
        printf("- Symbol type: %d\n", type);
    }
}