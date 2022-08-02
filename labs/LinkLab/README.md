# Linking Lab

> About CS:APP Chapter 7.

[utah cs4400: Linking Assignment](https://my.eng.utah.edu/~cs4400/linklab.html)

<img src="https://user-images.githubusercontent.com/70138429/182342361-89ee79a0-d4a6-4e1b-a6c3-5c4d3e3b3787.jpg" width="500px">

## Prepare

> Before starting the lab, I found that I have a poor understanding of Link.

<strong>*</strong> Theory understanding:

[Slide](https://my.eng.utah.edu/~cs4400/elf.pdf)

[Video](https://www.youtube.com/watch?v=6cq9T2zvuz4)

In runtime view, ELF file is a set of segments;<br/>
In linking view, ELF file is a set of sections.

* [ELF](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format):
  Executable and Linkable Format
    * Object File: `.o` (sections only)
    * Executable File: `.out` (segments only)
    * Shared Object File: `.so`
* Segments: have **types** assigned by program headers (meaning of each type
  is defined by OS)
* Sections: have **names** assigned by section headers
    * `.dynsym`: array of `Elf64_Sym` (`Elf64_Sym` is a symbol table)
    * `.dynstr`: array of strings (use `.dynsym` to get the index)
    * `.rela.dyn` or `.rela.plt`: array of `Elf64_Rela` (`Elf64_Rela` is a relocation record)
    * `.plt`: procedure linkage table

<strong>*</strong> ELF helpers:

```c
/* Functions that you should be able to write: */
Elf64_Shdr *section_by_index(Elf64_Ehdr *ehdr, int idx);
Elf64_Shdr *section_by_name(Elf64_Ehdr *ehdr, char *name);
/* Helper to get pointer to section content: */
#define AT_SEC(ehdr, shdr) ((void *)(ehdr) + (shdr)->sh_offset)
```

<strong>*</strong> `decode` API:

```c
typedef struct {
    instruction_op_t op; /* operation performed by the instruction */
    int length;          /* length of the instruction in bytes */
    Elf64_Addr addr;     /* set only for some kinds of instructions */
} instruction_t;

/* Fill `*ins` with information about the instruction at `code_ptr`,
   where `code_addr` is the address where the code will reside at run
   time. */
void decode(instruction_t *ins, code_t *code_ptr, Elf64_Addr code_addr);

/* Changes the instruction at `code_ptr` so that it's a crash
   (followed by filler no-ops). The `ins` argument determines how many
   bytes are replaced. */
void replace_with_crash(code_t *code_ptr, instruction_t *ins);
```

<strong>*</strong> `mmap()`: map files or devices into memory

shared libraries are loaded into memory by the kernel.

```c
#include <sys/mman.h>
/* Create a new mapping in the virtual address space of the calling process. The
   starting address for the new mapping is specified in addr. The length argument
   specifies the length of the mapping (which must be greater than 0). */
void *mmap(void *addr, size_t lenth, int prot, int flags, int fd, off_t offset);
```

<strong>*</strong> Tips:

All of the information that you need from the ELF file can be found via
**section headers** and **section content**, so you will not need to use program
headers. You’ll need to consult the `.dynsym`, `.dynstr`, `.rela.dyn`, `.plt`,
and `.rela.plt` sections, at least.

## Implementation

<img src="https://user-images.githubusercontent.com/70138429/182309175-ad154769-ca0d-4bfc-8c64-5a136d494fa0.png" width="500px"/>

What we need to do is to fill in the implementation of enforce so that it
changes the destination file. `enforce` function will modify the in-memory
copy of the ELF file. Through the magic of mmap, those in-memory changes
will be reflected in the destination file.
