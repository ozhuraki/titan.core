##############################################################################
# Copyright (c) 2000-2020 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Delic, Adam
#   Forstner, Matyas
#   Koppany, Csaba
#   Kovacs, Ferenc
#   Raduly, Csaba
#   Szabo, Bence Janos
#   Szabo, Janos Zoltan – initial implementation
#
##############################################################################
TOPDIR := ../..
include $(TOPDIR)/Makefile.regression

.SUFFIXES: .ttcn .asn .hh
.PHONY: all clean dep run

TTCN3_LIB = ttcn3$(RT2_SUFFIX)$(DYNAMIC_SUFFIX)

TTCN3_MODULES = BERTest.ttcn X682_Test.ttcn X682_simplified_Test.ttcn
TTCN3_MODULES += BER_EncDec_1.ttcn BER_EncDec_10.ttcn BER_EncDec_100.ttcn BER_EncDec_101.ttcn BER_EncDec_102.ttcn BER_EncDec_103.ttcn BER_EncDec_104.ttcn BER_EncDec_105.ttcn BER_EncDec_106.ttcn BER_EncDec_107.ttcn BER_EncDec_108.ttcn BER_EncDec_109.ttcn BER_EncDec_11.ttcn BER_EncDec_110.ttcn BER_EncDec_111.ttcn BER_EncDec_112.ttcn BER_EncDec_113.ttcn BER_EncDec_114.ttcn BER_EncDec_115.ttcn BER_EncDec_116.ttcn BER_EncDec_117.ttcn BER_EncDec_118.ttcn BER_EncDec_119.ttcn BER_EncDec_12.ttcn BER_EncDec_120.ttcn BER_EncDec_121.ttcn BER_EncDec_122.ttcn BER_EncDec_123.ttcn BER_EncDec_124.ttcn BER_EncDec_125.ttcn BER_EncDec_126.ttcn BER_EncDec_127.ttcn BER_EncDec_128.ttcn BER_EncDec_129.ttcn BER_EncDec_13.ttcn BER_EncDec_130.ttcn BER_EncDec_131.ttcn BER_EncDec_132.ttcn BER_EncDec_133.ttcn BER_EncDec_134.ttcn BER_EncDec_135.ttcn BER_EncDec_136.ttcn BER_EncDec_137.ttcn BER_EncDec_138.ttcn BER_EncDec_139.ttcn BER_EncDec_14.ttcn BER_EncDec_140.ttcn BER_EncDec_141.ttcn BER_EncDec_142.ttcn BER_EncDec_143.ttcn BER_EncDec_144.ttcn BER_EncDec_145.ttcn BER_EncDec_146.ttcn BER_EncDec_147.ttcn BER_EncDec_148.ttcn BER_EncDec_149.ttcn BER_EncDec_15.ttcn BER_EncDec_150.ttcn BER_EncDec_151.ttcn BER_EncDec_152.ttcn BER_EncDec_153.ttcn BER_EncDec_154.ttcn BER_EncDec_155.ttcn BER_EncDec_156.ttcn BER_EncDec_157.ttcn BER_EncDec_158.ttcn BER_EncDec_159.ttcn BER_EncDec_16.ttcn BER_EncDec_160.ttcn BER_EncDec_161.ttcn BER_EncDec_162.ttcn BER_EncDec_163.ttcn BER_EncDec_164.ttcn BER_EncDec_165.ttcn BER_EncDec_17.ttcn BER_EncDec_18.ttcn BER_EncDec_19.ttcn BER_EncDec_2.ttcn BER_EncDec_20.ttcn BER_EncDec_21.ttcn BER_EncDec_22.ttcn BER_EncDec_23.ttcn BER_EncDec_24.ttcn BER_EncDec_25.ttcn BER_EncDec_26.ttcn BER_EncDec_27.ttcn BER_EncDec_28.ttcn BER_EncDec_29.ttcn BER_EncDec_3.ttcn BER_EncDec_30.ttcn BER_EncDec_31.ttcn BER_EncDec_32.ttcn BER_EncDec_33.ttcn BER_EncDec_34.ttcn BER_EncDec_35.ttcn BER_EncDec_36.ttcn BER_EncDec_37.ttcn BER_EncDec_38.ttcn BER_EncDec_39.ttcn BER_EncDec_4.ttcn BER_EncDec_40.ttcn BER_EncDec_41.ttcn BER_EncDec_42.ttcn BER_EncDec_43.ttcn BER_EncDec_44.ttcn BER_EncDec_45.ttcn BER_EncDec_46.ttcn BER_EncDec_47.ttcn BER_EncDec_48.ttcn BER_EncDec_49.ttcn BER_EncDec_5.ttcn BER_EncDec_50.ttcn BER_EncDec_51.ttcn BER_EncDec_52.ttcn BER_EncDec_53.ttcn BER_EncDec_54.ttcn BER_EncDec_55.ttcn BER_EncDec_56.ttcn BER_EncDec_57.ttcn BER_EncDec_58.ttcn BER_EncDec_59.ttcn BER_EncDec_6.ttcn BER_EncDec_60.ttcn BER_EncDec_61.ttcn BER_EncDec_62.ttcn BER_EncDec_63.ttcn BER_EncDec_64.ttcn BER_EncDec_65.ttcn BER_EncDec_66.ttcn BER_EncDec_67.ttcn BER_EncDec_68.ttcn BER_EncDec_69.ttcn BER_EncDec_7.ttcn BER_EncDec_70.ttcn BER_EncDec_71.ttcn BER_EncDec_72.ttcn BER_EncDec_73.ttcn BER_EncDec_74.ttcn BER_EncDec_75.ttcn BER_EncDec_76.ttcn BER_EncDec_77.ttcn BER_EncDec_78.ttcn BER_EncDec_79.ttcn BER_EncDec_8.ttcn BER_EncDec_80.ttcn BER_EncDec_81.ttcn BER_EncDec_82.ttcn BER_EncDec_83.ttcn BER_EncDec_84.ttcn BER_EncDec_85.ttcn BER_EncDec_86.ttcn BER_EncDec_87.ttcn BER_EncDec_88.ttcn BER_EncDec_89.ttcn BER_EncDec_9.ttcn BER_EncDec_90.ttcn BER_EncDec_91.ttcn BER_EncDec_92.ttcn BER_EncDec_93.ttcn BER_EncDec_94.ttcn BER_EncDec_95.ttcn BER_EncDec_96.ttcn BER_EncDec_97.ttcn BER_EncDec_98.ttcn BER_EncDec_99.ttcn

