Cx expressions design document

[07-30-2013] Ideas for expressions design
The initial design of Cx expressions works well with basic types (int, bool, char).
However, the current design does not take in to account I/O streams. In this
document I/O stream support for expressions will be pondered.

Making expressions stream aware will provide simpler string and I/O operations
to the user.

Operators
--------------------------------------

Arithmetic
--------------------------------------
The five arithmetical operations supported by the Cx language are:

+	addition
-	subtraction
*	multiplication
/	division
%	modulo

Assignment
--------------------------------------
The assignment operator assigns a value to a variable.

=

Compound Assignment
--------------------------------------
When we want to modify the value of a variable by performing an operation on the
value currently stored in that variable we can use compound assignment
operators.

+=
-=
*=
/=
%=
>>=
<<=
&=
^=
|=

expression              is equivalent to
value += increase;	value = value + increase;
a -= 5;                 a = a - 5;
a /= b;                 a = a / b;
price *= units + 1;	price = price * (units + 1);


Increase and Decrease
--------------------------------------
Shortening even more some expressions, the increase operator (++) and the
decrease operator (--) increase or reduce by one the value stored in a variable.
They are equivalent to +=1 and to -=1, respectively.

++
--

Relational and Equality
--------------------------------------
In order to evaluate a comparison between two expressions we can use the
relational and equality operators. The result of a relational operation is a
Boolean value that can only be true or false, according to its Boolean result.

==	Equal to
!=	Not equal to
>	Greater than
<	Less than
>=	Greater than or equal to
<=	Less than or equal to

Logical
--------------------------------------
! Operator
Inverse of boolean expression

!(true expression) == false
!(false expression) == true

&& Operator
a	b	a && b
true	true	true
true	false	false
false	true	false
false	false	false


|| Operator
a	b	a || b
true	true	true
true	false	true
false	true	true
false	false	false

Bitwise
--------------------------------------
Bitwise operators modify variables considering the bit patterns that represent the values they store.

operator	asm equivalent	description
&               AND             Bitwise AND
|               OR              Bitwise Inclusive OR
^               XOR             Bitwise Exclusive OR
~               NOT             Unary complement (bit inversion)
<<              SHL             Shift Left
>>              SHR             Shift Right
