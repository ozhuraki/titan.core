/******************************************************************************
 * Copyright (c) 2000-2018 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Baji, Laszlo
 *   Balasko, Jeno
 *   Baranyi, Botond
 *   Szabo, Bence Janos
 *
 ******************************************************************************/
#include "JSON.hh"
#include "../common/memory.h"
#include "Integer.hh"
#include "Float.hh"
#include "Bitstring.hh"
#include "Hexstring.hh"
#include "Octetstring.hh"
#include "Charstring.hh"
#include "Universal_charstring.hh"
#include "Addfunc.hh"

#include <openssl/bn.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// JSON descriptors for base types
const TTCN_JSONdescriptor_t INTEGER_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t FLOAT_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t BOOLEAN_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t BITSTRING_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t HEXSTRING_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t OCTETSTRING_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t CHARSTRING_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t UNIVERSAL_CHARSTRING_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t VERDICTTYPE_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t GeneralString_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t NumericString_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t UTF8String_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t PrintableString_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t UniversalString_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t BMPString_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t GraphicString_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t IA5String_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t TeletexString_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t VideotexString_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t VisibleString_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t ASN_NULL_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t OBJID_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t ASN_ROID_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t ASN_ANY_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };

const TTCN_JSONdescriptor_t ENUMERATED_json_ = { FALSE, NULL, FALSE, NULL, FALSE, FALSE, FALSE };



////////////////////////////////////////////////////////////////////////////////
//// CBOR conversion
////////////////////////////////////////////////////////////////////////////////

// Never use buff.get_read_data() without checking if it has enough bytes in the
// buffer.
const unsigned char* check_and_get_buffer(const TTCN_Buffer& buff, int bytes) {
  if (bytes < 0) {
    TTCN_error("Incorrect length byte received: %d, while decoding using cbor2json()", bytes);
  }
  if (buff.get_pos() + bytes > buff.get_len()) {
    TTCN_error("Not enough bytes in bytestream while decoding using cbor2json().");
  }
  return buff.get_read_data();
}

void encode_ulong_long_int_cbor(TTCN_Buffer& buff, int bytes, unsigned long long int value) {
  for (int i = bytes - 1; i >= 0; i--) {
    buff.put_c(static_cast<unsigned char>((value >> i*8)));
  }
}


// major_type parameter needed for the string encoding
void encode_int_cbor(TTCN_Buffer& buff, int major_type, INTEGER& int_num) {
  bool is_negative = false;
  int_val_t num = int_num.get_val();
  if (num.is_negative()) {
    major_type = 1 << 5;
    int_num = (int_num * -1) - 1;
    num = int_num.get_val();
    is_negative = true;
  }
  if (num.is_native()) {
    unsigned int uns_num = num.get_val();
    if (uns_num <= 23) {
      buff.put_c(static_cast<unsigned char>(major_type + uns_num));
    } else if (uns_num <= 0xFF) { // 8 bit
      buff.put_c(static_cast<unsigned char>(major_type + 24));
      encode_ulong_long_int_cbor(buff, 1, uns_num);
    } else if (uns_num <= 0xFFFF) { // 16 bit
      buff.put_c(static_cast<unsigned char>(major_type + 25));
      encode_ulong_long_int_cbor(buff, 2, uns_num);
    } else if (uns_num <= 0xFFFFFFFF) { // 32 bit
      buff.put_c(static_cast<unsigned char>(major_type + 26));
      encode_ulong_long_int_cbor(buff, 4, uns_num);
    }
  } else {
    BIGNUM* bn = BN_dup(int_num.get_val().get_val_openssl());
    INTEGER bn_length = BN_num_bytes(bn);
    long long int long_int = int_num.get_long_long_val();
    if (bn_length <= 4) { // 32 bit
      buff.put_c(static_cast<unsigned char>(major_type + 26));
      encode_ulong_long_int_cbor(buff, 4, long_int);
    } else if (bn_length <= 8) {
      buff.put_c(static_cast<unsigned char>(major_type + 27));
      encode_ulong_long_int_cbor(buff, 8, long_int);
    } else {
      // It is a bignum. Encode as bytestring
      major_type = 6 << 5; // Major type 6 for bignum
      major_type += is_negative ? 3 : 2; // Tag 2 or 3 for negative
      buff.put_c(static_cast<unsigned char>(major_type));
      major_type = 2 << 5; // Major type 2 for bytestring
      encode_int_cbor(buff, major_type, bn_length); // encode the length of the bignum
      size_t buff_len = bn_length.get_val().get_val();
      unsigned char* tmp_num = static_cast<unsigned char*>(Malloc(buff_len*sizeof(unsigned char)));
      BN_bn2bin(bn, tmp_num);
      buff.put_s(buff_len, reinterpret_cast<const unsigned char*>(tmp_num));
      Free(tmp_num);
    }
    BN_free(bn);
  }
}


void decode_int_cbor(TTCN_Buffer& buff, int bytes, INTEGER& value) {
  const unsigned char* tmp = check_and_get_buffer(buff, bytes);
  TTCN_Buffer tmp_buf;
  tmp_buf.put_s(bytes, tmp);
  OCTETSTRING os;
  tmp_buf.get_string(os);
  value = oct2int(os);
  buff.increase_pos(bytes);
}

void decode_uint_cbor(TTCN_Buffer& buff, int bytes, unsigned int& value) {
  value = 0;
  const unsigned char* tmp = check_and_get_buffer(buff, bytes);
  for (int i = bytes - 1; i >= 0; i--) {
    value += *(tmp) << i*8;
    tmp++;
  }
  buff.increase_pos(bytes);
}

void decode_ulong_long_int_cbor(TTCN_Buffer& buff, int bytes, unsigned long long int& value) {
  value = 0;
  const unsigned char* tmp = check_and_get_buffer(buff, bytes);
  for (int i = bytes - 1; i >= 0; i--) {
    value += *(tmp) << i*8;
    tmp++;
  }
  buff.increase_pos(bytes);
}

