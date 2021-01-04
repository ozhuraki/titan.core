##############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Baranyi, Botond
#
##############################################################################
TOPDIR := ../../..
include ../../../Makefile.regression

#
# Do NOT touch this line...
#
.PHONY: all run archive check clean dep objects

.SUFFIXES: .d

#
# Set these variables...
#

# Flags for the C++ preprocessor (and makedepend as well):
CPPFLAGS += -I. -I../../ProjA/bin -I../../ProjA/src -I../src

#CXXFLAGS += -save-temps
CXXFLAGS += -g

# Flags for dependency generation
CXXDEPFLAGS = -MM

# Flags for the linker:
LDFLAGS += -g

ifeq ($(PLATFORM), WIN32)
# Silence linker warnings.
LDFLAGS += -Wl,--enable-auto-import,--enable-runtime-pseudo-reloc
endif

# Flags for the TTCN-3 and ASN.1 compiler:
COMPILER_FLAGS += -I

# Execution mode: (either ttcn3 or ttcn3-parallel)
TTCN3_LIB = ttcn3$(RT2_SUFFIX)-parallel$(DYNAMIC_SUFFIX)


# TTCN-3 modules of this project:
TTCN3_MODULES = ../src/B.ttcn

# TTCN-3 modules used from central project(s):
BASE_TTCN3_MODULES = ../../ProjA/src/A.ttcn

# ASN.1 modules of this project:
ASN1_MODULES =

# ASN.1 modules used from central project(s):
BASE_ASN1_MODULES =

# C++ source & header files generated from the TTCN-3 & ASN.1 modules of
# this project:
GENERATED_SOURCES = B.cc
GENERATED_HEADERS = $(GENERATED_SOURCES:.cc=.hh)
ifdef SPLIT_TO_SLICES
POSTFIXES := $(foreach file, $(SPLIT_TO_SLICES), $(addsuffix $(file), _part_))
POSTFIXES := $(foreach file, $(POSTFIXES), $(addprefix $(file), .cc))
GENERATED_SOURCES2 := $(foreach file, $(GENERATED_SOURCES:.cc=), $(addprefix $(file), $(POSTFIXES)))
GENERATED_SOURCES += $(GENERATED_SOURCES2)
endif

# C++ source & header files generated from the TTCN-3 & ASN.1 modules of
# central project(s):
BASE_GENERATED_SOURCES = ../../ProjA/bin/A.cc
BASE_GENERATED_HEADERS = $(BASE_GENERATED_SOURCES:.cc=.hh)
ifdef SPLIT_TO_SLICES
BASE_GENERATED_SOURCES2 := $(foreach file, $(BASE_GENERATED_SOURCES:.cc=), $(addprefix $(file), $(POSTFIXES)))
BASE_GENERATED_SOURCES += $(GENERATED_SOURCES2)
endif

# C/C++ Source & header files of Test Ports, external functions and
# other modules:
USER_SOURCES = ../src/PortA2.cc ../src/PortB.cc ../src/PortB_RT.cc
USER_HEADERS = $(USER_SOURCES:.cc=.hh)

# C/C++ Source & header files of Test Ports, external functions and
# other modules used from central project(s):
BASE_USER_SOURCES = ../../ProjA/src/PortA.cc ../../ProjA/src/PortA_RT.cc
BASE_USER_HEADERS = $(BASE_USER_SOURCES:.cc=.hh)

# Object files of this project that are needed for the executable test suite:
OBJECTS = $(GENERATED_OBJECTS) $(USER_OBJECTS)

GENERATED_OBJECTS = $(GENERATED_SOURCES:.cc=.o)

USER_OBJECTS = PortA2.o PortB.o PortB_RT.o

# Object files of central project(s) that are needed for the executable test suite:
BASE_OBJECTS = $(BASE_GENERATED_SOURCES:.cc=.o) ../../ProjA/bin/PortA.o ../../ProjA/bin/PortA_RT.o

