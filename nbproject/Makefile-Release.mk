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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/AppConfig.o \
	${OBJECTDIR}/GpgMECWorker.o \
	${OBJECTDIR}/GpgMEWorker.o \
	${OBJECTDIR}/K7Main.o \
	${OBJECTDIR}/PopupDeleter.o \
	${OBJECTDIR}/PopupUploader.o \
	${OBJECTDIR}/TransientMessageWidget.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-Wno-deprecated -Wno-deprecated-declarations
CXXFLAGS=-Wno-deprecated -Wno-deprecated-declarations

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/Wt/lib -lwt -lwtfcgi -lgpgmepp -lgpgme

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/k7

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/k7: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/k7 ${OBJECTFILES} ${LDLIBSOPTIONS} -s

${OBJECTDIR}/AppConfig.o: AppConfig.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -I/usr/local/Wt/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AppConfig.o AppConfig.cpp

${OBJECTDIR}/GpgMECWorker.o: GpgMECWorker.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -I/usr/local/Wt/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GpgMECWorker.o GpgMECWorker.cpp

${OBJECTDIR}/GpgMEWorker.o: GpgMEWorker.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -I/usr/local/Wt/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GpgMEWorker.o GpgMEWorker.cpp

${OBJECTDIR}/K7Main.o: K7Main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -I/usr/local/Wt/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/K7Main.o K7Main.cpp

${OBJECTDIR}/PopupDeleter.o: PopupDeleter.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -I/usr/local/Wt/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PopupDeleter.o PopupDeleter.cpp

${OBJECTDIR}/PopupUploader.o: PopupUploader.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -I/usr/local/Wt/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PopupUploader.o PopupUploader.cpp

${OBJECTDIR}/TransientMessageWidget.o: TransientMessageWidget.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -I/usr/local/Wt/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TransientMessageWidget.o TransientMessageWidget.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -I/usr/local/Wt/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
