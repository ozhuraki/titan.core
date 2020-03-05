/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Raduly, Csaba
 *
 ******************************************************************************/
#include <Integer.hh>
#include <Objid.hh>

namespace TobjidOper {

INTEGER indexer(OBJID const& o, INTEGER const& idx)
{
  INTEGER r;
  r.set_long_long_val(o[idx]);
  // Use long long because OBJID::objid_element (currently unsigned 32bit)
  // does not fit into the native representation (_signed_ 32bit) of INTEGER
  return r;
}


}
