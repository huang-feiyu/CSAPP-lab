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
