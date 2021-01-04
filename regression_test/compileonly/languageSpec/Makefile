##############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Szabo, Bence Janos
#
##############################################################################
TOPDIR := ../..
include $(TOPDIR)/Makefile.regression

ifdef LCOV
COVERAGE_FLAG := -C
endif


MAKEPROG := ${MAKE}

all: 
	rm -rf bin && mkdir -p bin && cd bin && $(TTCN3_COMPILER) -t ../*.ttcn && $(TTCN3_DIR)/bin/ttcn3_makefilegen -f ../*.ttcn && make


clean:
	-rm -rf bin

distclean: clean
	-rm -f *.out

.PHONY: all clean distclean Normal Recursive RecursiveDynamic

