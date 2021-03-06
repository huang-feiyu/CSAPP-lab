/*
 * CS:APP Data Lab
 *
 * Huang huangblog.com
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

Replace the "return" statement in each function with one
or more lines of C code that implements the function. Your code
must conform to the following style:

int Funct(arg1, arg2, ...) {
    /* brief description of how your implementation works */
    int var1 = Expr1;
    ...
    int varM = ExprM;

    varJ = ExprJ;
    ...
    varN = ExprN;
    return ExprR;
}

Each "Expr" is an expression using ONLY the following:
* 1. Integer constants 0 through 255 (0xFF), inclusive. You are
*    not allowed to use big constants such as 0xffffffff.
* 2. Function arguments and local variables (no global variables).
* 3. Unary integer operations ! ~
* 4. Binary integer operations & ^ | + << >>

Some of the problems restrict the set of allowed operators even further.
Each "Expr" may consist of multiple operators. You are not restricted to
one operator per line.

You are expressly forbidden to:
* 1. Use any control constructs such as if, do, while, for, switch, etc.
* 2. Define or use any macros.
* 3. Define any additional functions in this file.
* 4. Call any functions.
* 5. Use any other operations, such as &&, ||, -, or ?:
* 6. Use any form of casting.
* 7. Use any data type other than int.  This implies that you
*    cannot use arrays, structs, or unions.


You may assume that your machine:
* 1. Uses 2s complement, 32-bit representations of integers.
* 2. Performs right shifts arithmetically.
* 3. Has unpredictable behavior when shifting if the shift amount
*    is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
/*
 * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
 */
int pow2plus1(int x) {
   /* exploit ability of shifts to compute powers of 2 */
    return (1 << x) + 1;
}

/*
 * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
 */
int pow2plus4(int x) {
    /* exploit ability of shifts to compute powers of 2 */
    int result = (1 << x);
    result += 4;
    return result;
}

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
* 1. Define or use any macros.
* 2. Define any additional functions in this file.
* 3. Call any functions.
* 4. Use any form of casting.
* 5. Use any data type other than int or unsigned.  This means that you
*    cannot use arrays, structs, or unions.
* 6. Use any floating point data types, operations, or constants.


NOTES:
* 1. Use the dlc (data lab checker) compiler (described in the handout) to
*    check the legality of your solutions.
* 2. Each function has a maximum number of operations (integer, logical,
*    or comparison) that you are allowed to use for your implementation
*    of the function.  The max operator count is checked by dlc.
*    Note that assignment ('=') is not counted; you may use as many of
*    these as you want without penalty.
* 3. Use the btest test harness to check your functions for correctness.
* 4. Use the BDD checker to formally verify your functions
* 5. The maximum number of ops for each function is given in the
*    header comment for each function. If there are any inconsistencies
*    between the maximum ops in the writeup and in this file, consider
*    this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */

