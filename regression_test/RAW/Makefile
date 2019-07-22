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
TOPDIR := ..
include $(TOPDIR)/Makefile.regression

RDIRS = Examples HN25015 HQ26535 HQ49956 HS16977 ustr Annex_E_variants Bug521125 \
        Lengthto_Offset Bug522656 RAW_integer RAW_bitstring ForceOmit Bug546231 \
        Bug547385 IntX CSN1_LH

all dep clean run distclean:
	for dir in $(RDIRS); do (cd $$dir && $(MAKE) $@) || exit; done

