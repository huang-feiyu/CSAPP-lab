# edi: &tree; esi: num
0000000000401204 <fun7>:
    401204:	48 83 ec 08          	sub    $0x8,%rsp
    401208:	48 85 ff             	test   %rdi,%rdi
    40120b:	74 2b                	je     401238 <fun7+0x34> # if tree == null, goto failed
    40120d:	8b 17                	mov    (%rdi),%edx # edx: tree->value
    40120f:	39 f2                	cmp    %esi,%edx
    401211:	7e 0d                	jle    401220 <fun7+0x1c> # if num <= tree->value, goto 0x401220
    401213:	48 8b 7f 08          	mov    0x8(%rdi),%rdi # rdi: tree->left
    401217:	e8 e8 ff ff ff       	call   401204 <fun7> # func7(tree->left, num) -> ret
    40121c:	01 c0                	add    %eax,%eax # eax: 2 * ret
    40121e:	eb 1d                	jmp    40123d <fun7+0x39> # to the end, so if we go this way, we must make ret = 0
    401220:	b8 00 00 00 00       	mov    $0x0,%eax # eax: 0
    401225:	39 f2                	cmp    %esi,%edx
    401227:	74 14                	je     40123d <fun7+0x39> # if num == tree->value, goto end
    401229:	48 8b 7f 10          	mov    0x10(%rdi),%rdi # rdi: tree->right
    40122d:	e8 d2 ff ff ff       	call   401204 <fun7> # func7(tree->right, num) -> ret
    401232:	8d 44 00 01          	lea    0x1(%rax,%rax,1),%eax # eax: ret = 2 * ret + 1
    401236:	eb 05                	jmp    40123d <fun7+0x39>
    401238:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
    40123d:	48 83 c4 08          	add    $0x8,%rsp
    401241:	c3                   	ret