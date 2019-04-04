/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************///
//  File:               IP_EncDec.cc
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 418
//  Updated:            2011-03-10
//  Contact:            http://ttcn.ericsson.se
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "IP_Types.hh"


/* ================= Common defines ================= */

/* Logs the name of the function on entering */
#define Log_function_name_on_enter() \
TTCN_logger.log(TTCN_DEBUG, "Entering %s", __FUNCTION__);

/* Logs the name of the function on leaving */
#define Log_function_name_on_leave() \
TTCN_logger.log(TTCN_DEBUG, "Leaving %s", __FUNCTION__);

#define Log_object(o) \
TTCN_logger.begin_event(TTCN_DEBUG); \
(o).log(); \
TTCN_logger.end_event(); \

#define Get_MSB(val) (((val) & 0xff00) >> 8)
#define Get_LSB(val) ((val) & 0x00ff)

namespace IP__Types {

TTCN_Module IP__EncDec("IP__EncDec", __DATE__, __TIME__);


/* ================= Common part for IPv4 and IPv6 ================= */

/* Returns the non-const pointer to the data area of the TTCN_Buffer, i.e.
  the same pointer as the TTCN_Buffer::get_data() method. */
unsigned char*
Get_TTCN_Buffer_data(TTCN_Buffer & bb)
{
  unsigned char *ptr;
  size_t tmp = 0;
  
  bb.get_end(ptr, tmp);
  ptr = ptr - bb.get_len();
  return ptr;
}

General__Types::OCT2
Calculate_cksum(const unsigned char *ptr, int datalen)
{
  unsigned long sum = 0;
  unsigned char ret[2];
  
  Log_function_name_on_enter();
  
  for (int i = 0; i <= datalen - 2; i = i + 2)
    sum += (ptr[i + 1] << 8) + ptr[i]; 

  if (datalen % 2) // datalen is odd
  {
    sum += ptr[datalen - 1];
  }

  sum = (sum & 0xFFFF) + (sum >> 16);
  sum = (sum & 0xFFFF) + (sum >> 16);
  sum = ~sum;

  ret[0] = Get_LSB(sum);
  ret[1] = Get_MSB(sum);
  return OCTETSTRING(2, &ret[0]);
}

void
Calculate_MINE_cksum(TTCN_Buffer & bb, const IP__MINE__header & field)
{
  unsigned char *ptr = Get_TTCN_Buffer_data(bb);
  General__Types::OCT2 cksum;
  int datalen = 8;
  
  Log_function_name_on_enter();
  
  if (field.srcaddr().ispresent()) datalen += 4;
  cksum = Calculate_cksum(ptr, datalen);
  ptr[2] = ((const unsigned char*) cksum)[0];
  ptr[3] = ((const unsigned char*) cksum)[1];
}

int
Decode_MINE_header(TTCN_Buffer & bb, IP__MINE__header & field)
{
  Log_function_name_on_enter();

  field.decode(IP__MINE__header_descr_, bb, TTCN_EncDec::CT_RAW);
  return field.protocol();
}


int
Decode_AH_header(TTCN_Buffer & bb, IP__AH__header & field)
{
  Log_function_name_on_enter();

  field.decode(IP__AH__header_descr_, bb, TTCN_EncDec::CT_RAW);
  return field.next__hdr();
}


int
Decode_GRE2_header(TTCN_Buffer & bb, IP__GRE2__header & field)
{
  Log_function_name_on_enter();

  field.decode(IP__GRE2__header_descr_, bb, TTCN_EncDec::CT_RAW);
  // Decode IPv4 or IPv6 ethertypes
  if (field.protocol__type() == c__ip__gre__proto__ipv4)
    return c__ip__proto__ipv4;
  else if (field.protocol__type() == c__ip__gre__proto__ipv6)
    return c__ip__proto__ipv6;
  else
  {
    unsigned int tmp = field.protocol__type();
    TTCN_error("Unknown protocol-type in GRE2 header "
               "(must be 0x0800 for IPv4 or 0x86DD for IPv6): 0x%02x", tmp);
  }
}


int
Decode_ESP_header(TTCN_Buffer & bb, IP__ESP__header & field)
{
  const unsigned char *ptr;
  size_t pad_length;
  size_t pos;

  Log_function_name_on_enter();

  field.header().decode(IP__ESP__header__part_descr_, bb, TTCN_EncDec::CT_RAW);

  // Decode ESP tail and invalidate buffer tail
  ptr = bb.get_read_data();
  pad_length = ptr[bb.get_read_len() - 1 - 13];
  field.tail().esp__null__tail().pad__length() = INTEGER(pad_length);
  field.tail().esp__null__tail().next__hdr() = 
    INTEGER(ptr[bb.get_read_len() - 1 - 12]);
  field.tail().esp__null__tail().padding()() =
    OCTETSTRING(pad_length, &ptr[bb.get_read_len() - 1 - pad_length - 13]);
  field.tail().esp__null__tail().auth__data() =
    OCTETSTRING(12, &ptr[bb.get_read_len() - 1 - 11]);
  // Remove the esp tail from the buffer
  pos = bb.get_pos();
  bb.set_pos(bb.get_len() - pad_length - 14);
  bb.cut_end();
  bb.set_pos(pos);
  return field.tail().esp__null__tail().next__hdr();
}

/* ================= End of common part for IPv4 and IPv6 ================= */



/* ================= IPv4 ================= */

void
Encode_IPv4_total_length(TTCN_Buffer & bb)
{
  size_t pkt_len = bb.get_len();
  unsigned char *ptr = Get_TTCN_Buffer_data(bb);
  
  ptr[2] = pkt_len / 256;
  ptr[3] = pkt_len - (pkt_len / 256) * 256;
}

void
Encode_IPv4_ESP_tails(TTCN_Buffer & bb,
                      const OPTIONAL < IPv4__extension__headers > &field)
{
  Log_function_name_on_enter();

  if (! field.ispresent()) return;
  for (int i = field().size_of(); i > 0; i--)
  {
    if (field()[i - 1].get_selection() == IPv4__extension__header::ALT_esp__hdr)
      field()[i - 1].esp__hdr().tail().encode(IP__ESP__tail__part_descr_, bb, 
                                                TTCN_EncDec::CT_RAW);
  }
}

void
Encode_IPv4_MINE_header(TTCN_Buffer & bb, const IP__MINE__header & field)
{
  TTCN_Buffer mine_bb;
  
  Log_function_name_on_enter();
  
  field.encode(IP__MINE__header_descr_, mine_bb, TTCN_EncDec::CT_RAW);
  // Calculating checksum
  Calculate_MINE_cksum(mine_bb, field);
  bb.put_s(mine_bb.get_len(), mine_bb.get_data());
}

void
Encode_IPv4_extension_headers(TTCN_Buffer & bb,
                              const OPTIONAL < IPv4__extension__headers > &field)
{
  Log_function_name_on_enter();

  if (! field.ispresent()) return;
  
  for (int i = 0; i < field().size_of(); i++)
  {
    switch ( field()[i].get_selection())
    {
    case IPv4__extension__header::ALT_mine__hdr:
      Encode_IPv4_MINE_header(bb, field()[i].mine__hdr());
      break;
    case IPv4__extension__header::ALT_ah__hdr:
       field()[i].ah__hdr().encode(IP__AH__header_descr_, bb, TTCN_EncDec::CT_RAW);
      break;
    case IPv4__extension__header::ALT_gre2__hdr:
       field()[i].gre2__hdr().encode(IP__GRE2__header_descr_, bb, TTCN_EncDec::CT_RAW);
      break;
    case IPv4__extension__header::ALT_esp__hdr:
       // NOTE: ESP tails are encoded after the complete packet is encoded
       field()[i].esp__hdr().header().encode(IP__ESP__header__part_descr_, bb, TTCN_EncDec::CT_RAW);
      break;
    case IPv4__extension__header::ALT_raw__hdr:
       field()[i].raw__hdr().encode(OCTETSTRING_descr_, bb, TTCN_EncDec::CT_RAW);
      break;
    default:
       TTCN_error("Unknown IPv4 extension header type");
    }
  }
}

OCTETSTRING
f__IPv4__enc(IPv4__packet const &pdu)
{
  TTCN_Buffer bb;

  Log_function_name_on_enter();
  if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("Encoding IPv4_packet: ");
    pdu.log();
    TTCN_Logger::end_event();
  }
  pdu.header().encode(IPv4__header_descr_, bb, TTCN_EncDec::CT_RAW);
  Encode_IPv4_extension_headers(bb, pdu.ext__headers());
  if (pdu.payload().ispresent())
    pdu.payload()().encode(OCTETSTRING_descr_, bb, TTCN_EncDec::CT_RAW);
  Encode_IPv4_ESP_tails(bb, pdu.ext__headers());
  Encode_IPv4_total_length(bb);
  return OCTETSTRING(bb.get_len(), bb.get_data());
}

