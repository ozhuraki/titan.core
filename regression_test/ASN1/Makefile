##############################################################################
# Copyright (c) 2000-2019 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Forstner, Matyas
#   Koppany, Csaba
#   Kovacs, Ferenc
#   Kulcsar, Endre
#   Raduly, Csaba
#   Szabo, Janos Zoltan – initial implementation
#
##############################################################################
TOPDIR := ..
include $(TOPDIR)/Makefile.regression

ADIRS = parse errorMessages errorMessages2 transformations keyword hyphen enum1 enum2 codeGeneration2 Test38 Test303 Test307 Test308 Test309 Test310 Test338 Test340 Test342 Test344 Test346 Test348 Test350 Test352 Test354 Test356 Test358 Test360 namedBits

#still not OK: Test330 Test332

all dep clean run distclean:
	for dir in $(ADIRS); do $(MAKE) -C $$dir $@ || exit; done
