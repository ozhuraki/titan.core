#!/bin/bash
##############################################################################
# Copyright (c) 2000-2020 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
##############################################################################
# This script is used for testing time of compiling and running in case of different Titan versions.
# Prerequisites:
# 
# NO- titan versions are compiled
# NO:- build folder is prepared i.e: all source files are linked into it and Makefile is generated
#
# It will do the following for the compiled titan versions listed in TTCN3_DIRS:
# 1. creates the build dir if ONLYRUN == false
# 2. goes to the build dir
# 2. make clean, rm sip_...xml
# 3. make
# 4. prints the elapsed time
# 5. executes the application
# 6. extracts the max cps value and collect them into an xml file
#
# Usage:
#   version_test.sh [options] YourProject.prj
#1.Go to the main dir. (Its subdir will be the build, it contains the prj file)
#2.Start the script
#

MINPARAMS=0

LOG_FILE="exec_times_`date +%y_%m_%d_%H-%M`.txt"
versions="daily 3.1.pl0 4.2.pl0 5.5.pl0 6.3.pl0"
#TTCN3_DIRS="/app/TITAN/R8H/LMWP3.1"
echo "Compilation and execution times:" >  ${LOG_FILE}
LOG_FILE="../${LOG_FILE}"
TTCN3_DIR_ORIG=${TTCN3_DIR}
PATH_ORIG=$PATH
LD_LIBRARY_PATH_ORIG=$LD_LIBRARY_PATH
WORKING_DIR="build"
ONLYRUN=false
TTCN3_DIR_PREFIX="/home/titanrt/TTCN3/TTCNv3-"
TTCN3_DIR_SUFFIX=""
MAKE_CLEAN="false"
EXE_FILE="EPTF_SIP_PerfTest"
CFG_FILE="EPTF_SIP_PerfTest.cfg"
STARTDIR=`pwd`
XML_NAME="sip_basic_call_result.xml"
RESULT_FILE=${STARTDIR}/"test.xml"
version1="v"
#===========================
# showUsage
#=========================== 
function showUsage
{
  echo "Usage: $0 <options>"
  
  echo "Options:"
  echo "  -c: clean before make (if -r not applied) - NOT WORKING NOW"
  echo "  -r: only run"
  echo "  -p: TTCN3_DIR prefix, default: ${TTCN3_DIR_PREFIX}"
  echo "  -s: TTCN3_DIR suffix, default: ${TTCN3_DIR_SUFFIX}"
  echo "  -v: version list, default: ${versions}"
}


#==== init ====
init() {
  if [ $# -lt $MINPARAMS ]
  then
    showUsage "$@"
    exit 1
  fi

  while getopts "cp:rs:v:" o
  do
    case "$o" in
      c) MAKE_CLEAN="true";;
      p) TTCN3_DIR_PREFIX=$OPTARG;;
      r) ONLYRUN="true";;
      s) TTCN3_DIR_SUFFIX=$OPTARG;;
      v) versions=$OPTARG;;
      h) showUsage; exit 1;;
      [?]) showUsage; exit 1;;
    esac
  done
  #shift $((OPTIND-1))  
}


#====================================
# MAIN ####
#====================================
init "$@"

echo "script started"
echo "<report>" > ${RESULT_FILE}
for version in ${versions}
do
  
  WORKING_DIR="build_${version}"
  if [ "$version" == "daily" ]
  then
    echo "daily version branch"
    version1="${version}"
    TTCN3_DIR=${TTCN3_DIR_ORIG}
    PATH=${PATH_ORIG}
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH_ORIG}
  else
    echo "version: ${version} branch"
    version1="v${version}"
    TTCN3_DIR="${TTCN3_DIR_PREFIX}${version}${TTCN3_DIR_SUFFIX}"
    PATH="${TTCN3_DIR}/bin:${PATH_ORIG}"
    LD_LIBRARY_PATH="${TTCN3_DIR}/lib:${LD_LIBRARY_PATH_ORIG}"   
  fi
  
  export TTCN3_DIR PATH LD_LIBRARY_PATH
  
  if [ ${ONLYRUN} == false ]
  then
    rm -rf "$WORKING_DIR"
  fi
  
  if [ ! -d "$WORKING_DIR" ]
  then
    mkdir -p ${WORKING_DIR}
    cp src/* ${WORKING_DIR}
  fi 
  
  if [ -d "$WORKING_DIR" ]
  then
    cd ${WORKING_DIR}
    echo "Current dir: `pwd`" 
    echo "==============================================================" >> ${LOG_FILE}

    echo path: $PATH
    echo lib: $LD_LIBRARY_PATH
    which compiler >> ${LOG_FILE}
    echo ">>>$version1<<<"
    echo "Titan version: $version" >> ${LOG_FILE}
    s=`date +%s`

    if [ ${ONLYRUN} == false -o ! -f "${EXE_FILE}" ]
    then
      make clean
      rm ${XML_NAME}
      echo "Compilation started at:  `date`" >> ${LOG_FILE}
      make
      echo "Compilation finished at: `date`" >> ${LOG_FILE}
      d=`date +%s`
      echo "Compilation time: `expr $d - $s` s" >> ${LOG_FILE}
      echo "$TTCN3_DIR finished" >> ${LOG_FILE}
    else
      d=`date +%s`
    fi

    #running test:
    echo ttcn3_start "${EXE_FILE}" "${CFG_FILE}"
    ttcn3_start "${EXE_FILE}" "${CFG_FILE}"
    e=`date +%s`
    echo "execution finished at `date +%y_%m_%d_%H-%M`" >> ${LOG_FILE}
    echo "Execution time: `expr $e - $d` s" >> ${LOG_FILE}
    echo "==============================================================" >> ${LOG_FILE}
    # filter out the useful info:
    CPS_RESULT=`grep "final max CPS result" ${XML_NAME} | sed -e 's!.*<tq0001:result name=.final max CPS result. unit=.-.>\([0-9]*\).*</tq0001:result>.*!\1!g'`
    echo "${version1} cps result:>>$CPS_RESULT<<"  
    echo "  <${version1}>${CPS_RESULT}</${version1}>" >> ${RESULT_FILE} 

    TTCN3_DIR=${TTCN3_DIR_ORIG}
    PATH=${PATH_ORIG}
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH_ORIG}
    export TTCN3_DIR PATH LD_LIBRARY_PATH
    cd ..
  else
    echo "The working directory is not exist, test cannot be run"
  fi
done

echo "</report>" >> ${RESULT_FILE}
echo "script finished"
