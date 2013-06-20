#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/include/symtable.o \
	${OBJECTDIR}/src/buffer.o \
	${OBJECTDIR}/src/common.o \
	${OBJECTDIR}/src/complist.o \
	${OBJECTDIR}/src/error.o \
	${OBJECTDIR}/src/iform.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/parse_expression.o \
	${OBJECTDIR}/src/parse_statement.o \
	${OBJECTDIR}/src/parser.o \
	${OBJECTDIR}/src/scanner.o \
	${OBJECTDIR}/src/tknnum.o \
	${OBJECTDIR}/src/tknstrsp.o \
	${OBJECTDIR}/src/tknword.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cx

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cx: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cx ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/include/symtable.o: include/symtable.cpp 
	${MKDIR} -p ${OBJECTDIR}/include
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/symtable.o include/symtable.cpp

${OBJECTDIR}/src/buffer.o: src/buffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/buffer.o src/buffer.cpp

${OBJECTDIR}/src/common.o: src/common.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/common.o src/common.cpp

${OBJECTDIR}/src/complist.o: src/complist.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/complist.o src/complist.cpp

${OBJECTDIR}/src/error.o: src/error.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/error.o src/error.cpp

${OBJECTDIR}/src/iform.o: src/iform.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/iform.o src/iform.cpp

${OBJECTDIR}/src/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/parse_expression.o: src/parse_expression.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/parse_expression.o src/parse_expression.cpp

${OBJECTDIR}/src/parse_statement.o: src/parse_statement.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/parse_statement.o src/parse_statement.cpp

${OBJECTDIR}/src/parser.o: src/parser.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/parser.o src/parser.cpp

${OBJECTDIR}/src/scanner.o: src/scanner.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/scanner.o src/scanner.cpp

${OBJECTDIR}/src/tknnum.o: src/tknnum.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tknnum.o src/tknnum.cpp

${OBJECTDIR}/src/tknstrsp.o: src/tknstrsp.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tknstrsp.o src/tknstrsp.cpp

${OBJECTDIR}/src/tknword.o: src/tknword.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I/Users/aaro3965/Downloads/clang+llvm-3.2-x86_64-apple-darwin11/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tknword.o src/tknword.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cx

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
