Cx
==
A scripting language for fun and profit.<br>
``` cpp
use io;

io.puts("Hello, World!");

return 0;
```

Cx syntax is designed to be a hybrid of C++ and C#. What makes Cx unique is the ability to easy extend the language using C or C++. Even basic data types can be extended to add functionality.<br>
All data types can be extended by additional class declarations. This makes it easy to define objects in C/C++ to later extend those objects in Cx without having to recompile the library.<br>
For example, the below code will add the function <i><b>is_digit</b></i> to the basic Cx char type.
``` cpp
class char {
    /** is_digit - Check if character is decimal digit
     * Checks whether "this" is a decimal digit character.
     * Decimal digits are any of: 0 1 2 3 4 5 6 7 8 9 
     *
     * @param this : char to test if digit.
     * @return true if digit, false if not. */
     
    bool is_digit() {
        return ((this >= '0') && (this <= '9'));
    }
};
```

The added function can then be called as follows:
``` cpp
char c = '5';
if(c.is_digit)
	// true
else
	// false
```
### Supported compilers and platforms
Popular compilers which support C++11 are used to build the Cx interpreter.

* clang++
* g++4.7 or later
* VS 2012 express

Cx language features are restricted to the C++11 standard library and will compile on OSX, Linux, and Windows. Any features deemed platform specific will be implemented in dynamic libraries which can be loaded at run-time.

### Cx Documentation
All documentation will be posted and updated at the link below. More information may be found in Cx/doc
* [Cx-doc](https://github.com/ahebert/cx/wiki)

### Status
This project is for educational purposes only and is still in early development.<br>

### Getting involved
* Fork a copy of the repo.
* Create one branch per feature/bug fix.
* Submit pull request to develop branch for code review.
* ???
* profit!
