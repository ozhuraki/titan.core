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
 *   Szabo, Janos Zoltan – initial implementation
 *
 ******************************************************************************/
#include "dbgnew.hh"
#include <stddef.h>

#undef new

static void *dummy = NULL;

void *operator new(size_t size)
{
    return Malloc(size);
}

void *operator new[](size_t size)
{
    if (size == 0) return &dummy;
    else return Malloc(size);
}

void operator delete(void *ptr)
{
    Free(ptr);
}

void operator delete(void *ptr, std::size_t)
{
    Free(ptr);
}

void operator delete[](void *ptr)
{
    if (ptr != static_cast<void*>(&dummy)) Free(ptr);
}

void operator delete[](void *ptr, std::size_t)
{
    if (ptr != static_cast<void*>(&dummy)) Free(ptr);
}

/**************************************************************************/

#ifdef MEMORY_DEBUG

// overloads for memory debug
void* operator new(size_t size, const char* file, int line)
{
    return Malloc_dbg(file, line, size);
}

void* operator new[](size_t size, const char* file, int line)
{
    if (size == 0) return &dummy;
    else return Malloc_dbg(file, line, size);
}

void* operator new(size_t size, const std::nothrow_t&, const char* file, int line)
{
    return Malloc_dbg(file, line, size);
}

void* operator new[](size_t size, const std::nothrow_t&, const char* file, int line)
{
    if (size == 0) return &dummy;
    else return Malloc_dbg(file, line, size);
}

#if __cplusplus >= 201703L
void* operator new(size_t size, std::align_val_t, const char* file, int line)
{
    return Malloc_dbg(file, line, size);
}

void* operator new[](size_t size, std::align_val_t, const char* file, int line)
{
    if (size == 0) return &dummy;
    else return Malloc_dbg(file, line, size);
}
#endif // C++11

int debug_new_counter_t::count = 0; // initial value

#if defined(__CYGWIN__) || defined(INTERIX)

extern char**__argv;

#else

const char * __argv [] __attribute__((weak)) =
{
  "program"
};

#endif

const char * debug_new_counter_t::progname = __argv[0];

debug_new_counter_t::debug_new_counter_t()
{
  ++count;
}

debug_new_counter_t::~debug_new_counter_t()
{
  if (--count == 0) {
    check_mem_leak(progname);
  }
}

void debug_new_counter_t::set_program_name(const char * pgn)
{
  progname = pgn;
}

#endif // MEMORY_DEBUG
