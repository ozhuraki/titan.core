##############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Delic, Adam
#   Forstner, Matyas
#   Kovacs, Ferenc
#   Lovassy, Arpad
#   Raduly, Csaba
#   Szabo, Bence Janos
#   Szabo, Janos Zoltan – initial implementation
#
##############################################################################
# Makefile for installing the "Hello World!" example.

TOP := $(shell cd .. && pwd)

include ../Makefile.cfg

DEMOFILES := MyExample.ttcn PCOType.hh PCOType.cc hello_world.ttcn MyExample.cfg MyExample2.cfg

MAKEFILEGENFLAGS := -g
ifeq ($(COVERAGE), yes)
MAKEFILEGENFLAGS += -C
endif

install:
ifdef MINGW
	@echo Skipped ${CURDIR} for MinGW
else
	mkdir -p $(DEMODIR)
	cp $(DEMOFILES) $(DEMODIR)
	cd $(DEMODIR) && $(BINDIR)/ttcn3_makefilegen $(MAKEFILEGENFLAGS) $(DEMOFILES) && \
	$(MAKE) CXX='$(CXX)' CXXFLAGS='$(CXXFLAGS)' LDFLAGS='$(LDFLAGS)' && \
	(sleep 5 && echo 'Hello, TTCN-3!') | ttcn3_start -ip "127.0.0.1" MyExample MyExample.cfg && \
	logformat -o MyExample_merged.log *.log
	cd $(DEMODIR) && make clean
endif