#endif
// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
    /* x^y = ~(~(x & ~(x&y)) & ~(y & ~(x&y))) */
    int not_x_y = ~(x & y);
    return ~(~(x & not_x_y) & ~(y & not_x_y));
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    /* -2^(32-1) = -2147483648 */
    return 0x1 << 31;
}
/*
 * isTmax - returns 1 if x is the maximum, two's complement number, and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
    /* !(x ^ (x + 1)) => x == {0b'*,0b7FFFFFFF} ? 1 : 0
     * !!y => x == 0xFFFFFFFF, !!y=0
     *        x == 0x7FFFFFFF, !!y=1
     */
    int y = x + 1;
    return !(x ^ ~y) & !!y;
}
// 2
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
    /* is x's odd-bits equals to 0xAAAAAAAA */
    int quarter = 0xAA;
    int half = quarter + (quarter << 8);
    int all = half + (half << 16);
    return !((all & x) ^ all);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    /* -x <- not x + 1 */
    return ~x + 1;
}
// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    /* x - 0x30 >= 0 && 0x39 - x >= 0 */
    // the following code: 0 represents TRUE, 1 represents FALSE
    int is_neg = !(x >> 31);
    int is_lesser_than_0x30 = !((x + (~0x30 + 1)) >> 31);
    int is_greater_than_0x39 = !((0x39 + (~x + 1)) >> 31);
    return is_neg & is_lesser_than_0x30 & is_greater_than_0x39;
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    /* x == 0 ? ~0x00000000 & z
     * x != 0 ?  0xFFFFFFFF & y
     */
    // END: x == 0 ? 0x0 : 0xFFFFFFFF
    int is_x_zero = !!x;
    is_x_zero += is_x_zero << 1;
    is_x_zero += is_x_zero << 2;
    is_x_zero += is_x_zero << 4;
    is_x_zero += is_x_zero << 8;
    is_x_zero += is_x_zero << 16;

    return (is_x_zero & y) | (~is_x_zero & z);
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    /* sign cmp, then x == y | x - y < 0 */
    int x_sign = !!(x >> 31);
    int y_sign = !!(y >> 31);
    int diff_sign = x_sign ^ y_sign;

    int is_equal = !(x ^ y);
    int x_sub_y = x + (~y + 1);
    int is_less = !!(x_sub_y >> 31);

    return (diff_sign & !(x_sign ^ 0x1)) | (!diff_sign & (is_equal | is_less));
}
// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x) {
    /* sign(x) | sign(-x) == 0 iff x == 0
     *  * 0: (x | -x) >> 31 = 0 => ++ => 0x1
     *  * other: (x | -x) >> 31 = 0xFFFFFFFF => ++ => 0x0 (overflow)
     */
    return ((x | (~x + 1)) >> 31) + 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
    /* b# <- # or 0 => represents high-bits
     * if b16 == 0:                                    donot care high 16 bits
     * else       : b16=16, throw away high 16 bits => donot care high 16 bits
     * ... (the same with other bits)
     */
    int sign, b16, b8, b4, b2, b1;
    sign = x >> 31;
    x = (sign & ~x) | (~sign & x);

    // dichotomy
    b16 = !!(x >> 16) << 4;
    x >>= b16;
    b8 = !!(x >> 8) << 3;
    x >>= b8;
    b4 = !!(x >> 4) << 2;
    x >>= b4;
    b2 = !!(x >> 2) << 1;
    x >>= b2;
    b1 = !!(x >> 1) << 0;
    x >>= b1;

    return x + b16 + b8 + b4 + b2 + b1 + 1;
}
// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
    /* decode to get sign and exponent, take action according to the exponent */
    int exponent, sign;
    sign = uf & (1 << 31);
    exponent = (uf >> 23) & 0xFF;
    // too small a number/denormalized number
    if (exponent == 0) {
        return (uf << 1) | sign;
    }
    // NaN or infinity
    exponent += exponent != 255;
    uf = (uf & 0x807FFFFF) | (exponent << 23);
    return uf;
}
/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
    /*    float is too big or too small?
     * -> need to round?
     * -> need to add sign?
     */
    int exponent, sign, mantissa;
    sign = uf & (1 << 31);
    exponent = ((uf >> 23) & 0xFF) - 127;
    mantissa = (uf & 0x7FFFFF) | 0x800000;
    // too big
    if (exponent > 31) {
        return 0x80000000u;
    }
    // too small
    if (exponent < 0) {
        return 0;
    }
    // mantissa is 23 bit
    if (exponent < 23) {
        mantissa >>= 23 - exponent;
    } else {
        mantissa <<= exponent - 23;
    }

    return sign ? ~mantissa + 1 : mantissa;
}
/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    /* denorm : exponent == 0
     * middle : exponent << 23 (exponent = x + 127)
     * too big: exponent == 255
     */
    if (x < -127) {
        return 0;
    }
    if (x <= 127) {
        return (x + 127) << 23;
    }
    return 0x7F800000;
}
