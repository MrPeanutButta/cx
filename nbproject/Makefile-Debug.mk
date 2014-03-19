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
	${OBJECTDIR}/src/buffer.o \
	${OBJECTDIR}/src/common.o \
	${OBJECTDIR}/src/complist.o \
	${OBJECTDIR}/src/cx-debug/additive.o \
	${OBJECTDIR}/src/cx-debug/assign.o \
	${OBJECTDIR}/src/cx-debug/cx_stdio.o \
	${OBJECTDIR}/src/cx-debug/do.o \
	${OBJECTDIR}/src/cx-debug/exec.o \
	${OBJECTDIR}/src/cx-debug/expression.o \
	${OBJECTDIR}/src/cx-debug/factor.o \
	${OBJECTDIR}/src/cx-debug/function.o \
	${OBJECTDIR}/src/cx-debug/init_list.o \
	${OBJECTDIR}/src/cx-debug/logical.o \
	${OBJECTDIR}/src/cx-debug/malloc.o \
	${OBJECTDIR}/src/cx-debug/multiplicative.o \
	${OBJECTDIR}/src/cx-debug/relational.o \
	${OBJECTDIR}/src/cx-debug/statment.o \
	${OBJECTDIR}/src/cx-debug/std_members.o \
	${OBJECTDIR}/src/cx-debug/tracer.o \
	${OBJECTDIR}/src/cx-debug/unary.o \
	${OBJECTDIR}/src/cx-debug/while.o \
	${OBJECTDIR}/src/error.o \
	${OBJECTDIR}/src/icode.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/parse_declarations.o \
	${OBJECTDIR}/src/parse_directive.o \
	${OBJECTDIR}/src/parse_expression.o \
	${OBJECTDIR}/src/parse_routine1.o \
	${OBJECTDIR}/src/parse_routine2.o \
	${OBJECTDIR}/src/parse_statement.o \
	${OBJECTDIR}/src/parse_type1.o \
	${OBJECTDIR}/src/parse_type2.o \
	${OBJECTDIR}/src/parser.o \
	${OBJECTDIR}/src/scanner.o \
	${OBJECTDIR}/src/symtable.o \
	${OBJECTDIR}/src/tknnum.o \
	${OBJECTDIR}/src/tknstrsp.o \
	${OBJECTDIR}/src/tknword.o \
	${OBJECTDIR}/src/types.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-g3
CXXFLAGS=-g3

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-ldl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/cx

${CND_DISTDIR}/cx: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}
	${LINK.cc} -o ${CND_DISTDIR}/cx ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/buffer.o: nbproject/Makefile-${CND_CONF}.mk src/buffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/buffer.o src/buffer.cpp

${OBJECTDIR}/src/common.o: nbproject/Makefile-${CND_CONF}.mk src/common.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/common.o src/common.cpp

${OBJECTDIR}/src/complist.o: nbproject/Makefile-${CND_CONF}.mk src/complist.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/complist.o src/complist.cpp

${OBJECTDIR}/src/cx-debug/additive.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/additive.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/additive.o src/cx-debug/additive.cpp

${OBJECTDIR}/src/cx-debug/assign.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/assign.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/assign.o src/cx-debug/assign.cpp

${OBJECTDIR}/src/cx-debug/cx_stdio.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/cx_stdio.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/cx_stdio.o src/cx-debug/cx_stdio.cpp

${OBJECTDIR}/src/cx-debug/do.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/do.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/do.o src/cx-debug/do.cpp

${OBJECTDIR}/src/cx-debug/exec.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/exec.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/exec.o src/cx-debug/exec.cpp

${OBJECTDIR}/src/cx-debug/expression.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/expression.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/expression.o src/cx-debug/expression.cpp

${OBJECTDIR}/src/cx-debug/factor.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/factor.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/factor.o src/cx-debug/factor.cpp

${OBJECTDIR}/src/cx-debug/function.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/function.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/function.o src/cx-debug/function.cpp

${OBJECTDIR}/src/cx-debug/init_list.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/init_list.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/init_list.o src/cx-debug/init_list.cpp

${OBJECTDIR}/src/cx-debug/logical.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/logical.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/logical.o src/cx-debug/logical.cpp

