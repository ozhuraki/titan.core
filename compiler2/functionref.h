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
 *   Szabados, Kristof
 *   Szabo, Janos Zoltan – initial implementation
 *
 ******************************************************************************/
#ifndef FUNCTIONREF_H
#define FUNCTIONREF_H

#include "datatypes.h"
#include "ttcn3/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

void defFunctionrefClass(const funcref_def *fdef, output_struct *output);
void defFunctionrefTemplate(const funcref_def *fdef, output_struct *output);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FUNCTIONREF_H */
