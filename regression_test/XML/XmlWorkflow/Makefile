##############################################################################
# Copyright (c) 2000-2019 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Raduly, Csaba
#   Szabo, Bence Janos
#
##############################################################################
TOPDIR := ../..
include   ../../Makefile.regression

all: all1 all2

run: run1 run2

clean: clean1 clean2

distclean: distclean1 distclean2

# for general XML tests
all1: bin/Makefile
	$(MAKE) -C bin all

run1: bin/Makefile
	$(MAKE) -C bin run

clean1:
	if [ -f bin/Makefile ] ; then $(MAKE) -C bin clean ; else true; fi

distclean1: clean1
	find bin -type l | xargs rm -f
	rm -f bin/Makefile bin/*.ttcn bin/core

# for tests from protocol modules
all2: bin2/Makefile
	$(MAKE) -C bin2 all

run2: bin2/Makefile
	$(MAKE) -C bin2 run

clean2:
	if [ -f bin2/Makefile ] ; then $(MAKE) -C bin2 clean ; else true; fi

distclean2: clean2
	find bin2 -type l | xargs rm -f
	rm -f bin2/Makefile bin2/*.ttcn bin2/*.ttcn bin2/core


bin/Makefile: $(ABS_SRC)/bin/prj2mk.pl $(ABS_SRC)/src/xmlTest.prj
	+cd bin && perl $^

bin2/Makefile: $(ABS_SRC)/bin/prj2mk.pl $(ABS_SRC)/src/xmlTest_protocolXsds.prj
	+cd bin2 && perl $^

.PHONY: all all1 all2 run run1 run2 clean clean1 clean2 distclean distclean1 distclean2

.NOTPARALLEL:
