/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               TestResultGen_ExternalFunctions.cc
//  Description:        Implementation of the external functions of TestResultGen
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 512
//  Updated:            2010-11-26
//  Contact:            http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////

#include "TestResultGen.hh"

namespace TestResultGen {

static const CHARSTRING cs_7('\''),
cs_3(18, "+%Y-%m-%d %H:%M:%S"),
cs_8(5, ";exit"),
cs_4(7, "Unknown"),
cs_5(4, "bash"),
cs_13(48, "compiler -v 2>&1 | gawk \"/version/ {print \\$11}\""),
cs_6(6, "date '"),
cs_9(10, "echo $USER"),
cs_1(55, "f_TestResultGen_appendResult: Cannot decode input file "),
cs_0(47, "f_TestResultGen_appendResult: Cannot open file "),
cs_2(46, "f_TestResultGen_writeResult: Cannot open file "),
cs_12(16, "g++ -dumpversion"),
cs_15(56, "grep MemTotal /proc/meminfo | awk '{print $2/1024\"MB\" }'"),
cs_14(67, "top bd00.50n2|grep Cpu| tail -n1 | sed 's/.*Cpu(s): //g;s/us,.*//g'"),
cs_10(8, "uname -n"),
cs_11(9, "uname -sr");


class TestResultGen_Dummy  {
public:
  TestResultGen_Dummy() {
    TestResultGen__CT_component_pipe.activate_port();
    TestResultGen__CT_component_pipe.start();
  }
};

} // namespace TestResultGen
