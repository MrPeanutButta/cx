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
CND_PLATFORM=None-Linux-x86
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
	${OBJECTDIR}/src/error.o \
	${OBJECTDIR}/src/exec.o \
	${OBJECTDIR}/src/exec_expression.o \
	${OBJECTDIR}/src/exec_statment.o \
	${OBJECTDIR}/src/icode.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/parse_declarations.o \
	${OBJECTDIR}/src/parse_expression.o \
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

${OBJECTDIR}/src/buffer.o: src/buffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/buffer.o src/buffer.cpp

${OBJECTDIR}/src/common.o: src/common.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/common.o src/common.cpp

${OBJECTDIR}/src/complist.o: src/complist.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/complist.o src/complist.cpp

${OBJECTDIR}/src/error.o: src/error.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/error.o src/error.cpp

${OBJECTDIR}/src/exec.o: src/exec.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/exec.o src/exec.cpp

${OBJECTDIR}/src/exec_expression.o: src/exec_expression.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/exec_expression.o src/exec_expression.cpp

${OBJECTDIR}/src/exec_statment.o: src/exec_statment.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/exec_statment.o src/exec_statment.cpp

${OBJECTDIR}/src/icode.o: src/icode.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/icode.o src/icode.cpp

${OBJECTDIR}/src/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/parse_declarations.o: src/parse_declarations.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/parse_declarations.o src/parse_declarations.cpp

${OBJECTDIR}/src/parse_expression.o: src/parse_expression.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/parse_expression.o src/parse_expression.cpp

${OBJECTDIR}/src/parse_statement.o: src/parse_statement.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/parse_statement.o src/parse_statement.cpp

${OBJECTDIR}/src/parse_type1.o: src/parse_type1.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/parse_type1.o src/parse_type1.cpp

${OBJECTDIR}/src/parse_type2.o: src/parse_type2.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/parse_type2.o src/parse_type2.cpp

${OBJECTDIR}/src/parser.o: src/parser.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/parser.o src/parser.cpp

${OBJECTDIR}/src/scanner.o: src/scanner.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/scanner.o src/scanner.cpp

${OBJECTDIR}/src/symtable.o: src/symtable.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/symtable.o src/symtable.cpp

${OBJECTDIR}/src/tknnum.o: src/tknnum.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tknnum.o src/tknnum.cpp

${OBJECTDIR}/src/tknstrsp.o: src/tknstrsp.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tknstrsp.o src/tknstrsp.cpp

${OBJECTDIR}/src/tknword.o: src/tknword.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tknword.o src/tknword.cpp

${OBJECTDIR}/src/types.o: src/types.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -O -w -Iinclude -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/types.o src/types.cpp

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