void decode_integer_cbor(TTCN_Buffer& buff, int minor_type, INTEGER& result) {
  if (minor_type <= 23) {
    result = minor_type;
  } else if (minor_type == 24) { // A number in 8 bits
    unsigned int num;
    decode_uint_cbor(buff, 1, num);
    result = num;
  } else if (minor_type == 25) { // A number in 16 bits
    unsigned int num;
    decode_uint_cbor(buff, 2, num);
    result = num;
  } else if (minor_type == 26) { // A number in 32 bits
    unsigned int num;
    decode_uint_cbor(buff, 4, num);
    result = num;
  } else if (minor_type == 27) { // A number in 64 bits
    decode_int_cbor(buff, 8, result);
  }
}


void decode_bytestring_cbor(TTCN_Buffer& buff, JSON_Tokenizer& tok, int minor_type, int tag) {
  INTEGER length;
  decode_integer_cbor(buff, minor_type, length);
  const unsigned char* tmp = check_and_get_buffer(buff, length.get_val().get_val());
  OCTETSTRING os(length.get_val().get_val(), tmp);
  buff.increase_pos(length.get_val().get_val());
  CHARSTRING cs;
  if (tag == 22 || tag == 23 || tag == 2 || tag == 3) { // base64 or base64url or +-bigint
    cs = encode_base64(os);
    // The difference between the base64 and base64url encoding is that the
    // + is replaced with -, the / replaced with _ and the trailing = padding
    // is removed.
    if (tag != 22) { // if tag is not base64 >--> base64url
      const char* data = (const char*)cs;
      char* pch = strchr(const_cast<char*>(data),'+');
      while (pch!=NULL)
      {
        *pch = '-';
        pch=strchr(pch+1,'+');
      }
      pch = strchr(const_cast<char*>(data),'/');
      while (pch!=NULL)
      {
        *pch = '_';
        pch=strchr(pch+1,'/');
      }
      // Max 2 padding = is possible
      if (cs[cs.lengthof()-1] == "=") {
        cs = replace(cs, cs.lengthof()-1, 1, "");
      }
      if (cs[cs.lengthof()-1] == "=") {
        cs = replace(cs, cs.lengthof()-1, 1, "");
      }
    }    
  } else if (tag == 21) { // base16
    cs = oct2str(os);
  }
  // If the bignum encoded as bytestring is negative the tilde is needed before
  // the base64url encoding
  char* tmp_str = mprintf("\"%s%s\"", tag == 3 ? "~" : "", (const char*)cs);
  tok.put_next_token(JSON_TOKEN_STRING, tmp_str);
  Free(tmp_str);
}

// RAW descriptor for raw encoding
TTCN_RAWdescriptor_t cbor_float_raw_ = {64,SG_NO,ORDER_LSB,ORDER_LSB,ORDER_LSB,ORDER_LSB,EXT_BIT_NO,ORDER_LSB,ORDER_LSB,TOP_BIT_INHERITED,0,0,0,8,0,NULL,-1,CharCoding::UNKNOWN};
const TTCN_Typedescriptor_t cbor_float_descr_ = { NULL, NULL, &cbor_float_raw_, NULL, NULL, NULL, NULL, NULL, TTCN_Typedescriptor_t::DONTCARE };


void json2cbor_coding(TTCN_Buffer& buff, JSON_Tokenizer& tok, size_t& num_of_items) {
  json_token_t token;
  char* content = NULL;
  size_t len;
  size_t prev_pos = tok.get_buf_pos();
  tok.get_next_token(&token, &content, &len);
  switch(token) {
    case JSON_TOKEN_NUMBER: {
      char *str = mcopystrn(content, len);
      size_t curr_pos = tok.get_buf_pos();
      tok.set_buf_pos(prev_pos);
      bool is_float = false;
      tok.check_for_number(&is_float);
      tok.set_buf_pos(curr_pos);
      if (is_float) {
        int c = 7 << 5; // Major type 7
        c += 27; // Minor type 27 (64 bit floating point) always
        buff.put_c(c);
        double d;
        sscanf(str, "%lf", &d);
        FLOAT f = d;
        f.encode(cbor_float_descr_, buff, TTCN_EncDec::CT_RAW);
      } else {
        int c = 0; // Major type 0
        INTEGER int_num = str2int(str);
        encode_int_cbor(buff, c, int_num);
      }
      Free(str);
      num_of_items++;
      break;
    }
    case JSON_TOKEN_STRING:
    case JSON_TOKEN_NAME: {
      int c = 3 << 5; // Major type 3
      INTEGER length = token == JSON_TOKEN_NAME ? len : len - 2; // 2 "-s
      encode_int_cbor(buff, c, length);
      char * str = mcopystrn(token == JSON_TOKEN_NAME ? content : content+1, length.get_val().get_val()); // Remove "-s
      buff.put_string(str);
      Free(str);
      num_of_items++;
      break;
    }
    case JSON_TOKEN_ARRAY_START: {
      int c = 4 << 5; // Major type 4
      size_t nof_items = 0;
      TTCN_Buffer sub_buff;
      while ((prev_pos = tok.get_buf_pos(), tok.get_next_token(&token, NULL, NULL))) {
        if (token != JSON_TOKEN_ARRAY_END) {
          tok.set_buf_pos(prev_pos);
          json2cbor_coding(sub_buff, tok, nof_items);
        } else {
          INTEGER num = nof_items;
          encode_int_cbor(buff, c, num);
          buff.put_buf(sub_buff);
          break;
        }
      }
      num_of_items++;
      break;
    }
    case JSON_TOKEN_ARRAY_END:
      TTCN_error("Unexpected array end character while encoding using json2cbor().");
      break;
    case JSON_TOKEN_OBJECT_START: {
      int c = 5 << 5; // Major type 5
      size_t nof_items = 0;
      TTCN_Buffer sub_buff;
      while ((prev_pos = tok.get_buf_pos(), tok.get_next_token(&token, NULL, NULL))) {
        if (token != JSON_TOKEN_OBJECT_END) { // todo hibas json eseten vegtelen ciklus?
          tok.set_buf_pos(prev_pos);
          json2cbor_coding(sub_buff, tok, nof_items);
        } else {
          INTEGER num = nof_items / 2; // num is the number of key-value pairs
          encode_int_cbor(buff, c, num);
          buff.put_buf(sub_buff);
          break;
        }
      }
      num_of_items++;
      break;
    }
    case JSON_TOKEN_OBJECT_END:
      TTCN_error("Unexpected object end character while encoding using json2cbor().");
    case JSON_TOKEN_LITERAL_FALSE:
    case JSON_TOKEN_LITERAL_TRUE:
    case JSON_TOKEN_LITERAL_NULL: {
      int c = 7 << 5; // Major type 7
      INTEGER i;
      if (token == JSON_TOKEN_LITERAL_FALSE) {
        i = 20;
      } else if (token == JSON_TOKEN_LITERAL_TRUE) {
        i = 21;
      } else if (token == JSON_TOKEN_LITERAL_NULL) {
        i = 22;
      }
      encode_int_cbor(buff, c, i);
      num_of_items++;
      break;
    }
    default:
      TTCN_error("Unexpected json token %i, while encoding using json2cbor().", token);
  }
}

