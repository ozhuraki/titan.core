#!/bin/bash
##############################################################################
# Copyright (c) 2000-2019 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
##############################################################################
CURRENT_TIME=`date '+%Y-%m-%d %H:%M:%S'`
TESTREPORTFILE=`eval "ls -1t sip_basic_call_result*.xml | awk '// {print; exit;}'"`

echo "TESTREPORTFILE: $TESTREPORTFILE"

if [ -f "$TESTREPORTFILE" ]
then
  echo "<h3>SIP Applib Performance Test Report</h3>" >> temp1.txt
  echo "Test report was generated at: $CURRENT_TIME<br>" >> temp1.txt

  echo "<div class=\"SIPApplib_PerfTestResults\">" >> temp1.txt
  echo "<h4>TestCase $1:</h4>" >> temp1.txt
  testresultsXML2HTML.sh $TESTREPORTFILE >> temp1.txt
  echo "</div>" >> temp1.txt
  \rm $TESTREPORTFILE
fi
