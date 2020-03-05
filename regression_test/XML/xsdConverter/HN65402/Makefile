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

TARGET := jellystone$(EXESUFFIX)

XML_SCHEMAS := jellystone.xsd

GENERATED_TTCN3_MODULES := ttcn_ericsson_se_xsd2ttcn_HN65402.ttcn \
XSD.ttcn UsefulTtcn3Types.ttcn

USER_TTCN3_MODULES := jellystone.ttcn

include ../Makefile.converter

run: $(TARGET)
