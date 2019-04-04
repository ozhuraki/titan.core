/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               TCP_EncDec.cc
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 675
//  Updated:            2010-03-09
//  Contact:            http://ttcn.ericsson.se
//  Reference:          RFC 793          

#include "TCP_Types.hh"


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

namespace TCP__Types 
{

void 
Calculate_cksum(const unsigned char *ptr, int datalen, unsigned char * pl_checksum)
{    
  Log_function_name_on_enter();
  
  unsigned long sum = 0;
  
  for (int i = 0; i <= datalen - 2; i = i + 2)
    sum += (ptr[i + 1] << 8) + ptr[i]; 

  if (datalen % 2) // datalen is odd
  {
    sum += ptr[datalen - 1];
  }

  sum = (sum & 0xFFFF) + (sum >> 16);
  sum = (sum & 0xFFFF) + (sum >> 16);
  sum = ~sum;

  pl_checksum[0] = Get_LSB(sum);
  pl_checksum[1] = Get_MSB(sum);
  
  Log_function_name_on_leave();

  return;
}

void
calc_TCP_checksum_IPv4(
       const unsigned char * pl__ip__source,
       const unsigned char * pl__ip__dest,
       const unsigned char * pl__tcp__segment,
       unsigned int pl_tcp_segment_length,
       unsigned char * pl__checksum  
      ) 
{
   Log_function_name_on_enter();

   unsigned char tcpBuf[65536];
   unsigned char zero = 0x00;   
   unsigned char six = 0x06;
   unsigned char length_u =(pl_tcp_segment_length & 0xff00 ) >> 8;   
   unsigned char length_l =(pl_tcp_segment_length & 0xff   );
   
   //construct pseudo header
   memcpy(tcpBuf,pl__ip__source,4);
   memcpy(tcpBuf+4,pl__ip__dest,4);
   memcpy(tcpBuf+8,&zero,1);     
   memcpy(tcpBuf+9,&six,1);    
   memcpy(tcpBuf+10,&length_u,1);        
   memcpy(tcpBuf+11,&length_l,1);
   memcpy(tcpBuf+12,pl__tcp__segment,pl_tcp_segment_length);  
   
   Calculate_cksum(tcpBuf, pl_tcp_segment_length+12,pl__checksum); 
   
   Log_function_name_on_leave();
   
   return;       
}

void
calc_TCP_checksum_IPv6(
       const unsigned char * pl__ip__source,
       const unsigned char * pl__ip__dest,
       const unsigned char * pl__tcp__segment,
       unsigned int pl_tcp_segment_length,
       unsigned char * pl__checksum  
      ) 
{
   Log_function_name_on_enter();

   unsigned char tcpBuf[65536];
   unsigned char zero = 0x00;   
   unsigned char six = 0x06;
   
   unsigned char length_1 =(pl_tcp_segment_length & 0xff000000 ) >> 24; 
   unsigned char length_2 =(pl_tcp_segment_length & 0xff0000 ) >> 16; 
   unsigned char length_3 =(pl_tcp_segment_length & 0xff00 ) >> 8;   
   unsigned char length_4 =(pl_tcp_segment_length & 0xff   );
  
   //construct pseudo header
   memcpy(tcpBuf,pl__ip__source,16);
   memcpy(tcpBuf+16,pl__ip__dest,16);
   memcpy(tcpBuf+32,&length_1,1); 
   memcpy(tcpBuf+33,&length_2,1);  
   memcpy(tcpBuf+34,&length_3,1);  
   memcpy(tcpBuf+35,&length_4,1);         
   memcpy(tcpBuf+36,&zero,1);
   memcpy(tcpBuf+37,&zero,1);
   memcpy(tcpBuf+38,&zero,1); 
   memcpy(tcpBuf+39,&six,1);      
   memcpy(tcpBuf+40,pl__tcp__segment,pl_tcp_segment_length);  
   
   Calculate_cksum(tcpBuf, pl_tcp_segment_length+40,pl__checksum); 
   
   Log_function_name_on_leave();
   
   return;       
}


 OCTETSTRING
  f__enc__PDU__TCP(             
              const OCTETSTRING& pl__ip__source,
              const OCTETSTRING& pl__ip__dest,
              const PDU__TCP& pdu, 
              const BOOLEAN& pl__autoDataOffset,
              const BOOLEAN& pl__autoChecksum
  )
  {
    
    Log_function_name_on_enter();
    
    if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
      TTCN_Logger::begin_event(TTCN_DEBUG);
      TTCN_Logger::log_event("Encoding PDU_TCP: ");
      pdu.log();
      TTCN_Logger::end_event();
    }
            
    TTCN_Buffer bb;
  
    // make local copy of PDU
    PDU__TCP pdu2 = pdu;
    
    // Automatically add padding to options field if length of options is not divisible by 4 
    if(pdu2.options().ispresent())
      {
        int remainder = pdu2.options()().lengthof() % 4; 
        if(remainder > 0)
        {           
          pdu2.options()() = pdu2.options()() + int2oct(0,4-remainder);
        }
      } 
    
    // Automatically Calculate data_offset if pl__autoDataOffset is true (default)
    if(pl__autoDataOffset)
    {
       if (pdu2.options().ispresent())
       {
         pdu2.data__offset() = 5 + (pdu2.options()().lengthof())/4;
       }
       else 
       { 
         pdu2.data__offset() = 5; 
       } 
    } 
    
    // RAW Encode PDU  
    pdu2.encode(PDU__TCP_descr_, bb, TTCN_EncDec::CT_RAW);       
    OCTETSTRING PDU2 = OCTETSTRING(bb.get_len(), bb.get_data());
    
