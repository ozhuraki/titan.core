##############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Baji, Laszlo
#   Balasko, Jeno
#   Baranyi, Botond
#   Ormandi, Matyas
#   Pilisi, Gergely
#   Raduly, Csaba
#   Szabo, Bence Janos
#
##############################################################################
TOPDIR := ..
include   ../Makefile.regression
unexport ABS_SRC
unexport SRCDIR

ifdef RT2
RT2_ONLY := NegativeTest
SHADOWED := XER  TTCNandXML  EXER-whitepaper  XMLqualif
else
SHADOWED := XER* TTCNandXML* EXER-whitepaper* XMLqualif*
endif

XDIRS := $(wildcard $(SHADOWED)) xsdConverter \
HM60295 HN15589 HQ30408 HR49727 HU13380 $(RT2_ONLY) \
XmlWorkflow tpdValidTest AbstractBlock UseNilLong AttributeFormDefault \
RecordOmit XSDBaseType LegacyUntaggedUnion Printing Bug570707

# List of fake targets:
.PHONY: all dep clean run $(XDIRS) $(addsuffix /, $(XDIRS)) profile

all dep clean distclean:
	for dir in $(XDIRS); do $(MAKE) -C $$dir $@ || exit; done

run:
	for dir in $(XDIRS); do $(MAKE) -C $$dir $@ || exit; done

# To run just one test, type "make dir" or "make "dir/"
$(XDIRS) $(addsuffix /, $(XDIRS)):
	$(MAKE) -C $@ run

# To run all tests, possibly in parallel
runall: $(XDIRS)


