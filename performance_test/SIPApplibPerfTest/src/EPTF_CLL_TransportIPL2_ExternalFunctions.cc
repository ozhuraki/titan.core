/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               EPTF_CLL_TransportIPL2_ExternalFunctions.cc
//  Description:        Implementation of the external functions of EPTF Common Transport IPL2
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 512
//  Updated:            2010-07-21
//  Contact:            http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <TTCN3.hh>
#include <netinet/in.h> // htons
//#include <arpa/inet.h> // inet_ntoa
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include "EPTF_CLL_TransportIPL2_Functions.hh"
#include "IP_Types.hh"

using namespace IP__Types;

namespace EPTF__CLL__TransportIPL2__Functions
{

  INTEGER f__EPTF__TransportIPL2__tcpIsn(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long tmp = 1000000 * (unsigned long long)tv.tv_sec + tv.tv_usec;
    tmp >>= 2;
    
//    tmp &= 0x0fffffff;
    
    return INTEGER((int)tmp);
  }

  // and operation for integers
  INTEGER f__EPTF__and4i(const INTEGER& i1, const INTEGER& i2)
  {
    return INTEGER((int)i1 & (int)i2);
  }

  // or operation for integers
  INTEGER f__EPTF__or4i(const INTEGER& i1, const INTEGER& i2)
  {
    return INTEGER((int)i1 | (int)i2);
  }

  // xor operation for integers
  INTEGER f__EPTF__xor4i(const INTEGER& i1, const INTEGER& i2)
  {
    return INTEGER((int)i1 ^ (int)i2);
  }

  // not operation for integers
  INTEGER f__EPTF__not4i(const INTEGER& i)
  {
    return INTEGER(~(int)i);
  }

  // shift left operation for integers
  INTEGER f__EPTF__intShiftLeft(const INTEGER& i, const INTEGER& bits)
  {
    return INTEGER((int)i << (int)bits);
  }

  // shift right operation for integers
  INTEGER f__EPTF__intShiftRight(const INTEGER& i, const INTEGER& bits)
  {
    return INTEGER((int)i >> (int)bits);
  }

  INTEGER f__EPTF__mod32Add(const INTEGER& i1, const INTEGER& i2)
  {
    unsigned long l1 = (int)i1;
    unsigned long l2 = (int)i2;
    l1 += l2;
    return INTEGER((int)l1);
  }

  INTEGER f__EPTF__mod32Sub(const INTEGER& i1, const INTEGER& i2)
  {
    unsigned long l1 = (int)i1;
    unsigned long l2 = (int)i2;
    l1 -= l2;
    return INTEGER((int)l1);
  }

  BOOLEAN f__EPTF__mod32Less(const INTEGER& i1, const INTEGER& i2)
  {
    if(i1 == i2) return BOOLEAN(FALSE);
    return f__EPTF__mod32LessOrEqual(i1, i2);
  }

  BOOLEAN f__EPTF__mod32LessOrEqual(const INTEGER& i1, const INTEGER& i2)
  {
    unsigned long l1 = (int)i1;
    unsigned long l2 = (int)i2;
    l2 -= l1;
    return BOOLEAN(l2 <= 0x7FFFFFFF);
  }

  BOOLEAN f__EPTF__mod32Greater(const INTEGER& i1, const INTEGER& i2)
  {
    if(i1 == i2) return BOOLEAN(FALSE);
    return f__EPTF__mod32GreaterOrEqual(i1, i2);
  }

  BOOLEAN f__EPTF__mod32GreaterOrEqual(const INTEGER& i1, const INTEGER& i2)
  {
    unsigned long l1 = (int)i1;
    unsigned long l2 = (int)i2;
    l1 -= l2;
    return BOOLEAN(l1 <= 0x7FFFFFFF);
  }

  OCTETSTRING f__EPTF__TransportIPL2__insertOctets(
    const OCTETSTRING &pl_buffer,
    const OCTETSTRING &pl_fragment,
    const INTEGER &pl_offset)
  {
    int len = (int)pl_offset + pl_fragment.lengthof();
    int bufLen = pl_buffer.lengthof();
    if(len < bufLen) len = bufLen;

    unsigned char *buf = new unsigned char[len + 1];

    memcpy(buf, (const unsigned char*)pl_buffer, pl_buffer.lengthof());
    memcpy(buf + pl_offset, (const unsigned char*)pl_fragment, pl_fragment.lengthof());
    OCTETSTRING retVal = OCTETSTRING(len, buf);

    delete []buf;
    return retVal;
  }

  CHARSTRING f__EPTF__TransportIPL2__ip2str(const OCTETSTRING& pl_ipAddr)
  {
    if(pl_ipAddr.lengthof() != 4) return CHARSTRING("");
    char str[32];
    const unsigned char *addr = pl_ipAddr;
    char *p = str;
    for(int i=0; i<4; i++) {
      unsigned char tmp = *addr++;
      bool gr100 = false;
      if(tmp>=200) {
        *p++ = '2';
        tmp -= 200;
        gr100 = true;
      } else if(tmp>=100) {
        *p++ = '1';
        tmp -= 100;
        gr100 = true;
      }
      if(tmp >= 10) {
        *p = '0';
        do { (*p)++; tmp -= 10; } while(tmp >= 10);
        p++;
      } else if(gr100) {
        *p++ = '0';
      }
      *p++ = tmp + '0';
      *p++= '.';
    }
    return CHARSTRING(p-str-1, str);
/*    struct in_addr tmp;
    const unsigned char *addr = pl_ipAddr;
    tmp.s_addr = (addr[0]<<24) | (addr[1]<<16) | (addr[2]<<8) | addr[3];
    return CHARSTRING(inet_ntoa(tmp));*/
  }

  OCTETSTRING f__EPTF__TransportIPL2__getHashKey4UDP(const OCTETSTRING &pl__locAddr, const INTEGER &pl__locPort)
  {
    static unsigned char buff[8];
    if(pl__locAddr.lengthof() != 4) {
      TTCN_error("f_EPTF_TransportIPL2_getHashKey4UDP: local address length is %d octet, expected 4.", pl__locAddr.lengthof());
      return OCTETSTRING(0,NULL);
    }
    int tmp = (int)pl__locPort;
    const unsigned char *addr = (const unsigned char *) pl__locAddr;
    buff[0] = tmp;
    buff[1] = addr[3];
    buff[2] = tmp >> 8;
    buff[3] = addr[2];
    buff[4] = addr[1];
    buff[5] = addr[0];
    buff[6] = 1; // UDP
    return OCTETSTRING(7, buff);
  }

  OCTETSTRING f__EPTF__TransportIPL2__getHashKey4TCP(
    const OCTETSTRING &pl__locAddr,
    const INTEGER &pl__locPort,
    const OCTETSTRING &pl__remAddr,
    const INTEGER &pl__remPort)
  {
    static unsigned char buff[16];
    if(pl__locAddr.lengthof() != 4) {
      TTCN_error("f_EPTF_TransportIPL2_getHashKey4UDP: local address length is %d octet, expected 4.", pl__locAddr.lengthof());
      return OCTETSTRING(0,NULL);
    }
    int locPort = (int)pl__locPort;
    const unsigned char *locAddr = (const unsigned char *) pl__locAddr;
    if(pl__remAddr.lengthof() != 4) {
      // listen socket
      buff[0] = locPort;
      buff[1] = locAddr[3];
      buff[2] = locPort >> 8;
      buff[3] = locAddr[2];
      buff[4] = locAddr[1];
      buff[5] = locAddr[0];
      buff[6] = 2; // TCP
      return OCTETSTRING(7, buff);
    } else {
      // connected socket
      int remPort = (int)pl__remPort;
      const unsigned char *remAddr = (const unsigned char *) pl__remAddr;
      buff[0] = locPort;
      buff[1] = locAddr[3];
      buff[2] = locPort >> 8;
      buff[3] = locAddr[2];
      buff[4] = locAddr[1];
      buff[5] = locAddr[0];
      buff[6] = 2; // TCP
      buff[7] = remPort;
      buff[8] = remAddr[3];
      buff[9] = remPort >> 8;
      buff[10] = remAddr[2];
      buff[11] = remAddr[1];
      buff[12] = remAddr[0];
      return OCTETSTRING(13, buff);
    }
  }

