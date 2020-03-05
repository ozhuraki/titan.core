##############################################################################
# Copyright (c) 2000-2020 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Baranyi, Botond
#
##############################################################################
TOPDIR := ..
include ../Makefile.regression

.PHONY: all run clean distclean dep

all clean distclean dep run:
	$(MAKE) -C ProjA/bin $@
	$(MAKE) -C ProjB/bin $@

