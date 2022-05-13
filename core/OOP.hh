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


// OBJECT_REF
// ----------

template <typename T>
class OBJECT_REF
#ifdef TITAN_RUNTIME_2
  : public Base_Type
#endif
{
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
  
  const T* operator*() const { // de-referencing operator (for OBJECT::equals)
    return ptr;
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
    return TRUE;
  }
  
  boolean is_value() const {
    return TRUE;
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
  
  template<typename T2>
  OBJECT_REF<T2>* cast_to_dyn() const {
    if (ptr == NULL) {
      TTCN_error("Internal error: Casting a null reference");
    }
    T2* new_ptr = dynamic_cast<T2*>(ptr);
    if (new_ptr == NULL) {
      TTCN_error("Internal error: Invalid casting to class type `%s'", T2::class_name());
    }
    return new OBJECT_REF<T2>(new_ptr);
  }

#ifdef TITAN_RUNTIME_2

  boolean is_equal(const Base_Type*) const {
    TTCN_error("Internal error: OBJECT_REF::is_equal called");
    return false;
  }

  void set_value(const Base_Type*) {
    TTCN_error("Internal error: OBJECT_REF::set_value called");
  }

  Base_Type* clone() const {
    TTCN_error("Internal error: OBJECT_REF::clone called");
    return NULL;
  }

  void set_param(Module_Param&) {
    TTCN_error("Internal error: OBJECT_REF::set_param called");
  }

  Module_Param* get_param(Module_Param_Name&) const {
    TTCN_error("Internal error: OBJECT_REF::get_param called");
    return NULL;
  }

  boolean is_seof() const {
    TTCN_error("Internal error: OBJECT_REF::is_seof called");
    return FALSE;
  }

  void encode_text(Text_Buf&) const {
    TTCN_error("Internal error: OBJECT_REF::encode_text called");
  }

  void decode_text(Text_Buf&) {
    TTCN_error("Internal error: OBJECT_REF::decode_text called");
  }

  const TTCN_Typedescriptor_t* get_descriptor() const {
    TTCN_error("Internal error: OBJECT_REF::get_descriptor called");
    return NULL;
  }

#endif
};

template<typename T>
boolean operator==(null_type, const OBJECT_REF<T>& right_val) { // equality operator (with null reference, inverted)
  return right_val.ptr == NULL;
}

template<typename T>
boolean operator!=(null_type, const OBJECT_REF<T>& right_val) { // inequality operator (with null reference, inverted)
  return right_val.ptr != NULL;
}

// OBJECT
// ------

class CLASS_BASE {
  size_t ref_count;
  boolean destructor; // true, if the destructor is currently running;
  // also makes sure the object is not deleted again when inside the destructor

  CLASS_BASE(const CLASS_BASE&); // copy disabled
  CLASS_BASE operator=(const CLASS_BASE&); // assignment disabled
  boolean operator==(const CLASS_BASE&); // equality operator disabled
public:
  CLASS_BASE(): ref_count(0), destructor(FALSE) {}
  virtual ~CLASS_BASE() {
    if (ref_count != 0) {
      TTCN_error("Internal error: deleting an object with %lu reference(s) left.", ref_count);
    }
  }
  virtual void add_ref() { ++ref_count; }
  virtual boolean remove_ref() {
    --ref_count;
    if (destructor) {
      return FALSE;
    }
    destructor = ref_count == 0;
    return destructor;
  }
};

class OBJECT : public CLASS_BASE {
private:
  OBJECT(const OBJECT&); // copy disabled
  OBJECT operator=(const OBJECT&); // assignment disabled
  boolean operator==(const OBJECT&); // equality operator disabled
public:
  OBJECT(): CLASS_BASE() { }
  virtual ~OBJECT() { }
  virtual void log() const {
    TTCN_Logger::log_event_str("object: { }");
  }
  virtual UNIVERSAL_CHARSTRING toString() {
    return UNIVERSAL_CHARSTRING("Object");
  }
  virtual BOOLEAN equals(const OBJECT_REF<OBJECT>& obj) {
    return *obj == this;
  }
  static const char* class_name() { return "object"; }
};

// EXCEPTION
// ---------

class EXCEPTION_BASE {
protected:
  CHARSTRING exception_log; // this is set by the EXCEPTION class
public:
  EXCEPTION_BASE() { }
  EXCEPTION_BASE(const EXCEPTION_BASE& p): exception_log(p.exception_log) { }
  virtual ~EXCEPTION_BASE() { }
  CHARSTRING get_log() const { return exception_log; }
  virtual boolean is_class() const { return FALSE; }
  virtual boolean is_type(const char* p_type_name) const { return FALSE; }
  virtual OBJECT* get_object() const { return NULL; }
  virtual OBJECT_REF<OBJECT> get_object_ref() { return OBJECT_REF<OBJECT>(); }
};

template<typename T>
class NON_CLASS_EXCEPTION : public EXCEPTION_BASE {
public:
  struct exception_struct {
    T* val_ptr;
    size_t ref_count;
  };
private:
  exception_struct* exc_ptr;
  const char* type_name;
  NON_CLASS_EXCEPTION operator=(const NON_CLASS_EXCEPTION&); // assignment disabled
public:
  NON_CLASS_EXCEPTION(T* p_value, const char* p_type_name)
  : EXCEPTION_BASE(), exc_ptr(new exception_struct), type_name(p_type_name) {
    exc_ptr->val_ptr = p_value;
    exc_ptr->ref_count = 1;
    exception_log = (TTCN_Logger::begin_event_log2str(),
      p_value->log(), TTCN_Logger::end_event_log2str());
  }
  NON_CLASS_EXCEPTION(const NON_CLASS_EXCEPTION& p)
    : EXCEPTION_BASE(p), exc_ptr(p.exc_ptr), type_name(p.type_name) {
    ++exc_ptr->ref_count;
  }
  virtual ~NON_CLASS_EXCEPTION() {
    --exc_ptr->ref_count;
    if (exc_ptr->ref_count == 0) {
      delete exc_ptr->val_ptr;
      delete exc_ptr;
    }
  }
  T& operator()() { return *exc_ptr->val_ptr; }
  virtual boolean is_type(const char* p_type_name) const {
    return strcmp(p_type_name, type_name) == 0;
  }
};

template<typename T>
class CLASS_EXCEPTION : public EXCEPTION_BASE {
public:
  struct exception_struct {
    OBJECT_REF<T>* val_ptr;
    size_t ref_count;
  };
private:
  exception_struct* exc_ptr;
  CLASS_EXCEPTION operator=(const CLASS_EXCEPTION&); // assignment disabled
public:
  CLASS_EXCEPTION(OBJECT_REF<T>* p_value)
  : EXCEPTION_BASE(), exc_ptr(new exception_struct) {
    exc_ptr->val_ptr = p_value;
    exc_ptr->ref_count = 1;
    exception_log = (TTCN_Logger::begin_event_log2str(),
      p_value->log(), TTCN_Logger::end_event_log2str());
  }
  CLASS_EXCEPTION(const CLASS_EXCEPTION& p)
    : EXCEPTION_BASE(p), exc_ptr(p.exc_ptr) {
    ++exc_ptr->ref_count;
  }
  virtual ~CLASS_EXCEPTION() {
    --exc_ptr->ref_count;
    if (exc_ptr->ref_count == 0) {
      delete exc_ptr->val_ptr;
      delete exc_ptr;
    }
  }
  OBJECT_REF<T>& operator()() { return *exc_ptr->val_ptr; }
  virtual boolean is_class() const { return TRUE; }
  virtual OBJECT* get_object() const {
    return **exc_ptr->val_ptr;
  }
  virtual OBJECT_REF<OBJECT> get_object_ref() {
    return exc_ptr->val_ptr->template cast_to<OBJECT>();
  }
};


#if __cplusplus >= 201103L
class FINALLY_CPP11
{
  std::function<void(void)> functor;
public:
  FINALLY_CPP11(const std::function<void(void)> &p_functor) : functor(p_functor) {}
  ~FINALLY_CPP11()
  {
    try {
      functor();
    }
    catch (...) {
      fprintf(stderr, "Unhandled exception or dynamic test case error in a finally block. Terminating application.\n");
      exit(EXIT_FAILURE);
    }
  }
};
#endif // C++11

template<typename T>
class DEFPAR_IN_WRAPPER {
protected:
  boolean def_;
  T def_val;
  const T& act_val;
public:
  DEFPAR_IN_WRAPPER(): def_(TRUE), def_val(), act_val(def_val) { }
  DEFPAR_IN_WRAPPER(const T& par): def_(FALSE), def_val(), act_val(par) { }
  DEFPAR_IN_WRAPPER(const DEFPAR_IN_WRAPPER& par): def_(par.def_), def_val(), act_val(def_ ? def_val : par.act_val) { }
};

template<typename T>
class DEFPAR_OUT_WRAPPER {
protected:
  boolean def_;
  T dummy;
  T& act_val;
public:
  DEFPAR_OUT_WRAPPER(): def_(TRUE), dummy(), act_val(dummy) { }
  DEFPAR_OUT_WRAPPER(T& par): def_(FALSE), dummy(), act_val(par) { }
  DEFPAR_OUT_WRAPPER(const DEFPAR_OUT_WRAPPER& par): def_(par.def_), dummy(), act_val(par.act_val) { }
};

#endif /* OOP_HH */

