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
#ifndef OERAST_HH
#define OERAST_HH

#include "../vector.hh"

class OerAST {
public:
  int bytes;
  bool signed_;
  int length;
  bool extendable;
  int nr_of_root_comps;
  vector<int> ext_attr_groups;
  vector<int> p;
  
  OerAST();
  ~OerAST();
  bool empty() const;
};

#endif /* OERAST_HH */

