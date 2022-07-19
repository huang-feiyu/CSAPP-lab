# Bomb Lab

> This is an x86-64 bomb for self-study students.

## Prepare

```bash
objdump -d bomb > bomb.dump.asm
chmod 755 bomb
```

Read the Dr.Evil's words and skeleton source [file](./bomb.c) to defuse the bomb. It is funny to see a childish comment when defusing the bomb.

## Defuse

### phase_1

```assembly
phase_1:
sub    $0x8,%rsp
mov    $0x402400,%esi
call   401338 <strings_not_equal>
test   %eax,%eax
je     400ef7 <phase_1+0x17>
call   40143a <explode_bomb>
add    $0x8,%rsp
ret
```

Use the first `read_line()`'s output as the input of the second `phase_1()`.
Then, call `strings_not_equal()` to compare the input and reference string.
If they are not equal, call `explode_bomb()` to explode the bomb.

Step into `strings_not_equal()` to see the which registers are the args store in,
and use gdb to find the address of the reference string. Echo the address to the console.

* `strings_not_equal`: `%rdi` vs. `%rsi`

```gdb
# Make a breakpoint in `strings_not_equal()` and print the reference string.
(gdb) b strings_not_equal
(gdb) run
(gdb) info reg
(gdb) x/s 0x402400
```

**Answer 1**: Border relations with Canada have never been better.

### phase_2

```assembly
phase_2
push   %rbp
push   %rbx
sub    $0x28,%rsp
mov    %rsp,%rsi
call   40145c <read_six_numbers>
cmpl   $0x1,(%rsp)
je     400f30 <phase_2+0x34>
call   40143a <explode_bomb>
jmp    400f30 <phase_2+0x34>
mov    -0x4(%rbx),%eax
add    %eax,%eax
cmp    %eax,(%rbx)
je     400f25 <phase_2+0x29>
call   40143a <explode_bomb>
add    $0x4,%rbx
cmp    %rbp,%rbx
jne    400f17 <phase_2+0x1b>
jmp    400f3c <phase_2+0x40>
lea    0x4(%rsp),%rbx
lea    0x18(%rsp),%rbp
jmp    400f17 <phase_2+0x1b>
add    $0x28,%rsp
pop    %rbx
pop    %rbp
ret
```

Read the six numbers from the 2nd line, and store them in `%rsp` as an array.
Write C code, according to the assembly code.

```c
void phase_2() {
    int my_rsp, my_rbx, my_eax, my_rbp;
    if (my_rsp[0] == 1) {
        my_rbx = &my_rsp[1];
        my_rbp = &my_rsp[5];
    } else
        goto explode;

    while (my_eax != *my_rbx) {
        my_eax = *my_rbx + *my_rbx;
        if (my_eax == *my_rbx) {
            ; // nothing to do
        } else {
            goto explode;
        }
        next: my_rbx += 4;
    }

    return;
}

// code...

explode: explode_bomb();
```

**Answer 2**: 1 2 4 8 16 32

### phase_3

```assembly
phase_3:
sub    $0x18,%rsp
lea    0xc(%rsp),%rcx
lea    0x8(%rsp),%rdx
mov    $0x4025cf,%esi
mov    $0x0,%eax
call   400bf0 <__isoc99_sscanf@plt>
cmp    $0x1,%eax
jg     400f6a <phase_3+0x27>
call   40143a <explode_bomb>
cmpl   $0x7,0x8(%rsp)
ja     400fad <phase_3+0x6a>
mov    0x8(%rsp),%eax
jmp    *0x402470(,%rax,8)
mov    $0xcf,%eax
jmp    400fbe <phase_3+0x7b>
mov    $0x2c3,%eax
jmp    400fbe <phase_3+0x7b>
mov    $0x100,%eax
jmp    400fbe <phase_3+0x7b>
mov    $0x185,%eax
jmp    400fbe <phase_3+0x7b>
mov    $0xce,%eax
jmp    400fbe <phase_3+0x7b>
mov    $0x2aa,%eax
jmp    400fbe <phase_3+0x7b>
mov    $0x147,%eax
jmp    400fbe <phase_3+0x7b>
call   40143a <explode_bomb>
mov    $0x0,%eax
jmp    400fbe <phase_3+0x7b>
mov    $0x137,%eax
cmp    0xc(%rsp),%eax
je     400fc9 <phase_3+0x86>
call   40143a <explode_bomb>
add    $0x18,%rsp
ret
```

Use `scanf()` read from line 2, return the number of inputs (stored in `%eax`).
Test with a lot of numbers to see how many it can read. => 2

According to the next `compl` instruction, we know that our numbers are stored in `%rsp+0x8` & `%rsp+0xc`.
Also, we know the first number must be less than 7.

Then, jump to a specific address according to the number `*0x402470(,%rax,8)`.
Aka. 0x402470 + (number * 8). For me(number=5), it is `0x402498` => `0x400f98` => `0xce` => 206
