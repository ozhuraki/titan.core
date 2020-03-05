/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//  File:     TCCMessageHandling.cc
//  Rev:      <RnXnn>
//  Prodnr:   CNL 113 472
//  Updated:  2012-04-02
//  Contact:  http://ttcn.ericsson.se
//
///////////////////////////////////////////////////////////////////////////////

#include "TTCN3.hh"
#include "TCCMessageHandling_Functions.hh"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

int get_content_length(char *buff, int length)
{
  int current_state=0;
  int ret_val=-1;
  for(int a=0;a<length;a++){
    switch(current_state){
      case 0:
        if(buff[a]=='\n') current_state=1;
        break;
      case 1:
        if(buff[a]=='l' || buff[a]=='L') current_state=2;
        else if(!strncasecmp(buff+a,"content-length",14)) {current_state=2;a+=13;}
        else current_state=0;
        break;
      case 2:
        if(buff[a]==':') current_state=3;
        else if(buff[a]==' ' || buff[a]=='\t' || buff[a]=='\r' || buff[a]=='\n') current_state=2;
        else current_state=0;
        break;
      case 3:
        if(buff[a]>='0' && buff[a]<='9'){
          sscanf(buff+a,"%d",&ret_val);
          return ret_val;
        }
        else if(buff[a]==' ' || buff[a]=='\t' || buff[a]=='\r' || buff[a]=='\n') current_state=3;
        else current_state=0;
        break;
    }
  }
  return ret_val;
}

char* get_header_end(char *buff, int length)
{
  int current_state = 0;
  char* ret_val = NULL;
  for (int a=0;a<length;a++)
  {
    switch(current_state){
      case 0:
        if(buff[a]=='\n') current_state=1;
        break;
      case 1:
        if(buff[a]=='\r') current_state=2;
        else current_state=0;
        break;
      case 2:
        if(buff[a]=='\n')
          return buff+a-2;
        else current_state=0;
        break;
    }
  }
  return ret_val;
}

INTEGER TCCMessageHandling__Functions::f__TCCMessageHandling__getMessageLength
(
  const OCTETSTRING& stream
)
{
  char *msg_in_buffer_loc = (char*) ((const unsigned char*) stream);
  const int   buf_length = stream.lengthof();
  const char *header_end;
  char *atm = msg_in_buffer_loc;
  int num_chars;

  //check for STUN response
  if (2 == buf_length && '\r' == msg_in_buffer_loc[0] && '\n' == msg_in_buffer_loc[1])
    return 2;
  
  while((*atm<=' ') && (*atm>0) && (atm-msg_in_buffer_loc < buf_length-1)) {atm++;}  // skip leading blanks

  //header_end=strstr(atm,"\n\r\n");  // search for alone crlf
  header_end = get_header_end(atm, buf_length-(atm-msg_in_buffer_loc));

  if(header_end)
    num_chars=(header_end-msg_in_buffer_loc)+3;
  else
    return -1;

  int cont_len = get_content_length(msg_in_buffer_loc,num_chars);
  if (cont_len == -1) 
    return -1;

  return INTEGER(num_chars + cont_len);
}

INTEGER TCCMessageHandling__Functions::f__TCCMessageHandling__getMessageLength4Diameter
(
  const OCTETSTRING& stream
)
{
  if (stream.lengthof() < 4){ return -1; }
  const unsigned char *msg_in_buffer_loc =  ((const unsigned char*) stream);
  
  int ret_value = (msg_in_buffer_loc[1] << 16) + (msg_in_buffer_loc[2] << 8) + (msg_in_buffer_loc[3]);
  return ret_value;
}

INTEGER TCCMessageHandling__Functions::f__TCCMessageHandling__getMessageLength4Radius
(
  const OCTETSTRING& stream
)
{
  if (stream.lengthof() < 4){ return -1; }
  const unsigned char *msg_in_buffer_loc =  ((const unsigned char*) stream);
  
  int ret_value = (msg_in_buffer_loc[2] << 8) + (msg_in_buffer_loc[3]);
  return ret_value;
}
