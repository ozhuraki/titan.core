/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Baranyi, Botond
 *
 ******************************************************************************/

#include "Custom2.hh"
#include "Custom1.hh"
#include "Phantom.hh"

#ifndef CODERS_HH
#define CODERS_HH

namespace Custom2 {

// Coding functions for the record type in test 1
BITSTRING f__enc__rec(const Custom3::Rec& x);
INTEGER f__dec__rec(BITSTRING& b, Custom3::Rec& x);

// Coding functions for the union type in test 3
BITSTRING f__enc__uni(const Custom1::Uni& x);
INTEGER f__dec__uni(BITSTRING& b, Custom1::Uni& x);

// Coding functions for the bitstring type in test 4
BITSTRING f__enc__bs(const BITSTRING& x);
INTEGER f__dec__bs(BITSTRING& b, BITSTRING& x);

}

namespace Custom1 {

// Coding functions for the record-of type in test 2
BITSTRING f__enc__recof(const RecOf& x);
INTEGER f__dec__recof(BITSTRING& b, RecOf& x);

}

namespace AsnAndPer {

// "PER" coder functions, using the built-in JSON codec
extern BITSTRING f__enc__seqof(const Types::SeqOf& x);
extern INTEGER f__dec__seqof(BITSTRING& x, Types::SeqOf& y);
extern INTEGER f__dec__choice(BITSTRING& x, Types::Choice& y);

}

namespace Phantom {

// Coding functions for the set type in test 5
BITSTRING f__enc__set(const Custom3::Set& x);
INTEGER f__dec__set(BITSTRING& b, Custom3::Set& x);

// "PER" coder functions, using the built-in JSON codec
extern BITSTRING f__enc__setof(const Types::SetOf& x);
extern INTEGER f__dec__setof(BITSTRING& x, Types::SetOf& y);

}

#endif
