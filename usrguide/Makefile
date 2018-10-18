##############################################################################
# Copyright (c) 2000-2018 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   
#   >
#   Baji, Laszlo
#   Balasko, Jeno
#   Baranyi, Botond
#   Forstner, Matyas
#   Gecse, Roland
#   Gillespie, Raymond
#   Kremer, Peter
#   Lovassy, Arpad
#   Ormandi, Matyas
#   Pandi, Krisztian
#   Raduly, Csaba
#   Szabados, Kristof
#   Szabo, Janos Zoltan – initial implementation
#   Szalai, Gabor
#   Tatarka, Gabor
#   Torpis, Zsolt
#   Zalanyi, Balazs Andor
#
##############################################################################
# Makefile for the TITAN User Guide

TOP  := ..
include ../Makefile.cfg

.SUFFIXES: .tex .labels .dvi .ps .pdf

PDF_GEN_CALL := asciidoctor-pdf --attribute skip-front-matter

TITAN_PDFS := apiguide/Apiguide.pdf userguide/UserGuide.pdf installationguide/installationguide.pdf referenceguide/ReferenceGuide.pdf releasenotes/releasenotes.pdf

ifeq ($(GEN_PDF), yes)

all run: $(TITAN_PDFS)

# On success (PDF files are successfully created), the PDF files are copied to the $(DOCDIR) directory.
install: $(TITAN_PDFS)
ifdef MINGW
$(info Skipped ${CURDIR} for MinGW)
else
	mkdir -p $(DOCDIR)
	cp $(TITAN_PDFS) $(DOCDIR)
endif

clean:
	rm -f $(TITAN_PDFS)

apiguide/Apiguide.pdf:
	$(PDF_GEN_CALL) -o $@ apiguide/README.adoc

userguide/UserGuide.pdf:
	$(PDF_GEN_CALL) userguide/UserGuide.adoc

installationguide/installationguide.pdf:
	$(PDF_GEN_CALL) installationguide/installationguide.adoc

referenceguide/ReferenceGuide.pdf:
	$(PDF_GEN_CALL) referenceguide/ReferenceGuide.adoc

releasenotes/releasenotes.pdf:
	$(PDF_GEN_CALL) releasenotes/releasenotes.adoc

else
# GEN_PDF == no
all run:
install:
clean:
endif

release:
	$(MAKE) install GEN_PDF=yes



distclean:clean

dep:

