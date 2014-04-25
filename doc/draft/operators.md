---
layout: page
title: "Cx-operators"
date: 2013-11-02 16:18
comments: true
sharing: true
footer: true
---
## Arithmetic
The five arithmetical operations supported by the Cx language are:<br>

<table><tr><th>Op</th></tr>
<tr><td>+</td><td>Addition</td></tr>
<tr><td>-</td><td>Subtraction</td></tr>
<tr><td>*</td><td>Multiplication</td></tr>
<tr><td>/</td><td>Division</td></tr>
<tr><td>%</td><td>Modulo</td></tr>
</table>

The addition operator `+` has special meaning for array types. Having an array on either side of
this binary operator will concatenate both side of the expression into a allocated temporary value.
Both sides of the expression must be of the same base type, e.g. an array of char elements is compatible with a primitive char type, or 2 arrays of int elements would also be OK. 

``` cpp
// gets() and string allocation example

#include cxstdio

int main () {

    do{
        puts("Please enter your name: ");
        // read string until '\n'
        char *in = gets();
    }while(!in);
    
    char *t1 = "beep";
    char *t2 = "-boop";
    char *greet = "Hello " + in + ", you a bad " + t1 + t2 + " !";

    puts(greet);

    return 0;
}
```

## Assignment
The assignment operator assigns a value to a variable. The assignment operator
has diferent behavior for primitive versus array types.

<table><tr><th>Op</th></tr>
<tr><td>=</td><td>Assignment</td></tr>
</table>

``` cpp
	// Simple copy assignment
	char c = 'a';
	char b = c;

	// Reallocation assignment on array2
	int *int_array1 = {1,2,3,4,5,6,7,8,9,0};
	int *int_array2 = {3,2,1};

	// int_array2 gets reallocated with the contents of array1
	int_array2 = int_array1;
```

## Compound Assignment
When we want to modify the value of a variable by performing an operation on the
value currently stored in that variable we can use compound assignment
operators.<br>

<table><tr><th>Op</th><th>Arithmetic</th></tr>
<tr><td>+=</td><td>Compound Addition</td></tr>
<tr><td>-=</td><td>Compound Subtraction<td></tr>
<tr><td>*=</td><td>Compound Multiplication</td></tr>
<tr><td>/=</td><td>Compound Division</td></tr>
<tr><td>%=</td><td>Compound Modulo</td></tr>
<tr><td>>>=</td><td>Compound Shift Right</td></tr>
<tr><td><<=</td><td>Compound Shift Left</td></tr>
<tr><td>&=</td><td>Compound AND</td></tr>
<tr><td>^=</td><td>Compound XOR</td></tr>
<tr><td>|=</td><td>Compound OR</td></tr>
</table>
<table><tr><th>expression</th><th>is equivalent to</th></tr>
<tr><td>value += increase;</td><td>value = value + increase;</td></tr>
<tr><td>a -= 5;</td><td>a = a - 5;</td></tr>
<tr><td>a /= b;</td><td>a = a / b;</td></tr>
<tr><td>price *= units + 1;</td><td>price = price * (units + 1);</td></tr>
</table>

The `+=` operator has special uses for an array of primitive types. The following will perform
a reallocation for `array1` and compound assign the contents of `array2`.

        array1 += array2

If the right hand side is a primitive type of the left hand side array, the operator
will perform the same operation but for the single element.

        array1 += 'c'

``` cpp
#include cxstdio // for puts

// Array compound assignment example

int main(){

	// character array type
	char *string = "this is a really long string for you!!!!!";

	// All arrays perform compound assignment via += operator.
	string += "... this was allocated and appended";

	puts(string);

	return 0;
}

```

## Increase and Decrease
The increase operator (++) and the decrease operator (--) increase or
reduce by one the value stored in a variable. They are equivalent
to +=1 and to -=1, respectively.<br>

</table>
<table><tr><th>Op</th></tr>
<tr><td>++</td><td>Increase</td></tr>
<tr><td>--</td><td>Decrease</td></tr>
</table>

Increase `++`<br>
Decrease `--`

``` cpp
	int i = 0;
	i++;	// i is now equal to 1
```

## Relational and Equality
In order to evaluate a comparison between two expressions we can use the
relational and equality operators. The result of a relational operation is a
Boolean value that can only be true or false, according to its Boolean result.<br>

Equal `==`<br>
Not Equal `!=`<br>
Grater than `>`<br>
Less than `<`<br>
Greater than of equal to `>=`<br>
Less than or equal to `<=`<br>

``` cpp
	// equal is assigned true based on the expression
	bool equal = (1 == 1);

	// i is assigned a 0(zero) from the false expression
	int i = (1 > 1);
```

## Logical
Not `!`<br>
And `&&`<br>
Or `||`

### `!` Not Operator
The Operator `!` is the Cx operator to perform the Boolean operation NOT, it has only one operand, located at its right, and the only thing that it does is to inverse the value of it, producing false if its operand is true and true if its operand is false. Basically, it returns the opposite Boolean value of evaluating its operand. For example:

``` cpp
!(true expression) == false
!(false expression) == true
```

### `&&` And Operator
The logical operators && and || are used when evaluating two expressions to obtain a single relational result. The operator && corresponds with Boolean logical operation AND. This operation results true if both its two operands are true, and false otherwise. The following panel shows the result of operator && evaluating the expression a && b:

```
a         b               a && b
true      true            true
true      false           false
false     true            false
false     false           false
```

### `||` Or Operator
The operator || corresponds with Boolean logical operation OR. This operation results true if either one of its two operands is true, thus being false only when both operands are false themselves. Here are the possible results of a || b:

```
a         b               a || b
true      true            true
true      false           true
false     true            true
false     false           false
```

## Bitwise
Bitwise operators modify variables considering the bit patterns that represent the values they store.

Bitwise AND `&`<br>
Bitwise OR `|`<br>
Bitwise XOR `^`<br>
Bitwise NOT `~`<br>
Bitwise SHL `<<`<br>
Bitwise SHR `>>`

	operator        asm equivalent	description
	&               AND             Bitwise AND
	|               OR              Bitwise Inclusive OR
	^               XOR             Bitwise Exclusive OR
	~               NOT             Unary complement (bit inversion)
	<<              SHL             Shift Left
	>>              SHR             Shift Right

``` cpp
// Binary numeral system (base 2) square root (integer)
int isqrt(int num) {
    int res = 0;
    int bit = 1 << 14;

    // "bit" starts at the highest power of four <= the argument.
    while (bit > num)
        bit >>= 2;

    while (bit != 0) {
        if (num >= res + bit) {
            num -= res + bit;
            res = (res >> 1) + bit;
        }else{
            res >>= 1;
        }

        bit >>= 2;
    }

    return res;
}
```