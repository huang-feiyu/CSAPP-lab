/******************************************************************************
 * enforce.c: modify the in-memory copy of the ELF file to change the dest file
 * NAME:    Huang
 * DATE:    08/02/2022
 * WEBSITE: huangblog.com
 ******************************************************************************/

#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "decode.h"

/*  Given the in-memory ELF header pointer as `ehdr` and a section
    header pointer as `shdr`, returns a pointer to the memory that
    contains the in-memory content of the section */
#define AT_SEC(ehdr, shdr) ((void *)(ehdr) + (shdr)->sh_offset)

#define MODE_CLODED 0
#define MODE_OPEN 1

void enforce_func(instruction_t, code_t *, Elf64_Addr, Elf64_Ehdr *, Elf64_Sym *, int);
int is_protected(char *);
Elf64_Shdr *section_by_name(Elf64_Ehdr *, const char *);
Elf64_Shdr *section_by_index(Elf64_Ehdr *, int);

/*************************************************************/

/**
 * Enforce the ELF file to change the dest file
 */
void enforce(Elf64_Ehdr *ehdr) {
    /*  Your work starts here --- but add helper functions that you call
        from here, instead of trying to put all the work in one
        function. */
    int i;
    Elf64_Sym **local_funcs = malloc(100 * sizeof(Elf64_Sym *));

    // print all section names
    printf("\n=====Print all section names=====\n");

    Elf64_Shdr *shdrs = (void *)ehdr + ehdr->e_shoff;             // get section infos
    char *section_strs = AT_SEC(ehdr, shdrs + ehdr->e_shstrndx);  // get section strings

    for (i = 0; i < ehdr->e_shnum; i++) {
        printf("%s\n", section_strs + shdrs[i].sh_name);
    }

    // print all local function names
    printf("\n=====Print all local function names=====\n");

    Elf64_Shdr *dynsym_shdr = section_by_name(ehdr, ".dynsym");
    Elf64_Sym *syms = AT_SEC(ehdr, dynsym_shdr);
    char *syms_strs = AT_SEC(ehdr, section_by_name(ehdr, ".dynstr"));
    int syms_len = dynsym_shdr->sh_size / sizeof(Elf64_Sym);
    int local_funcs_idx = 0;

    for (i = 0; i < syms_len; i++) {
        /* st_size = 0 => function comes from a different file
         * st_size > 0 => function size is determined
         */
        if (ELF64_ST_TYPE(syms[i].st_info) == STT_FUNC && syms[i].st_size > 0) {
            local_funcs[local_funcs_idx++] = &syms[i];
            printf("size: %ld\tname: %s\n", syms[i].st_size, syms_strs + syms[i].st_name);
        }
    }

    // print all global variable names
    printf("\n=====Print all global variable names=====\n");

    Elf64_Shdr *rela_dyn_shdr = section_by_name(ehdr, ".rela.dyn");
    Elf64_Rela *dyn_relas = AT_SEC(ehdr, rela_dyn_shdr);
    int dyn_relas_len = rela_dyn_shdr->sh_size / sizeof(Elf64_Rela);

    for (i = 0; i < dyn_relas_len; i++) {
        printf("offset: %lx\tname: %s\n", dyn_relas[i].r_offset, syms_strs + syms[ELF64_R_SYM(dyn_relas[i].r_info)].st_name);
    }

    // print all global function names
    printf("\n=====Print all global function names=====\n");

    Elf64_Shdr *rela_plt_shdr = section_by_name(ehdr, ".rela.plt");
    Elf64_Rela *plt_relas = AT_SEC(ehdr, rela_plt_shdr);
    int plt_relas_len = rela_plt_shdr->sh_size / sizeof(Elf64_Rela);

    for (i = 0; i < plt_relas_len; i++) {
        printf("offset: %lx\tname: %s\n", plt_relas[i].r_offset, syms_strs + syms[ELF64_R_SYM(plt_relas[i].r_info)].st_name);
    }

    //----- print info end -----
    printf("\n\n");

    // parse local functions to replace *crash* with `crash()`
    for (i = 0; i < local_funcs_idx; i++) {
        printf("\n=====Parse local function: %s=====\n", syms_strs + local_funcs[i]->st_name);

        int j = local_funcs[i]->st_shndx;  // section index
        instruction_t ins = {0, 0, 0};     // op, length, addr
        code_t *code_ptr = AT_SEC(ehdr, shdrs + j) + (local_funcs[i]->st_value - shdrs[j].sh_addr);
        Elf64_Addr code_addr = local_funcs[i]->st_value;

        enforce_func(ins, code_ptr, code_addr, ehdr, local_funcs[i], MODE_CLODED);
    }

    printf("\n\n");
}

/**
 * Enforce each crash to `crash()`
 */
