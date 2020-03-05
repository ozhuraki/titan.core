##############################################################################
# Copyright (c) 2000-2020 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   
#   Balasko, Jeno
#   Delic, Adam
#   Hanko, Matyas
#   Kovacs, Ferenc
#   Lovassy, Arpad
#   Ormandi, Matyas
#   Raduly, Csaba
#   Szabados, Kristof
#   Szabo, Janos Zoltan – initial implementation
#   Tarasov, Victor
#
##############################################################################
TOPDIR := ../../../
include $(TOPDIR)/Makefile.regression

.SUFFIXES: .ttcn .asn .hh
.PHONY: all clean dep run

TTCN3_LIB = ttcn3$(RT2_SUFFIX)$(DYNAMIC_SUFFIX)

TTCN3_MODULES = ISOIEC8825_Testcases.ttcn ETSI_TS_103_097_v131_Testcases.ttcn ITSPKI_Testcases.ttcn

ASN1_MODULES = ISOIEC8825.asn ETSITS103097v131.asn ITSPKI.asn IEEE1609dot2BaseTypes.asn IEEE1609dot2.asn EtsiTs103097Module.asn

GENERATED_SOURCES = $(ASN1_MODULES:.asn=.cc) $(TTCN3_MODULES:.ttcn=.cc)

GENERATED_HEADERS = $(GENERATED_SOURCES:.cc=.hh)
ifdef CODE_SPLIT
GENERATED_SOURCES := $(foreach file, $(GENERATED_SOURCES:.cc=), $(addprefix $(file), .cc _seq.cc _set.cc  _seqof.cc _setof.cc _union.cc))
else ifdef SPLIT_TO_SLICES
POSTFIXES := $(foreach file, $(SPLIT_TO_SLICES), $(addsuffix $(file), _part_))
POSTFIXES := $(foreach file, $(POSTFIXES), $(addprefix $(file), .cc))
GENERATED_SOURCES2 := $(foreach file, $(GENERATED_SOURCES:.cc=), $(addprefix $(file), $(POSTFIXES)))
GENERATED_SOURCES += $(GENERATED_SOURCES2)
endif

OBJECTS = $(GENERATED_SOURCES:.cc=.o)

TARGET = OER$(EXESUFFIX)

all: $(TARGET)

$(TARGET): $(GENERATED_SOURCES) $(USER_SOURCES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ -L$(TTCN3_DIR)/lib -l$(TTCN3_LIB) -L$(OPENSSL_DIR)/lib -lcrypto $($(PLATFORM)_LIBS)

$(GENERATED_SOURCES) $(GENERATED_HEADERS): compile

compile: $(TTCN3_MODULES) $(ASN1_MODULES)
	$(TTCN3_COMPILER) $(COMPILER_FLAGS) $^

clean distclean:
	$(RM) $(TARGET) $(GENERATED_HEADERS) $(GENERATED_SOURCES) $(OBJECTS) *.log Makefile.bak compile

dep: $(GENERATED_SOURCES)
	makedepend $(CPPFLAGS) $(GENERATED_SOURCES)

run: $(TARGET)
	./$(TARGET) OER_EncDec.cfg 2> result.txt
	cat result.txt
	grep "Overall verdict: pass" result.txt