  // Note: the following function doesn't support extension headers.
  OCTETSTRING f__EPTF__TransportIPL2__encodeIP(
    const IPv4__packet &pdu)
  {
    int len = 0;
    if(pdu.payload().ispresent()) len = pdu.payload()().lengthof();
    if(len >= (65536 - 20)) {
      TTCN_warning("f_EPTF_TransportIPL2_encodeIP: invalid payload length %d, should be less than (65536 - 20)", len);
      return OCTETSTRING(0,NULL);
    }
    if(pdu.header().srcaddr().lengthof() < 4) {
      TTCN_Logger::begin_event(TTCN_WARNING);
      TTCN_Logger::log_event_str("f_EPTF_TransportIPL2_encodeIP: invalid source IP address ");
      pdu.header().srcaddr().log();
      TTCN_Logger::end_event();
      return OCTETSTRING(0,NULL);
    }
    if(pdu.header().dstaddr().lengthof() < 4) {
      TTCN_Logger::begin_event(TTCN_WARNING);
      TTCN_Logger::log_event_str("f_EPTF_TransportIPL2_encodeIP: invalid destination IP address ");
      pdu.header().dstaddr().log();
      TTCN_Logger::end_event();
      return OCTETSTRING(0,NULL);
    }
    static unsigned char buff[65536];
    unsigned char *p = buff;
    *p++ = 0x45; // note the version is fixed at v4 and header length is fixed at 5 words (20 bytes)
//    *p++ = ((int)pdu.header().ver())<<4 | (int)pdu.header().hlen(); // - don't uncomment unless extension headers are implemented
    *p++ = pdu.header().tos(); // Type Of Service
    int tmp = len + 20;
    *p++ = tmp >> 8; // Total Length
    *p++ = tmp;
    tmp = pdu.header().id();
    *p++ = tmp >> 8; // Identification
    *p++ = tmp;
    tmp = (int)pdu.header().foffset();
    *p = (tmp >> 8) & 0x1f; // MSB of fragment offset
    if(*((const unsigned char *)pdu.header().dfrag())) *p |= 0x40; // flags
    if(*((const unsigned char *)pdu.header().mfrag())) *p |= 0x20;
    p++;
    *p++ = tmp; // LSB of fragment offset
    *p++ = (int)pdu.header().ttl(); // time to live
    *p++ = (int)pdu.header().proto(); // protocol
    *p++ = 0; // checksum MSB - will be calculated later
    *p++ = 0; // checksum LSB
    const unsigned char* addr = (const unsigned char*) pdu.header().srcaddr();
    *p++ = addr[0];
    *p++ = addr[1];
    *p++ = addr[2];
    *p++ = addr[3];
    addr = (const unsigned char*) pdu.header().dstaddr();
    *p++ = addr[0];
    *p++ = addr[1];
    *p++ = addr[2];
    *p++ = addr[3];

    if(pdu.ext__headers().ispresent()) {
      TTCN_warning("f_EPTF_TransportIPL2_encodeIP: extension headers are not supported, use f_IPv4_enc instead.");
    }

    if(len > 0) memcpy(p, (const unsigned char*)pdu.payload()(), len);

    const unsigned short *stemp = (const unsigned short*) buff;
    unsigned long sum = 0;
    // calculate checksum for IP header
    for (int i = 0; i < 10; i++) {
      sum += htons(*stemp++);
    }
    sum = (sum & 0xFFFF) + (sum >> 16);
    sum = ~sum;
    buff[10] = sum >> 8;
    buff[11] = sum;

    return OCTETSTRING(len + 20, buff);
  }