OCTETSTRING
f__IPv4__enc__eth(IPv4__packet const &pdu)
{
  TTCN_Buffer bb;

  Log_function_name_on_enter();
  if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("Encoding IPv4_packet: ");
    pdu.log();
    TTCN_Logger::end_event();
  }
  pdu.header().encode(IPv4__header_descr_, bb, TTCN_EncDec::CT_RAW);
  Encode_IPv4_extension_headers(bb, pdu.ext__headers());
  if (pdu.payload().ispresent())
    pdu.payload()().encode(OCTETSTRING_descr_, bb, TTCN_EncDec::CT_RAW);
  Encode_IPv4_ESP_tails(bb, pdu.ext__headers());
  Encode_IPv4_total_length(bb);
  int data_length = bb.get_len();
  if (data_length > 45)
    return OCTETSTRING(bb.get_len(), bb.get_data());
  else
    return OCTETSTRING(bb.get_len(), bb.get_data()) + int2oct(0,46-data_length);
}

int
Decode_IPv4_extension_header(TTCN_Buffer & bb,
                             IPv4__extension__header & field,
                             int this_proto)
{
  Log_function_name_on_enter();

  if (this_proto == c__ip__proto__gre2)
    return Decode_GRE2_header(bb, field.gre2__hdr());
  else if (this_proto == c__ip__proto__mine)
    return Decode_MINE_header(bb, field.mine__hdr());
  else if (this_proto == c__ip__proto__ah)
    return Decode_AH_header(bb, field.ah__hdr());
  else if (this_proto == c__ip__proto__esp)
    return Decode_ESP_header(bb, field.esp__hdr());
  else
    TTCN_error("Unknown extension header type %u", this_proto);
}

