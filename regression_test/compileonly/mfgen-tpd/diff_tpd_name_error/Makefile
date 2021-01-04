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
TOPDIR := ../../../..
ifdef LCOV
COVERAGE_FLAG := -C
endif

# ${MAKEPROG} has the same content as the built-in ${MAKE},
# except the special handling of ${MAKE} does not apply.
# If we used ${MAKE} in the rules below, 'make -n' would succeed once,
# then fail on every subsequent invocation until a 'make clean' is done. 


MAKEPROG := ${MAKE}

all: CheckTpd 

BuildTpd:
	$(TTCN3_DIR)/bin/ttcn3_makefilegen -f $(MFGEN_FLAGS) $(COVERAGE_FLAG) \
	-t a.tpd -r -c -W -g 2>&1 | tee output

CheckTpd: BuildTpd
	if [ `grep -c "c.tpd's name attribute should be the same as the referenced project's name" ./output` -ne 2 ]; \
	then echo "Makefilegen different tpd name in referenced project test failed: Overall verdict: fail" && exit 1; fi
	if [ `grep -c "b.tpd's name attribute should be the same as the referenced project's name" ./output` -ne 1 ]; \
	then echo "Makefilegen different tpd name in referenced project test failed: Overall verdict: fail" && exit 1; fi
	if [ `grep -c "a.tpd's name attribute should be the same as the referenced project's name" ./output` -ne 1 ]; \
	then echo "Makefilegen different tpd name in referenced project test failed: Overall verdict: fail" && exit 1; fi

clean:
	-rm -rf output a_binDefault b_binDefault c_binDefault

distclean: clean
	-rm -f *.out

.PHONY: all clean distclean BuildTpd CheckTpd

