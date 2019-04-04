/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               TCCEncoding.cc
//  Description:        TCC Useful Functions: Message Encoding Functions.
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 472
//  Updated:            2012-06-28
//  Contact:            http://ttcn.ericsson.se
//
///////////////////////////////////////////////////////////////////////////////

#include "TCCEncoding_Functions.hh"

namespace TCCEncoding__Functions {

CHARSTRING enc_Base64(const OCTETSTRING& msg, bool use_linebreaks);
OCTETSTRING dec_Base64(const CHARSTRING& b64, bool warn_invalid_char);

////////////////////////////
// MIME Base64 (RFC 2045) //
////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//  Function: enc__MIME__Base64
// 
//  Purpose:
//    Encode message to MIME Base64 format (RFC 2045)
//
//  Parameters:
//    p__msg - *in* *octetstring* - message to encode
// 
//  Return Value:
//    charstring - encoded message
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
CHARSTRING enc__MIME__Base64(const OCTETSTRING& p__msg)
{
  return enc_Base64(p__msg, true);
}

///////////////////////////////////////////////////////////////////////////////
//  Function: dec__MIME__Base64
// 
//  Purpose:
//    Decode message from MIME Base64 format (RFC 2045)
//
//  Parameters:
//    p__b64 - *in* *charstring* - message to decode
// 
//  Return Value:
//    octetstring - decoded message
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
OCTETSTRING dec__MIME__Base64(const CHARSTRING& p__b64)
{
  return dec_Base64(p__b64, false);
}

////////////////////////////
// LDIF Base64 (RFC 2849) //
////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//  Function: enc__LDIF__Base64
// 
//  Purpose:
//    Encode message to LDIF Base64 format (RFC 2849)
//
//  Parameters:
//    p__msg - *in* *octetstring* - message to encode
// 
//  Return Value:
//    charstring - encoded message
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
CHARSTRING enc__LDIF__Base64(const OCTETSTRING& p__msg)
{
  return enc_Base64(p__msg, false);
}

///////////////////////////////////////////////////////////////////////////////
//  Function: dec__LDIF__Base64
// 
//  Purpose:
//    Decode message from LDIF Base64 format (RFC 2849)
//
//  Parameters:
//    p__b64 - *in* *charstring* - message to decode
// 
//  Return Value:
//    octetstring - decoded message
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
OCTETSTRING dec__LDIF__Base64(const CHARSTRING& p__b64)
{
  return dec_Base64(p__b64, true);
}


// implementation

CHARSTRING enc_Base64(const OCTETSTRING& msg, bool use_linebreaks)
{
  const char *code_table = {
    "ABCDEFGHIJKLMNOP"
    "QRSTUVWXYZabcdef"
    "ghijklmnopqrstuv"
    "wxyz0123456789+/"
  };
  const char pad = '=';
  const unsigned char *p_msg = (const unsigned char *)msg;
  int octets_left = msg.lengthof();
  //char *output = new char[(octets_left/3+1)*4 + (octets_left/76+1)*2 + 1];
  // quick approximation:
  char *output = new char[((octets_left*22)>>4) + 7];
  char *p_output = output;
  int n_4chars = 0;
  while(octets_left >= 3) {
    *p_output++ = code_table[p_msg[0] >> 2];
    *p_output++ = code_table[((p_msg[0] << 4) | (p_msg[1] >> 4)) & 0x3f];
    *p_output++ = code_table[((p_msg[1] << 2) | (p_msg[2] >> 6)) & 0x3f];
    *p_output++ = code_table[p_msg[2] & 0x3f];
    n_4chars++;
    if(use_linebreaks && n_4chars>=19 && octets_left != 3) {
      *p_output++ = '\r';
      *p_output++ = '\n';
      n_4chars = 0;
    }
    p_msg += 3;
    octets_left -= 3;
  }
  switch(octets_left) {
  case 1:
    *p_output++ = code_table[p_msg[0] >> 2];
    *p_output++ = code_table[(p_msg[0] << 4) & 0x3f];
    *p_output++ = pad;
    *p_output++ = pad;
    break;
  case 2:
    *p_output++ = code_table[p_msg[0] >> 2];
    *p_output++ = code_table[((p_msg[0] << 4) | (p_msg[1] >> 4)) & 0x3f];
    *p_output++ = code_table[(p_msg[1] << 2) & 0x3f];
    *p_output++ = pad;
    break;
  default:
    break;
  }
  *p_output = '\0';
  CHARSTRING ret_val(output);
  delete []output;
  return ret_val;
}

OCTETSTRING dec_Base64(const CHARSTRING& b64, bool warn_invalid_char)
{
  const unsigned char decode_table[] = {
    80, 80, 80, 80, 80, 80, 80, 80,   80, 80, 80, 80, 80, 80, 80, 80,
    80, 80, 80, 80, 80, 80, 80, 80,   80, 80, 80, 80, 80, 80, 80, 80,
    80, 80, 80, 80, 80, 80, 80, 80,   80, 80, 80, 62, 80, 80, 80, 63,
    52, 53, 54, 55, 56, 57, 58, 59,   60, 61, 80, 80, 80, 70, 80, 80,
    80,  0,  1,  2,  3,  4,  5,  6,    7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22,   23, 24, 25, 80, 80, 80, 80, 80,
    80, 26, 27, 28, 29, 30, 31, 32,   33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48,   49, 50, 51, 80, 80, 80, 80, 80
  };
  const unsigned char *p_b64 = (const unsigned char *) ((const char *) b64);
  int chars_left = b64.lengthof();
  unsigned char *output = new unsigned char[((chars_left>>2)+1)*3];
  unsigned char *p_output = output;
  unsigned int bits = 0;
  size_t n_bits = 0;
  bool non_base64_char = false;
  while(chars_left--) {
    unsigned char dec;
    if(*p_b64 > 0 && (dec = decode_table[*p_b64])<64) {
      bits <<= 6;
      bits |= dec;
      n_bits += 6;
      if(n_bits>=8) {
        *p_output++ = (bits >> (n_bits-8)) & 0xff;
        n_bits-=8;
      }
    } else if (*p_b64 == '=') {
      break;
    } else {
      non_base64_char = true;
    }
    p_b64++;
  }
  if(warn_invalid_char && non_base64_char) {
      TTCN_Logger::begin_event(TTCN_WARNING);
      TTCN_Logger::log_event_str("Warning: Invalid character in Base64 encoded "
        "data: ");
      b64.log();
      TTCN_Logger::end_event();
  }
  OCTETSTRING ret_val(p_output - output, output);
  delete []output;
  return ret_val;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f__enc__TBCD
// 
//  Purpose:
//    Encode charstring to TBCD
//
//  Parameters:
//    pl__char - *in* *charstring* - message to encode
// 
//  Return Value:
//    octetstring - TBCD encoding
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
OCTETSTRING f__enc__TBCD(const CHARSTRING& pl__char)
{
 const unsigned char code_table[] = {
    0x00,0x01,0x02,0x03,0x04,
    0x05,0x06,0x07,0x08,0x09 
  };
 const char* p_char = (const char*) pl__char;
 int char_length = pl__char.lengthof();
 int oct_length;
 
 if ((char_length % 2) == 1){
  oct_length = (char_length + 1) / 2;
 }
 else {
  oct_length = (char_length) / 2;
 }
 
 unsigned char half_byte;
 unsigned char *output = new unsigned char[oct_length]; 
  
 for (int i = 0; i < char_length; i++) {    
   if((p_char[i] >= 0x30) && (p_char[i] <= 0x39)) {    
     half_byte =  code_table[p_char[i]-0x30];    
   }
   else if (p_char[i] == 0x2A)
     half_byte = 0x0A;
   else if (p_char[i] == 0x23)
     half_byte = 0x0B;
   else if (p_char[i] == 0x61)
     half_byte = 0x0C;
   else if (p_char[i] == 0x62)
     half_byte = 0x0D;
   else if (p_char[i] == 0x63) 
     half_byte = 0x0E;
   else {  
      TTCN_Logger::log(TTCN_WARNING,"Warning : Invalid TBCD digit!");
      return OCTETSTRING(0,0);
   }
     
  if ((i % 2) == 0) {    
    output[i/2] = half_byte; 
    if((i+1) == char_length) {
      output[i/2] = output[i/2] | 0xF0;
    }    
  }
  else {   
    output[(i-1)/2] = output[(i-1)/2] | ( half_byte << 4);
  } 
 } 
  
  OCTETSTRING ret_val(oct_length, output);
  delete []output;
  return ret_val;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f__dec__TBCD
// 
//  Purpose:
//    Decode octetstring from TBCD encoding
//
//  Parameters:
//    pl__oct - *in* *octetstring* - message to decode
// 
//  Return Value:
//    charstring - decoded message
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
CHARSTRING f__dec__TBCD(const OCTETSTRING& pl__oct) 
{
 unsigned const char* p_char = (const unsigned char*) pl__oct;
 int oct_length = pl__oct.lengthof();
 char *output = new char[oct_length*2+1];
 
 const char *code_table = {
    "0123456789*#abc"
  };
 unsigned char msb;
 unsigned char lsb;
  
 int i = 0;
 while (i < oct_length) { 
    msb = (unsigned char)((p_char[i] & 0xF0) >> 4);
    lsb = (unsigned char)(p_char[i] & 0x0F);
        
    if(lsb != 0x0F){ // lsb not filler
      output[2*i] = code_table[lsb];
    }
    else { // lsb is filler 
      TTCN_Logger::log(TTCN_WARNING,"Warning : Filler digit at invalid location!");
      return CHARSTRING("");
    } 
       
    if(msb != 0x0F) { // msb not filler
      output[2*i+1] = code_table[msb];
      if (i == (oct_length-1)) {
        output[2*i+2] = '\0';
      }      
    }
    else { // msb is filler    
      if (i == (oct_length-1))  {   
        output[2*i+1] = '\0';
      }
      else {
        TTCN_Logger::log(TTCN_WARNING,"Warning : Filler digit at invalid location!");
        return CHARSTRING("");      
      }      
    }     
   i=i+1; 
 }  
  CHARSTRING ret_val(output);
  delete []output;
  return ret_val;   
}

}//namespace