void cbor2json_coding(TTCN_Buffer& buff, JSON_Tokenizer& tok, bool in_object) {
  unsigned char type = *(check_and_get_buffer(buff, 1));
  buff.increase_pos(1);
  int major_type = type >> 5; // First 3 bit of byte
  int minor_type = type & 0x1F; // Get the last 5 bits
  switch(major_type) {
    case 0: { // Integer
      INTEGER i;
      decode_integer_cbor(buff, minor_type, i);
      if (i.is_native()) {
        char* tmp_str = mprintf("%u", i.get_val().get_val());
        tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
        Free(tmp_str);
      } else {
        char* tmp_str = i.get_val().as_string();
        tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
        Free(tmp_str);
      }
      break;
    }
    case 1: // Negative integer
      switch (minor_type) {
        case 24: { // Integer on 1 byte
          unsigned int num;
          decode_uint_cbor(buff, 1, num);
          char* tmp_str = mprintf("%d", (num+1)*-1);
          tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
          Free(tmp_str);
          break;
        }
        case 25: { // Integer on 2 byte
          unsigned int num;
          decode_uint_cbor(buff, 2, num);
          char* tmp_str = mprintf("%d", (num+1)*-1);
          tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
          Free(tmp_str);
          break;
        }
        case 26: { // Integer on 4 byte
          unsigned long long int num;
          decode_ulong_long_int_cbor(buff, 4, num);
          INTEGER i;
          i.set_long_long_val((num+1)*-1);
          char* tmp_str = i.get_val().as_string();
          tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
          Free(tmp_str);
          break;
        }
        case 27: { // Integer on 8 byte
          INTEGER i;
          decode_int_cbor(buff, 8, i);
          i = i + 1;
          i = i * -1;
          char* tmp_str = i.get_val().as_string();
          tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
          Free(tmp_str);
          break;
        }
        default:
          if (minor_type < 24) { // Integer  0 <= num <= 24
            char* tmp_str = mprintf("%d", (minor_type+1)*-1);
            tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
            Free(tmp_str);
          }
      }
      break;
    case 2: // Bytestring, encoded into a base64url json string
      decode_bytestring_cbor(buff, tok, minor_type, 23); // base64url by default
      break;
    case 3: { // String
      INTEGER length;
      decode_integer_cbor(buff, minor_type, length);
      const unsigned char* tmp = check_and_get_buffer(buff, length.get_val().get_val());
      
      char* json_str = mcopystrn((const char*)tmp, length.get_val().get_val());
      if (in_object) {
        tok.put_next_token(JSON_TOKEN_NAME, json_str);
      } else {
        char* tmp_str = mprintf("\"%s\"", json_str);
        tok.put_next_token(JSON_TOKEN_STRING, tmp_str);
        Free(tmp_str);
      }
      Free(json_str);
      buff.increase_pos(length.get_val().get_val());
      break;
    }
    case 4: { // Array
      tok.put_next_token(JSON_TOKEN_ARRAY_START, NULL);
      INTEGER num_of_items;
      decode_integer_cbor(buff, minor_type, num_of_items);
      for (INTEGER i = 0; i < num_of_items; i = i + 1) {
        cbor2json_coding(buff, tok, false);
      }
      tok.put_next_token(JSON_TOKEN_ARRAY_END, NULL);
      break;
    }
    case 5: { // Object
      tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
      INTEGER num_of_items;
      decode_integer_cbor(buff, minor_type, num_of_items);
      num_of_items = num_of_items * 2; // Number of all keys and values
      for (INTEGER i = 0; i < num_of_items; i = i + 1) {
        // whether to put : or , after the next token
        in_object = i.get_val().get_val() % 2 == 0;
        cbor2json_coding(buff, tok, in_object); 
      }
      tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
      break;
    }
    case 6: {
      int tag = minor_type;
      switch(tag) {
        case 2: // Positive bignum
        case 3: // Negative bignum
        case 21: // bytestring as base16
        case 22: // bytestring as base64
        case 23: // bytestring as base64url
          type = *(check_and_get_buffer(buff, 1));
          buff.increase_pos(1);
          minor_type = type & 0x1F; // Get the last 5 bits
          decode_bytestring_cbor(buff, tok, minor_type, tag);
          break;
        default:
          cbor2json_coding(buff, tok, in_object);
      }
      break;
    }
    case 7: // Other values
      switch (minor_type) {
        case 20: // False
          tok.put_next_token(JSON_TOKEN_LITERAL_FALSE, NULL);
          break;
        case 21: // True
          tok.put_next_token(JSON_TOKEN_LITERAL_TRUE, NULL);
          break;
        case 22: // NULL
        tok.put_next_token(JSON_TOKEN_LITERAL_NULL, NULL);
          break;
        case 25: { // Half precision float 16 bit
          // Decoding algorithm from the standard
          const unsigned char* halfp = check_and_get_buffer(buff, 2);
          buff.increase_pos(2);
          int half = (halfp[0] << 8) + halfp[1];
          int exp = (half >> 10) & 0x1f;
          int mant = half & 0x3ff;
          double val;
          if (exp == 0) val = ldexp(mant, -24);
          else if (exp != 31) val = ldexp(mant + 1024, exp - 25);
          else val = mant == 0 ? PLUS_INFINITY : NOT_A_NUMBER;
          val = half & 0x8000 ? -val : val;
          FLOAT f = val;
          f.JSON_encode(cbor_float_descr_, tok);
          break;
        }
        case 26: { // Single precision float 32bit
          OCTETSTRING os(4, check_and_get_buffer(buff, 4));
          buff.increase_pos(4);
          INTEGER i = oct2int(os);
          // Some non standard way but it is widely accepted
          union
          {
            unsigned int num;
            float fnum;
          } my_union;
          my_union.num = i.get_long_long_val();
          float f2 = my_union.fnum;
          FLOAT f = f2;
          f.JSON_encode(cbor_float_descr_, tok);
          break;
        }
        case 27: { // Double precision float 64bit
          cbor_float_raw_.fieldlength = 64;
          FLOAT f;
          OCTETSTRING os(8, check_and_get_buffer(buff, 8));
          INTEGER i = oct2int(os);
          if (i.get_long_long_val() != 0x7FF8000000000000ULL) { // NAN    
            f.decode(FLOAT_descr_, buff, TTCN_EncDec::CT_RAW);
            f.JSON_encode(cbor_float_descr_, tok);
          } else {
            tok.put_next_token(JSON_TOKEN_STRING, "\"not_a_number\"");
            buff.increase_pos(8);
          }
          break;
        }
        default: {
          // put the simple value into the the json
          if (minor_type >= 0 && minor_type <= 23) {
            char* tmp_str = mprintf("%d", minor_type);
            tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
            Free(tmp_str);
          } else if (minor_type == 24) { // The value is on the next byte
            int simple_value = *(buff.get_read_data());
            buff.increase_pos(1);
            char* tmp_str = mprintf("%d", simple_value);
            tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
            Free(tmp_str);
          }
        }
      }
      break;
    default:
      TTCN_error("Unexpected major type %i while decoding using cbor2json().", major_type);
  }
}