  BOOLEAN f__EPTF__TransportIPL2__decodeIP(
    const OCTETSTRING &data,
    IPv4__packet &pdu) // returns true if packet is a fragment, false if not
  {
    const unsigned char *p = (const unsigned char*) data;
    boolean isFragment = FALSE;
    pdu.header().ver() = *p >> 4;
    int hlen = *p & 0x0f;
    pdu.header().hlen() = hlen;
    hlen <<= 2;
    p++;
    pdu.header().tos() = *p++;
    int pdulen = (*p << 8) + *(p+1);
    p+=2;
    if(pdulen > data.lengthof()) { // libpcap (1.0.0.6) bug maybe?
      TTCN_warning("IP datagram length %d, ethernet payload length only %d bytes, using latter.",
        pdulen, data.lengthof());
      pdulen = data.lengthof();
    }
    pdu.header().tlen() = pdulen;
    pdu.header().id() = (*p << 8) + *(p+1);
    p+=2;
    unsigned char zero = 0, one = 1;
    pdu.header().res() = BITSTRING(1, &zero);
    if(*p & 0x40) pdu.header().dfrag() = BITSTRING(1, &one);
    else pdu.header().dfrag() = BITSTRING(1, &zero);
    if(*p & 0x20) {
      pdu.header().mfrag() = BITSTRING(1, &one);
      isFragment = TRUE;
    }
    else pdu.header().mfrag() = BITSTRING(1, &zero);
    int tmp = ((*p & 0x1f) << 8) + *(p+1);
    pdu.header().foffset() = tmp;
    isFragment |= tmp > 0;
    p+=2;
    pdu.header().ttl() = *p++;
    pdu.header().proto() = *p++;
    pdu.header().cksum() = (*p << 8) + *(p+1);
    p+=2;
    pdu.header().srcaddr() = OCTETSTRING(4, p);
    p+=4;
    pdu.header().dstaddr() = OCTETSTRING(4, p);
    p+=4;
    p += hlen - 20;
    pdu.payload() = OCTETSTRING(pdulen - hlen, p);
    pdu.ext__headers() = OMIT_VALUE;
    return BOOLEAN(isFragment);
  }

/*OCTETSTRING f__EPTF__TransportIPL2__encodeUDPPseudoHead(
    const OCTETSTRING &srcaddr,
    const OCTETSTRING &dstaddr,
    const INTEGER &payloadlength)
  {
    static unsigned char buff[16];
    unsigned char *p = buff;
    *p++ = srcaddr[0].get_octet();
    *p++ = srcaddr[1].get_octet();
    *p++ = srcaddr[2].get_octet();
    *p++ = srcaddr[3].get_octet();
    *p++ = dstaddr[0].get_octet();
    *p++ = dstaddr[1].get_octet();
    *p++ = dstaddr[2].get_octet();
    *p++ = dstaddr[3].get_octet();
    *p++ = 0;
    *p++ = 17; // UDP protocol
    int len = (int) payloadlength;
    *p++ = len >> 8;
    *p = len;
    return OCTETSTRING(12, buff);
  }

  OCTETSTRING f__EPTF__TransportIPL2__encodeUDP(
    const INTEGER &srcport,
    const INTEGER &dstport,
    const OCTETSTRING &payload)
  {
    int len = payload.lengthof();
    if(len > (65536 - 8)) return OCTETSTRING(0,NULL);
    static short sbuff[32768];
    short *p = &sbuff[0];
    *p++ = htons((int) srcport);
    *p++ = htons((int) dstport);
    *p++ = htons(len);
    *p++ = 0; // checksum
    memcpy(p, (const unsigned char *) payload, len);
    return OCTETSTRING(len + 8, (unsigned char *)sbuff);
  }*/

  OCTETSTRING f__EPTF__TransportIPL2__encodeUDPWithChecksum(
    const OCTETSTRING &srcaddr,
    const INTEGER &srcport,
    const OCTETSTRING &dstaddr,
    const INTEGER &dstport,
    const OCTETSTRING &payload)
  {
    int len = payload.lengthof();
    int pdulen = len + 8;
    if(pdulen > 65535) return OCTETSTRING(0,NULL);
    const unsigned char *psrcaddr = (const unsigned char *)srcaddr;
    const unsigned char *pdstaddr = (const unsigned char *)dstaddr;
    if(psrcaddr == NULL || pdstaddr == NULL) return OCTETSTRING(0,NULL);

    // initializing checksum with UDP pseudo header
    unsigned long sum = psrcaddr[0] << 8 | psrcaddr[1];
    sum += psrcaddr[2] << 8 | psrcaddr[3];
    sum += pdstaddr[0] << 8 | pdstaddr[1];
    sum += pdstaddr[2] << 8 | pdstaddr[3];
    sum += 17; // UDP protocol
    sum += pdulen;

    // create UDP header + checksum update
    static unsigned short sbuff[32768];
    unsigned short *p = &sbuff[0];
    *p++ = htons((int) srcport);
    sum += (int) srcport;
    *p++ = htons((int) dstport);
    sum += (int) dstport;
    *p++ = htons(pdulen);
    sum += pdulen;
    p++; // skip checksum field - set later via sbuff[3]

    // copy payload + checksum update
    const unsigned char *ctemp = (const unsigned char*) payload;
    const unsigned short *stemp = (const unsigned short*) ctemp;
    for ( int i = 0; i < ( len >> 1 ); i++ ) {
      *p++ = stemp[i];
      sum += htons(stemp[i]);
    }
    if(len & 1) {
      unsigned char *cbuff = (unsigned char*)sbuff;
      cbuff[pdulen-1] = ctemp[len-1];
      sum += ctemp[len-1]<<8;
    }

    // finalize checksum, copy to buffer
    unsigned short checksum = htons(~((sum & 0xFFFF) + ((sum >> 16) & 0xFFFF)));
    if(checksum == 0) checksum = 0xFFFF;
    sbuff[3] = checksum;

    return OCTETSTRING(pdulen, (unsigned char *)sbuff);
  }

  BOOLEAN f__EPTF__TransportIPL2__decodeUDPWithChecksum(
    const OCTETSTRING &srcaddr,
    const OCTETSTRING &dstaddr,
    const OCTETSTRING &udppdu,
    INTEGER &srcport,
    INTEGER &dstport,
    OCTETSTRING &payload)
  {
    int pdulen = udppdu.lengthof(); // length of the whole UDP PDU
    int len = pdulen - 8; // length of payload
    if(pdulen > 65535 || len < 0 || srcaddr.lengthof() < 4 || dstaddr.lengthof() < 4) return BOOLEAN(FALSE);

    const unsigned char *psrcaddr = (const unsigned char *)srcaddr;
    const unsigned char *pdstaddr = (const unsigned char *)dstaddr;
    if(psrcaddr == NULL || pdstaddr == NULL) return BOOLEAN(FALSE);

    const unsigned char *ctemp = (const unsigned char*) udppdu;
    const unsigned short *stemp = (const unsigned short*) ctemp;
    int encoded_pdulen = ntohs(stemp[2]);
    if(encoded_pdulen != pdulen) {
#ifdef EPTF_DEBUG
      TTCN_Logger::log(TTCN_DEBUG, "f_EPTF_TransportIPL2_decodeUDPWithChecksum: "
        "incoming UDP PDU has invalid length. Encoded: %d, should be: %d",
        encoded_pdulen, pdulen);
#endif
      if(encoded_pdulen < pdulen) {
        pdulen = encoded_pdulen;
        len = encoded_pdulen - 8;
        if(len < 0) return BOOLEAN(FALSE);
      } else return BOOLEAN(FALSE);
    }

    // initializing checksum with UDP pseudo header
    unsigned long sum = psrcaddr[0] << 8 | psrcaddr[1];
    sum += psrcaddr[2] << 8 | psrcaddr[3];
    sum += pdstaddr[0] << 8 | pdstaddr[1];
    sum += pdstaddr[2] << 8 | pdstaddr[3];
    sum += 17; // UDP protocol
    sum += pdulen;

    // UDP header
    srcport = ntohs(stemp[0]);
    sum += (int)srcport;
    dstport = ntohs(stemp[1]);
    sum += (int)dstport;
    sum += encoded_pdulen;
    unsigned short encoded_checksum = ntohs(stemp[3]);
    stemp += 4;

    // payload
    for ( int i = 0; i < ( len >> 1 ); i++ ) {
      sum += ntohs(*stemp++);
    }
    if(len & 1) sum += ctemp[pdulen-1] << 8;

    unsigned short checksum = ~((sum & 0xFFFF) + ((sum >> 16) & 0xFFFF));
    if(checksum == 0) checksum = 0xFFFF;

    if(encoded_checksum != checksum) {
#ifdef EPTF_DEBUG
      TTCN_Logger::log(TTCN_DEBUG, "f_EPTF_TransportIPL2_decodeUDPWithChecksum: "
        "incoming UDP PDU has invalid checksum. Encoded: %4.4X, should be: %4.4X",
        encoded_checksum, checksum);
#endif
      return BOOLEAN(FALSE);
    }

    payload = OCTETSTRING(len, ctemp + 8);
    return BOOLEAN(TRUE);
  }


