/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               EPTF_CLL_Common_ExternalFunctions.cc
//  Description:        Implementation of the external functions of EPTF Common
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 512
//  Updated:            2009-12-09
//  Contact:            http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////

#include "EPTF_CLL_Common_Functions.hh"

EPTF__CLL__Common__Definitions::EPTF__CharstringList v__EPTF__Common__errorMsgs(NULL_VALUE);

void EPTF__CLL__Common__Functions::f__EPTF__Common__initErrorMsgs(void) {
  v__EPTF__Common__errorMsgs = NULL_VALUE;
}

void f__EPTF__Common__addErrorMsg(const CHARSTRING& pl__newMsg)
{
  v__EPTF__Common__errorMsgs[v__EPTF__Common__errorMsgs.size_of()] = pl__newMsg;
}

INTEGER EPTF__CLL__Common__Functions::f__EPTF__Common__nofErrorMsgs()
{
  return v__EPTF__Common__errorMsgs.size_of();
}

CHARSTRING EPTF__CLL__Common__Functions::f__EPTF__Common__getErrorMsg(const INTEGER& pl__errorNum)
{
  if (v__EPTF__Common__errorMsgs.size_of() == 0) {
    return CHARSTRING("");
  }
  {
    boolean tmp_16;
    tmp_16 = (v__EPTF__Common__errorMsgs.size_of() <= pl__errorNum);
    if (!tmp_16) tmp_16 = (pl__errorNum < 0);
    if (tmp_16) {
      return CHARSTRING("");
    }
  }
  return v__EPTF__Common__errorMsgs[pl__errorNum];
}


void EPTF__CLL__Common__Functions::f__EPTF__Common__error(
  const CHARSTRING& pl__message)
{
  f__EPTF__Common__addErrorMsg(pl__message);
  TTCN_Logger::log_str(TTCN_Logger::ERROR_UNQUALIFIED, pl__message);
}

void EPTF__CLL__Common__Functions::f__EPTF__Common__warning(
  const CHARSTRING& pl__message)
{
  TTCN_Logger::log_str(TTCN_Logger::WARNING_UNQUALIFIED, pl__message);
}

void EPTF__CLL__Common__Functions::f__EPTF__Common__user(
  const CHARSTRING& pl__message)
{
  TTCN_Logger::log_str(TTCN_Logger::USER_UNQUALIFIED, pl__message);
}
