##############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Baranyi, Botond
#   Lovassy, Arpad
#   Szabados, Kristof
#
##############################################################################
TOPDIR := ../..
include $(TOPDIR)/Makefile.regression

.PHONY: all clean dep run

TTCN3_LIB = ttcn3$(RT2_SUFFIX)$(DYNAMIC_SUFFIX)

TTCN3_MODULES = XER_EncDec_1.ttcn XER_EncDec_10.ttcn XER_EncDec_11.ttcn XER_EncDec_12.ttcn XER_EncDec_13.ttcn XER_EncDec_14.ttcn XER_EncDec_15.ttcn XER_EncDec_16.ttcn XER_EncDec_17.ttcn XER_EncDec_18.ttcn XER_EncDec_19.ttcn XER_EncDec_2.ttcn XER_EncDec_20.ttcn XER_EncDec_21.ttcn XER_EncDec_22.ttcn XER_EncDec_23.ttcn XER_EncDec_24.ttcn XER_EncDec_25.ttcn XER_EncDec_26.ttcn XER_EncDec_27.ttcn XER_EncDec_28.ttcn XER_EncDec_29.ttcn XER_EncDec_3.ttcn XER_EncDec_30.ttcn XER_EncDec_4.ttcn XER_EncDec_5.ttcn XER_EncDec_6.ttcn XER_EncDec_7.ttcn XER_EncDec_8.ttcn XER_EncDec_9.ttcn

GENERATED_SOURCES = $(TTCN3_MODULES:.ttcn=.cc)
GENERATED_HEADERS = $(GENERATED_SOURCES:.cc=.hh)
ifdef CODE_SPLIT
GENERATED_SOURCES := $(foreach file, $(GENERATED_SOURCES:.cc=), $(addprefix $(file), .cc _seq.cc _set.cc  _seqof.cc _setof.cc _union.cc))
else ifdef SPLIT_TO_SLICES
POSTFIXES := $(foreach file, $(SPLIT_TO_SLICES), $(addsuffix $(file), _part_))
POSTFIXES := $(foreach file, $(POSTFIXES), $(addprefix $(file), .cc))
GENERATED_SOURCES2 := $(foreach file, $(GENERATED_SOURCES:.cc=), $(addprefix $(file), $(POSTFIXES)))
GENERATED_SOURCES += $(GENERATED_SOURCES2)
endif

USER_SOURCES =

# All object files needed for the executable test suite:
OBJECTS = $(GENERATED_SOURCES:.cc=.o) $(USER_SOURCES:.cc=.o)

# The name of the executable test suite:
TARGET = XER_EncDec$(EXESUFFIX)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) -L$(TTCN3_DIR)/lib -l$(TTCN3_LIB) \
	-L$(OPENSSL_DIR)/lib -lcrypto $($(PLATFORM)_LIBS)

$(GENERATED_SOURCES) $(GENERATED_HEADERS): compile
	@if [ ! -f $@ ]; then $(RM) compile; $(MAKE) compile; fi

compile: $(TTCN3_MODULES) $(ASN1_MODULES)
	$(TTCN3_COMPILER) $(COMPILER_FLAGS) $^
	touch $@

clean distclean:
	$(RM) $(TARGET) $(OBJECTS) $(GENERATED_HEADERS) \
	$(GENERATED_SOURCES) compile *.log

dep: $(GENERATED_SOURCES)
	makedepend $(CPPFLAGS) $(USER_SOURCES) $(GENERATED_SOURCES)

run: $(TARGET) XER_EncDec.cfg
	./$^ 2> result.txt
	cat result.txt
	grep "Overall verdict: pass" result.txt

# DO NOT DELETE
