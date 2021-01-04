/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Godar, Marton
 *   Raduly, Csaba
 *   Szabo, Bence Janos
 *
 ******************************************************************************/
#ifndef ANNOTATION_HH_
#define ANNOTATION_HH_

#include "RootType.hh"

/**
 * Type that contains information coming from XSD annotation and comments
 *
 * Source in XSD:
 *
 * 	* <annotation> element whose parent element is <schema>
 * 	* xml comments ( closed between <!-- ... --> signs)
 *
 * Result in TTCN-3:
 *
 * 	* TTCN-3 comment
 *
 */
class Annotation : public RootType {
public:
  Annotation(XMLParser * a_parser, TTCN3Module * a_module, ConstructType a_construct);
  Annotation(const Annotation &); // not implemented
  Annotation & operator=(const Annotation &); // not implemented
  // Default destructor is used

  /** Virtual methods
   *  inherited from the abstract RootType
   */
  void loadWithValues();
  void printToFile(FILE * file);

  void dump(unsigned int depth) const;
};

#endif /* ANNOTATION_HH_ */