ASN1_MODULES = BER1.asn BER2.asn X682.asn X682_simplified.asn
ASN1_MODULES += BER_EncDecA_1.asn BER_EncDecA_10.asn BER_EncDecA_100.asn BER_EncDecA_101.asn BER_EncDecA_102.asn BER_EncDecA_103.asn BER_EncDecA_104.asn BER_EncDecA_105.asn BER_EncDecA_106.asn BER_EncDecA_107.asn BER_EncDecA_108.asn BER_EncDecA_109.asn BER_EncDecA_11.asn BER_EncDecA_110.asn BER_EncDecA_111.asn BER_EncDecA_112.asn BER_EncDecA_113.asn BER_EncDecA_114.asn BER_EncDecA_115.asn BER_EncDecA_116.asn BER_EncDecA_117.asn BER_EncDecA_118.asn BER_EncDecA_119.asn BER_EncDecA_12.asn BER_EncDecA_120.asn BER_EncDecA_121.asn BER_EncDecA_122.asn BER_EncDecA_123.asn BER_EncDecA_124.asn BER_EncDecA_125.asn BER_EncDecA_126.asn BER_EncDecA_127.asn BER_EncDecA_128.asn BER_EncDecA_129.asn BER_EncDecA_13.asn BER_EncDecA_130.asn BER_EncDecA_131.asn BER_EncDecA_132.asn BER_EncDecA_133.asn BER_EncDecA_134.asn BER_EncDecA_135.asn BER_EncDecA_136.asn BER_EncDecA_137.asn BER_EncDecA_138.asn BER_EncDecA_139.asn BER_EncDecA_14.asn BER_EncDecA_140.asn BER_EncDecA_141.asn BER_EncDecA_142.asn BER_EncDecA_143.asn BER_EncDecA_144.asn BER_EncDecA_145.asn BER_EncDecA_146.asn BER_EncDecA_147.asn BER_EncDecA_148.asn BER_EncDecA_149.asn BER_EncDecA_15.asn BER_EncDecA_150.asn BER_EncDecA_151.asn BER_EncDecA_152.asn BER_EncDecA_153.asn BER_EncDecA_154.asn BER_EncDecA_155.asn BER_EncDecA_156.asn BER_EncDecA_157.asn BER_EncDecA_158.asn BER_EncDecA_159.asn BER_EncDecA_16.asn BER_EncDecA_160.asn BER_EncDecA_161.asn BER_EncDecA_162.asn BER_EncDecA_163.asn BER_EncDecA_164.asn BER_EncDecA_165.asn BER_EncDecA_17.asn BER_EncDecA_18.asn BER_EncDecA_19.asn BER_EncDecA_2.asn BER_EncDecA_20.asn BER_EncDecA_21.asn BER_EncDecA_22.asn BER_EncDecA_23.asn BER_EncDecA_24.asn BER_EncDecA_25.asn BER_EncDecA_26.asn BER_EncDecA_27.asn BER_EncDecA_28.asn BER_EncDecA_29.asn BER_EncDecA_3.asn BER_EncDecA_30.asn BER_EncDecA_31.asn BER_EncDecA_32.asn BER_EncDecA_33.asn BER_EncDecA_34.asn BER_EncDecA_35.asn BER_EncDecA_36.asn BER_EncDecA_37.asn BER_EncDecA_38.asn BER_EncDecA_39.asn BER_EncDecA_4.asn BER_EncDecA_40.asn BER_EncDecA_41.asn BER_EncDecA_42.asn BER_EncDecA_43.asn BER_EncDecA_44.asn BER_EncDecA_45.asn BER_EncDecA_46.asn BER_EncDecA_47.asn BER_EncDecA_48.asn BER_EncDecA_49.asn BER_EncDecA_5.asn BER_EncDecA_50.asn BER_EncDecA_51.asn BER_EncDecA_52.asn BER_EncDecA_53.asn BER_EncDecA_54.asn BER_EncDecA_55.asn BER_EncDecA_56.asn BER_EncDecA_57.asn BER_EncDecA_58.asn BER_EncDecA_59.asn BER_EncDecA_6.asn BER_EncDecA_60.asn BER_EncDecA_61.asn BER_EncDecA_62.asn BER_EncDecA_63.asn BER_EncDecA_64.asn BER_EncDecA_65.asn BER_EncDecA_66.asn BER_EncDecA_67.asn BER_EncDecA_68.asn BER_EncDecA_69.asn BER_EncDecA_7.asn BER_EncDecA_70.asn BER_EncDecA_71.asn BER_EncDecA_72.asn BER_EncDecA_73.asn BER_EncDecA_74.asn BER_EncDecA_75.asn BER_EncDecA_76.asn BER_EncDecA_77.asn BER_EncDecA_78.asn BER_EncDecA_79.asn BER_EncDecA_8.asn BER_EncDecA_80.asn BER_EncDecA_81.asn BER_EncDecA_82.asn BER_EncDecA_83.asn BER_EncDecA_84.asn BER_EncDecA_85.asn BER_EncDecA_86.asn BER_EncDecA_87.asn BER_EncDecA_88.asn BER_EncDecA_89.asn BER_EncDecA_9.asn BER_EncDecA_90.asn BER_EncDecA_91.asn BER_EncDecA_92.asn BER_EncDecA_93.asn BER_EncDecA_94.asn BER_EncDecA_95.asn BER_EncDecA_96.asn BER_EncDecA_97.asn BER_EncDecA_98.asn BER_EncDecA_99.asn

