#!/bin/bash
##############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
##############################################################################
# This script is made for Jenkins test
LOG_FILE="test.xml"
STARTDIR=`pwd`
WORKING_DIR="build"
if [ ! -d "$WORKING_DIR" ]
then
  mkdir -p ${WORKING_DIR}
  cp src/* ${WORKING_DIR}
fi

if [ -d "$WORKING_DIR" ]
then
  cd ${WORKING_DIR}
  make clean
  rm *.xml
  make
  XML_NAME="sip_basic_call_result.xml"
  ttcn3_start EPTF_SIP_PerfTest EPTF_SIP_PerfTest.cfg
  # filter out the useful info:
  CPS_RESULT=`grep "final max CPS result" ${XML_NAME} | sed -e 's!.*<tq0001:result name=.final max CPS result. unit=.-.>\([0-9]*\).*</tq0001:result>.*!\1!g'`
  echo "cps result:$CPS_RESULT<<"
  echo "<report>" > ${LOG_FILE}
  echo "  <cps_result>${CPS_RESULT}</cps_result>" >> ${LOG_FILE}
  echo "</report>" >> ${LOG_FILE}
  cd ..
else
  echo "The working directory is not exist, test cannot be run"
fi
