##############################################################################
# Copyright (c) 2000-2019 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Baranyi, Botond
#   Hanko, Matyas
#   Kovacs, Ferenc
#   Raduly, Csaba
#   Szabo, Bence Janos
#   Szabo, Janos Zoltan – initial implementation
#
##############################################################################
TOPDIR := ../../
include $(TOPDIR)/Makefile.regression

RDIRS = JsonBasicTest  JsonComplexTest JsonModule

all dep clean run distclean:
	for dir in $(RDIRS); do (cd $$dir && $(MAKE) $@) || exit; done

