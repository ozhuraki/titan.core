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
TOPDIR := ../../..
include $(TOPDIR)/Makefile.regression

ifdef LCOV
COVERAGE_FLAG := -C
endif


MAKEPROG := ${MAKE}

all: Recursive RecursiveAbsolute

Recursive:
	-rm -rf a/binDefault b/binDefault c/binDefault && \
	cd c && $(TTCN3_DIR)/bin/ttcn3_makefilegen $(MFGEN_FLAGS) $(COVERAGE_FLAG) \
	-t c.tpd -cVfgpl && cd binDefault && make shared_objects && \
	cd ../../b && $(TTCN3_DIR)/bin/ttcn3_makefilegen $(MFGEN_FLAGS) $(COVERAGE_FLAG) \
	-t b.tpd -cVfgpl && cd binDefault && make shared_objects && \
	cd ../../a && $(TTCN3_DIR)/bin/ttcn3_makefilegen $(MFGEN_FLAGS) $(COVERAGE_FLAG) \
	-t a.tpd -FTrcgpl && cd binDefault && make > compile.txt && \
	if [ `grep -c "ModuleB\." compile.txt` -ne 4 ] || [ `grep -c "ModuleC\." compile.txt` -ne 5 ]; \
	then echo "Makefilegen compile once test failed: Overall verdict: fail" && exit 1; fi

RecursiveAbsolute:
	-rm -rf a/binDefault b/binDefault c/binDefault && \
	cd c && $(TTCN3_DIR)/bin/ttcn3_makefilegen $(MFGEN_FLAGS) $(COVERAGE_FLAG) \
	-at c.tpd -cVfgpl && cd binDefault && make shared_objects && \
	cd ../../b && $(TTCN3_DIR)/bin/ttcn3_makefilegen $(MFGEN_FLAGS) $(COVERAGE_FLAG) \
	-at b.tpd -cVfgpl && cd binDefault && make shared_objects && \
	cd ../../a && $(TTCN3_DIR)/bin/ttcn3_makefilegen $(MFGEN_FLAGS) $(COVERAGE_FLAG) \
	-at a.tpd -FTrcgpl && cd binDefault && make > compile.txt && \
	if [ `grep -c "ModuleB\." compile.txt` -ne 4 ] || [ `grep -c "ModuleC\." compile.txt` -ne 5 ]; \
	then echo "Makefilegen compile once test failed: Overall verdict: fail" && exit 1; fi

clean:
	-rm -rf a/binDefault b/binDefault c/binDefault

distclean: clean
	-rm -f *.out

.PHONY: all clean distclean Normal Recursive RecursiveAbsolute