  void calcTcpChecksum(
    const unsigned char *pdu, int pdulen,
    const unsigned char *psrcaddr,
    const unsigned char *pdstaddr,
    unsigned char *ck1, unsigned char *ck2) // out
  {
    //if(pdulen < 20) return;
    // initializing checksum with TCP pseudo header
    unsigned long sum = psrcaddr[0] << 8 | psrcaddr[1];
    sum += psrcaddr[2] << 8 | psrcaddr[3];
    sum += pdstaddr[0] << 8 | pdstaddr[1];
    sum += pdstaddr[2] << 8 | pdstaddr[3];
    sum += 6; // TCP protocol
    sum += pdulen;

    for (int i=0; i<16; i+=2) {
      sum += (pdu[i] << 8) + pdu[i + 1];
    } // skip checksum field
    for (int i=18; i<pdulen; i+=2) {
      sum += (pdu[i] << 8) + pdu[i + 1];
    }

    if (pdulen & 1) {
      sum += pdu[pdulen-1] << 8;
    }

    // finalize checksum, copy to buffer
    sum = (sum & 0xFFFF) + (sum >> 16);
    sum = (sum & 0xFFFF) + (sum >> 16);
    sum = ~sum;
    *ck1 = (sum&0xff00)>>8;
    *ck2 = sum&0xff;
  }


  OCTETSTRING f__EPTF__TransportIPL2__encodeTCPWithChecksum(
  		const OCTETSTRING &srcaddr,
  		const OCTETSTRING &dstaddr,
  		const EPTF__CLL__TransportIPL2__Definitions::EPTF__TransportIPL2__PDU__TCP & pdu) {

	// for getting the necessary padding values
	// optionslen % 4 == 0 -> no pad needed
	// optionslen % 4 == 1 -> 3 zero pad needed
	// optionslen % 4 == 2 -> 2 zero pad  needed
	// optionslen % 4 == 3 -> 1 zero pad needed
	// The index of this array is the reminder of div 4
  	static const int padtable[4] = { 0, 3, 2, 1 };
  	int datalen = pdu.data().lengthof();

  	int optionslen_orig = pdu.options().lengthof();
  	int options_padlen = padtable[optionslen_orig & 0x03];
  	int optionslen = optionslen_orig + options_padlen;

  	int headlen = 20 + optionslen;
  	int pdulen = datalen + headlen;
  	if (pdulen > 65535) {
  		return OCTETSTRING(0, NULL);
  	}
  	const unsigned char *psrcaddr = (const unsigned char *) srcaddr;
  	const unsigned char *pdstaddr = (const unsigned char *) dstaddr;
  	if (psrcaddr == NULL || pdstaddr == NULL) {
  		return OCTETSTRING(0, NULL);
  	}

  	// create TCP header + checksum update
  	static unsigned char buff[65535];
  	unsigned char *p = &buff[0];
  	unsigned char *checksum_addr;

  	*p++ = pdu.source__port() >> 8;
  	*p++ = pdu.source__port();

  	*p++ = pdu.dest__port() >> 8;
  	*p++ = pdu.dest__port();

  	unsigned long seqno = (int) pdu.sequence__number();

  	*p++ = seqno >> 24;
  	*p++ = seqno >> 16;
  	*p++ = seqno >> 8;
  	*p++ = seqno;

  	unsigned long ackno = (int) pdu.acknowledgment__number();
  	*p++ = ackno >> 24;
  	*p++ = ackno >> 16;
  	*p++ = ackno >> 8;
  	*p++ = ackno;

  	unsigned char dataoffset = (headlen << 2) & 0xF0;
  	unsigned char flags = pdu.control__bits();
  	*p++ = dataoffset;
  	*p++ = flags;

  	*p++ = pdu.window() >> 8;
  	*p++ = pdu.window();

  	// Save the checksum pointer. It will be set later.
  	checksum_addr = p;
  	*p++ = 0;
  	*p++ = 0;

        // urgent pointer not used
  	*p++ = 0;
  	*p++ = 0;

  	// Adding options
  	if (optionslen_orig > 0) {
  		// OCTETSTRING in network byte order!!!!

  		memcpy(p, (const unsigned char*) pdu.options(), optionslen_orig);
                p += optionslen_orig;

  		//Add padding. Checksum update doesn't needed (sum+=0)
  		for (int i = 0; i < options_padlen; i++) {
  			*p++ = 0;
  		}
  	}

  	if (datalen > 0) {
  		// OCTETSTRING in network byte order!!!!
  		// copy payload + checksum update
  		memcpy(p, (const unsigned char*) pdu.data(), datalen);
                p += datalen;
  	}

        calcTcpChecksum(
          buff, pdulen,
          psrcaddr,
          pdstaddr,
          checksum_addr,
          checksum_addr + 1);

  	return OCTETSTRING(pdulen, (unsigned char *) buff);
  }

