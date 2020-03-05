##############################################################################
# Copyright (c) 2000-2020 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Raduly, Csaba
#
##############################################################################
TOPDIR := ../../..
include $(TOPDIR)/Makefile.regression

#
# Makefilegen is supposed to fail
#
all:
	cd bin && \
	if $(TTCN3_DIR)/bin/ttcn3_makefilegen -t ../counter.tpd > output.txt 2>&1 ; \
	then echo makefilegen was supposed to fail!; exit 1; \
	else	if [ -h counter.ttcn ]; then echo symlink should not be created; exit 1; \
		else echo OK; fi  ; \
	fi

clean distclean:
	$(RM) bin/output.txt
