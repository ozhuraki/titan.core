/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Bene, Tamas
 *   Lovassy, Arpad
 *   Szabo, Janos Zoltan – initial implementation
 *   Vilmos Varga - author
 *
 ******************************************************************************/
//
// Description:           Implementation file for UserInterface
//----------------------------------------------------------------------------
#include "UserInterface.h"

using namespace mctr;

//----------------------------------------------------------------------------

UserInterface::~UserInterface()
{

}

//----------------------------------------------------------------------------

void UserInterface::initialize()
{

}

void UserInterface::executeBatchFile(const char* /* filename */)
{
  error(/* severity */ 0, "This user interface does not support batch files.");
}

//----------------------------------------------------------------------------
// Local Variables:
// mode: C++
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