void enforce_func(instruction_t ins, code_t *code_ptr, Elf64_Addr code_addr, Elf64_Ehdr *ehdr,
                  Elf64_Sym *func, int mode) {
    char *syms_strs = AT_SEC(ehdr, section_by_name(ehdr, ".dynstr"));
    Elf64_Shdr *rela_dyn_shdr = section_by_name(ehdr, ".rela.dyn");
    Elf64_Rela *dyn_relas = AT_SEC(ehdr, rela_dyn_shdr);
    Elf64_Shdr *dynsym_shdr = section_by_name(ehdr, ".dynsym");
    Elf64_Sym *syms = AT_SEC(ehdr, dynsym_shdr);
    Elf64_Shdr *rela_plt_shdr = section_by_name(ehdr, ".rela.plt");
    Elf64_Rela *plt_relas = AT_SEC(ehdr, rela_plt_shdr);
    int k, count, offset;

    int i = 0;  // bytes
    for (i = 0; i < func->st_size; i += ins.length) {
        // get the instruction
        decode(&ins, code_ptr, code_addr);

        if (ins.op == CALL_OP) {
            instruction_t called_ins = {0, 0, 0};
            Elf64_Addr called_addr = ins.addr;
            offset = called_addr - code_addr;
            code_t *called_ptr = code_ptr + offset;

            decode(&called_ins, called_ptr, called_addr);

            count = rela_plt_shdr->sh_size / sizeof(Elf64_Rela);
            for (k = 0; k < count; k++) {
                if (plt_relas[k].r_offset == called_ins.addr) {
                    // print outer function name
                    printf("%s\n", syms_strs + syms[ELF64_R_SYM(plt_relas[k].r_info)].st_name);

                    // closed: 0; open: 1; crash: 2;
                    if (strcmp(syms_strs + syms[ELF64_R_SYM(plt_relas[k].r_info)].st_name, "open_it") == 0)
                        mode++;
                    else if (strcmp(syms_strs + syms[ELF64_R_SYM(plt_relas[k].r_info)].st_name, "close_it") == 0)
                        mode--;

                    if (!(mode == MODE_CLODED || mode == MODE_OPEN))
                        replace_with_crash(code_ptr, &ins);
                }
            }

            code_ptr += ins.length;
            code_addr += ins.length;

        } else if (ins.op == RET_OP) {
            if (mode != MODE_CLODED)
                replace_with_crash(code_ptr, &ins);
            return;

        } else if (ins.op == OTHER_OP) {
            code_ptr += ins.length;
            code_addr += ins.length;
        }
    }
}

/**
 * Judge if the variable is protected
 */
int is_protected(char *v) {
    int len = strlen(v);
    char *sub = malloc(11 * sizeof(char));

    if (len < 10)
        return 0;
    else {
        for (int i = 0; i < 10; i++)
            sub[i] = v[i];
        if (strcmp(sub, "protected_") == 0)
            return 1;
    }
    return 0;
}

/**
 * Find the section header by name
 */
Elf64_Shdr *section_by_name(Elf64_Ehdr *ehdr, const char *name) {
    Elf64_Shdr *shdrs = (void *)ehdr + ehdr->e_shoff;     // get section infos
    char *strs = AT_SEC(ehdr, shdrs + ehdr->e_shstrndx);  // get string table

    // find the section header by name
    int i;
    for (i = 0; i < ehdr->e_shnum; i++) {
        if (strcmp(strs + shdrs[i].sh_name, name) == 0) {
            return &shdrs[i];
        }
    }
    return NULL;
}

/**
 * Find the section head by index
 */
Elf64_Shdr *section_by_index(Elf64_Ehdr *ehdr, int index) {
    Elf64_Shdr *shdrs = (void *)ehdr + ehdr->e_shoff;
    return shdrs + index;
}

/*************************************************************/

static void fail(char *reason, int err_code) {
    fprintf(stderr, "%s (%d)\n", reason, err_code);
    exit(1);
}

static void check_for_shared_object(Elf64_Ehdr *ehdr) {
    if ((ehdr->e_ident[EI_MAG0] != ELFMAG0) || (ehdr->e_ident[EI_MAG1] != ELFMAG1) || (ehdr->e_ident[EI_MAG2] != ELFMAG2) || (ehdr->e_ident[EI_MAG3] != ELFMAG3))
        fail("not an ELF file", 0);

    if (ehdr->e_ident[EI_CLASS] != ELFCLASS64)
        fail("not a 64-bit ELF file", 0);

    if (ehdr->e_type != ET_DYN)
        fail("not a shared-object file", 0);
}

int main(int argc, char **argv) {
    int fd_in, fd_out;
    size_t len;
    void *p_in, *p_out;
    Elf64_Ehdr *ehdr;

    if (argc != 3)
        fail("expected two file names on the command line", 0);

    /* Open the shared-library input file */
    fd_in = open(argv[1], O_RDONLY);
    if (fd_in == -1)
        fail("could not open input file", errno);

    /* Find out how big the input file is: */
    len = lseek(fd_in, 0, SEEK_END);

    /* Map the input file into memory: */
    p_in = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd_in, 0);
    if (p_in == (void *)-1)
        fail("mmap input failed", errno);

    /* Since the ELF file starts with an ELF header, the in-memory image
       can be cast to a `Elf64_Ehdr *` to inspect it: */
    ehdr = (Elf64_Ehdr *)p_in;

    /* Check that we have the right kind of file: */
    check_for_shared_object(ehdr);

    /* Open the shared-library output file and set its size: */
    fd_out = open(argv[2], O_RDWR | O_CREAT, 0777);
    if (fd_out == -1)
        fail("could not open output file", errno);
    if (ftruncate(fd_out, len))
        fail("could not set output file file", errno);

    /* Map the output file into memory: */
    p_out = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd_out, 0);
    if (p_out == (void *)-1)
        fail("mmap output failed", errno);

    /* Copy input file to output file: */
    memcpy(p_out, p_in, len);

    /* Close input */
    if (munmap(p_in, len))
        fail("munmap input failed", errno);
    if (close(fd_in))
        fail("close input failed", errno);

    ehdr = (Elf64_Ehdr *)p_out;

    enforce(ehdr);

    if (munmap(p_out, len))
        fail("munmap input failed", errno);
    if (close(fd_out))
        fail("close input failed", errno);

    return 0;
}
