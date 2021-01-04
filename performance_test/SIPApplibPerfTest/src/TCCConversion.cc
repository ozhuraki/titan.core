/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               TCCConversion.cc
//  Description:        TCC Useful Functions: Conversion Functions
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 472
//  Updated:            2011-05-06
//  Contact:            http://ttcn.ericsson.se
//
///////////////////////////////////////////////////////////////////////////////

#include "TCCConversion_Functions.hh"
#include <ctype.h>
#include <strings.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace TCCConversion__Functions
{

///////////////////////////////////////////////////////////////////////////////
//  Function: f__putInLowercase
//
//  Purpose:
//    Put charstring to lowercase
//
//  Parameters:
//    pl__string - *in* *charstring* - charstring
//
//  Return Value:
//    charstring - converted value
//
//  Errors:
//    -
//
//  Detailed description:
//    -
//
///////////////////////////////////////////////////////////////////////////////
  CHARSTRING f__putInLowercase(const CHARSTRING& pl__string)
  {
    int str_len = pl__string.lengthof();
    char *tmp_buf = new char[str_len];
    const char *src_ptr = pl__string;
    for (int  i = 0; i < str_len; i++)
    tmp_buf[i] = tolower(src_ptr[i]);
    CHARSTRING ret_val(str_len, tmp_buf);
    delete [] tmp_buf;
    return ret_val;
  }

///////////////////////////////////////////////////////////////////////////////
//  Function: f__putInUppercase
//
//  Purpose:
//    Put charstring to uppercase
//
//  Parameters:
//    pl__string - *in* *charstring* - charstring
//
//  Return Value:
//    charstring - converted value
//
//  Errors:
//    -
//
//  Detailed description:
//    -
//
///////////////////////////////////////////////////////////////////////////////
  CHARSTRING f__putInUppercase(const CHARSTRING& pl__string)
  {
    int str_len = pl__string.lengthof();
    char *tmp_buf = new char[str_len];
    const char *src_ptr = pl__string;
    for (int  i = 0; i < str_len; i++)
    tmp_buf[i] = toupper(src_ptr[i]);
    CHARSTRING ret_val(str_len, tmp_buf);
    delete [] tmp_buf;
    return ret_val;
  }

  ///////////////////////////////////////////////////////////////////////////////
//  Function: f__addOctetstring
//
//  Purpose:
//    Add two integer values represented in OCTETSTRING
//
//  Parameters:
//    par1 - *in* *octetstring* - first octetstring value
//    par2 - *in* *octetstring* - second octetstring value
//
//  Return Value:
//    octetstring - sum of input
//
//  Errors:
//    -
//
//  Detailed description:
//    Negative values are unhandled!
//
///////////////////////////////////////////////////////////////////////////////
OCTETSTRING f__addOctetstring(const OCTETSTRING& par1, const OCTETSTRING& par2)
{
	par1.must_bound("incOctetstring() parameter #1: unbound octetstring value");
	par2.must_bound("incOctetstring() parameter #2: unbound octetstring value");

	size_t len1 = (size_t)par1.lengthof();
	size_t len2 = (size_t)par2.lengthof();
	size_t len = len1 > len2 ? len1 : len2;
	OCTETSTRING ret_val = int2oct(0,len);

    const unsigned char* data1 = (const unsigned char*)par1;
    const unsigned char* data2 = (const unsigned char*)par2;

	int tmp = 0;
	int carry = 0;
	for (size_t idx = 0; idx<len; idx++)
	{
		if (len1 > idx) { tmp = data1[len1-idx-1]&0xff; }  else { tmp = 0x00; }
		if (len2 > idx) { tmp += (data2[len2-idx-1]&0xff) + carry; } else { tmp += carry; }
		carry = tmp >> 8;
		ret_val[len-1-idx] = int2oct(tmp&0xff,1);
	}
	if (carry) { ret_val = int2oct(carry&0xff,1) + ret_val; }
	return ret_val;

}

///////////////////////////////////////////////////////////////////////////////
//  Function: f__subOctetstring
//
//  Purpose:
//    Substract two integer values represented in OCTETSTRING
//
//  Parameters:
//    par1 - *in* *octetstring* - first octetstring value
//    par2 - *in* *octetstring* - second octetstring value
//
//  Return Value:
//    octetstring - difference of input
//
//  Errors:
//    -
//
//  Detailed description:
//    Negative values are unhandled!
//
///////////////////////////////////////////////////////////////////////////////
OCTETSTRING f__subOctetstring(const OCTETSTRING& par1, const OCTETSTRING& par2)
{
	par1.must_bound("decOctetstring() parameter #1: unbound octetstring value");
	par2.must_bound("decOctetstring() parameter #2: unbound octetstring value");

	size_t len1 = (size_t)par1.lengthof();
	size_t len2 = (size_t)par2.lengthof();
	size_t len = len1 > len2 ? len1 : len2;
	OCTETSTRING ret_val = int2oct(0,len);

    const unsigned char* data1 = (const unsigned char*)par1;
    const unsigned char* data2 = (const unsigned char*)par2;

	int tmp = 0;
	int carry = 1;	// +1 for two's complement
	for (size_t idx = 0; idx<len; idx++)
	{
		if (len1 > idx) { tmp = data1[len1-idx-1]&0xff; }  else { tmp = 0x00; }
		if (len2 > idx) { tmp += ((~data2[len2-idx-1])&0xff) + carry; } else { tmp += carry + 0xff; }
		carry = tmp >> 8;
		ret_val[len-1-idx] = int2oct(tmp&0xff,1);
	}
	return ret_val;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f__compOctetstring
//
//  Purpose:
//    Compare two integer values represented in OCTETSTRING
//
//  Parameters:
//    par1 - *in* *octetstring* - first octetstring value
//    par2 - *in* *octetstring* - second octetstring value
//
//  Return Value:
//    integer - 0: par1 = par2
//              1: par1>par2
//              2: par1<par2
//
//  Errors:
//    -
//
//  Detailed description:
//    Negative values are unhandled!
//
///////////////////////////////////////////////////////////////////////////////
INTEGER f__compOctetstring(const OCTETSTRING& par1, const OCTETSTRING& par2)
{
	// Return value 0: par1 = par2, 1: par1>par2, 2: par1<par2
	par1.must_bound("decOctetstring() parameter #1: unbound octetstring value");
	par2.must_bound("decOctetstring() parameter #2: unbound octetstring value");

	size_t len1 = (size_t)par1.lengthof();
	size_t len2 = (size_t)par2.lengthof();
	size_t len = len1 > len2 ? len1 : len2;

    const unsigned char* data1 = (const unsigned char*)par1;
    const unsigned char* data2 = (const unsigned char*)par2;

	int tmp1 = 0;
	int tmp2 = 0;
	for (size_t idx = 0; idx<len; idx++)
	{
		if (len1 + idx>= len ) { tmp1 = data1[len1-len+idx]&0xff; }  else { tmp1 = 0x00; }
		if (len2 + idx>= len) { tmp2 = data2[len2-len+idx]&0xff; } else { tmp2 = 0x00; }
		if (tmp1>tmp2) { return 1; }
		if (tmp1<tmp2) { return 2; }
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f_substr_token
//
//  Purpose:
//    The function returns a substring from a value. The starting and the ending
//    points are defined by the begin and end tokens.
//
//  Parameters:
//    str - *in* *charstring* - the value
//    begin_token - *in* *charstring* - begin token
//    end_token - *in* *charstring* - end token
//
//  Return Value:
//    charstring. If one of the tokens is not found it returns an empty string
//
//  Errors:
//    -
//
//  Detailed description:
//    If end_token is an empty string the function returns the part of the value
//    after the begin_token.
//    If begin_token is an empty string the function returns the part of the
//    value until the end_token.
//    If both of them empty string the function returns the part whole value
//
///////////////////////////////////////////////////////////////////////////////
CHARSTRING f__substr__token(const CHARSTRING& str,
                                 const CHARSTRING& begin_token,
                                 const CHARSTRING& end_token){

  const char* InStr=(const char*)str;
  const char* SubStrA=(const char*)begin_token;
  const char* SubStrB=(const char*)end_token;
  int lenA=begin_token.lengthof();
  const char* first=NULL;
  const char* second;

  if(str.lengthof()==0) return "";
  if(begin_token.lengthof()>0){
    first=strstr(InStr,SubStrA);
    if(first==NULL) return "";
    first+=lenA;
  } else first=str;
  if(end_token.lengthof()>0){
    second=strstr(first,SubStrB);
    if(second==NULL) return "";
  } else second=InStr+str.lengthof();
  return CHARSTRING(second-first,first);
}


///////////////////////////////////////////////////////////////////////////////
//  Function: f_strstr
//
//  Purpose:
//    The f_strstr function locates the first  occurrence  of  the string  s2
//    in string s1 and returns an index of starting pont of the located string,
//    or  -1 if the string is not found. If  s2 is an empty, the  func-
//    tion returns 0.
//
//  Parameters:
//    s1 - *in* *charstring* - input string
//    s2 - *in* *charstring* - string to search
//    offset - *in* *integer* - start offset.
//
//  Return Value:
//    index of starting pont of the located string
//
//  Errors:
//    -
//
//  Detailed description:
//    The offset determines the starting point of the search. Any occurance of
//    the s2 before the offset is ignored. The offset is optional
//
///////////////////////////////////////////////////////////////////////////////
INTEGER f__strstr(const CHARSTRING& s1,
                                 const CHARSTRING& s2,
                                 const INTEGER& offset){
  if(s2.lengthof()==0) return 0;
  if(s1.lengthof()==0) return -1;
  if(offset<0) return -1;
  if(s1.lengthof()<=offset) return -1;

  const char* str1=(const char*)s1+(int)offset;
  const char* str2=(const char*)s2;
  const char* first=strstr(str1,str2);
  if(first) return first-str1+(int)offset;
  return -1;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f_convertIPAddrToBinary
//
//  Purpose:
//    Converts an IPv4 and IPv6 address to its hex representation.
//    IPv6 address is assumed if the input string contains ':'
//
//  Parameters:
//    pl_ip - *in* *charstring* - input string
//
//  Return Value:
//    hex value of the Ip in an octetstring
//
//  Errors:
//    -
//
//  Detailed description:
//
///////////////////////////////////////////////////////////////////////////////
OCTETSTRING f__convertIPAddrToBinary(const CHARSTRING& pl__ip) {
  bool ipv6 = index(pl__ip, ':');
  unsigned char addr[17];

  if(inet_pton(ipv6 ? AF_INET6 : AF_INET, pl__ip, addr)==1) {
    return OCTETSTRING(ipv6 ? 16 : 4, addr);
  }

  return OCTETSTRING(0, addr);
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f_oct2char_safe
//
//  Purpose:
//    Fault tolerant version of the oct2str Titan built-in function.
//
//  Parameters:
//    s1 - *in* *octetsttring* - input string
//    s2 - *out* *charstring* - output string
//
//  Return Value:
//    false on fault, ie. on unbound input or invalid character in input.
//    true on success
//
//  Errors:
//    -
//
//  Detailed description:
//    On fault, the longest chunk that could be decoded is returned.
//
///////////////////////////////////////////////////////////////////////////////
BOOLEAN f__oct2char__safe(const OCTETSTRING& par1, CHARSTRING& par2){
  if (!par1.is_bound())
    return BOOLEAN(false);
  int value_length = par1.lengthof();
  const unsigned char *octets_ptr = (const unsigned char*)par1;
  for (int i = 0; i < value_length; i++) {
    unsigned char octet = octets_ptr[i];
    if (octet > 127) {
      par2 = CHARSTRING(i, (const char*)octets_ptr);
      return BOOLEAN(false);
    }
  }
  par2 = CHARSTRING(value_length, (const char*)octets_ptr);
  return BOOLEAN(true);
}

}