DEPFILES = $(USER_OBJECTS:.o=.d)  $(GENERATED_OBJECTS:.o=.d)

# Other files of the project (Makefile, configuration files, etc.)
# that will be added to the archived source files:
OTHER_FILES = Makefile ../ProjB.cfg

# The name of the executable test suite:
TARGET = ProjB$(EXESUFFIX)

#
# Rules for building the executable...
#

all: $(TARGET) ;

objects: $(OBJECTS) ;

$(TARGET): $(OBJECTS) $(BASE_OBJECTS)
	$(TTCN3_DIR)/bin/titanver $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ \
	-L$(TTCN3_DIR)/lib -l$(TTCN3_LIB) \
	-L$(OPENSSL_DIR)/lib -lcrypto \
	-L$(XMLDIR)/lib $($(PLATFORM)_LIBS)

../../ProjA/bin/PortA.o:  ../../ProjA/src/PortA.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

PortA2.o:  ../src/PortA2.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

../../ProjA/bin/PortA_RT.o:  ../../ProjA/src/PortA_RT.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

PortB.o:  ../src/PortB.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

PortB_RT.o:  ../src/PortB_RT.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

.cc.o .c.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

../../ProjA/bin/PortA.d :  ../../ProjA/src/PortA.cc
	@echo Creating dependency file for '$<'; set -e; \
	$(CXX) $(CXXDEPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

PortA2.d :  ../src/PortA2.cc
	@echo Creating dependency file for '$<'; set -e; \
	$(CXX) $(CXXDEPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

../../ProjA/bin/PortA_RT.d :  ../../ProjA/src/PortA_RT.cc
	@echo Creating dependency file for '$<'; set -e; \
	$(CXX) $(CXXDEPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

PortB.d :  ../src/PortB.cc
	@echo Creating dependency file for '$<'; set -e; \
	$(CXX) $(CXXDEPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

PortB_RT.d :  ../src/PortB_RT.cc
	@echo Creating dependency file for '$<'; set -e; \
	$(CXX) $(CXXDEPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

.cc.d .c.d:
	@echo Creating dependency file for '$<'; set -e; \
	$(CXX) $(CXXDEPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

$(GENERATED_SOURCES) $(GENERATED_HEADERS): compile-all compile ./compile ../../ProjA/bin/compile
	@if [ ! -f $@ ]; then $(RM) compile-all; $(MAKE) compile-all; fi

compile: $(TTCN3_MODULES) $(ASN1_MODULES)
	$(TTCN3_COMPILER) $(COMPILER_FLAGS) \
	$(TTCN3_MODULES) $(BASE_TTCN3_MODULES) \
	$(ASN1_MODULES) $(BASE_ASN1_MODULES) \
	- $?
	touch $@

compile-all: $(BASE_TTCN3_MODULES) $(BASE_ASN1_MODULES)
	$(TTCN3_COMPILER) $(COMPILER_FLAGS) \
	$(TTCN3_MODULES) $(BASE_TTCN3_MODULES) \
	$(ASN1_MODULES) $(BASE_ASN1_MODULES) \
	- $(TTCN3_MODULES) $(ASN1_MODULES)
	touch $@ compile

clean distclean:
	-$(RM) $(TARGET) $(OBJECTS) $(GENERATED_HEADERS) \
	$(GENERATED_SOURCES) compile compile-all $(DEPFILES) \
	*.log

dep: $(GENERATED_SOURCES) $(USER_SOURCES) ;

ifeq ($(findstring n,$(MAKEFLAGS)),)
ifeq ($(filter clean distclean check compile archive diag,$(MAKECMDGOALS)),)
-include $(DEPFILES)
endif
endif


#
# Add your rules here if necessary...
#
run: $(TARGET) ../ProjB.cfg
	$(TTCN3_DIR)/bin/ttcn3_start $^

