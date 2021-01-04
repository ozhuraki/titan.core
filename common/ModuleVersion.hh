/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   
 *   Balasko, Jeno
 *
 ******************************************************************************/
#ifndef _MODULE_VERSION_H  
#define _MODULE_VERSION_H  

#undef new
#include <new>
#include <string>
#include "dbgnew.hh"

class ModuleVersion {
  public:
    ModuleVersion() {}

    ModuleVersion(const char* const p_productNumber, 
        unsigned int p_suffix, unsigned int p_release,
        unsigned int p_patch, unsigned int p_build,
        const char* const p_extra)
      : productNumber(p_productNumber ? p_productNumber : ""),
      suffix(p_suffix),
      release(p_release),
      patch(p_patch),
      build(p_build),
      extra(p_extra ? p_extra : "") {
        // Do nothing
      }

    bool hasProductNumber() const { return !productNumber.empty(); }
    std::string toString() const; 

    bool operator<(const ModuleVersion& other) const;

  private:
    std::string productNumber;
    unsigned int suffix;
    unsigned int release;
    unsigned int patch;
    unsigned int build;
    std::string extra;
};

#endif