void
Decode_IPv4_extension_headers(TTCN_Buffer & bb,
                              OPTIONAL < IPv4__extension__headers > &field,
                              int next_proto)
{
  int level = 0;

  Log_function_name_on_enter();

  while (next_proto == c__ip__proto__ah  || next_proto == c__ip__proto__mine || 
         next_proto == c__ip__proto__esp || next_proto == c__ip__proto__gre2)
  {
    next_proto = Decode_IPv4_extension_header(bb, field()[level], next_proto);
    level++;
  }

  // No extension headers found
  if (level == 0)
    field = OMIT_VALUE;
}

IPv4__packet
f__IPv4__dec(OCTETSTRING const &data)
{
  const unsigned char *raw_data = (const unsigned char *) data;
  IPv4__packet pdu;
  TTCN_Buffer bb;

  Log_function_name_on_enter();

  bb.clear();
  bb.put_s(data.lengthof(), raw_data);

  pdu.header().decode(IPv4__header_descr_, bb, TTCN_EncDec::CT_RAW);
  Decode_IPv4_extension_headers(bb, pdu.ext__headers(), pdu.header().proto());
  int payload_length = pdu.header().tlen() - 4 * pdu.header().hlen();
  if (payload_length == 0)
    pdu.payload() = OMIT_VALUE;
  else{
    int rem_data_length=bb.get_read_len();
    pdu.payload()() = OCTETSTRING(payload_length>rem_data_length?rem_data_length:payload_length, bb.get_read_data());
    
  }

  Log_object(pdu);
  return pdu;
}

