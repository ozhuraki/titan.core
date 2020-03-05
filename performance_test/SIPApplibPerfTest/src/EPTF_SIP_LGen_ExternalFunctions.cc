/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//  File:     EPTF_SIP_LGen_ExternalFunctions.cc
//  Rev:      <RnXnn>
//  Prodnr:   CNL 113 522
//  Updated:  2008-02-15
//  Contact:  http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////

#include "TTCN3.hh"
#include "SIPmsg_Types.hh"
#include "SIPmsg_PT.hh"
#include "EPTF_SIP_UserDatabase_Definitions.hh"
#include <ctype.h>
#include "memory.h"

using namespace SIPmsg__PortType;
namespace EPTF__SIP__Common__Functions {

  void f__EPTF__SIP__MethodToInt(
    const SIPmsg__Types::Method& pl__method,
    INTEGER& pl__int)
  {
    pl__int = (int)pl__method;
  }

  void f__EPTF__SIP__IntToMethod(
    const INTEGER& pl__int,
    SIPmsg__Types::Method& pl__method)
  {
    pl__method = (SIPmsg__Types::Method)pl__int;
  }

  BOOLEAN f__EPTF__SIP__isDigit(const CHARSTRING& p__char)
  {
    if (p__char.lengthof() == 1)
    {
      const char* charpoi = (const char*) p__char;
      if (isdigit(charpoi[0])) return BOOLEAN(true);
    }
    return BOOLEAN(false);
  }
}
