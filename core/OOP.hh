/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Baranyi, Botond
 *
 ******************************************************************************/

#ifndef OOP_HH
#define OOP_HH

#include "Universal_charstring.hh"

// OBJECT
// ------

class OBJECT {
public:
  boolean operator==(const OBJECT&) const { return TRUE; }
  UNIVERSAL_CHARSTRING toString() const {
    return UNIVERSAL_CHARSTRING("Object");
  }
};

// OBJECT_REF
// ----------

template <typename T>
class OBJECT_REF {
public:
  struct object_ref_struct {
    T* obj_ptr;
    size_t ref_count;
  };
  
private:
  object_ref_struct* val_ptr; // NULL if it's a null reference
  
  void clean_up() {
    if (val_ptr != NULL) {
      --val_ptr->ref_count;
      if (val_ptr->ref_count == 0) {
        delete val_ptr->obj_ptr;
        delete val_ptr;
      }
      val_ptr = NULL;
    }
  }
  
public:
  OBJECT_REF(): val_ptr(NULL) {} // constructor for null reference
  
  OBJECT_REF(T* p_obj_ptr): val_ptr(new object_ref_struct) { // constructor for new value (.create)
    val_ptr->obj_ptr = p_obj_ptr;
    val_ptr->ref_count = 1;
  }
  
  OBJECT_REF(const OBJECT_REF<T>& p_other): val_ptr(p_other.val_ptr) { // copy constructor
    if (val_ptr != NULL) {
      ++val_ptr->ref_count;
    }
  }
  
  ~OBJECT_REF() {
    clean_up();
  }
  
  OBJECT_REF& operator=(null_type) { // assignment operator for null reference
    clean_up();
  }
  
  OBJECT_REF& operator=(const OBJECT_REF<T>& p_other) { // assignment operator for actual reference
    clean_up();
    if (p_other.val_ptr != NULL) {
      val_ptr = p_other.val_ptr;
      ++val_ptr->ref_count;
    }
    return *this;
  }
  
  boolean operator==(const OBJECT_REF<T>& p_other) const { // equality operator
    if (val_ptr == p_other.val_ptr) return TRUE;
    if (val_ptr == NULL || p_other.val_ptr == NULL) return FALSE;
    return *val_ptr->obj_ptr == *p_other.val_ptr->obj_ptr;
  }
  
  boolean operator!=(const OBJECT_REF<T>& p_other) const { // inequality operator
    return !(*this == p_other);
  }
  
  T* operator*() { // de-referencing operator
    if (val_ptr != NULL) {
      return val_ptr->obj_ptr;
    }
    TTCN_error("Accessing a null reference.");
  }
  
  T* operator->() { // de-referencing operator (for methods)
    if (val_ptr != NULL) {
      return val_ptr->obj_ptr;
    }
    TTCN_error("Accessing a null reference.");
  }
  
  const T* operator->() const { // de-referencing operator (for constant methods)
    if (val_ptr != NULL) {
      return val_ptr->obj_ptr;
    }
    TTCN_error("Accessing a null reference.");
  }
};

// EXCEPTION
// ---------

template<typename T, int type_id>
class EXCEPTION {
public:
  struct exception_struct {
    T* val_ptr;
    size_t ref_count;
  };
private:
  exception_struct* ex_ptr;
  EXCEPTION operator=(const EXCEPTION&); // assignment disabled
public:
  EXCEPTION(T* p_value): ex_ptr(new exception_struct) {
    ex_ptr->val_ptr = p_value;
    ex_ptr->ref_count = 1;
  }
  EXCEPTION(const EXCEPTION& p): ex_ptr(p.ex_ptr) {
    ++ex_ptr->ref_count;
  }
  ~EXCEPTION() {
    --ex_ptr->ref_count;
    if (ex_ptr->ref_count == 0) {
      delete ex_ptr->val_ptr;
      delete ex_ptr;
    }
  }
  T& operator()() { return *ex_ptr->val_ptr; }
};

#endif /* OOP_HH */