////////////////////////////////////////////////////////////////////////////////
////  BSON conversion
////////////////////////////////////////////////////////////////////////////////

const TTCN_RAWdescriptor_t bson_float_raw_ = {64,SG_NO,ORDER_MSB,ORDER_LSB,ORDER_LSB,ORDER_LSB,EXT_BIT_NO,ORDER_LSB,ORDER_LSB,TOP_BIT_INHERITED,0,0,0,8,0,NULL,-1,CharCoding::UNKNOWN};
const TTCN_Typedescriptor_t bson_float_descr_ = { NULL, NULL, &bson_float_raw_, NULL, NULL, NULL, NULL, NULL, TTCN_Typedescriptor_t::DONTCARE };

// Never use buff.get_read_data() without checking if it has enough bytes in the
// buffer.
const unsigned char* check_and_get_buffer_bson(const TTCN_Buffer& buff, int bytes) {
  if (bytes < 0) {
    TTCN_error("Incorrect length byte received: %d, while decoding using bson2json()", bytes);
  }
  if (buff.get_pos() + bytes > buff.get_len()) {
    TTCN_error("Not enough bytes in bytestream while decoding using bson2json().");
  }
  return buff.get_read_data();
}

void encode_int_bson(TTCN_Buffer& buff, const INTEGER& int_num, INTEGER& length) {
  if (int_num.is_native()) { // 32 bit
    length = length + 4;
    RInt value = (int)int_num;
    for (size_t i = 0; i < 4; i++) {
      buff.put_c(static_cast<unsigned char>(value >> i*8));
    }
  } else {
    BIGNUM* bn = BN_dup(int_num.get_val().get_val_openssl());
    INTEGER bn_length = BN_num_bytes(bn);
    BN_free(bn);
    long long int long_int = 0;
    int bytes = 0;
    if (bn_length <= 4) { // 32 bit
      bytes = 4;
      long_int = int_num.get_long_long_val();
    } else if (bn_length <= 8) { //64 bit
      bytes = 8;
      long_int = int_num.get_long_long_val();
    } else {
      // The standard encodes max 64 bits
      TTCN_error("An integer value which cannot be represented "
                 "on 64bits cannot be encoded using json2bson()");
    }
    for (int i = 0; i < bytes; i++) {
      buff.put_c(static_cast<unsigned char>(long_int >> i*8));
    }
    length = length + bytes;
  }
}

INTEGER decode_int_bson(TTCN_Buffer& buff, int bytes) {
  const unsigned char* uc = check_and_get_buffer_bson(buff, bytes);
  buff.increase_pos(bytes);
  if (bytes <= 4) { //32 bit
    RInt value = 0;
    for (size_t i = 0; i < 4; i++) {
      value += uc[i] << i*8;
    }
    return INTEGER(value);
  } else if (bytes <= 8) {
    TTCN_Buffer tmp_buf;
    for (int i = 0; i < bytes; i++) {
      tmp_buf.put_c(uc[bytes-i-1]);
    }
    OCTETSTRING os;
    tmp_buf.get_string(os);
    INTEGER value = oct2int(os);
    return value;
  } else {
    TTCN_error("An integer value larger than "
               "64 bytes cannot be decoded using bson2json()");
  }
}

void put_name(TTCN_Buffer& buff, INTEGER& length, CHARSTRING& name, bool in_array) {
  if (in_array) {
    buff.put_cs(name);
    buff.put_c(0); // Closing 0
    length = length + name.lengthof() + 1;
    // TODO: is it very slow?
    // Increment index
    INTEGER num = str2int(name);
    num = num + 1;
    name = int2str(num);
  } else {
    buff.put_cs(name);
    buff.put_c(0); // Closing 0
    length = length + name.lengthof() + 1;
  }
}

