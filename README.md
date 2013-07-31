Cx
==
Cx (C Code Executor, Count Register, Cancel, Sex...idk take your pick)<br>
A lightwieght, interepreted, functional, object oriented language intended to be a subset of C/C++ syntax. Cx intended use
is aimed towards system administrative tasks, automation, and concurrency.

### Supported compilers and platforms
Popular compilers which support C++11 are used to build the Cx interpretor.
* clang++
* g++4.7 or later
* VS 2012 express

Cx language features are restricted to the C++11 standard library and will compile on OSX, Linux, and Windows. Any 
features deemed platform specific will be implemented in dynamic libraries which can be loaded at runtime.

### Cx stdlib
Cx math routines and I/O will be implemented within the interpretor. Other aspects of the language are to be 
implemented in Cx libraries that are translated to intermediate code.

### Status
This project is for educational purposes only.<br>
Sections currently in progress include:
* types, and type safety
* dynamic run time stack
* expressions and operators
* standard statements
* documentation and project organization

### Getting involved
Fork the repo, email aaron.hebert@gmail.com and request to be added to the repo members roster.<br>
We would be happy to welcome anyone willing to learn and help build this project. Ad-hoc SCRUM meetings are to be held to
discuss various designs and implementations. These discussions are intended to help the group learn and stay on track with
whats going on in the project.<br>

The design RFC (Request for Comment) directory in Cx/doc/dev explains the thought processes around the various stages of
development. Also, there is a super secret (and super lonely) IRC channel on freenode.<br>
IRC: irc.freenode.net #Cx-lang