  BOOLEAN f__EPTF__TransportIPL2__verifyTCPChecksum(
                const OCTETSTRING &srcaddr,
                const OCTETSTRING &dstaddr,
  		const OCTETSTRING &tcppdu) {
    unsigned char ck1, ck2;
    const unsigned char *ppdu = (const unsigned char *)tcppdu;
    calcTcpChecksum(
      ppdu, tcppdu.lengthof(),
      (const unsigned char *)srcaddr,
      (const unsigned char *)dstaddr,
      &ck1, &ck2);
    if(ck1 != ppdu[16] || ck2 != ppdu[17]) {
      TTCN_Logger::log(TTCN_DEBUG,
        "TCP checksum verification failed.\n"
        " Expected %2.2X %2.2X\n Received %2.2X %2.2X",
        ck1, ck2, ppdu[16], ppdu[17]);
      return BOOLEAN(FALSE);
    }
    else return BOOLEAN(TRUE);
  }

  BOOLEAN f__EPTF__TransportIPL2__decodeTCP(
  		const OCTETSTRING &tcppdu,
  		EPTF__CLL__TransportIPL2__Definitions::EPTF__TransportIPL2__PDU__TCP &pdu) {

  	const unsigned char *ctemp = (const unsigned char*) tcppdu; // char list
  	int pdulen = tcppdu.lengthof(); // length of the whole TCP PDU

  	// Invalid pdu length (20=min header size)
  	if (pdulen > 65535 || pdulen < 20) {
  		return BOOLEAN(FALSE);
  	}

  	// s:0 c:0
        pdu.source__port() = (*ctemp << 8) + *(ctemp+1);
        ctemp += 2;

  	// s:1 c:2
  	pdu.dest__port() = (*ctemp << 8) + *(ctemp+1);
        ctemp += 2;

  	// s:2 c:4
        unsigned long tmp = 0;
        tmp = *ctemp++;
        tmp<<=8;
        tmp += *ctemp++;
        tmp<<=8;
        tmp += *ctemp++;
        tmp<<=8;
        tmp += *ctemp++;
        pdu.sequence__number() = (int)tmp;

  	// s:4 c:8
        tmp = *ctemp++;
        tmp<<=8;
        tmp += *ctemp++;
        tmp<<=8;
        tmp += *ctemp++;
        tmp<<=8;
        tmp += *ctemp++;
        pdu.acknowledgment__number() = (int)tmp;

  	// s:6 c:12
        int data_offset = (*ctemp & 0xF0) >> 2;
        ctemp++;
  	// reserved is zero but not verified

  	// s:6 c:13
        pdu.control__bits() = *ctemp++;

  	// s:7 c:14
  	pdu.window() = (*ctemp << 8) + *(ctemp+1);
        ctemp += 2;

  	// s:8 c:16 !!! CHECKSUM !!!
        ctemp += 2;

  	// s:9 c:18
        // urgent pointer not used
        ctemp += 2;

  	// s:10 c:20
  	int optionslen = data_offset - 20;
  	if (optionslen == 0) {
  		pdu.options() = OCTETSTRING(0, NULL);
  	} else if (optionslen > 0) {
                pdu.options() = OCTETSTRING(optionslen, ctemp);
                ctemp += optionslen;
  	} else {
  		// TODO: Log some fancy message
  	  	pdu.options() = OCTETSTRING(0, NULL);
  		pdu.data() = OCTETSTRING(0, NULL);
  		return BOOLEAN(FALSE);
  	}

  	int datalen = pdulen - data_offset;
  	if (datalen == 0) {
  		pdu.data() = OCTETSTRING(0, NULL);
  	} else if (datalen > 0) {
                pdu.data() = OCTETSTRING(datalen, ctemp);
                ctemp += datalen;
  	} else {
  		// TODO: Log some fancy message
  		pdu.data() = OCTETSTRING(0, NULL);
  		return BOOLEAN(FALSE);
  	}

      return BOOLEAN(TRUE);
  }

