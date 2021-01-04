#!/bin/sh
###############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Raduly, Csaba
#
###############################################################################

# SCCSID: %Z%%M% %I% %G%
#

head -1 > /tmp/$$
if xterm -title "Notice from TTCN" -e sh -c "echo ''; cat /tmp/$$; echo ''; echo 'Press RETURN to Continue'; read x"
  then
    /bin/rm -f /tmp/$$
    exit 0
  else
    /bin/rm -f /tmp/$$
    exit 1
fi