General__Types::OCT2
f__IPv4__checksum(OCTETSTRING const &data)
{
  // IPv4 chekcsum is calculated over the IPv4 header only
  
  Log_function_name_on_enter();
  if(data.lengthof()<1) return OCTETSTRING(2,(const unsigned char *)"\0\0");
  int datalen = 4 * (((const unsigned char *) data)[0] & 0x0f);
  if(data.lengthof()<datalen) return OCTETSTRING(2,(const unsigned char *)"\0\0");
  
  
  return Calculate_cksum((const unsigned char *) data, datalen);
}

OCTETSTRING
f__IPv4__addr__enc(CHARSTRING const &cs__addr)
{
  unsigned char toAddr[5];
  
  Log_function_name_on_enter();

  if(inet_pton(AF_INET, cs__addr, toAddr)==1){
    return OCTETSTRING(4, toAddr);
  }

  return OCTETSTRING(0, toAddr);
}

CHARSTRING
f__IPv4__addr__dec(OCTETSTRING const &os__addr)
{
  char toAddr[INET_ADDRSTRLEN + 1];
  
  Log_function_name_on_enter();
  
  if(os__addr.lengthof()!=4) return CHARSTRING(0, "");
  
  inet_ntop(AF_INET, os__addr, toAddr, INET6_ADDRSTRLEN);

  return CHARSTRING(strlen(toAddr), toAddr);
}




/* ================= IPv6 ================= */

void
Encode_IPv6_payload_length(TTCN_Buffer & bb)
{
  size_t payload_len;
  unsigned char *ptr = Get_TTCN_Buffer_data(bb);
  
  if (bb.get_len() < 40)
  {
    unsigned int len = bb.get_len();
    TTCN_error("IPv6 packet is less than 40 bytes, cannot calculate the "
               "payload length. Length is %u.", len);
  }
  payload_len = bb.get_len() - 40;
  ptr[4] = payload_len / 256;
  ptr[5] = payload_len - (payload_len / 256) * 256;
  
  Log_function_name_on_leave();
}

void
Encode_IPv6_ESP_tails(TTCN_Buffer & bb,
                      const OPTIONAL < IPv6__extension__headers > &field)
{
  Log_function_name_on_enter();

  if (! field.ispresent()) return;
  for (int i = field().size_of(); i > 0; i--)
  {
    if (field()[i - 1].get_selection() == IPv6__extension__header::ALT_esp__hdr )
      field()[i - 1].esp__hdr().tail().encode(IP__ESP__tail__part_descr_, bb, 
                                                TTCN_EncDec::CT_RAW);
  }
  
  Log_function_name_on_leave();
}