void get_name(TTCN_Buffer& buff, JSON_Tokenizer& tok, bool in_array) {
  const unsigned char* uc = buff.get_read_data();
  // Copy until closing 0
  char* tmp_str = mcopystr(reinterpret_cast<const char*>(uc));
  if (in_array == false) { // We dont need name when in array
    tok.put_next_token(JSON_TOKEN_NAME, tmp_str);
  }
  buff.increase_pos(strlen(tmp_str)+1);
  Free(tmp_str);
}

bool encode_bson_binary(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  // Check if this is really binary
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_STRING) {
    return false;
  }
  CHARSTRING cs2(len-2, content+1);
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NAME) {
    return false;
  }
  CHARSTRING cs3(len, content);
  if (cs3 != "$type") {
    return false;
  }
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_STRING) {
    return false;
  }
  CHARSTRING cs4(len-2, content+1);
  if (cs4.lengthof() != 2) {
    return false;
  }
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  
  buff.put_c(5);
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  OCTETSTRING os = decode_base64(cs2);
  INTEGER os_len = os.lengthof();
  encode_int_bson(buff, os_len, length);
  unsigned int type = 0;
  if (sscanf((const char*)cs4, "%02x", &type) != 1) {
    TTCN_error("Incorrect binary format while encoding with json2bson()");
  }
  buff.put_c(type);
  length = length + 1;
  buff.put_os(os);
  length = length + os_len;
  return true;
}

bool encode_bson_date(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_START) {
    return false;
  }
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NAME) {
    return false;
  }
  CHARSTRING cs(len, content);
  if (cs != "$numberLong") {
    return false;
  }
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NUMBER) {
    return false;
  }
  CHARSTRING cs2(len, content);
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  INTEGER int_num = str2int(cs2);
  buff.put_c(9); // datetime
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  // Encode on 64 bit
  long long int long_int = int_num.get_long_long_val();
  for (int i = 0; i < 8; i++) {
    buff.put_c(static_cast<unsigned char>(long_int >> i*8));
  }
  length = length + 8;
  return true;
}
  

bool encode_bson_timestamp(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_START) {
    return false;
  }
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NAME) {
    return false;
  }
  CHARSTRING cs(len, content);
  if (cs != "t") {
    return false;
  }
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NUMBER) {
    return false;
  }
  CHARSTRING cs2(len, content);
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NAME) {
    return false;
  }
  CHARSTRING cs3(len, content);
  if (cs3 != "i") {
    return false;
  }
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NUMBER) {
    return false;
  }
  CHARSTRING cs4(len, content);
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  INTEGER timestamp = str2int(cs2);
  INTEGER increment = str2int(cs4);
  buff.put_c(17);
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  encode_int_bson(buff, increment, length);
  encode_int_bson(buff, timestamp, length);
  return true;
}

bool encode_bson_regex(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_STRING) {
    return false;
  }
  CHARSTRING regex(len-2, content+1);
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NAME) {
    return false;
  }
  CHARSTRING cs2(len, content);
  if (cs2 != "$options") {
    return false;
  }
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_STRING) {
    return false;
  }
  CHARSTRING options(len-2, content+1);
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  
  buff.put_c(11);
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  buff.put_cs(regex);
  length = length + regex.lengthof();
  buff.put_c(0); // Closing 0
  length = length + 1;
  buff.put_cs(options);
  length = length + options.lengthof();
  buff.put_c(0); // Closing 0
  length = length + 1;
  return true;
}

bool encode_bson_oid(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_STRING) {
    return false;
  }
  CHARSTRING id(len-2, content+1);
  if (id.lengthof() != 24) {
    return false;
  }
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  
  buff.put_c(7);
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  unsigned char hex[12];
  for (size_t i = 0; i < 24; i = i + 2) {
    unsigned int value;
    if (sscanf(((const char*)id)+i, "%02x", &value) != 1) {
      TTCN_error("Incorrect binary format while encoding with json2bson()");
    }
    hex[i/2] = value;
  }
  buff.put_s(12, hex);
  length = length + 12;
  return true;
}

bool encode_bson_ref(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_STRING) {
    return false;
  }
  CHARSTRING name(len-2, content+1);
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NAME) {
    return false;
  }
  CHARSTRING cs(len, content);
  if (cs != "$id") {
    return false;
  }
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_STRING) {
    return false;
  }
  CHARSTRING id(len-2, content+1);
  if (id.lengthof() != 24) {
    return false;
  }
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }

  buff.put_c(12);
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  INTEGER name_length = name.lengthof()+1;
  encode_int_bson(buff, name_length, length);
  buff.put_cs(name);
  buff.put_c(0); // Closing 0
  length = length + name_length;
  unsigned char hex[12];
  for (size_t i = 0; i < 24; i = i + 2) {
    unsigned int value;
    if (sscanf(((const char*)id)+i, "%02x", &value) != 1) {
      TTCN_error("Incorrect binary format while encoding with json2bson()");
    }
    hex[i/2] = value;
  }
  buff.put_s(12, hex);
  length = length + 12;
  return true;
}

bool encode_bson_undefined(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_LITERAL_TRUE) {
    return false;
  }
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  
  buff.put_c(6);
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  return true;
}

bool encode_bson_minkey(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NUMBER) {
    return false;
  }
  CHARSTRING cs(len, content);
  if (cs != "1") {
    return false;
  }
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  
  buff.put_c(255);
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  return true;
}

bool encode_bson_maxkey(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NUMBER) {
    return false;
  }
  CHARSTRING cs(len, content);
  if (cs != "1") {
    return false;
  }
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  
  buff.put_c(127);
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  return true;
}

bool encode_bson_numberlong(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_STRING) {
    return false;
  }
  CHARSTRING cs(len-2, content+1);
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  
  buff.put_c(18);
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  INTEGER number = str2int(cs);
  long long int value = number.get_long_long_val();
  for (int i = 0; i < 8; i++) {
    buff.put_c(static_cast<unsigned char>(value >> i*8));
  }
  length = length + 8;
  return true;
}

