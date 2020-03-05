##############################################################################
# Copyright (c) 2000-2020 Ericsson Telecom AB
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

# ${MAKEPROG} has the same content as the built-in ${MAKE},
# except the special handling of ${MAKE} does not apply.
# If we used ${MAKE} in the rules below, 'make -n' would succeed once,
# then fail on every subsequent invocation until a 'make clean' is done. 

MAKEPROG := ${MAKE}

all: CheckTpdTrue CheckTpdFalse CheckTpdMissing

BuildTpdTrue:
	$(TTCN3_DIR)/bin/ttcn3_makefilegen -f $(MFGEN_FLAGS) $(COVERAGE_FLAG) \
	-t a.tpd

CheckTpdTrue: BuildTpdTrue
	if [ ! -f "./bin/Makefile" ] \
	|| [ `grep -c "COMPILER_FLAGS = -L  -O" ./bin/Makefile` -ne 1 ]; \
	then echo "Makefilegen enable_legacy_encoding test: Overall verdict: fail" && exit 1; fi

BuildTpdFalse:
	$(TTCN3_DIR)/bin/ttcn3_makefilegen -f $(MFGEN_FLAGS) $(COVERAGE_FLAG) \
	-t a.tpd -b False

CheckTpdFalse: BuildTpdFalse
	if [ ! -f "./bin2/Makefile" ] \
	|| [ `grep -c "COMPILER_FLAGS = -L  -O" ./bin2/Makefile` -ne 0 ]; \
	then echo "Makefilegen enable_legacy_encoding test: Overall verdict: fail" && exit 1; fi

BuildTpdMissing:
	$(TTCN3_DIR)/bin/ttcn3_makefilegen -f $(MFGEN_FLAGS) $(COVERAGE_FLAG) \
	-t a.tpd -b Missing

CheckTpdMissing: BuildTpdMissing
	if [ ! -f "./bin3/Makefile" ] \
	|| [ `grep -c "COMPILER_FLAGS = -L  -O" ./bin3/Makefile` -ne 0 ]; \
	then echo "Makefilegen enable_legacy_encoding test: Overall verdict: fail" && exit 1; fi

clean:
	-rm -rf bin bin2 bin3

distclean: clean
	-rm -f *.out

.PHONY: all clean distclean CheckTpdTrue CheckTpdFalse CheckTpdMissing BuildTpdTrue BuildTpdFalse BuildTpdMissing