    //calculate checksum and put it in stream
    if(pl__autoChecksum)
    {   
      unsigned char ip_type;
      if ((pl__ip__source.lengthof() == 4) && (pl__ip__dest.lengthof() == 4))
      ip_type = 4;
      else if ((pl__ip__source.lengthof() == 16) && (pl__ip__dest.lengthof() == 16))
      ip_type = 6;
      else
      TTCN_error("Source and Destination IP addresses are not both IPv4 or IPv6");
         
      unsigned char *  pdu2_stream =  (unsigned char * )(const unsigned char *) PDU2;
      pdu2_stream[16] = 0x00;
      pdu2_stream[17] = 0x00;       
      unsigned char checksum[2]; 
        
      if (ip_type == 4)
      {
       calc_TCP_checksum_IPv4(
         (const unsigned char *)pl__ip__source,
         (const unsigned char *)pl__ip__dest,
         (const unsigned char *)pdu2_stream,
          bb.get_len(),
          checksum );
      }    
      else  // IPv6  
      {
       calc_TCP_checksum_IPv6(
         (const unsigned char *)pl__ip__source,
         (const unsigned char *)pl__ip__dest,
         (const unsigned char *)pdu2_stream,
         bb.get_len(),
         checksum );
      }         
                
      pdu2_stream[16] = checksum[0];
      pdu2_stream[17] = checksum[1];  
    }
        
    bb.clear(); 
    
     if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
      TTCN_Logger::begin_event(TTCN_DEBUG);
      TTCN_Logger::log_event("Encoded PDU_TCP: ");
      PDU2.log();
      TTCN_Logger::end_event();
    }
    
    Log_function_name_on_leave(); 
             
    return PDU2;
  }
  
 PDU__TCP
  f__dec__PDU__TCP(OCTETSTRING const &stream)
  {  
    Log_function_name_on_enter();
    
    if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
      TTCN_Logger::begin_event(TTCN_DEBUG);
      TTCN_Logger::log_event("Decoding PDU_TCP: ");
      stream.log();
      TTCN_Logger::end_event();
    }
        
    const unsigned char *raw_data = (const unsigned char *) stream;
    PDU__TCP tcp_packet;
    TTCN_Buffer bb;     
                
    bb.put_s(20,raw_data);
    tcp_packet.decode(PDU__TCP_descr_, bb, TTCN_EncDec::CT_RAW);
    
    int data_length = stream.lengthof() - tcp_packet.data__offset()*4;
    int options_length = stream.lengthof() - data_length - 20;
        
    if (options_length == 0)
      tcp_packet.options() = OMIT_VALUE;
    else
      tcp_packet.options()() = OCTETSTRING(options_length,raw_data + 20 );

    if (data_length == 0)
      tcp_packet.data() = OMIT_VALUE;
    else
      tcp_packet.data()() = OCTETSTRING(data_length,raw_data + 20 + options_length);
   
    bb.clear();
     
    Log_object(tcp_packet);
     
    if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
      TTCN_Logger::begin_event(TTCN_DEBUG);
      TTCN_Logger::log_event("Decoded PDU_TCP: ");
      tcp_packet.log();
      TTCN_Logger::end_event();
    }    
        
    Log_function_name_on_leave();
    
    return tcp_packet;  
 } 
 
 
 BOOLEAN 
 f__TCP__verify__checksum
 (
 const OCTETSTRING& stream,
 const OCTETSTRING& pl__ip__source, // from IPv6 header
 const OCTETSTRING& pl__ip__dest // from IPv6 header
)
{
   Log_function_name_on_enter();

   unsigned char ip_type;
   if ((pl__ip__source.lengthof() == 4) && (pl__ip__dest.lengthof() == 4))
     ip_type = 4;
   else if ((pl__ip__source.lengthof() == 16) && (pl__ip__dest.lengthof() == 16))
     ip_type = 6;
      else
   TTCN_error("Source and Destination IP addresses are not both IPv4 or IPv6");
   
    unsigned char *  pdu2_stream =  (unsigned char * )(const unsigned char *) stream;
    
    unsigned char received_checksum[2];
    received_checksum[0] = pdu2_stream[16];
    received_checksum[1] = pdu2_stream[17];

    pdu2_stream[16] = 0x00;
    pdu2_stream[17] = 0x00;             
    unsigned char calculated_checksum[2]; 
   if (ip_type == 4)
      {
       calc_TCP_checksum_IPv4(
         (const unsigned char *)pl__ip__source,
         (const unsigned char *)pl__ip__dest,
         (const unsigned char *)stream,
          stream.lengthof(),
          calculated_checksum );
      }    
      else  // IPv6  
      {
       calc_TCP_checksum_IPv6(
         (const unsigned char *)pl__ip__source,
         (const unsigned char *)pl__ip__dest,
         (const unsigned char *)stream,
         stream.lengthof(),
         calculated_checksum );
      }      
 
   if ((received_checksum[0] == calculated_checksum[0]) && (received_checksum[1] == calculated_checksum[1]))
   {
     Log_function_name_on_leave();  
     return TRUE;
   }
   else
   {
     TTCN_warning("Incorrect checksum received! \n Expected checksum: %x %x \n Received checksum: %x %x ",
     calculated_checksum[0],
     calculated_checksum[1],
     received_checksum[0],
     received_checksum[1]
   ); 
   Log_function_name_on_leave(); 
   return FALSE;
   }
} 
 
 
 
 
}
