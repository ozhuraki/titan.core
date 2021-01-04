/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Cserveni, Akos
 *   Kremer, Peter
 *   Raduly, Csaba
 *   Szabo, Janos Zoltan – initial implementation
 *
 ******************************************************************************/
#ifndef ENUM_H
#define ENUM_H

#include "datatypes.h"
#include "ttcn3/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

void defEnumClass(const enum_def *edef, output_struct *output);
void defEnumTemplate(const enum_def *edef, output_struct *output);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ENUM_H */
