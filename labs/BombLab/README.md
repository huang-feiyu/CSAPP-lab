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

Use `scanf()` read from line 2, return the number of inputs (stored in `%eax`).
Test with a lot of numbers to see how many it can read. => 2

According to the next `compl` instruction, we know that our numbers are stored in `%rsp+0x8` & `%rsp+0xc`.
Also, we know the first number must be less than 7.

Then, jump to a specific address according to the number `*0x402470(,%rax,8)`.
Aka. 0x402470 + (number * 8). For me(number=5), it is `0x402498` => `0x400f98` => `0xce` => 206

### phase_4

> Recursion.

According to `scanf()`, the number of inputs is 2.
According to `cmpl` instruction, the first number <= 15.

Then, jump to `func4`. (After `func4`, `%eax` must equals to 0) In my case, I test 0, it is correct in `func4`.

The 2nd number is easy to find, it is 0.

### phase_5

> Heap.

`%fs:0x28` is a random value to protect stack.

According to `string_length()`, we know our string is 6 chars. Skip the addresss calculation,
we move to `strings_not_equal` to get the reference string. My test answer is "answer",
I got "ayused" after processing the string. The reference strin is "flyers".

So, we have to step into the process.

```assembly
### at start: rax=0, rbx=0x6038c0, rsp=$my_string
40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx # ecx <- (rbx + rax)[i]
40108f:	88 0c 24             	mov    %cl,(%rsp) # rsp[i] <- ecx
401092:	48 8b 14 24          	mov    (%rsp),%rdx # rdx <- rsp[i]
401096:	83 e2 0f             	and    $0xf,%edx # edx <- rdx & 0xf, get the low 4 bits of rsp[i]
401099:	0f b6 92 b0 24 40 00 	movzbl 0x4024b0(%rdx),%edx # edx <- (0x4024b0 + rdx)[0]
4010a0:	88 54 04 10          	mov    %dl,0x10(%rsp,%rax,1) # rsp[16 + i] <- edx
4010a4:	48 83 c0 01          	add    $0x1,%rax # rax <- rax + 0x1
4010a8:	48 83 f8 06          	cmp    $0x6,%rax
4010ac:	75 dd                	jne    40108b <phase_5+0x29> # if rax != 0x6, goto 40108b
```

We got `0x4024b0` "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"
We need to get the reference string "flyers" from the string. The low 4 bits of our string as index.

```
9 :
	)9IYiy
15 :
/?O_o
14 :
.>N^n~
5 :
%5EUeu
6 :
&6FVfv
7 :
'7GWgw
```

I choose ")_^%&7" as the answer.

### phase_6

> Linked List

Use "1 2 4 8 16 32" as the test answer, after `read_six_numbers()`,
we know that our array store in `%rbp`/`%r13`/`%rsp`/`%r14`.

The next loop code block is equal to the following C code:

```c
int i = 0;
while (i != 6) {
    if (arr[i] - 1 > 5)
        bomb()
    i++;
    for (j = i; j <= 5; j++) {
        if (arr[j] == arr[i])
            bomb()
    }
}
```

So we know that: `arr[i] <= 6` && `arr[j] != arr[i]`.
According to `read_six_numbers()`, we need to choose 6 numbers from "0 1 2 3 4 5 6".

The following code is the same as the C code:

```c
for (int i = 0; i < 6; i++){
    arr[i] = 7 - arr[i];
}
```

The following code is the same as the C code:

```c
for (int i = 0; i < 6; i++) {
    int temp = arr[i];
    int addr = 0x6032d0;
    for (int j = 1; j < temp; j++) {
        addr = *(addr + 8);
    }
    // *(rbx+2*i) = addr; aka, a new array of 8-bits element.
    *(arr + 2 * i + 0x20) = addr; // n_arr[2*i] = addr;
}
```

The following code is the same as the C code:

```c
// n_arr is new array of 8-bits element.
for(int i = 1; i < 6; i++){
    *(n_arr[i-1]+0x8) = n_arr[i];
}
```

* `n_arr[0~5]` is `0x6032d0` ~ `0x603320`.
* `*(n_arr[0~5]+0x8)` is `*0x6032d0` ~ `*0x603320`.

The last code is the same as the C code:

```c
for (int i = 1; i < 6; i++) {
    if (*(n_arr[i-1]) < *(n_arr[i]))
        bomb()
}
```

The first must be less than the second.

Use gdb to get `*(n_arr[0~5]+0x8)`, it is: 0x14c, 0x0a8, 0x39c, 0x2b3, 0x1dd, 0x1bb.
We need to sort them desc and our "123456" => "3 4 5 6 1 2". Every element is subtracted 7 => "4 3 2 1 6 5".

After reading others' solution, I know that: phase_5 is about heap, phase_6 is about linked-list.
**AND**, there is a secret_phase about binary tree.
