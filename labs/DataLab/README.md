# Data Lab

> Students implement simple logical, two's complement, and floating point functions, but using a highly restricted subset of C. For example, they might be asked to compute the absolute value of a number using only bit-level operations and straightline code. This lab helps students understand the bit-level representations of C data types and the bit-level behavior of the operations on data.

数的机器级表示: 复习完计组之后做, 感觉简单很多.

## Notes

* `bitXor`:<br/>![img](https://i.stack.imgur.com/3YG4z.png)
* `tmin`: 补码最小值: -2^(32-1) = -2147483648
* `isTmax`: 补码最大值: 2^(32-1) - 1 = 2147483647
  * I cannot do this myself
* `allOddBits`: 获取所有奇数位的值: 0xAAAAAAAA, 再判断奇数位是否相等
  * 相等通过 XOR 判断, 再取 `!` 即可
  * `!` 是逻辑非, `~` 是位取反
* `negate`: 取反后加一
  * 之前写[汇编](https://github.com/huang-feiyu/Computer-Design-and-Practice-Exp/tree/main/lab1)时, 用到的思路
* `isAsciiDigit`: 两数相减(使用 `negate`)判断符号位
* `conditional`: `?:` 对 0 取 `0x000000000` 与 `0xFFFFFFFF`, 分别与 y 和 z 相与
* `isLessOrEqual`: 先判断符号位, 再通过 `isAsciiDigit` 中方法判断是否小于等于