bool encode_bson_code_with_scope(TTCN_Buffer& buff, JSON_Tokenizer& tok, INTEGER& length) {
  char *content;
  size_t len;
  json_token_t token;
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_STRING) {
    return false;
  }
  CHARSTRING cs(len-2, content+1);
  
  tok.get_next_token(&token, &content, &len);
  if (token != JSON_TOKEN_NAME) {
    return false;
  }
  CHARSTRING cs2(len, content);
  if (cs2 != "$scope") {
    return false;
  }
  
  INTEGER code_w_scope_length = 0;
  bool is_special = false;
  CHARSTRING f_name;
  TTCN_Buffer sub_buff;
  json2bson_coding(sub_buff, tok, false, false, code_w_scope_length, f_name, is_special);
  
  tok.get_next_token(&token, NULL, NULL);
  if (token != JSON_TOKEN_OBJECT_END) {
    return false;
  }
  
  buff.put_c(15);
  length = length + 1;
  // We do not know the name here. It will be inserted later.
  code_w_scope_length = code_w_scope_length + cs.lengthof() + 4 + 1;
  encode_int_bson(buff, code_w_scope_length, code_w_scope_length);
  encode_int_bson(buff, cs.lengthof()+1, length);
  buff.put_string(cs);
  buff.put_c(0); // Closing 0
  buff.put_buf(sub_buff);
  length = length + code_w_scope_length - 4; // We added the length of cs twice
  return true;
}


void json2bson_coding(TTCN_Buffer& buff, JSON_Tokenizer& tok, bool in_object,
       bool in_array, INTEGER& length, CHARSTRING& obj_name, bool& is_special) {
  json_token_t token;
  char* content = NULL;
  size_t len;
  size_t prev_pos = tok.get_buf_pos();
  tok.get_next_token(&token, &content, &len);
  if (in_object == false && token != JSON_TOKEN_OBJECT_START && token != JSON_TOKEN_ARRAY_START) {
    TTCN_error("Json document must be an object or array when encoding with json2bson()");
  }
  switch(token) {
    case JSON_TOKEN_OBJECT_START: {
      TTCN_Buffer sub_buff;
      INTEGER sub_len = 0;
      CHARSTRING subobj_name;
      if (obj_name.is_bound()) {
        subobj_name = obj_name;
      }
      while ((prev_pos = tok.get_buf_pos(), tok.get_next_token(&token, NULL, NULL))) {
        if (token != JSON_TOKEN_OBJECT_END) {
          tok.set_buf_pos(prev_pos);
          json2bson_coding(sub_buff, tok, true, false, sub_len, subobj_name, is_special);
          // We found a specially translated json
          if (is_special) {
            // The sub_buff contains the encoded bson except the obj_name.
            // We put it in here after the first byte
            TTCN_Buffer tmp_buff;
            tmp_buff.put_c(*(sub_buff.get_data()));
            put_name(tmp_buff, sub_len, subobj_name, in_array);
            tmp_buff.put_s(sub_buff.get_len()-1, sub_buff.get_data()+1);
            sub_buff = tmp_buff;
            in_object = false;
            break;
          }
        } else {
          sub_buff.put_c(0);// Closing zero
          sub_len = sub_len + 1;
          break;
        }
      }
      
      if (in_object == true) {
        TTCN_Buffer tmp_buff;
        tmp_buff.put_c(3); // embedded document
        length = length + 1;
        put_name(tmp_buff, length, obj_name, in_array);
        encode_int_bson(tmp_buff, sub_len, sub_len);
        length = length + sub_len;
        tmp_buff.put_buf(sub_buff);
        sub_buff = tmp_buff;
      } else if (is_special == false) {
        length = length + sub_len;
        encode_int_bson(buff, length, length);
      } else {
        length = length + sub_len;
        is_special = false;
      }
      buff.put_buf(sub_buff);
      break;
    }
    case JSON_TOKEN_OBJECT_END:
      TTCN_error("Unexpected object end character while encoding using json2bson().");
      break;
    case JSON_TOKEN_NAME: {
      CHARSTRING cs(len, content);
      prev_pos = tok.get_buf_pos();
      if (cs == "$binary") {
        is_special = encode_bson_binary(buff, tok, length);
      } else if (cs == "$date") {
        is_special = encode_bson_date(buff, tok, length);
      } else if (cs == "$timestamp") {
        is_special = encode_bson_timestamp(buff, tok, length);
      } else if (cs == "$regex") {
        is_special = encode_bson_regex(buff, tok, length);
      } else if (cs == "$oid") {
        is_special = encode_bson_oid(buff, tok, length);
      } else if (cs == "$ref") {
        is_special = encode_bson_ref(buff, tok, length);
      } else if (cs == "$undefined") {
        is_special = encode_bson_undefined(buff, tok, length);
      } else if (cs == "$minKey") {
        is_special = encode_bson_minkey(buff, tok, length);
      } else if (cs == "$maxKey") {
        is_special = encode_bson_maxkey(buff, tok, length);
      } else if (cs == "$numberLong") {
        is_special = encode_bson_numberlong(buff, tok, length);
      } else if (cs == "$code") {
        is_special = encode_bson_code_with_scope(buff, tok, length);
      } else {
        obj_name = cs;
      }
      if (!is_special) {
        tok.set_buf_pos(prev_pos);
        obj_name = cs;
      }
      break; }
    case JSON_TOKEN_STRING: {
      buff.put_c(2); // string
      length = length + 1;
      put_name(buff, length, obj_name, in_array);
      encode_int_bson(buff, len-1, length); // Remove "-s but add terminating null
      char * tmp_str = mcopystrn(content+1, len-2); // Remove "-s
      buff.put_string(tmp_str);
      buff.put_c(0); // Closing 0
      length = length + (int)len-1; // Remove "-s but add terminating null
      Free(tmp_str);
      break; }
    case JSON_TOKEN_NUMBER: {
      char *str = mcopystrn(content, len);
      size_t curr_pos = tok.get_buf_pos();
      tok.set_buf_pos(prev_pos);
      bool is_float = false;
      tok.check_for_number(&is_float);
      tok.set_buf_pos(curr_pos);
      if (is_float) {
        buff.put_c(1); // 64bit float
        put_name(buff, length, obj_name, in_array);
        double d;
        sscanf(str, "%lf", &d);
        FLOAT f = d;
        f.encode(bson_float_descr_, buff, TTCN_EncDec::CT_RAW);
      } else {
        INTEGER int_num = str2int(str);
        if (int_num.is_native()) {
          buff.put_c(16); //32bit integer
          length = length + 1;
        } else {
          buff.put_c(18); // 64bit integer
          length = length + 1;
        }
        put_name(buff, length, obj_name, in_array);
        encode_int_bson(buff, int_num, length);
      }
      Free(str);
      break; }
    case JSON_TOKEN_LITERAL_FALSE: {
      buff.put_c(8); // true or false
      put_name(buff, length, obj_name, in_array);
      buff.put_c(0); // false
      break; }
    case JSON_TOKEN_LITERAL_TRUE: {
      buff.put_c(8); // true or false
      put_name(buff, length, obj_name, in_array);
      buff.put_c(1); // true
      break; }
    case JSON_TOKEN_LITERAL_NULL: {
      buff.put_c(10); // null
      put_name(buff, length, obj_name, in_array);
      break; }
    case JSON_TOKEN_ARRAY_START: {
      if (in_object == false) { // The top level json is an array
        in_object = true;
      } else {
        buff.put_c(4); // array
        length = length + 1;
        put_name(buff, length, obj_name, in_array);
      }
      obj_name = "0"; // arrays are objects but the key is a number which increases
      TTCN_Buffer sub_buff;
      INTEGER sub_length = 0;
      while ((prev_pos = tok.get_buf_pos(), tok.get_next_token(&token, NULL, NULL))) {
        if (token != JSON_TOKEN_ARRAY_END) {
          tok.set_buf_pos(prev_pos);
          in_array = true;
          json2bson_coding(sub_buff, tok, in_object, in_array, sub_length, obj_name, is_special);
        } else {
          sub_buff.put_c(0);// Closing zero
          sub_length = sub_length + 1;
          break;
        }
      }
      encode_int_bson(buff, sub_length, sub_length);
      length = length + sub_length;
      buff.put_buf(sub_buff);
      break; }
    default:
      TTCN_error("Unexpected json token %i, while encoding using json2bson().", token);
  }
}

