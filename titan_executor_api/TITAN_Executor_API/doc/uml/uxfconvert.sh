###############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Lovassy, Arpad
#
###############################################################################

# Converts UML diagrams from uxf format to pdf and png

# Prerequisites:
#   sudo apt-get install umlet

for fe in *.uxf; do
    [ -f "$fe" ] || break
    #remove extension
    f=${fe%.uxf}
    echo $f
    umlet -action=convert -format=pdf -filename=$f.uxf -output=$f.pdf
    umlet -action=convert -format=png -filename=$f.uxf -output=$f.png
done

