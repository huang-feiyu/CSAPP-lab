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

/* Given the in-memory ELF header pointer as `ehdr` and a section
   header pointer as `shdr`, returns a pointer to the memory that
   contains the in-memory content of the section */
#define AT_SEC(ehdr, shdr) ((void *)(ehdr) + (shdr)->sh_offset)

/*************************************************************/

void enforce(Elf64_Ehdr *ehdr) {
    /* Your work starts here --- but add helper functions that you call
       from here, instead of trying to put all the work in one
       function. */
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
