The Cx Language
==
A scripting language for fun and profit.<br>
``` cpp
use io;	// Standard IO
io.puts("Hello, World!");
```

Intro
--

The Cx language is designed to be a very simple subset of C++ and C#. What makes Cx unique is the ability to easy extend the language using C++, and Cx. Basic data types can also be extended to add functionality. All data types can be extended by additional class declarations. This makes it easy to define objects in C++ to later extend those objects in Cx without having to recompile the library. For example, the below code will add the function <i><b>is_digit</b></i> to the basic Cx char type.
``` cpp
class char {
    /*!is_digit - Check if character is decimal digit
       Checks whether "this" is a decimal digit character.
       Decimal digits are any of: 0 1 2 3 4 5 6 7 8 9
       @param this : char to test if digit.
       @return true if digit, false if not. */
    bool is_digit() {
        return ((this >= '0') && (this <= '9'));
    }
}
```

The added function can then be called as follows:
``` cpp
char c = '5';
if(c.is_digit)
	// true
else
	// false
```

The Cx Virtual Machine (CxVM)
--
The Cx back-end is a very simple op-code, stack-based processor built from scratch which (very) loosely follows the JVM instruction syntax. CxVM instrucions can be directly emitted from within Cx source.

``` cpp

int assembly_example(int i){
	
	int a;
	asm iconst 0x3E8;	// Load 1000
	asm istore a;		// a = 1000
	
	asm{
		iconst 0b11111111 		// Load 255
		iload a		     		// Load 1000 (a == 1000)
		iadd				 	// a + 255 = 1255
		iload i			 		// Load 100 (if i == 100)
		iadd				 	// 1255 + 100 = 1355
		istore i			 	// i = 1355
		iload i          		// Load 1355
		istore assembly_test_0 	// assembly_test_0 = 1355
		ret						// return
	}
}

// Returns 1355 to the OS
return assembly_example(100);
```

Supported Compilers and Platforms
--
##### 32bit and 64bit

Popular compilers which support C++11 are used to build the Cx interpreter.

* clang++
* g++4.7 or later
* VS 2012 express

Cx language features are restricted to the C++11 standard library and will compile on OSX, Linux, and Windows. Any features deemed platform specific will be implemented in dynamic libraries which can be loaded at run-time.

### Cx Documentation
All documentation will be posted and updated at the link below. More information may be found in Cx/doc
* [Cx-doc](https://github.com/ahebert/cx/wiki)

#### Influences
* [Writing Compilers and Interpreters 2nd Edition](http://www.amazon.com/dp/0471113530/ref=cm_sw_r_tw_dp_Trazwb07Z6NRP)
* [The Java® Virtual Machine Specification](https://docs.oracle.com/javase/specs/jvms/se7/html/)

### Status
This project is for educational purposes only and is still in early development.
A working prototype is well underway and current work is focused on the base language.