void bson2json_coding(TTCN_Buffer& buff, JSON_Tokenizer& tok, bool in_object, bool in_array) {
  INTEGER length = 0;
  // Beginning of the document
  if (in_object == false) {
    length = decode_int_bson(buff, 4);
    // Check if the input is long enough
    check_and_get_buffer_bson(buff, length-4);
    tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
    while (*(check_and_get_buffer_bson(buff, 1)) != 0) {
      bson2json_coding(buff, tok, true, in_array);
    }
    buff.increase_pos(1);
    tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
  } else {
    const unsigned char* type = check_and_get_buffer_bson(buff, 1);
    buff.increase_pos(1);
    // There is always a name
    get_name(buff, tok, in_array);
    switch(*type) {
      case 0: // document end
        TTCN_error("Unexpected document end character while decoding with bson2json()");
        break;
      case 1: { // 64bit float
        FLOAT f;
        check_and_get_buffer_bson(buff, 8);
        f.decode(bson_float_descr_, buff, TTCN_EncDec::CT_RAW);
        f.JSON_encode(bson_float_descr_, tok);
        break;
      }
      case 13: // Javascript code. Decoded as string
      case 14: // Symbol. Decoded as string
      case 2: { // UTF8 string
        INTEGER len = decode_int_bson(buff, 4);
        // Get the value of the pair
        const unsigned char* uc = check_and_get_buffer_bson(buff, (int)len);
        char *tmp_str = mcopystrn(reinterpret_cast<const char*>(uc), (int)len);
        buff.increase_pos((int)len);
        char* tmp_str2 = mprintf("\"%s\"", tmp_str);
        tok.put_next_token(JSON_TOKEN_STRING, tmp_str2);
        Free(tmp_str2);
        Free(tmp_str);
        break; }
      case 3: { // Embedded document
        length = decode_int_bson(buff, 4);
        // Check if the input is long enough
        check_and_get_buffer_bson(buff, length-4);
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        while (*(check_and_get_buffer_bson(buff, 1)) != 0) { // error message while converting
          bson2json_coding(buff, tok, in_object, false);
        }
        buff.increase_pos(1); // Skip the closing 0
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        break; }
      case 4: { // array
        length = decode_int_bson(buff, 4);
        // Check if the input is long enough
        check_and_get_buffer_bson(buff, length-4);
        tok.put_next_token(JSON_TOKEN_ARRAY_START, NULL);
        in_array = true;
        while (*(check_and_get_buffer_bson(buff, 1)) != 0) { // erorr message while converting
          bson2json_coding(buff, tok, in_object, in_array);
        }
        buff.increase_pos(1); // Skip the closing 0
        tok.put_next_token(JSON_TOKEN_ARRAY_END, NULL);
        break; }
      case 5: { // bytestring
        // decode bytestring length
        INTEGER bytestr_length = decode_int_bson(buff, 4);
        OCTETSTRING os(1, check_and_get_buffer_bson(buff, 1));
        buff.increase_pos(1);
        INTEGER typestr_type = oct2int(os);
        char* str_type = mprintf("\"%02x\"", (int)typestr_type);
        OCTETSTRING data(bytestr_length, check_and_get_buffer_bson(buff, bytestr_length));
        buff.increase_pos(bytestr_length);
        CHARSTRING cs = encode_base64(data);
        char* data_str = mprintf("\"%s\"", (const char*)cs);
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$binary");
        tok.put_next_token(JSON_TOKEN_STRING, data_str);
        tok.put_next_token(JSON_TOKEN_NAME, "$type");
        tok.put_next_token(JSON_TOKEN_STRING, str_type);
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        Free(data_str);
        Free(str_type);
        break; }
      case 6: { // undefined
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$undefined");
        tok.put_next_token(JSON_TOKEN_LITERAL_TRUE);
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        break; }
      case 7: { // oid
        OCTETSTRING os(12, check_and_get_buffer_bson(buff, 12));
        char* tmp_oct = NULL;
        for (size_t i = 0; i < 12; i++) {
          tmp_oct = mputprintf(tmp_oct, "%02X", os[i].get_octet());
        }
        char *str_hex = mprintf("\"%s\"", tmp_oct);
        buff.increase_pos(12);
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$oid");
        tok.put_next_token(JSON_TOKEN_STRING, str_hex);
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        Free(str_hex);
        Free(tmp_oct);
        break; }
      case 8: {  // true or false
        const unsigned char* uc = check_and_get_buffer_bson(buff, 1);
        if (*uc == 0) {
          tok.put_next_token(JSON_TOKEN_LITERAL_FALSE, NULL);
        } else {
          tok.put_next_token(JSON_TOKEN_LITERAL_TRUE, NULL);
        }
        buff.increase_pos(1);
        break;
      }
      case 9: { // datetime
        INTEGER date = decode_int_bson(buff, 8);
        char *tmp_str = mprintf("%lld", date.get_long_long_val());
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$date");
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$numberLong");
        tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        Free(tmp_str);
        break; }
      case 10: { // null
        tok.put_next_token(JSON_TOKEN_LITERAL_NULL, NULL);
        break;
      }
      case 11: { // regex
        // copy until closing 0
        const unsigned char* uc = check_and_get_buffer_bson(buff, 1);
        char *tmp_str = mcopystr(reinterpret_cast<const char*>(uc));
        buff.increase_pos(strlen(tmp_str)+1);
        char *regex = mprintf("\"%s\"", tmp_str);
        Free(tmp_str);
        uc = check_and_get_buffer_bson(buff, 1);
        tmp_str = mcopystr(reinterpret_cast<const char*>(uc));
        buff.increase_pos(strlen(tmp_str)+1);
        char *options = mprintf("\"%s\"", tmp_str);
        Free(tmp_str);
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$regex");
        tok.put_next_token(JSON_TOKEN_STRING, regex);
        tok.put_next_token(JSON_TOKEN_NAME, "$options");
        tok.put_next_token(JSON_TOKEN_STRING, options);
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        Free(options);
        Free(regex);
        break; }
      case 12: { // dbref
        INTEGER name_len = decode_int_bson(buff, 4);
        const unsigned char* uc = check_and_get_buffer_bson(buff, (int)name_len);
        char *tmp_name = mcopystrn(reinterpret_cast<const char*>(uc), (int)name_len);
        buff.increase_pos((int)name_len);
        char* tmp_str = mprintf("\"%s\"", tmp_name);
        OCTETSTRING os(12, check_and_get_buffer_bson(buff, 12));
        buff.increase_pos(12);
        char* tmp_oct = NULL;
        for (size_t i = 0; i < 12; i++) {
          tmp_oct = mputprintf(tmp_oct, "%02X", os[i].get_octet());
        }
        char *str_hex = mprintf("\"%s\"", tmp_oct);
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$ref");
        tok.put_next_token(JSON_TOKEN_STRING, tmp_str);
        tok.put_next_token(JSON_TOKEN_NAME, "$id");
        tok.put_next_token(JSON_TOKEN_STRING, str_hex);
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        Free(tmp_oct);
        Free(str_hex);
        Free(tmp_str);
        Free(tmp_name);
        break; }
      case 15: { // code_with_scope
        INTEGER len = decode_int_bson(buff, 4);
        check_and_get_buffer_bson(buff, (int)len-4); // len contains the length of itself
        len = decode_int_bson(buff, 4);
        const unsigned char* uc = check_and_get_buffer_bson(buff, (int)len);
        char *tmp_str = mcopystrn(reinterpret_cast<const char*>(uc), (int)len);
        char *tmp_str2 = mprintf("\"%s\"", tmp_str);
        buff.increase_pos((int)len);
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$code");
        tok.put_next_token(JSON_TOKEN_STRING, tmp_str2);
        tok.put_next_token(JSON_TOKEN_NAME, "$scope");
        bson2json_coding(buff, tok, false, false);
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        Free(tmp_str2);
        Free(tmp_str);
        break; }
      case 16: { // 32bit integer
        INTEGER value = decode_int_bson(buff, 4);
        char *tmp_str = mprintf("%d", (int)value);
        tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
        Free(tmp_str);
        break; }
      case 17: { // timestamp
        INTEGER increment = decode_int_bson(buff, 4);
        INTEGER timestamp = decode_int_bson(buff, 4);
        char *increment_str = mprintf("%i", (int)increment);
        char *timestamp_str = mprintf("%i", (int)timestamp);
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$timestamp");
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "t");
        tok.put_next_token(JSON_TOKEN_STRING, timestamp_str);
        tok.put_next_token(JSON_TOKEN_NAME, "i");
        tok.put_next_token(JSON_TOKEN_STRING, increment_str);
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        Free(timestamp_str);
        Free(increment_str);
        break; }
      case 18: { //64 bit integer
        INTEGER value = decode_int_bson(buff, 8);
        char *tmp_str = mprintf("%lld", value.get_long_long_val());
        tok.put_next_token(JSON_TOKEN_NUMBER, tmp_str);
        Free(tmp_str);
        break; }
      case 127: { // maxkey
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$maxKey");
        tok.put_next_token(JSON_TOKEN_NUMBER, "1");
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        break; }
      case 255: { // minkey
        tok.put_next_token(JSON_TOKEN_OBJECT_START, NULL);
        tok.put_next_token(JSON_TOKEN_NAME, "$minKey");
        tok.put_next_token(JSON_TOKEN_NUMBER, "1");
        tok.put_next_token(JSON_TOKEN_OBJECT_END, NULL);
        break; }
      default:
        TTCN_error("Unexpected type %i while decoding using bson2json().", *type);
    }
  }
}