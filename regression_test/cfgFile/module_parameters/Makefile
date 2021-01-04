##############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Baranyi, Botond
#   Beres, Szabolcs
#
##############################################################################
TOPDIR := ../../
include $(TOPDIR)/Makefile.regression
unexport ABS_SRC
unexport SRCDIR

DIRS := assignment concat special_values
ifdef RT2
DIRS += references nested
endif

# List of fake targets:
.PHONY: all dep clean run $(DIRS) $(addsuffix /, $(DIRS)) profile

all dep clean distclean run:
	for dir in $(DIRS); do $(MAKE) -C $$dir $@ || exit; done

# To run just one test, type "make dir" or "make "dir/"
$(DIRS) $(addsuffix /, $(DIRS)):
	$(MAKE) -C $@ run

# To run all tests, possibly in parallel
runall: $(DIRS)


