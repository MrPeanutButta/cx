# Arithmetic types
--------------------------------------

## Character types
`byte` - 1 byte unsigned char
`char` - type for character representation which can be most efficiently processed on the target system
`wchar` - type for wide character representation.

## Integer types
`int` - basic integer type. Guaranteed to have width of at least 16 bits. However, on 32/64 bit systems it is almost exclusively guaranteed to have width of at least 32 bits.

## Floating point type
 	`float` - single precision floating point type. Usually IEEE-754 32 bit floating point type

## Boolean type
`bool` - type, capable of holding one of the two values: true or false.

## Type Qualifier
`const` - The followed type is declared as constant and cannot be modified. (incomplete)
``` cpp
const int a = 100; // a is permantly equal to 100
```