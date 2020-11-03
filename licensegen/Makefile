##############################################################################
# Copyright (c) 2000-2020 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Forstner, Matyas
#   Gecse, Roland
#   Raduly, Csaba
#   Szabo, Janos Zoltan – initial implementation
#
##############################################################################
# Makefile for the license generator program

TOP := ..
include ../Makefile.cfg


#valid on on $TITAN7, RHEL7 :
MYSQL_DIR := /app/vbuild/RHEL7-x86_64/mysql/5.5.30/
#valid on on $TITAN7, RHEL7 :
OPENSSL_DIR := /local/home/titanrt/tools/openssl
TARGET_DIR := /proj/ttcn3titan/license

CPPFLAGS += -I$(OPENSSL_DIR)/include -I$(MYSQL_DIR)/include

SOLARIS_LIBS := -lsocket -lnsl -lm -lz
SOLARIS8_LIBS := -lsocket -lnsl -lm -lz
LINUX_LIBS := -lpthread -lz -lm -lrt -ldl #-lnsl
FREEBSD_LIBS :=
WIN32_LIBS :=

SOURCES := license_gen.c
DEPFILES := $(patsubst %.cc,%.d,$(patsubst %.c,%.d,$(SOURCES)))
COMMON_OBJECTS := ../common/license.o ../common/memory.o
OBJECTS := $(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(SOURCES))) $(COMMON_OBJECTS)
TARGETS := license_gen$(EXESUFFIX)

license_gen$(EXESUFFIX): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -L$(MYSQL_DIR)/lib -lmysqlclient \
	$(OPENSSL_DIR)/lib/libcrypto.a $($(PLATFORM)_LIBS)
	chmod g+w $(TARGETS)

install: $(TARGETS)
	strip $(TARGETS)
	cp $(TARGETS) $(TARGET_DIR)
	chmod 710 $(TARGET_DIR)/license_gen$(EXESUFFIX)
	chmod u+s $(TARGET_DIR)/license_gen$(EXESUFFIX)

include ../Makefile.genrules
