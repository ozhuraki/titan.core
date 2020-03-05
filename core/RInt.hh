/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Kovacs, Ferenc
 *   Raduly, Csaba
 *
 ******************************************************************************/
#ifndef RInt_HH
#define RInt_HH

#include "Types.h"

struct bignum_st;
typedef bignum_st BIGNUM;

typedef int RInt;

class int_val_t
{
private:
  friend class INTEGER;
  friend class INTEGER_template;

  boolean native_flag;
  union {
    RInt native;
    BIGNUM *openssl;
  } val;

public:
  int_val_t();
  int_val_t(const int_val_t& v);
  explicit int_val_t(const char *s);
  explicit int_val_t(RInt v) : native_flag(TRUE) { val.native = v; }
  explicit int_val_t(BIGNUM *v) : native_flag(FALSE) { val.openssl = v; }
  ~int_val_t();
  /** Returns a newly allocated string. Caller must call Free() */
  char *as_string() const;
  const RInt& get_val() const;
  BIGNUM *get_val_openssl() const;
  double to_real() const;
  int_val_t operator&(RInt right) const;
  boolean operator==(const int_val_t& right) const;
  boolean operator<(const int_val_t& right) const;

  inline boolean operator!=(const int_val_t& right) const { return !(*this == right); }
  inline boolean operator>(const int_val_t& right) const { return *this != right && !(*this < right); }
  inline boolean operator>=(const int_val_t& right) const { return *this == right || *this > right; }
  inline boolean operator<=(const int_val_t& right) const { return *this == right || *this < right; }
  inline boolean operator==(RInt right) const { return *this == int_val_t(right); }
  inline boolean operator!=(RInt right) const { return !(*this == right); }
  inline boolean operator>(RInt right) const { return *this != right && !(*this < right); }
  inline boolean operator<(RInt right) const { return *this != right && *this < int_val_t(right); }
  inline boolean operator<=(RInt right) const { return *this == right || *this < right; }
  inline boolean operator>=(RInt right) const { return *this == right || *this > right; }

  int_val_t& operator=(const int_val_t& right);
  int_val_t& operator=(RInt right);
  int_val_t& operator+=(RInt right);
  int_val_t& operator<<=(RInt right);
  int_val_t& operator>>=(RInt right);
  inline boolean is_native() const { return native_flag; }
  boolean is_negative() const;
};

BIGNUM *to_openssl(RInt other_value);
RInt string2RInt(const char *s);

#endif  // RInt_HH
