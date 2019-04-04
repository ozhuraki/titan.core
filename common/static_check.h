/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
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
#ifndef STATIC_CHECK_HH
#define STATIC_CHECK_HH

#ifdef __cplusplus

template<bool> struct CompileTimeError;
template<>     struct CompileTimeError<true> {}; // false never defined

#define STATIC_ASSERT(expr) CompileTimeError<(expr) != 0>()

#else

#define STATIC_ASSERT(x) do { enum { assert_static_ = 1/(x) }; } while(0)

#endif

#define ENSURE_EQUAL(x, y) STATIC_ASSERT((int)(x) == (int)(y));

#endif
