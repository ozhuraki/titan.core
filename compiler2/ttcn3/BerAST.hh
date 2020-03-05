/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Zalanyi, Balazs Andor
 *
 ******************************************************************************/
#ifndef BER_AST_HH
#define BER_AST_HH



class BerAST {
  static const char* encode_string[2];
  static const char* decode_string[5];
public:

  enum ber_encode_t {
    CER,
    DER
  };

  enum ber_decode_t {
    LENGTH_ACCEPT_SHORT,
    LENGTH_ACCEPT_LONG,
    LENGTH_ACCEPT_INDEFINITE,
    LENGTH_ACCEPT_DEFINITE,
    ACCEPT_ALL
  };

  ber_encode_t encode_param;
  ber_decode_t decode_param;

  BerAST();

  const char* get_encode_str();
  const char* get_decode_str();
};

#endif