GENERATED_SOURCES = $(TTCN3_MODULES:.ttcn=.cc) $(ASN1_MODULES:.asn=.cc)
GENERATED_HEADERS = $(TTCN3_MODULES:.ttcn=.hh) $(ASN1_MODULES:.asn=.hh)
ifdef CODE_SPLIT
GENERATED_SOURCES := $(foreach file, $(GENERATED_SOURCES:.cc=), $(addprefix $(file), .cc _seq.cc _set.cc  _seqof.cc _setof.cc _union.cc))
else ifdef SPLIT_TO_SLICES
POSTFIXES := $(foreach file, $(SPLIT_TO_SLICES), $(addsuffix $(file), _part_))
POSTFIXES := $(foreach file, $(POSTFIXES), $(addprefix $(file), .cc))
GENERATED_SOURCES2 := $(foreach file, $(GENERATED_SOURCES:.cc=), $(addprefix $(file), $(POSTFIXES)))
GENERATED_SOURCES += $(GENERATED_SOURCES2)
endif

# Source & header files of Test Ports and your other modules:
USER_SOURCES =
USER_HEADERS =

OBJECTS = $(GENERATED_SOURCES:.cc=.o) $(USER_SOURCES:.cc=.o)

TARGET = BER$(EXESUFFIX)

all: $(TARGET)

$(TARGET): $(GENERATED_SOURCES) $(USER_SOURCES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ -L$(TTCN3_DIR)/lib -l$(TTCN3_LIB) -L$(OPENSSL_DIR)/lib -lcrypto $($(PLATFORM)_LIBS)

$(GENERATED_SOURCES) $(GENERATED_HEADERS): $(TTCN3_MODULES) $(ASN1_MODULES)
	$(TTCN3_COMPILER) $^

clean distclean:
	-rm -f $(TARGET) $(OBJECTS) $(GENERATED_HEADERS) \
	$(GENERATED_SOURCES) *.log *~ Makefile.bak core

dep: $(GENERATED_SOURCES)
	makedepend $(CPPFLAGS) $(USER_SOURCES) $(GENERATED_SOURCES)

run: $(TARGET) BER.cfg
	./$^ 2> result.txt
	cat result.txt
	grep "Overall verdict: pass" result.txt

ifdef SRCDIR
$(foreach src, $(USER_SOURCES), $(eval vpath $(src) $(ABS_SRC)))
endif

