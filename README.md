Cx
==
A type-safe, interpreted, procedural, object oriented language intended to be a subset of C/C++ syntax. Cx intended use is aimed towards system administrative tasks, and network programmability.

``` cpp
#include stdio

using std;

int main(){
	
	puts("Hello, World!");

	return 0;
}
```

### Supported compilers and platforms
Popular compilers which support C++11 are used to build the Cx interpreter.

* clang++
* g++4.7 or later
* VS 2012 express

Cx language features are restricted to the C++11 standard library and will compile on OSX, Linux, and Windows. Any features deemed platform specific will be implemented in dynamic libraries which can be loaded at run-time.

### Cx Documentation
All documentation will be posted and updated at the link below. More information may be found in Cx/doc
* [Cx-doc](https://github.com/Cx-lang/Cx/wiki)

### Status
This project is for educational purposes only.<br>
Sections currently in progress include:

* types, and type safety
	+ completed
		+ int : 32bit integer type
        + bool : true/false
		+ float : Basic real type
		+ char : 8bit character type
		+ primitive type array  
    + unsigned char/int, double, long, wchar_t are under development

* dynamic run time stack
	+ completed
		+ fully dynamic allocation/deallocation
		+ frame headers occupy a single stack element

* expressions and operators
	+ completed
		+ base language operators
		+ recursive expressions
		+ array initilization lists
* standard statements
* documentation and project organization

### Roadmap
* Object types
* Namespace controlled global scope
* File I/O - revision
* Preprocessing directives
* Just-in-Time compiling
    + debating on custom JIT or existing library

### Getting involved
Fork the repo. All of your changes are to be made on your copy of the repo. Changes to the master repo are done via pull requests followed by code review.<br>