void
Encode_IPv6_extension_headers(TTCN_Buffer & bb,
                              const OPTIONAL < IPv6__extension__headers > &field)
{ 
  Log_function_name_on_enter();

  if (! field.ispresent()) return;
  
  for (int i = 0; i < field().size_of(); i++)
  {
    switch ( field()[i].get_selection())
    {
    case IPv6__extension__header::ALT_ah__hdr:
      field()[i].ah__hdr().encode(IP__AH__header_descr_, bb, TTCN_EncDec::CT_RAW);
      break;
    case IPv6__extension__header::ALT_gre2__hdr:
      field()[i].gre2__hdr().encode(IP__GRE2__header_descr_, bb, TTCN_EncDec::CT_RAW);
      break;
    case IPv6__extension__header::ALT_esp__hdr:
      // NOTE: ESP tails are encoded after the complete packet is encoded
      field()[i].esp__hdr().header().encode(IP__ESP__header__part_descr_, bb, TTCN_EncDec::CT_RAW);
      break;
    case IPv6__extension__header::ALT_iPv6__Fragment__header:  
      field()[i].iPv6__Fragment__header().encode(IPv6__Fragment__header_descr_, bb, TTCN_EncDec::CT_RAW);
      break;      
    case IPv6__extension__header::ALT_raw__hdr:
      field()[i].raw__hdr().encode(OCTETSTRING_descr_, bb, TTCN_EncDec::CT_RAW);
      break;
    case IPv6__extension__header::ALT_general__exthdr: {
      // encode next__header field
      field()[i].general__exthdr().next__header().encode(INTEGER_descr_,bb, TTCN_EncDec::CT_RAW);
				  
      // calculate exthdr_length and add padding if necessary
      int remainder = (field()[i].general__exthdr().data().lengthof()+2) % 8;            
      if (remainder!=0)  //padding is needed
      {
        if((field()[i].general__exthdr().exthdr__length()) >= 0) // user given length
        {
         bb.put_c( (const unsigned char)field()[i].general__exthdr().exthdr__length());
        }
        else  // automatically calculated length
        {
         const unsigned char exthdr_length_modified = (2 + field()[i].general__exthdr().data().lengthof() + (8-remainder))/8 - 1;
	 bb.put_c(exthdr_length_modified);
        }        
        field()[i].general__exthdr().data().encode(OCTETSTRING_descr_,bb,TTCN_EncDec::CT_RAW);
	bb.put_os(int2oct(0,8-remainder));
      }
      else // no padding is needed
      {
        if((field()[i].general__exthdr().exthdr__length()) >= 0) // user given length
        {
         bb.put_c( (const unsigned char)field()[i].general__exthdr().exthdr__length()); 
        }     
        else  // automatically calculated length
        {
         const unsigned char exthdr_length_modified = (2 + field()[i].general__exthdr().data().lengthof())/8 - 1;
 	 bb.put_c(exthdr_length_modified); 
        }                 
        field()[i].general__exthdr().data().encode(OCTETSTRING_descr_,bb,TTCN_EncDec::CT_RAW);	     
      }          			  
      break;
      }
    default:
      TTCN_error("Unknown IPv6 extension header type");
    }
  }
  
  Log_function_name_on_leave();
}

OCTETSTRING
f__IPv6__enc(IPv6__packet const &pdu)
{
  TTCN_Buffer bb;
  OCTETSTRING result;

  Log_function_name_on_enter();
  
  pdu.header().encode(IPv6__header_descr_, bb, TTCN_EncDec::CT_RAW);
  Encode_IPv6_extension_headers(bb, pdu.ext__headers());
  if (pdu.payload().ispresent())
    pdu.payload()().encode(OCTETSTRING_descr_, bb, TTCN_EncDec::CT_RAW);
  Encode_IPv6_ESP_tails(bb, pdu.ext__headers());
  Encode_IPv6_payload_length(bb);
  result = OCTETSTRING(bb.get_len(), bb.get_data());
  
  Log_function_name_on_leave();
  
  return result;
}

int
Decode_Frag_header(TTCN_Buffer & bb, IPv6__Fragment__header & field)
{
  Log_function_name_on_enter();

  field.decode(IPv6__Fragment__header_descr_, bb, TTCN_EncDec::CT_RAW);
  return field.next__hdr();
}

int
Decode_general_IPv6_extension_header(TTCN_Buffer & bb,
                                     IPv6__general__IPv6__extension__header &
                                     field)
{
  const unsigned char *ptr;
  size_t data_len;

  Log_function_name_on_enter();

  ptr = bb.get_read_data();
  if (bb.get_read_len() < 8)
  {
    Log_object(bb);
    TTCN_error("There is not enough bytes in the packet to decode the "
               "general IPv6 extenstion header");
  }
  field.next__header() = INTEGER(ptr[0]);
  field.exthdr__length() = INTEGER(ptr[1]);
  data_len = (ptr[1]) * 8 + 6;
  if (bb.get_read_len() < data_len + 2)
  {
    Log_object(bb);
    TTCN_error("There is not enough bytes in the packet to decode the "
               "general IPv6 extenstion header");
  }
  field.data() = OCTETSTRING(data_len, &ptr[2]);
  bb.increase_pos(data_len + 2);
  return field.next__header();
}

