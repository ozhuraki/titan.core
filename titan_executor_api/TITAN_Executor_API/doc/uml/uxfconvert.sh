###############################################################################
# Copyright (c) 2000-2019 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
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

