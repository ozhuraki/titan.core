##############################################################################
# Copyright (c) 2000-2019 Ericsson Telecom AB
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

.SUFFIXES: .adoc .pdf

PDF_GEN_CALL := asciidoctor-pdf --attribute skip-front-matter

TITAN_PDFS := apiguide/Apiguide.pdf userguide/UserGuide.pdf installationguide/installationguide.pdf \
referenceguide/ReferenceGuide.pdf java_referenceguide/JavaReferenceGuide.pdf releasenotes/releasenotes.pdf PRI/PRI.pdf SoC_TITAN/SoC_TITAN.pdf \
SoC_XML_TITAN/SoC_XML_TITAN.pdf

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

apiguide/Apiguide.pdf: apiguide/*.adoc apiguide/images/*
	$(PDF_GEN_CALL) apiguide/Apiguide.adoc

userguide/UserGuide.pdf: userguide/*.adoc userguide/images/*
	$(PDF_GEN_CALL) userguide/UserGuide.adoc

installationguide/installationguide.pdf: installationguide/*.adoc installationguide/images/*
	$(PDF_GEN_CALL) installationguide/installationguide.adoc

referenceguide/ReferenceGuide.pdf: referenceguide/*.adoc referenceguide/images/*
	$(PDF_GEN_CALL) referenceguide/ReferenceGuide.adoc

java_referenceguide/JavaReferenceGuide.pdf: java_referenceguide/*.adoc java_referenceguide/images/*
	$(PDF_GEN_CALL) java_referenceguide/JavaReferenceGuide.adoc

releasenotes/releasenotes.pdf: releasenotes/*.adoc releasenotes/images/*
	$(PDF_GEN_CALL) releasenotes/releasenotes.adoc

PRI/PRI.pdf: PRI/*.adoc
	$(PDF_GEN_CALL) PRI/PRI.adoc

SoC_TITAN/SoC_TITAN.pdf: SoC_TITAN/*.adoc SoC_TITAN/images/*
	$(PDF_GEN_CALL) SoC_TITAN/SoC_TITAN.adoc

SoC_XML_TITAN/SoC_XML_TITAN.pdf: SoC_XML_TITAN/*.adoc SoC_XML_TITAN/images/*
	$(PDF_GEN_CALL) SoC_XML_TITAN/SoC_XML_TITAN.adoc

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

