# ArchLab

> This gives the students a deep appreciation for the interactions between hardware and software.

Before starting this lab, we need to do some setup. I am in the Fedora36:

```bash
# 1st step. To resolve the issue with "cannot find -lfl"
sudo dnf install flex bison flex-static
```

```makefile
# 2nd step. To resolve the issue with "cannot find -ltcl -ltk"
# Comment out the folloing lines if you donot have tcl/tk(below 8.5) installed.
# In file "./sim/Makefile"
GUIMODE=-DHAS_GUI
TKLIBS=-L/usr/lib -ltk -ltcl
TKINC=-isystem /usr/include/tcl8.5
```

```diff
# 3rd step. To resolve the issue with "sim/misc/yas.h:13: multiple definition of `lineno'"
# add "-fcommon" in all Makefiles's "?CFLAGS". such as:
< CFLAGS=-Wall -O1 -g
< LCFLAGS=-O1
---
> CFLAGS=-Wall -O1 -g -fcommon
> LCFLAGS=-O1 -fcommon
```

## Part A

Work in [misc](./sim/misc/), there are y86 assembler `yas` and y86 instruction simulator `yis`.
What we need to do is to translate [example.c](./sim/misc/examples.c)'s three functions
to y86 assembly code.

Look up to csapp figure 4.7, it is easy.

## Part B

Work in [seq](./sim/seq), it is a Sequential Y86-64 Simulator.
What we need to do is to add a new instruction `iaddr`.

Loop up to csapp figure 4.18 and homework 4.51 & 4.52.

* `valC`: constant V
* `valA`: register value of `R[rB]` (rA==rB)
* `valP`: next PC

| stage | iaddq V, rB                                                             |
| ----- | ----------------------------------------------------------------------- |
| IF    | icode: ifun⟵M1[PC]<br/>rA: rB⟵M1[PC+1]<br/>valC⟵M8[PC+2]<br/>valP⟵PC+10 |
| ID    | valA⟵R[rA]                                                              |
| EXE   | valE⟵valC+valA                                                          |
| MEM   |                                                                         |
| WB    | R[rB]⟵valE                                                              |
| NPC   | PC⟵valP                                                                 |

Add `IIADDR` to the block according the table above, it's done.
