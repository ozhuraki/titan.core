##############################################################################
# Copyright (c) 2000-2020 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Delic, Adam
#   Raduly, Csaba
#   Susanszky, Eszter
#
##############################################################################
TOP    := ..
include   ../Makefile.cfg

DIRS := \
JUnitLogger TSTLogger JUnitLogger2

ifeq ($(LTTNGUSTLOGGER),yes)
	DIRS += LTTngUSTLogger
endif

all run dep clean distclean install:
	for d in $(DIRS); do $(MAKE) -C $$d $@ || exit 1; done
