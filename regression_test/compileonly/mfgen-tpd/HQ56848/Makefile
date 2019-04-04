##############################################################################
# Copyright (c) 2000-2019 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Beres, Szabolcs
#   Raduly, Csaba
#
##############################################################################
TOPDIR := ../../..
include $(TOPDIR)/Makefile.regression

all:
	@echo Howdy!
	-mkdir -p bin
	$(TTCN3_DIR)/bin/ttcn3_makefilegen -f -t counter.tpd && \
	if [ -h bin/counter.ttcn ] ; then \
	  echo success ; \
	else \
	  echo Symlink bin/counter.ttcn does not exist; exit 1; \
	fi ; \

clean distclean:
	$(RM) bin/*
