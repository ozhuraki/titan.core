/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               EPTF_CLL_Buffer_Functions.cc
//  Description:        Implementation of the external functions of EPTF Buffer
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 512
//  Updated:            2010-06-29
//  Contact:            http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////
#include "EPTF_CLL_Buffer_Functions.hh"

namespace EPTF__CLL__Buffer__Functions {


void f_EPTF_Buffer_warning(const char* message) {
   TTCN_Logger::begin_event(TTCN_WARNING);
   TTCN_Logger::log_event_str("EPTF_Buffer: Warning: ");
   TTCN_Logger::log_event_str(message);
   TTCN_Logger::end_event();
}

int v_EPTF_numberOfBuffers = 0;
int v_EPTF_sizeOfBuffers = 0;

typedef TTCN_Buffer* TTCN_BufferPtr;
TTCN_BufferPtr* v_EPTF_buffers=NULL;

void f__EPTF__Buffer__init() {
  f__EPTF__Buffer__cleanup();
}

void f__EPTF__Buffer__cleanup() {
  if (v_EPTF_buffers!=NULL) {
    for(int i=0;i<v_EPTF_numberOfBuffers;i++) {
      if(v_EPTF_buffers[i] != NULL) delete v_EPTF_buffers[i];
    }
    Free(v_EPTF_buffers);
    v_EPTF_buffers = NULL;
  }
  v_EPTF_numberOfBuffers = 0;
  v_EPTF_sizeOfBuffers = 0;
}

#define EPTF_BUFFER_GROWBY 16

INTEGER f__EPTF__Buffer__new() {
  
  v_EPTF_numberOfBuffers++; // note: if f__EPTF__Buffer__delete is implemented, this counter must not be decremented!

  //printf("v_EPTF_numberOfBuffers: %i \n\r",v_EPTF_numberOfBuffers);

  if(v_EPTF_numberOfBuffers<v_EPTF_sizeOfBuffers)
  {
    v_EPTF_buffers[v_EPTF_numberOfBuffers-1] = new TTCN_Buffer;
    return v_EPTF_numberOfBuffers-1;
  }
  else
  {
/*    for(int i=0; i<v_EPTF_sizeOfBuffers; i++) {
      if (v_EPTF_buffers[i] == NULL)
      {
        v_EPTF_buffers[i]=new TTCN_Buffer;
        return i;
      }
    }*/ // FIXME: may be needed when f__EPTF__Buffer__delete is implemented
    // create a new array for TTCN_Buffers with new size (size+1)
    v_EPTF_sizeOfBuffers += EPTF_BUFFER_GROWBY;
    if(v_EPTF_buffers==NULL) v_EPTF_buffers = (TTCN_BufferPtr*)Malloc(v_EPTF_sizeOfBuffers * sizeof(TTCN_BufferPtr));
    else v_EPTF_buffers = (TTCN_BufferPtr*)Realloc(v_EPTF_buffers, v_EPTF_sizeOfBuffers * sizeof(TTCN_BufferPtr));
    v_EPTF_buffers[v_EPTF_numberOfBuffers-1] = new TTCN_Buffer;
    for(int i=v_EPTF_numberOfBuffers; i<v_EPTF_sizeOfBuffers; i++) {
      v_EPTF_buffers[i] = NULL;
    }
    return v_EPTF_numberOfBuffers-1;
  }
  //never reached
  return -1;
}

void f__EPTF__Buffer__clear(const INTEGER& pl__bufferId) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__clear: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return;
  }
  v_EPTF_buffers[pl__bufferId]->clear();
}

void f__EPTF__Buffer__rewind(const INTEGER& pl__bufferId) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__rewind: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return;
  }
  v_EPTF_buffers[pl__bufferId]->rewind();
}

INTEGER f__EPTF__Buffer__get__pos(const INTEGER& pl__bufferId) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__get__pos: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return -1;
  }
  return v_EPTF_buffers[pl__bufferId]->get_pos();
}

void f__EPTF__Buffer__set__pos(const INTEGER& pl__bufferId, const INTEGER& pl__pos) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__set__pos: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return;
  }
  v_EPTF_buffers[pl__bufferId]->set_pos(pl__pos);
}

INTEGER f__EPTF__Buffer__get__len(const INTEGER& pl__bufferId) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__get__len: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return -1;
  }
  return v_EPTF_buffers[pl__bufferId]->get_len();
}

void f__EPTF__Buffer__get__data(const INTEGER& pl__bufferId, OCTETSTRING& pl__data) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__get__data: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return;
  }
  pl__data = OCTETSTRING(v_EPTF_buffers[pl__bufferId]->get_len(),v_EPTF_buffers[pl__bufferId]->get_data());
}

INTEGER f__EPTF__Buffer__get__read__len(const INTEGER& pl__bufferId) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__get__read__len: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return -1;
  }
  return v_EPTF_buffers[pl__bufferId]->get_read_len();
}

void f__EPTF__Buffer__get__read__data(const INTEGER& pl__bufferId, OCTETSTRING& pl__data) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__get__read__data: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return;
  }
  pl__data = OCTETSTRING(v_EPTF_buffers[pl__bufferId]->get_read_len(),v_EPTF_buffers[pl__bufferId]->get_read_data());
}

void f__EPTF__Buffer__put__os(const INTEGER& pl__bufferId, const OCTETSTRING& pl__data) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__put__os: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return;
  }
 v_EPTF_buffers[pl__bufferId]->put_os(pl__data);
}

void f__EPTF__Buffer__increase__length(const INTEGER& pl__bufferId, const INTEGER& pl__count) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__increase__length: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return;
  }
  v_EPTF_buffers[pl__bufferId]->increase_length(pl__count);
}

void f__EPTF__Buffer__cut(const INTEGER& pl__bufferId) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__cut: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return;
  }
  v_EPTF_buffers[pl__bufferId]->cut();
}

void f__EPTF__Buffer__cut__end(const INTEGER& pl__bufferId) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__cut__end: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return;
  }
  v_EPTF_buffers[pl__bufferId]->cut_end();
}

BOOLEAN f__EPTF__Buffer__contains__complete__TLV(const INTEGER& pl__bufferId) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__contains__complete__TLV: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return false;
  }
  return v_EPTF_buffers[pl__bufferId]->contains_complete_TLV();
}

void f__EPTF__Buffer__getDataFromOffset(const INTEGER& pl__bufferId, const INTEGER& pl__pos, const INTEGER& pl__count, OCTETSTRING& pl__data) {
  if (pl__bufferId>=v_EPTF_numberOfBuffers) {
    f_EPTF_Buffer_warning(CHARSTRING("f__EPTF__Buffer__get__read__data: Buffer with id ")+int2str(pl__bufferId)+" does not exists.");
    return;
  }
  unsigned int pos = ((unsigned int)pl__pos>v_EPTF_buffers[pl__bufferId]->get_len()?v_EPTF_buffers[pl__bufferId]->get_len():(unsigned int)pl__pos);
  if (pos<0) {
    pos=0;
  }
  unsigned int count = (pos+(unsigned int)pl__count>v_EPTF_buffers[pl__bufferId]->get_len()?v_EPTF_buffers[pl__bufferId]->get_len()-pos:(unsigned int)pl__count);
  if (count<0) {
    count=0;
  }
  pl__data = OCTETSTRING(count,v_EPTF_buffers[pl__bufferId]->get_data()+pos);
}

} //namespace EPTF__CLL__Buffer__Functions
