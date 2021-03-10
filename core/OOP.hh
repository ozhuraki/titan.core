/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
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
#include <functional>

// OBJECT
// ------

class OBJECT {
private:
  size_t ref_count;
  boolean destructor; // true, if the destructor is currently running;
  // also makes sure the object is not deleted again when inside the destructor
  
  OBJECT(const OBJECT&); // copy disabled
  OBJECT operator=(const OBJECT&); // assignment disabled
  boolean operator==(const OBJECT&); // equality operator disabled
public:
  OBJECT(): ref_count(0), destructor(FALSE) {}
  virtual ~OBJECT() {
    if (ref_count != 0) {
      TTCN_error("Internal error: deleting an object with %lu reference(s) left.", ref_count);
    }
  }
  void add_ref() { ++ref_count; }
  boolean remove_ref() {
    --ref_count;
    if (destructor) {
      return FALSE;
    }
    destructor = ref_count == 0;
    return destructor;
  }
  virtual void log() const {
    TTCN_Logger::log_event_str("object: { }");
  }
  virtual UNIVERSAL_CHARSTRING toString() {
    return UNIVERSAL_CHARSTRING("Object");
  }
  static const char* class_name() { return "object"; }
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
  
  template <typename T2>
  OBJECT_REF(OBJECT_REF<T2>& p_other) {
    if (p_other != NULL_VALUE) {
      ptr = dynamic_cast<T*>(*p_other);
      if (ptr != NULL) {
        ptr->add_ref();
      }
      else {
        TTCN_error("Invalid dynamic type of initial value.");
      }
    }
    else {
      ptr = NULL;
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
  
  template <typename T2>
  OBJECT_REF& operator=(OBJECT_REF<T2>& p_other) {
    clean_up();
    if (p_other != NULL_VALUE) {
      ptr = dynamic_cast<T*>(*p_other);
      if (ptr != NULL) {
        ptr->add_ref();
      }
      else {
        TTCN_error("Invalid dynamic type of assigned value.");
      }
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
  
  template<typename T2>
  OBJECT_REF<T2> cast_to() const {
    if (ptr == NULL) {
      TTCN_error("Casting a null reference");
    }
    T2* new_ptr = dynamic_cast<T2*>(ptr);
    if (new_ptr == NULL) {
      TTCN_error("Invalid casting to class type `%s'", T2::class_name());
    }
    return OBJECT_REF<T2>(new_ptr);
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

class EXCEPTION_BASE {
private:
  const char** exc_types;
  size_t nof_types;
protected:
  CHARSTRING exception_log; // this is set by the EXCEPTION class
public:
  EXCEPTION_BASE(const char** p_exc_types, size_t p_nof_types)
  : exc_types(p_exc_types), nof_types(p_nof_types) { }
  EXCEPTION_BASE(const EXCEPTION_BASE& p)
  : exc_types(new const char*[p.nof_types]), nof_types(p.nof_types), exception_log(p.exception_log) {
    for (size_t i = 0; i < nof_types; ++i) {
      exc_types[i] = p.exc_types[i];
    }
  }
  ~EXCEPTION_BASE() {
    delete exc_types;
  }
  CHARSTRING get_log() const { return exception_log; }
  boolean has_type(const char* type_name) const {
    for (size_t i = 0; i < nof_types; ++i) {
      if (strcmp(exc_types[i], type_name) == 0) {
        return TRUE;
      }
    }
    return FALSE;
  }
};

template<typename T>
class EXCEPTION : public EXCEPTION_BASE {
public:
  struct exception_struct {
    T* val_ptr;
    size_t ref_count;
  };
private:
  exception_struct* exc_ptr;
  EXCEPTION operator=(const EXCEPTION&); // assignment disabled
public:
  EXCEPTION(T* p_value, const char** p_exc_types, size_t p_nof_types)
    : EXCEPTION_BASE(p_exc_types, p_nof_types), exc_ptr(new exception_struct) {
    exc_ptr->val_ptr = p_value;
    exc_ptr->ref_count = 1;
    exception_log = (TTCN_Logger::begin_event_log2str(),
      p_value->log(), TTCN_Logger::end_event_log2str());
  }
  EXCEPTION(const EXCEPTION& p)
    : EXCEPTION_BASE(p), exc_ptr(p.exc_ptr) {
    ++exc_ptr->ref_count;
  }
  ~EXCEPTION() {
    --exc_ptr->ref_count;
    if (exc_ptr->ref_count == 0) {
      delete exc_ptr->val_ptr;
      delete exc_ptr;
    }
  }
  T& operator()() { return *exc_ptr->val_ptr; }
};

class FINALLY
{
  std::function<void(void)> functor;
public:
  FINALLY(const std::function<void(void)> &p_functor) : functor(p_functor) {}
  ~FINALLY()
  {
    functor();
  }
};

#endif /* OOP_HH */

