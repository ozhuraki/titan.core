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
 *   Szabo, Janos Zoltan – initial implementation
 *   Tatarka, Gabor
 *
 ******************************************************************************/
#ifndef SIGNATURE_H
#define SIGNATURE_H

#include "../datatypes.h"
#include "compiler.h"

/* data structures for signature definitions */

typedef enum { PAR_IN, PAR_OUT, PAR_INOUT } signature_par_direction;

typedef struct {
  signature_par_direction direction;
  const char *type;
  const char *name;
  const char *dispname;
} signature_par;

typedef struct {
  size_t nElements;
  signature_par *elements;
} signature_par_list;

typedef struct {
  const char *name;
  const char *dispname;
  const char *altname;
  const char* name_w_no_prefix;
} signature_exception;

typedef struct {
  size_t nElements;
  signature_exception *elements;
} signature_exception_list;

typedef struct {
  const char *name;
  const char *dispname;
  signature_par_list parameters;
  const char *return_type;
  const char* return_type_w_no_prefix;
  boolean is_noblock;
  signature_exception_list exceptions;
} signature_def;

#ifdef __cplusplus
extern "C" {
#endif

void defSignatureClasses(const signature_def *sdef, output_struct *output);

#ifdef __cplusplus
}
#endif

#endif