${OBJECTDIR}/src/cx-debug/malloc.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/malloc.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/malloc.o src/cx-debug/malloc.cpp

${OBJECTDIR}/src/cx-debug/multiplicative.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/multiplicative.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/multiplicative.o src/cx-debug/multiplicative.cpp

${OBJECTDIR}/src/cx-debug/relational.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/relational.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/relational.o src/cx-debug/relational.cpp

${OBJECTDIR}/src/cx-debug/statment.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/statment.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/statment.o src/cx-debug/statment.cpp

${OBJECTDIR}/src/cx-debug/std_members.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/std_members.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/std_members.o src/cx-debug/std_members.cpp

${OBJECTDIR}/src/cx-debug/tracer.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/tracer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/tracer.o src/cx-debug/tracer.cpp

${OBJECTDIR}/src/cx-debug/unary.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/unary.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/unary.o src/cx-debug/unary.cpp

${OBJECTDIR}/src/cx-debug/while.o: nbproject/Makefile-${CND_CONF}.mk src/cx-debug/while.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cx-debug
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cx-debug/while.o src/cx-debug/while.cpp

${OBJECTDIR}/src/error.o: nbproject/Makefile-${CND_CONF}.mk src/error.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/error.o src/error.cpp

${OBJECTDIR}/src/icode.o: nbproject/Makefile-${CND_CONF}.mk src/icode.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/icode.o src/icode.cpp

${OBJECTDIR}/src/main.o: nbproject/Makefile-${CND_CONF}.mk src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/parse_declarations.o: nbproject/Makefile-${CND_CONF}.mk src/parse_declarations.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/parse_declarations.o src/parse_declarations.cpp

${OBJECTDIR}/src/parse_directive.o: nbproject/Makefile-${CND_CONF}.mk src/parse_directive.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/parse_directive.o src/parse_directive.cpp

${OBJECTDIR}/src/parse_expression.o: nbproject/Makefile-${CND_CONF}.mk src/parse_expression.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/parse_expression.o src/parse_expression.cpp

${OBJECTDIR}/src/parse_routine1.o: nbproject/Makefile-${CND_CONF}.mk src/parse_routine1.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/parse_routine1.o src/parse_routine1.cpp

${OBJECTDIR}/src/parse_routine2.o: nbproject/Makefile-${CND_CONF}.mk src/parse_routine2.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/parse_routine2.o src/parse_routine2.cpp

${OBJECTDIR}/src/parse_statement.o: nbproject/Makefile-${CND_CONF}.mk src/parse_statement.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/parse_statement.o src/parse_statement.cpp

${OBJECTDIR}/src/parse_type1.o: nbproject/Makefile-${CND_CONF}.mk src/parse_type1.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/parse_type1.o src/parse_type1.cpp

${OBJECTDIR}/src/parse_type2.o: nbproject/Makefile-${CND_CONF}.mk src/parse_type2.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/parse_type2.o src/parse_type2.cpp

${OBJECTDIR}/src/parser.o: nbproject/Makefile-${CND_CONF}.mk src/parser.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/parser.o src/parser.cpp

${OBJECTDIR}/src/scanner.o: nbproject/Makefile-${CND_CONF}.mk src/scanner.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/scanner.o src/scanner.cpp

${OBJECTDIR}/src/symtable.o: nbproject/Makefile-${CND_CONF}.mk src/symtable.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/symtable.o src/symtable.cpp

${OBJECTDIR}/src/tknnum.o: nbproject/Makefile-${CND_CONF}.mk src/tknnum.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tknnum.o src/tknnum.cpp

${OBJECTDIR}/src/tknstrsp.o: nbproject/Makefile-${CND_CONF}.mk src/tknstrsp.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tknstrsp.o src/tknstrsp.cpp

${OBJECTDIR}/src/tknword.o: nbproject/Makefile-${CND_CONF}.mk src/tknword.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tknword.o src/tknword.cpp

${OBJECTDIR}/src/types.o: nbproject/Makefile-${CND_CONF}.mk src/types.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -Iinclude -Iinclude/cx-debug -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/types.o src/types.cpp

# Subprojects
.build-subprojects:
	cd stdlib/linux && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/cx

# Subprojects
.clean-subprojects:
	cd stdlib/linux && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
