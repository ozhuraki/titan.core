/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               EPTF_CLL_DataSource_ExternalFunctions.cc
//  Description:        Implementation of the external functions of EPTF DataSource
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 512
//  Updated:            2011-02-23
//  Contact:            http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////

#include "EPTF_CLL_DataSource_Functions.hh"
#include <strings.h>
#include <regex.h>
#include <stdlib.h>

INTEGER EPTF__CLL__DataSource__Functions::f__EPTF__DataSource__compare(
  const EPTF__CLL__DataSource__Definitions::EPTF__DataSource__Param& pl__par1,
  const EPTF__CLL__DataSource__Definitions::EPTF__DataSource__Param& pl__par2
) {
  return strcmp((const char*)pl__par1.paramName(),(const char*)pl__par2.paramName());
}

INTEGER EPTF__CLL__DataSource__Functions::f__EPTF__DataSource__str2float(
  const CHARSTRING& pl__str,
  FLOAT& pl__float
) {
  
  const char* posix_str = "[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?";
  regex_t posix_regexp;
  
  int ret_val=regcomp(&posix_regexp, posix_str, REG_EXTENDED);
  
  if(ret_val!=0) {
      char err[512];
      regerror(ret_val, &posix_regexp, err, sizeof(err));
      regfree(&posix_regexp);
      TTCN_warning("Function regcomp() failed for string "
	    "\"%s\": %s", posix_str, err);
      return -1;
  }

  int result = regexec(&posix_regexp, (const char*)pl__str, 0, NULL, 0);
  regfree(&posix_regexp);

  if(result == REG_NOMATCH) {
    return -2;
  }
  
  pl__float = atof((const char*)pl__str);

  return 0;
  
}

