##############################################################################
# Copyright (c) 2000-2020 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Baranyi, Botond
#
##############################################################################
TOPDIR := ..
include   ../Makefile.regression
unexport ABS_SRC
unexport SRCDIR

# List of fake targets:
.PHONY: all dep clean run Common NewTests LegacyTests /Common /NewTests /LegacyTests profile

all dep clean distclean run:
	$(MAKE) -C NewTests $@
	$(MAKE) -C LegacyTests $@

new:
	$(MAKE) -C NewTests run
	
legacy:
	$(MAKE) -C LegacyTests run
