##############################################################################
# Copyright (c) 2000-2019 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Baji, Laszlo
#   Balasko, Jeno
#   Baranyi, Botond
#   Forstner, Matyas
#   Kovacs, Ferenc
#   Raduly, Csaba
#   Szabados, Kristof
#   Szabo, Bence Janos
#   Szabo, Janos Zoltan – initial implementation
#
##############################################################################
TOPDIR := ..
include $(TOPDIR)/Makefile.regression

CODIRS := dynamicTemplate styleGuide topLevelPdu \
	circularImport circularImport2 typeInstantiation \
	compareImported compoundif \
	centralstorage mfgen-tpd \
	openType optionalAssignCompare portConstructor \
	isbound namedActualParameters assignmentNotation \
	attribQualif HT48786 selectCase openTypeNames \
	defaultParamUsageBeforeDecl deterministic readFromFile \
	asn1_hyphen nameClash portTranslation UntaggedOnTopLevelUnionSwitch \
	typeDescrGenWIthNoXER languageSpec asn1_opentype openTypeXER \
	asnStringSubtypes componentParameter realtimeKeywords

ifdef DYN
CODIRS +=  mfgen-xsd
endif

ifdef RT2
CODIRS += typeCompat Bug544978
endif

all dep clean distclean:
	for dir in $(CODIRS); do $(MAKE) -C $$dir $@ || exit; done
