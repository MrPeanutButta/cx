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
	${OBJECTDIR}/_ext/1449855206/stdio.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-g3 -fPIC
CXXFLAGS=-g3 -fPIC

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../../cx-api/dist/Debug/GNU-Linux-x86/libcx-api.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../stdio.${CND_DLIB_EXT}

../stdio.${CND_DLIB_EXT}: ../../cx-api/dist/Debug/GNU-Linux-x86/libcx-api.a

../stdio.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ..
	${LINK.cc} -o ../stdio.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -g3 -shared

${OBJECTDIR}/_ext/1449855206/stdio.o: ../win32/stdio/stdio.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1449855206
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../include/cx-debug -I../../include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1449855206/stdio.o ../win32/stdio/stdio.cpp

# Subprojects
.build-subprojects:
	cd ../../cx-api && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../stdio.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:
	cd ../../cx-api && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
