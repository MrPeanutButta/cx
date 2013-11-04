---
layout: page
title: "Cx (C code eXecutor)"
date: 2013-11-02 06:39
comments: true
sharing: true
footer: true
---
A typesafe, interepreted, procedurally object oriented language intended to be a subset of C/C++ syntax. Cx intended use
is aimed towards system administrative tasks, and network programmability.

### Supported compilers and platforms
Popular compilers which support C++11 are used to build the Cx interpretor.

* clang++
* g++4.7 or later
* VS 2012 express

Cx language features are restricted to the C++11 standard library and will compile on OSX, Linux, and Windows. Any
features deemed platform specific will be implemented in dynamic libraries which can be loaded at runtime.

### Cx Documentation
* [Cx-doc](cx-doc.html)

### Status
This project is for educational purposes only.<br>
Sections currently in progress include:

* types, and type safety
	+ completed
		+ int : 32bit integer type
		+ float : Basic real type
		+ char : 8bit character type
		+ primitive type arrays
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

### Getting involved
Fork the repo, email aaron.hebert@gmail.com and request to be added to the repo members roster. All of your changes are to be made on your copy of the repo. Changes to the master repo are done via pull requests followed by code review.<br>
We would be happy to welcome anyone willing to learn and help build this project. Ad-hoc SCRUM meetings are to be held to
discuss various designs and implementations. These discussions are intended to help the group learn and stay on track with
whats going on in the project. Availability for voice chat is a minimum requirement.<br>

The design RFC (Request for Comment) directory in Cx/doc/dev explains the thought processes around the various stages of
development. Also, there is a super secret (and super lonely) IRC channel on freenode.<br>
IRC: irc.freenode.net #Cx-lang