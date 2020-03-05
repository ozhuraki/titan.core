/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               EPTF_CLL_Variable_ExternalFunctions.cc
//  Description:        Implementation of the external functions of EPTF Variables
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 512
//  Updated:            2008-10-17
//  Contact:            http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////

#include "EPTF_CLL_Variable_Definitions.hh"
#include "EPTF_CLL_Variable_Functions.hh"
#include <sys/time.h>

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__upcast
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_upcast>.
///////////////////////////////////////////////////////////
EPTF__CLL__Base__Definitions::EPTF__Base__CT EPTF__CLL__Variable__Functions::f__EPTF__Var__upcast(const EPTF__CLL__Variable__Definitions::EPTF__Var__CT& pl__compRef) {
  return EPTF__CLL__Base__Definitions::EPTF__Base__CT((component)pl__compRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__downcast
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_downcast>.
///////////////////////////////////////////////////////////
EPTF__CLL__Variable__Definitions::EPTF__Var__CT EPTF__CLL__Variable__Functions::f__EPTF__Var__downcast(const EPTF__CLL__Base__Definitions::EPTF__Base__CT& pl__baseCompRef) {
  return EPTF__CLL__Variable__Definitions::EPTF__Var__CT((component)pl__baseCompRef);
}

// moved to EPTF_Base
// FLOAT EPTF__CLL__Variable__Functions::f__EPTF__Var__getTimeOfDay() {
//   struct timeval tv;
//   gettimeofday(&tv,NULL);
//   return tv.tv_sec+tv.tv_usec/1000000.0;
// }

// integer

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getIntRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getIntRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getIntRef(const INTEGER& pl__compVar) {
  const INTEGER *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefInt
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefInt>.
///////////////////////////////////////////////////////////
INTEGER EPTF__CLL__Variable__Functions::f__EPTF__Var__derefInt(const OCTETSTRING& pl__compVarRef) {
  return **((INTEGER**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyIntRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyIntRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyIntRefValue(const OCTETSTRING& pl__compVarRef, const INTEGER& pl__newValue) {
  **((INTEGER**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}


// float

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getFloatRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getFloatRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getFloatRef(const FLOAT& pl__compVar) {
  const FLOAT *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefFloat
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefFloat>.
///////////////////////////////////////////////////////////
FLOAT EPTF__CLL__Variable__Functions::f__EPTF__Var__derefFloat(const OCTETSTRING& pl__compVarRef) {
  return **((FLOAT**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyFloatRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyFloatRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyFloatRefValue(const OCTETSTRING& pl__compVarRef, const FLOAT& pl__newValue) {
  **((FLOAT**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}

// boolean

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getBoolRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getBoolRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getBoolRef(const BOOLEAN& pl__compVar) {
  const BOOLEAN *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefBool
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefBool>.
///////////////////////////////////////////////////////////
BOOLEAN EPTF__CLL__Variable__Functions::f__EPTF__Var__derefBool(const OCTETSTRING& pl__compVarRef) {
  return **((BOOLEAN**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyBoolRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyBoolRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyBoolRefValue(const OCTETSTRING& pl__compVarRef, const BOOLEAN& pl__newValue) {
  **((BOOLEAN**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}

// charstring

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getCharstringRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getCharstringRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getCharstringRef(const CHARSTRING& pl__compVar) {
  const CHARSTRING *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefCharstring
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefCharstring>.
///////////////////////////////////////////////////////////
CHARSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__derefCharstring(const OCTETSTRING& pl__compVarRef) {
  return **((CHARSTRING**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyCharstringRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyCharstringRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyCharstringRefValue(const OCTETSTRING& pl__compVarRef, const CHARSTRING& pl__newValue) {
  **((CHARSTRING**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}

// octetstring

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getOctetstringRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getOctetstringRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getOctetstringRef(const OCTETSTRING& pl__compVar) {
  const OCTETSTRING *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefOctetstring
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefOctetstring>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__derefOctetstring(const OCTETSTRING& pl__compVarRef) {
  return **((OCTETSTRING**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyOctetstringRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyOctetstringRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyOctetstringRefValue(const OCTETSTRING& pl__compVarRef, const OCTETSTRING& pl__newValue) {
  **((OCTETSTRING**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}

// hexstring

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getHexstringRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getHexstringRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getHexstringRef(const HEXSTRING& pl__compVar) {
  const HEXSTRING *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefHexstring
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefHexstring>.
///////////////////////////////////////////////////////////
HEXSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__derefHexstring(const OCTETSTRING& pl__compVarRef) {
  return **((HEXSTRING**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyHexstringRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyHexstringRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyHexstringRefValue(const OCTETSTRING& pl__compVarRef, const HEXSTRING& pl__newValue) {
  **((HEXSTRING**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}

// bitstring

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getBitstringRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getBitstringRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getBitstringRef(const BITSTRING& pl__compVar) {
  const BITSTRING *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefBitstring
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefBitstring>.
///////////////////////////////////////////////////////////
BITSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__derefBitstring(const OCTETSTRING& pl__compVarRef) {
  return **((BITSTRING**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyBitstringRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyBitstringRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyBitstringRefValue(const OCTETSTRING& pl__compVarRef, const BITSTRING& pl__newValue) {
  **((BITSTRING**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}

// integerlist

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getIntegerlistRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getIntegerlistRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getIntegerlistRef(const EPTF__CLL__Common__Definitions::EPTF__IntegerList& pl__compVar) {
  const EPTF__CLL__Common__Definitions::EPTF__IntegerList *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefIntegerlist
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefIntegerlist>.
///////////////////////////////////////////////////////////
EPTF__CLL__Common__Definitions::EPTF__IntegerList EPTF__CLL__Variable__Functions::f__EPTF__Var__derefIntegerlist(const OCTETSTRING& pl__compVarRef) {
  return **((EPTF__CLL__Common__Definitions::EPTF__IntegerList**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyIntegerlistRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyIntegerlistRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyIntegerlistRefValue(const OCTETSTRING& pl__compVarRef, const EPTF__CLL__Common__Definitions::EPTF__IntegerList& pl__newValue) {
  **((EPTF__CLL__Common__Definitions::EPTF__IntegerList**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}

// floatlist

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getFloatlistRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getFloatlistRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getFloatlistRef(const EPTF__CLL__Common__Definitions::EPTF__FloatList& pl__compVar) {
  const EPTF__CLL__Common__Definitions::EPTF__FloatList *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefFloatlist
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefFloatlist>.
///////////////////////////////////////////////////////////
EPTF__CLL__Common__Definitions::EPTF__FloatList EPTF__CLL__Variable__Functions::f__EPTF__Var__derefFloatlist(const OCTETSTRING& pl__compVarRef) {
  return **((EPTF__CLL__Common__Definitions::EPTF__FloatList**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyFloatlistRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyFloatlistRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyFloatlistRefValue(const OCTETSTRING& pl__compVarRef, const EPTF__CLL__Common__Definitions::EPTF__FloatList& pl__newValue) {
  **((EPTF__CLL__Common__Definitions::EPTF__FloatList**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}

// charstringlist

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getCharstringlistRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getCharstringlistRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getCharstringlistRef(const EPTF__CLL__Common__Definitions::EPTF__CharstringList& pl__compVar) {
  const EPTF__CLL__Common__Definitions::EPTF__CharstringList *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefCharstringlist
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefCharstringlist>.
///////////////////////////////////////////////////////////
EPTF__CLL__Common__Definitions::EPTF__CharstringList EPTF__CLL__Variable__Functions::f__EPTF__Var__derefCharstringlist(const OCTETSTRING& pl__compVarRef) {
  return **((EPTF__CLL__Common__Definitions::EPTF__CharstringList**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyCharstringlistRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyCharstringlistRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyCharstringlistRefValue(const OCTETSTRING& pl__compVarRef, const EPTF__CLL__Common__Definitions::EPTF__CharstringList& pl__newValue) {
  **((EPTF__CLL__Common__Definitions::EPTF__CharstringList**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}

// statusLED

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__getStatusLEDRef
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_getStatusLEDRef>.
///////////////////////////////////////////////////////////
OCTETSTRING EPTF__CLL__Variable__Functions::f__EPTF__Var__getStatusLEDRef(const EPTF__CLL__Common__Definitions::EPTF__StatusLED& pl__compVar) {
  const EPTF__CLL__Common__Definitions::EPTF__StatusLED *p = &pl__compVar;
  return OCTETSTRING(sizeof(p), (unsigned char*)&p);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__derefStatusLED
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_derefStatusLED>.
///////////////////////////////////////////////////////////
EPTF__CLL__Common__Definitions::EPTF__StatusLED EPTF__CLL__Variable__Functions::f__EPTF__Var__derefStatusLED(const OCTETSTRING& pl__compVarRef) {
  return **((EPTF__CLL__Common__Definitions::EPTF__StatusLED**)(const unsigned char*)pl__compVarRef);
}

///////////////////////////////////////////////////////////
//  Function: f__EPTF__Var__modifyStatusLEDRefValue
// 
//  Purpose:
//    Implementation of the external function <f_EPTF_Var_modifyStatusLEDRefValue>.
///////////////////////////////////////////////////////////
void EPTF__CLL__Variable__Functions::f__EPTF__Var__modifyStatusLEDRefValue(const OCTETSTRING& pl__compVarRef, const EPTF__CLL__Common__Definitions::EPTF__StatusLED& pl__newValue) {
  **((EPTF__CLL__Common__Definitions::EPTF__StatusLED**)(const unsigned char*)pl__compVarRef) = pl__newValue;
}

//...

