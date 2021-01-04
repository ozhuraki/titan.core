/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Szabo, Bence Janos
 *
 ******************************************************************************/
#include <TTCN3.hh>
#include <ctype.h>

namespace xmlTest__Shell {

// Count the lines which starts with a number
INTEGER f__countDiffs(const CHARSTRING& diffoutput) {
  INTEGER result = 0;
  const char* c_diffoutput = (const char*)diffoutput;
  if (isdigit(*c_diffoutput)) {
    result = result + 1;
  }
  const char* pos = strchr(c_diffoutput, '\n');
  while (pos != NULL) {
    if ((pos + 1) != NULL && isdigit(*(pos + 1))) {
      result = result + 1;
    }
    pos = strchr(pos + 1, '\n');
  }
  return result;
}

}
