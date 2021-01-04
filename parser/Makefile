##############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Forstner, Matyas
#   Szabo, Janos Zoltan – initial implementation
#
##############################################################################
# Makefile for the TTCN-3 parser

OBJECTS= parser.tab.o lex.yy.o
TARGETS= parser

.c.o:
	gcc -c -O2 -Wall $<

all:	$(TARGETS)

clean:
	-rm $(OBJECTS) $(TARGETS) lex.yy.c parser.tab.c parser.output parser.tab.h

parser.tab.c: parser.y
	bison -dv $<

lex.yy.c: parser.l parser.tab.c
	flex parser.l

parser: $(OBJECTS)
	gcc -o $@ $(OBJECTS)
