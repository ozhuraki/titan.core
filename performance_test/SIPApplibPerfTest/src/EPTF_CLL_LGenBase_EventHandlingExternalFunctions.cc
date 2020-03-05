/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               EPTF_CLL_LGenBase_EventHandlingExternalFunctions.cc
//  Description:        Implementation of the external functions of EPTF LGenBase
//                      event handling feature
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 512
//  Updated:            2009-03-05
//  Contact:            http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////

#include "EPTF_CLL_LGenBase_Definitions.hh"
#include "EPTF_CLL_LGenBase_EventHandlingFunctions.hh"
//#include "EPTF_LGenBase_PerformanceTests.hh"
//#include "EPTF_CLL_Variable_Functions.hh"
#include <stdio.h>
#include <math.h> //delete if f__EPTF__LGenBase__rnd__normal function is deleted


const int intsize = sizeof(int);
const int masksize = intsize*2;
const int elemCount = 6;
//static const OCTETSTRING os_0(4, os_0_octets);
//FIXME 64bit
#define DEF_MASK "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"

void inline int2OctStr(char* octets, const unsigned int pl__i){
	const char bm[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	unsigned int i2Conv = pl__i;
	for(int i=masksize-1;i>=0;i--){
		octets[i] = bm[(i2Conv&0xF)];
		i2Conv=i2Conv>>4;
	}
}

/*
TTCN_Location current_location("EPTF_CLL_LGenBase_EventHandlingExternalFunctions.cc", 80, TTCN_Location::LOCATION_FUNCTION, "f_EPTF_LGenBase_listenableEvent2Mapper");
current_location.update_lineno(81);
TTCN_Logger::begin_event(TTCN_Logger::PARALLEL_PTC);
TTCN_Logger::log_str(TTCN_USER, "DEBUG: EPTF_CLL_LGenBase_EventHandlingExternalFunctions: #1.");
TTCN_Logger::end_event();
 */
namespace EPTF__CLL__LGenBase__ExternalFunctions {
CHARSTRING f__EPTF__LGenBase__listener2Str(
		const EPTF__CLL__LGenBase__Definitions::EPTF__LGenBase__EventListener__FT& pl__fn,
		const EPTF__CLL__Common__Definitions::EPTF__IntegerList& pl__args
)
{
	char cTemp[masksize+1];
	cTemp[masksize] = '\0';
	CHARSTRING vl__ret((TTCN_Logger::begin_event_log2str(),pl__fn.log(),TTCN_Logger::end_event_log2str()));
	vl__ret = (vl__ret + ";");
	INTEGER vl_size(pl__args.size_of());
	for ( int vl__i = 0; vl__i < vl_size; vl__i++) {
		int2OctStr(cTemp, pl__args[vl__i]);
		vl__ret = (vl__ret + cTemp);
	}
	return vl__ret;
}

CHARSTRING f__EPTF__LGenBase__genericIds2KeyStr(
  const INTEGER& pl__b,
  const INTEGER& pl__i
)
{
	char cTemp[2*masksize+1];
	cTemp[2*masksize] = '\0';
	int2OctStr(cTemp,pl__b);
	int2OctStr(cTemp+masksize,pl__i);
	return cTemp;
}

CHARSTRING f__EPTF__LGenBase__int2KeyStr(const INTEGER& pl__par)
{
	char cTemp[masksize+1];
	cTemp[masksize] = '\0';
	int2OctStr(cTemp,pl__par);
	return cTemp;
}

//TODO: !!!Move this function to TCC_Useful_Functions!!!
//normal random variate generator
//it uses the Box-Muller algorithm to generate the numbers
FLOAT f__EPTF__LGenBase__rnd__normal(const FLOAT& mean, const FLOAT& deviation)
{
  double uniform1, uniform2, s, normal1;
  static double normal2;
  static bool generate = true;

  if (generate)
  {
    do {
      uniform1 = 2.0 * rnd() - 1.0;
      uniform2 = 2.0 * rnd() - 1.0;
      s = uniform1 * uniform1 + uniform2 * uniform2;
    } while ((s >= 1.0) || (s == 0.0));
    
    s = sqrt((-2.0 * log(s)) / s);
    normal1 = uniform1 * s;
    normal2 = uniform2 * s;
    generate = false;
  }
  else //use the value from the previous generation
  {
    normal1 = normal2;
    generate = true;
  }

  return FLOAT(normal1 * (double)deviation + (double)mean);
}//f__EPTF__LGenBase__rnd__normal
}
//...

