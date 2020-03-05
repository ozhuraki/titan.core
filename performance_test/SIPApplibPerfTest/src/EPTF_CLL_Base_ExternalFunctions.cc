/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               EPTF_CLL_Base_ExternalFunctions.cc
//  Description:        Implementation of the external functions of EPTF Base
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 512
//  Updated:            2011-09-15
//  Contact:            http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////

#include "EPTF_CLL_Base_Definitions.hh"
#include "EPTF_CLL_Base_Functions.hh"
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Base__upcast
// 
//  Purpose:
//    Implementation of the external function f_EPTF_Base_upcast.
///////////////////////////////////////////////////////////
INTEGER EPTF__CLL__Base__Functions::f__EPTF__Base__upcast(const EPTF__CLL__Base__Definitions::EPTF__Base__CT__private& pl__compRef) {
  return INTEGER((component)pl__compRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Base__downcast
// 
//  Purpose:
//    Implementation of the external function f_EPTF_Base_downcast.
///////////////////////////////////////////////////////////
EPTF__CLL__Base__Definitions::EPTF__Base__CT EPTF__CLL__Base__Functions::f__EPTF__Base__downcast(const INTEGER& pl__baseCompRef) {
  return EPTF__CLL__Base__Definitions::EPTF__Base__CT(pl__baseCompRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Base__assert
// 
//  Purpose:
//    Implementation of the external function f_EPTF_Base_assert.
///////////////////////////////////////////////////////////
void EPTF__CLL__Base__Functions::f__EPTF__Base__assert(const CHARSTRING& pl__assertMessage, const BOOLEAN& pl__predicate)
{
#ifdef EPTF_DEBUG
  if (!(pl__predicate)) {
    f__EPTF__Base__addAssertMsg(pl__assertMessage);
    TTCN_Logger::log_str(TTCN_Logger::ERROR_UNQUALIFIED, CHARSTRING("f_EPTF_Base_assert: Assertion failed! ")+pl__assertMessage);
    if (EPTF__CLL__Base__Definitions::EPTF__Base__CT__private_component_v__EPTF__Base__negativeTestMode==true) {
      EPTF__CLL__Base__Functions::f__EPTF__Base__stop(NONE);
    } else {
      EPTF__CLL__Base__Functions::f__EPTF__Base__stop(FAIL);
    }
    //EPTF__CLL__Base__Functions::f__EPTF__Base__stopAll(EPTF__CLL__Base__Functions::f__EPTF__Base__stopAll_pl__noCleanup_defval);
  }
#endif
}

FLOAT EPTF__CLL__Base__Functions::f__EPTF__Base__getTimeOfDay()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec+tv.tv_usec/1000000.0;
}

INTEGER EPTF__CLL__Base__Functions::f__EPTF__Base__getPid()
{
  pid_t myPid = getpid();
  return (unsigned int)myPid;
}

// egbotat
CHARSTRING EPTF__CLL__Base__Functions::f__EPTF__Base__getHostName()
{
  char buf[1024];
  int res = gethostname(buf, sizeof(buf)-1);
  if(res) {
    TTCN_Logger::log(TTCN_Logger::ERROR_UNQUALIFIED, "gethostname failed with error code %d, h_errno: %d (%s)",
      res, h_errno, hstrerror(h_errno));
    return CHARSTRING("");
  }
  return CHARSTRING(buf);
}
//~egbotat
