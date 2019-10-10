/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Baranyi, Botond
 *
 ******************************************************************************/

#include <TTCN3.hh>

#ifndef EXTERNALCLASS_HH
#define EXTERNALCLASS_HH

namespace oop {

class ExternalClass : public OBJECT {
public:
  CHARSTRING f__ext(const INTEGER& x) {
    return int2str(x) + CHARSTRING("?");
  }
};

} // namespace

#endif