  #define RTF_UP          0x0001          /* route usable                 */
  #define PATH_PROCNET_ROUTE "/proc/net/route"

  static char *proc_gen_fmt(char *name, int more, FILE * fh, ...) {
  	char buf[512], format[512] = "";
  	char *title, *head, *hdr;
  	va_list ap;

  	if (!fgets(buf, (sizeof buf) - 1, fh))
  		return NULL;
  	strcat(buf, " ");

  	va_start(ap, fh);
  	title = va_arg(ap, char *);
  	for (hdr = buf; hdr;) {
  		while (isspace(*hdr) || *hdr == '|')
  			hdr++;
  		head = hdr;
  		hdr = strpbrk(hdr, "| \t\n");
  		if (hdr)
  			*hdr++ = 0;

  		if (!strcmp(title, head)) {
  			strcat(format, va_arg(ap, char *));
  			title = va_arg(ap, char *);
  			if (!title || !head)
  				break;
  		} else {
  			strcat(format, "%*s"); /* XXX */
  		}
  		strcat(format, " ");
  	}
  	va_end(ap);

  	if (!more && title) {
  		fprintf(stderr, "warning: %s does not contain required field %s\n",
  				name, title);
  		return NULL;
  	}
  	return strdup(format);
  }

  void f__EPTF__TransportIPL2__getHostRoutingTable(
		  EPTF__CLL__TransportIPL2__Definitions::EPTF__TransportIPL2__RouteTable &pl__table)
  {
	// Clear the table in any case.
	pl__table.set_size(0);

#ifdef LINUX
	char buff[1024], iface[16];
	unsigned int gate_addr, net_addr;
	unsigned int mask_addr;
	int num, iflags, metric, refcnt, use, mss, window, irtt;
	FILE *fp;

	fp = fopen(PATH_PROCNET_ROUTE, "r");
	char *fmt;

	if (!fp) {
		perror( PATH_PROCNET_ROUTE);
		printf("INET (IPv4) not configured in this system.\n");
		return;
	}

	irtt = 0;
	window = 0;
	mss = 0;

	fmt = proc_gen_fmt(PATH_PROCNET_ROUTE, 0, fp,
			"Iface", "%16s",
			"Destination", "%X",
			"Gateway", "%X",
			"Flags", "%X",
			"RefCnt", "%d",
			"Use", "%d",
			"Metric", "%d",
			"Mask", "%X",
			"MTU", "%d",
			"Window", "%d",
			"IRTT", "%d", NULL);
	/* "%16s %X %X %X %d %d %d %X %d %d %d\n" */

	int i=0;

	while (fgets(buff, 1023, fp)) {
		num = sscanf(buff, fmt, iface, &net_addr, &gate_addr, &iflags, &refcnt,
				&use, &metric, &mask_addr, &mss, &window, &irtt);

		if (num < 10 || !(iflags & RTF_UP)) {
			continue;
		}

		pl__table[i].destination() = OCTETSTRING(4, (const unsigned char*)&net_addr);
		pl__table[i].gateway() = OCTETSTRING(4, (const unsigned char*)&gate_addr);
		pl__table[i].genmask() = OCTETSTRING(4, (const unsigned char*)&mask_addr);
		pl__table[i].iface() = CHARSTRING(strlen(iface), (const char*)iface);

		i++;
	}
#endif
#if defined(SOLARIS) || defined(SOLARIS8)
  // Solaris8 to be implemented
#endif
  }

}
