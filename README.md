Cx
==
A typesafe, interepreted, procedurally object oriented language intended to be a subset of C/C++ 
syntax. Cx intended use is aimed towards system administrative tasks, and network programmability.

### Supported compilers and platforms
Popular compilers which support C++11 are used to build the Cx interpretor.

* clang++
* g++4.7 or later
* VS 2012 express

Cx language features are restricted to the C++11 standard library and will compile on OSX, Linux, 
and Windows. Any features deemed platform specific will be implemented in dynamic libraries which can 
be loaded at runtime.

### Cx Documentation
All documentaion will be posted and updated at the link below. More information ,ay be found in 
Cx/doc
* [Cx-doc](aaronhebert.net/cx/cx-doc.html)

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
We would be happy to welcome anyone willing to learn and help build this project. Ad-hoc SCRUM meetings are to be held to
discuss various designs and implementations. These discussions are intended to help the group learn and stay on track with
whats going on in the project. Availability for voice chat is a minimum requirement.<br>