/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
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
#include "Logger.hh"

// OBJECT
// ------

class OBJECT {
private:
  size_t ref_count;
  
  OBJECT(const OBJECT&); // copy disabled
  OBJECT operator=(const OBJECT&); // assignment disabled
  boolean operator==(const OBJECT&); // equality operator disabled
public:
  OBJECT(): ref_count(0) {}
  virtual ~OBJECT() {
    if (ref_count != 0) {
      TTCN_error("Internal error: deleting an object with %lu reference(s) left.", ref_count);
    }
  }
  void add_ref() { ++ref_count; }
  boolean remove_ref() {
    --ref_count;
    return ref_count == 0;
  }
  virtual void log() const {
    TTCN_Logger::log_event_str("object: { }");
  }
  virtual UNIVERSAL_CHARSTRING toString() {
    return UNIVERSAL_CHARSTRING("Object");
  }
};

// OBJECT_REF
// ----------

template <typename T>
class OBJECT_REF {
  template <typename T2>
  friend boolean operator==(null_type, const OBJECT_REF<T2>& right_val);
  template <typename T2>
  friend boolean operator!=(null_type, const OBJECT_REF<T2>& right_val);
private:
  T* ptr; // NULL if it's a null reference
  
public:
  OBJECT_REF(): ptr(NULL) {} // constructor with no parameters

  OBJECT_REF(null_type): ptr(NULL) {} // constructor for null reference
  
  OBJECT_REF(T* p_ptr): ptr(p_ptr) { // constructor for new value (.create)
    ptr->add_ref();
  }
  
  OBJECT_REF(const OBJECT_REF<T>& p_other): ptr(p_other.ptr) { // copy constructor
    if (ptr != NULL) {
      ptr->add_ref();
    }
  }
  
  void clean_up() {
    if (ptr != NULL) {
      if (ptr->remove_ref()) {
        delete ptr;
      }
      ptr = NULL;
    }
  }
  
  ~OBJECT_REF() {
    clean_up();
  }
  
  OBJECT_REF& operator=(null_type) { // assignment operator for null reference
    clean_up();
    return *this;
  }
  
  OBJECT_REF& operator=(const OBJECT_REF<T>& p_other) { // assignment operator for actual reference
    clean_up();
    if (p_other.ptr != NULL) {
      ptr = p_other.ptr;
      ptr->add_ref();
    }
    return *this;
  }
  
  boolean operator==(null_type) const { // equality operator (with null reference)
    return ptr == NULL;
  }
  
  boolean operator!=(null_type) const { // inequality operator (with null reference)
    return ptr != NULL;
  }
  
  boolean operator==(const OBJECT_REF<T>& p_other) const { // equality operator (with actual reference)
    return ptr == p_other.ptr;
  }
  
  boolean operator!=(const OBJECT_REF<T>& p_other) const { // inequality operator (with actual reference)
    return ptr != p_other.ptr;
  }
  
  T* operator*() { // de-referencing operator
    if (ptr != NULL) {
      return ptr;
    }
    TTCN_error("Accessing a null reference.");
  }
  
  T* operator->() { // de-referencing operator (for methods)
    if (ptr != NULL) {
      return ptr;
    }
    TTCN_error("Accessing a null reference.");
  }
  
  const T* operator->() const { // de-referencing operator (for constant methods)
    if (ptr != NULL) {
      return ptr;
    }
    TTCN_error("Accessing a null reference.");
  }
  
  void log() const {
    if (ptr == NULL) {
      TTCN_Logger::log_event_str("null");
    }
    else {
      ptr->log();
    }
  }
  
  boolean is_bound() const {
    return ptr != NULL;
  }
  
  boolean is_value() const {
    return ptr != NULL;
  }
  
  boolean is_present() const {
    return ptr != NULL;
  }
};

template<typename T>
boolean operator==(null_type, const OBJECT_REF<T>& right_val) { // equality operator (with null reference, inverted)
  return right_val.ptr == NULL;
}

template<typename T>
boolean operator!=(null_type, const OBJECT_REF<T>& right_val) { // inequality operator (with null reference, inverted)
  return right_val.ptr != NULL;
}

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