int
Decode_IPv6_extension_header(TTCN_Buffer & bb,
                             IPv6__extension__header & field,
                             int this_proto)
{
  Log_function_name_on_enter();

  if (this_proto == c__ip__proto__gre2)
    return Decode_GRE2_header(bb, field.gre2__hdr());
  else if (this_proto == c__ip__proto__ah)
    return Decode_AH_header(bb, field.ah__hdr());
  else if (this_proto == c__ip__proto__esp)
    return Decode_ESP_header(bb, field.esp__hdr());
  else if (this_proto == c__ip__proto__ipv6__frag)
    return Decode_Frag_header(bb, field.iPv6__Fragment__header());
  else
    return Decode_general_IPv6_extension_header(bb, field.general__exthdr());
}

void
Decode_IPv6_extension_headers(TTCN_Buffer & bb,
                              OPTIONAL < IPv6__extension__headers > &field,
                              int next_proto)
{
  int level = 0;

  Log_function_name_on_enter();
  
  while( next_proto == c__ip__proto__ipv6__hopopt ||   //0    Hop-by-hop -> decoded into IPv6_general_IPv6_extension_header
         next_proto == c__ip__proto__ipv6__dest ||     //60   Destination -> decoded into IPv6_general_IPv6_extension_header
         next_proto == c__ip__proto__ipv6__route ||    //43   Routing  -> decoded into IPv6_general_IPv6_extension_header
         next_proto == c__ip__proto__ipv6__frag ||     //44   Fragment -> decoded into IPv6_Fragment_header
         next_proto == c__ip__proto__ah ||             //51   Authentication  -> decoded into IP_AH_header
         next_proto == c__ip__proto__esp ||            //50   Encapsulation security -> decoded into IP_ESP_header
         next_proto == c__ip__proto__mobility ||       //135  Mobility (Including PMIP) -> decoded into IPv6_general_IPv6_extension_header
         next_proto == c__ip__proto__gre2              //47   GRE  -> decoded into IP_GRE2_header
  )      
  {
    next_proto = Decode_IPv6_extension_header(bb, field()[level], next_proto);
    level++;
  }

  // No extension headers found
  if (level == 0)
    field = OMIT_VALUE;
}

IPv6__packet
f__IPv6__dec(const OCTETSTRING &data)
{
  const unsigned char *raw_data = (const unsigned char *) data;
  IPv6__packet pdu;
  TTCN_Buffer bb;

  Log_function_name_on_enter();

  bb.clear();
  bb.put_s(data.lengthof(), raw_data);

  pdu.header().decode(IPv6__header_descr_, bb, TTCN_EncDec::CT_RAW);
  Decode_IPv6_extension_headers(bb, pdu.ext__headers(), 
                                pdu.header().nexthead());
  if (bb.get_read_len() == 0)
    pdu.payload() = OMIT_VALUE;
  else
    pdu.payload()() = OCTETSTRING(bb.get_read_len(), bb.get_read_data());

  Log_object(pdu);
  return pdu;
}

CHARSTRING
f__IPv6__addr__dec(OCTETSTRING const &os__addr)
{
  char toAddr[INET6_ADDRSTRLEN + 1];
  
  Log_function_name_on_enter();
  
  if(os__addr.lengthof()!=16) return CHARSTRING(0, "");
  
  inet_ntop(AF_INET6, os__addr, toAddr, INET6_ADDRSTRLEN);

  return CHARSTRING(strlen(toAddr), toAddr);
}

OCTETSTRING
f__IPv6__addr__enc(CHARSTRING const &cs__addr)
{
  unsigned char toAddr[17];
  
  Log_function_name_on_enter();

  if(inet_pton(AF_INET6, cs__addr, toAddr)==1){
    return OCTETSTRING(16, toAddr);
  }

  return OCTETSTRING(0, toAddr);

}

}//namespace
