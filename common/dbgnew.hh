/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
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
#ifndef DBGNEW_HH
#define DBGNEW_HH

#ifndef _Common_memory_H
#include "memory.h"
#endif

#include <new>

#ifdef MEMORY_DEBUG

class debug_new_counter_t
{
  static int count;
  static const char * progname;
public:
  debug_new_counter_t();
  ~debug_new_counter_t();
  void set_program_name(const char *pgn);
};
// implementation in new.cc

// An instance for every translation unit. Because each instance is constructed
// before main() and probably before any other global object,
// it is destroyed after main() ends and all global objects are destroyed.
// The last destructor runs check_mem_leak().
static debug_new_counter_t debug_new_counter;

// Custom placement new for memory tracking
void* operator new(size_t size, const char* file, int line);
void* operator new[](size_t size, const char* file, int line);

// TODO: these might be GCC version dependent
void* operator new(size_t size, const std::nothrow_t&, const char* file, int line);
void* operator new[](size_t size, const std::nothrow_t&, const char* file, int line);

inline void* operator new(size_t, void* __p, const char*, int) { return __p; }
inline void* operator new[](size_t, void* __p, const char*, int) { return __p; }

#if __cplusplus >= 201703L
void* operator new(size_t size, std::align_val_t, const char* file, int line);
void* operator new[](size_t size, std::align_val_t, const char* file, int line);
#endif // C++11

// Redirect "normal" new to memory-tracking placement new.
#define new(...) new(__VA_ARGS__, __FILE__, __LINE__)

#endif // MEMORY_DEBUG

#endif
