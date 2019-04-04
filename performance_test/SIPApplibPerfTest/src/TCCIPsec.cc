/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               TCCIPsec.cc
//  Description:        TCC Useful Functions: IPsec Functions
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 472
//  Updated:            2012-10-31
//  Contact:            http://ttcn.ericsson.se
//
///////////////////////////////////////////////////////////////////////////////

#include "TCCIPsec_Definitions.hh"
#include "TCCIPsec_Functions.hh"
#include "Logger.hh"

#if defined USE_IPSEC || defined USE_KAME_IPSEC

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#ifdef LINUX
#include <linux/pfkeyv2.h>
#else
#include <net/pfkeyv2.h>
#endif

#if defined USE_KAME_IPSEC
// Missing constants
#define IPSEC_POLICY_DISCARD    0       /* discard the packet */
#define IPSEC_POLICY_NONE       1       /* bypass IPsec engine */
#define IPSEC_POLICY_IPSEC      2       /* pass to IPsec */
//#define IPSEC_POLICY_ENTRUST    3       /* consulting SPD if present. */
//#define IPSEC_POLICY_BYPASS     4       /* only for privileged socket. */
#define IPSEC_DIR_INBOUND       1
#define IPSEC_DIR_OUTBOUND      2
#define IPSEC_DIR_FORWARD       3
#define IPSEC_MODE_ANY          0
#define IPSEC_MODE_TRANSPORT    1
#define IPSEC_MODE_TUNNEL       2
#define IPSEC_LEVEL_DEFAULT     0       /* reference to system default */
#define IPSEC_LEVEL_USE         1       /* use SA if present. */
#define IPSEC_LEVEL_REQUIRE     2       /* require SA. */
#define IPSEC_LEVEL_UNIQUE      3       /* unique SA. */
#endif // defined USE_KAME_IPSEC

// Note: IPPROTO_ESP == 50, IPPROTO_AH == 51, IPPROTO_IPCOMP == 108

#endif // defined USE_IPSEC || defined USE_KAME_IPSEC
#define UDP_ENCAP_ESPINUDP  2
#define DEFAULT_NATT_PORT 4500

namespace TCCIPsec__Functions {

using namespace TCCIPsec__Definitions;

#if defined USE_IPSEC || defined USE_KAME_IPSEC

struct Error {
  int result;
  Error ( int r ) : result ( r ) {}
private:
  Error ();
};

int           sd = -1;

class PfKey
{
  unsigned int  seq;
  unsigned char rdBuf[2048];

  int cnvErr ( int osErr ) {
    int err = TCCIPsec__Result::socketError;
    switch ( osErr ) {
      case EPERM:
      case EACCES:
        err = TCCIPsec__Result::insufficientPrivilege; break;
      case ESRCH:
      case ENOENT:
        err = TCCIPsec__Result::notFound; break;
      case EINVAL:
        err = TCCIPsec__Result::parameterInvalid; break;
      case EEXIST:
        err = TCCIPsec__Result::alreadyExisted; break;
      default: ;
    }
    return err;
  }

  inline void dump ( const char * descr, const void * buf, unsigned int len )
  {
//    const unsigned char * data = (const unsigned char*) buf;
    TTCN_Logger::begin_event( TTCN_DEBUG );
    TTCN_Logger::log_event( "%s:   Dump of %03X bytes: ", descr, len );
/*    if ( data == 0 ) {
      TTCN_Logger::log_event( "Null pointer" );
    } else {
      TTCN_Logger::log_event("\n %03X ", 0);
      for ( unsigned int i=0; i < len; i++ ) {
        TTCN_Logger::log_event( " %02X", data[i] );
        if ( i % 16 == 15 ) {
          TTCN_Logger::log_char( '\n' );
          TTCN_Logger::log_event( " %03X ", i + 1 );
        }
      }
    }*/
    TTCN_Logger::end_event();
  }
    
public:
  PfKey () throw ( Error ) {
    if (sd < 0){
      sd = socket ( PF_KEY, SOCK_RAW, PF_KEY_V2 );
      if ( sd < 0 ) {
	TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: PfKey::PfKey: OS error: %i: \"%s\"", errno, strerror ( errno ) );
	throw Error( cnvErr ( errno ) );
      }
    }
    seq = 0;
    //    timeval tv;
    //    seq = ( gettimeofday ( & tv, 0 ) == 0 ) ? tv.tv_usec + tv.tv_sec * 1000000 : 0;
    TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: PfKey::PfKey: sd: %i,   seq: 0x%08X", sd, seq );
  }
  ~PfKey () { /*close ( sd );*/ }
  unsigned int getSeq () { return seq; }

  int receive ( const void ** buf ) throw ( Error ) {
    for (;;) {
      * buf = 0;
      int r = recv ( sd, rdBuf, sizeof ( rdBuf ), MSG_DONTWAIT );
      if ( r < 0 ) {
        if ( errno != EAGAIN ) {
          TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: PfKey::receive: OS error: %i: \"%s\"", errno, strerror ( errno ) );
          throw Error( cnvErr ( errno ) );
        }
        return 0;
      }
      if ( r == 0 )
        return 0;
      dump ( "TCCIPsec: PfKey::receive: ", rdBuf, r );
      sadb_msg  * msg = (sadb_msg*) rdBuf;
      if ( (unsigned int) r < sizeof ( sadb_msg ) ||
           msg->sadb_msg_version != PF_KEY_V2 ||
           r < msg->sadb_msg_len ||
           msg->sadb_msg_seq != seq ||
           (int) msg->sadb_msg_pid != getpid () ) {
        TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: PfKey::receive: Received mesasage discarded" );
        continue;
      }
      * buf = (const void*) rdBuf;
      return r;
    }
  }

  void send ( const void * data, size_t len ) throw ( Error ) {
    while ( len > 0 ) {
      dump ( "TCCIPsec: PfKey::send: ", data, len );
      int r = ::send ( sd, data, len, MSG_DONTWAIT );
      TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: PfKey::send: done" );
      if ( r < 0 ) {
        if ( errno != EAGAIN ) {
          TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: PfKey::send: OS error: %i: \"%s\"", errno, strerror ( errno ) );
          throw Error( cnvErr ( errno ) );
        }
        r = 0;
      }
      if ( (size_t) r > len )
        throw Error( cnvErr ( errno ) );
      ( * (unsigned char**) & data ) += r;
      len -= r;
    }
  }
  
  void checkAnswer () throw ( Error ) {
    const void * buf;
    int r = 0;
    for ( int i = 1;; ++i ) {
      r = receive ( & buf );
      if ( r > 0 ) break;
      if ( i > 5 ) {
        TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: PfKey::checkAnswer: No answer" );
        return;
      }
      TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: PfKey::checkAnswer: try again (%i)", i );
      usleep ( 0 );
    }
    int res = ((const sadb_msg*) rdBuf )->sadb_msg_errno;
    if ( res != 0 ) {
      TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: PfKey::checkAnswer: OS error: %i: \"%s\"", res, strerror ( res ) );
      throw Error( cnvErr ( res ) );
    }        
    TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: PfKey::checkAnswer: Answer received: Ok" );
  }
}; //end of class PfKey

static const unsigned short SADB_MSG_LEN64 = sizeof ( sadb_msg ) / 8;

int setSadbMsg ( void * buf, unsigned char type, unsigned char saType, unsigned short len, unsigned int seq )
{
  sadb_msg  * msg = (sadb_msg*) buf;
  
  msg->sadb_msg_version = PF_KEY_V2;
  msg->sadb_msg_type = type;
  msg->sadb_msg_errno = 0;
  msg->sadb_msg_satype = saType;  // Depends on enum values of TCCIPsec_Protocol
  msg->sadb_msg_len = len;
  msg->sadb_msg_reserved = 0;
  msg->sadb_msg_seq = seq;
  msg->sadb_msg_pid = getpid ();
  return sizeof ( * msg );
}

int set_saEndPoint( void *buf, const char *address, int port)
{
   sockaddr_in *pSockAddr = (sockaddr_in *)buf;
   memset(pSockAddr, 0, sizeof(*pSockAddr));
   int res = inet_pton(AF_INET, address, &(pSockAddr->sin_addr));
   if(res > 0) {
     pSockAddr->sin_family = AF_INET;
     pSockAddr->sin_port = htons(port);
     return sizeof(*pSockAddr);
   }else
     throw Error ( TCCIPsec__Result::parameterInvalid );
}

int setAddressPart ( void * buf, unsigned short type, const char * address,
                     int prefixLen = c__TCCIPsec__prefixAll,
                     int proto = TCCIPsec__TranspProto::anyTranspProto,
                     int port = c__TCCIPsec__anyPort
                   ) throw ( Error )
{
  sockaddr_in       sockAddr;
  memset ( & sockAddr, 0, sizeof ( sockAddr ) );
  sockaddr_in6      sockAddr6;
  memset ( & sockAddr6, 0, sizeof ( sockAddr6 ) );
  const void        * pAddr;
  int               sockLen = 0;
  int res = inet_pton ( AF_INET, address, & sockAddr.sin_addr );
  if ( res > 0 ) {
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons ( port );
    pAddr = & sockAddr;
    sockLen = sizeof ( sockAddr );
    if ( prefixLen == c__TCCIPsec__prefixAll ) prefixLen = 32;
  } else {
    res = inet_pton ( AF_INET6, address, & sockAddr6.sin6_addr );
    if ( res <= 0 )
      throw Error ( TCCIPsec__Result::parameterInvalid );
    sockAddr6.sin6_family = AF_INET6;
    sockAddr6.sin6_port = htons ( port );
    pAddr = & sockAddr6;
    sockLen = sizeof ( sockAddr6 );
    if ( prefixLen == c__TCCIPsec__prefixAll ) prefixLen = 128;
  }
  sadb_address  * addrExt = (sadb_address*) buf;
  int len64 = sizeof ( * addrExt ) / 8 + ( sockLen + 7 ) / 8;
  memset ( buf, 0, len64 * 8 );
  addrExt->sadb_address_len = len64;
  addrExt->sadb_address_exttype = type;
  addrExt->sadb_address_proto = proto;
  addrExt->sadb_address_prefixlen = prefixLen;
  //addrExt->sadb_address_reserved = 0;
  memcpy ( addrExt + 1, pAddr, sockLen);
  return len64 * 8 ;
}

int castKey ( const TCCIPsec__Key & keyIn, char * key) throw ( Error )
{
  int len = 0;
  switch ( keyIn.get_selection() ) {
    case TCCIPsec__Key::ALT_hex:{
      unsigned char *hexkey = (unsigned char*)(const unsigned char*)hex2oct(keyIn.hex());
      len = keyIn.hex().lengthof()/2;
      memcpy(key, hexkey, len);
      break;
    }
  case TCCIPsec__Key::ALT_text:
    key = (char*)(const char*) ( keyIn.text() );
    len = keyIn.text().lengthof ();
    break;
  default:
    throw Error ( TCCIPsec__Result::parameterInvalid );
  }
  return len;
}

void f__IPsec__setParityBit(unsigned char* data){
  unsigned char d;
  unsigned char p = 1;
  d = *data;
  while (d>>=1){
    if(d & 1) {if(p) p=0; else p=1;} // inverting
  }
  if (p) *data |= p; else *data &=(~1); // setting the bit
}

void f__IPsec__setParityBits(unsigned char *data, int l){
  unsigned char *d = data;
  while (l--){
    f__IPsec__setParityBit(++d);
  }
}

inline unsigned short ipSecModeToIPMode ( const TCCIPsec__IPsecMode & ipSecMode ) throw ( Error ) {
  switch ( (int) ipSecMode ) {
    case TCCIPsec__IPsecMode::anyMode:   return IPSEC_MODE_ANY;
    case TCCIPsec__IPsecMode::transport:    return IPSEC_MODE_TRANSPORT;
    case TCCIPsec__IPsecMode::tunnel: return IPSEC_MODE_TUNNEL;
    default: throw Error ( TCCIPsec__Result::parameterInvalid );
  }
}

TCCIPsec__Result f__IPsec__SADB__add (
    const CHARSTRING& srcAddress,
    const CHARSTRING& dstAddress,
    const TCCIPsec__Protocol& protocol,
    const INTEGER& spi,
    const TCCIPsec__ExtensionList& extensionList,
    const TCCIPsec__Algorithm& alg,
    const BOOLEAN& setparitybit = 0,
    const BOOLEAN& useNatt = 0,
    const TCCIPsec__IPsecMode& ipSecMode = TCCIPsec__IPsecMode::anyMode)
{
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SADB__add: Enter" );
  bool setparity = setparitybit;
  try {
    int                     encrAlgo = SADB_EALG_NONE;
    //    const char *   encrKey = 0;
    char encrKey[255];
    int                     encrKeyLen = 0;
    int                     authAlgo = SADB_AALG_NONE;
    //    const char *   authKey = 0;
    char authKey[255];
    int                     authKeyLen = 0;
    switch ( alg.get_selection() ) {
      case TCCIPsec__Algorithm::ALT_encr:
        encrAlgo = TCCIPsec__EAlgo::enum_type ( alg.encr().algo() );
        encrKeyLen = castKey ( alg.encr().key(), encrKey );
	if (setparity) f__IPsec__setParityBits((unsigned char*)encrKey, encrKeyLen);
        break;
      case TCCIPsec__Algorithm::ALT_auth:
        authAlgo = TCCIPsec__AAlgo::enum_type ( alg.auth().algo() );
        authKeyLen = castKey ( alg.auth().key(), authKey );
        break;
      case TCCIPsec__Algorithm::ALT_encrAndAuth:
        encrAlgo = TCCIPsec__EAlgo::enum_type ( alg.encrAndAuth().ealgo() );
        encrKeyLen = castKey ( alg.encrAndAuth().ekey(), encrKey );
        authAlgo = TCCIPsec__AAlgo::enum_type ( alg.encrAndAuth().aalgo() );
        authKeyLen = castKey ( alg.encrAndAuth().akey(), authKey );
	if (setparity) f__IPsec__setParityBits((unsigned char*)encrKey, encrKeyLen);
        break;
      default:
        throw Error ( TCCIPsec__Result::parameterInvalid );
    }
    PfKey           pfKey;
    unsigned char   msg[1024];
    int             len = sizeof ( sadb_msg );
    len += setAddressPart ( msg + len, SADB_EXT_ADDRESS_SRC, srcAddress );
    len += setAddressPart ( msg + len, SADB_EXT_ADDRESS_DST, dstAddress );
    sadb_sa     * saExt = (sadb_sa*) ( msg + len );
    saExt->sadb_sa_len = sizeof ( * saExt ) / 8;
    saExt->sadb_sa_exttype = SADB_EXT_SA;
    saExt->sadb_sa_spi = htonl ( (unsigned int)spi.get_long_long_val() );
    saExt->sadb_sa_replay = 0;
    saExt->sadb_sa_state = 0;
    saExt->sadb_sa_auth = authAlgo;
    saExt->sadb_sa_encrypt = encrAlgo;
    saExt->sadb_sa_flags = 0; // TODO: check
    len += sizeof ( * saExt );
    if ( encrAlgo != SADB_EALG_NONE ) {
      sadb_key     * keyExt = (sadb_key*) ( msg + len );
      int   keyLen64 = ( encrKeyLen + 7 ) / 8;
      keyExt->sadb_key_len = sizeof ( * keyExt ) / 8 + keyLen64;
      keyExt->sadb_key_exttype = SADB_EXT_KEY_ENCRYPT;
      keyExt->sadb_key_bits = encrKeyLen * 8;
      keyExt->sadb_key_reserved = 0;
      len += sizeof ( * keyExt );
      memcpy ( msg + len, encrKey, encrKeyLen );
      if ( encrKeyLen < keyLen64 * 8 )
        memset ( msg + len + encrKeyLen, 0, keyLen64 * 8 - encrKeyLen );
      len += keyLen64 * 8;
    }
    if ( authAlgo != SADB_AALG_NONE ) {
      sadb_key     * keyExt = (sadb_key*) ( msg + len );
      int   keyLen64 = ( authKeyLen + 7 ) / 8;
      keyExt->sadb_key_len = sizeof ( * keyExt ) / 8 + keyLen64;
      keyExt->sadb_key_exttype = SADB_EXT_KEY_AUTH;
      keyExt->sadb_key_bits = authKeyLen * 8;
      keyExt->sadb_key_reserved = 0;
      len += sizeof ( * keyExt );
      memcpy ( msg + len, authKey, authKeyLen );
      if ( authKeyLen < keyLen64 * 8 )
        memset ( msg + len + authKeyLen, 0, keyLen64 * 8 - authKeyLen );
      len += keyLen64 * 8;
    }
    int nExtensions = extensionList.size_of ();
    for ( int i = 0; i < nExtensions; ++i ) {
      bool  isHLT = false;
      sadb_lifetime * ltExt = 0;
      switch ( extensionList[i].get_selection () ) {
        case TCCIPsec__Extension::ALT_hardLifetime:
          isHLT = true;
        case TCCIPsec__Extension::ALT_softLifetime:
          ltExt = (sadb_lifetime*) ( msg + len );
          ltExt->sadb_lifetime_len = sizeof ( * ltExt ) / 8;
          ltExt->sadb_lifetime_exttype = isHLT ?
              SADB_EXT_LIFETIME_HARD : SADB_EXT_LIFETIME_SOFT;
          ltExt->sadb_lifetime_allocations = 0;
          ltExt->sadb_lifetime_bytes = 0;
          ltExt->sadb_lifetime_addtime = isHLT ?
              extensionList[i].hardLifetime () : extensionList[i].softLifetime ();
          ltExt->sadb_lifetime_usetime = 0;
          len += sizeof ( * ltExt );
          break;
        case TCCIPsec__Extension::ALT_policyId:
#if defined USE_KAME_IPSEC
          {
            sadb_x_sa2    * sa2Ext = 0;
            sa2Ext = ( sadb_x_sa2* ) ( msg + len );
            memset ( sa2Ext, 0, sizeof ( * sa2Ext ) );
            sa2Ext->sadb_x_sa2_len = sizeof ( * sa2Ext ) / 8;
            sa2Ext->sadb_x_sa2_exttype = SADB_X_EXT_SA2;
            sa2Ext->sadb_x_sa2_mode = ipSecModeToIPMode(ipSecMode);
            sa2Ext->sadb_x_sa2_reqid = extensionList[i].policyId ();
            len += sizeof ( * sa2Ext );

            if (useNatt) {
                
                /*NAT-T type*/
                sadb_x_nat_t_type  * natt_type=0;
                natt_type = ( sadb_x_nat_t_type* ) ( msg + len );
                memset(natt_type, 0, sizeof(* natt_type));
                natt_type->sadb_x_nat_t_type_len = sizeof(* natt_type) / 8 ;
                natt_type->sadb_x_nat_t_type_exttype = SADB_X_EXT_NAT_T_TYPE;
                natt_type->sadb_x_nat_t_type_type = UDP_ENCAP_ESPINUDP;
                len += sizeof( *natt_type );
                  
                /*NAT-T source port */
                sadb_x_nat_t_port *natt_port=0;
                natt_port = (sadb_x_nat_t_port *) (msg + len);
                memset(natt_port, 0, sizeof(* natt_port));
                natt_port->sadb_x_nat_t_port_len = sizeof(* natt_port) / 8;
                natt_port->sadb_x_nat_t_port_exttype = SADB_X_EXT_NAT_T_SPORT;
                natt_port->sadb_x_nat_t_port_port = htons(DEFAULT_NATT_PORT);
                len += sizeof(* natt_port);    

                /*NAT-T destination port */
                natt_port = (sadb_x_nat_t_port *) (msg + len);
                memset(natt_port, 0, sizeof(* natt_port));
                natt_port->sadb_x_nat_t_port_len = sizeof(* natt_port) / 8;
                natt_port->sadb_x_nat_t_port_exttype = SADB_X_EXT_NAT_T_DPORT;
                natt_port->sadb_x_nat_t_port_port = htons(DEFAULT_NATT_PORT);
                len += sizeof(* natt_port);
            
            }
 
            break;
          }
#else
          throw Error ( TCCIPsec__Result::notImplemented );
#endif // defined USE_KAME_IPSEC
        default:
          throw Error ( TCCIPsec__Result::parameterInvalid );
      }
    }
    setSadbMsg ( & msg, SADB_ADD, protocol, len / 8, pfKey.getSeq () );
    pfKey.send ( & msg, len );
    pfKey.checkAnswer ();
  } catch ( Error err ) {
    TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SADB__add: Leave (error)" );
    return err.result;
  }
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SADB__add: Leave (ok)" );
  return TCCIPsec__Result::ok;
}

TCCIPsec__Result f__IPsec__SADB__delete (
    const CHARSTRING& srcAddress,
    const CHARSTRING& dstAddress,
    const TCCIPsec__Protocol& protocol,
    const INTEGER& spi )
{
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SADB__delete: Enter" );
  try {
    PfKey           pfKey;
    unsigned char   msg[1024];
    int             len = sizeof ( sadb_msg );
    len += setAddressPart ( msg + len, SADB_EXT_ADDRESS_SRC, srcAddress );
    len += setAddressPart ( msg + len, SADB_EXT_ADDRESS_DST, dstAddress );
    sadb_sa     * saExt = (sadb_sa*) ( msg + len );
    saExt->sadb_sa_len = sizeof ( * saExt ) / 8;
    saExt->sadb_sa_exttype = SADB_EXT_SA;
    saExt->sadb_sa_spi = htonl ( (unsigned int)spi.get_long_long_val() );
    saExt->sadb_sa_replay = 0;
    saExt->sadb_sa_state = 0;
    saExt->sadb_sa_auth = 0;
    saExt->sadb_sa_encrypt = 0;
    saExt->sadb_sa_flags = 0; // TODO: check
    len += sizeof ( * saExt );
    setSadbMsg ( & msg, SADB_DELETE, protocol, len / 8, pfKey.getSeq () );
    pfKey.send ( & msg, len );
    pfKey.checkAnswer ();
  } catch ( Error err ) {
    TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SADB__delete: Leave (error)" );
    return err.result;
  }
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SADB__delete: Leave (ok)" );
  return TCCIPsec__Result::ok;
}

TCCIPsec__Result f__IPsec__SADB__flush ()
{
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SADB__flush: Enter" );
  try {
    PfKey     pfKey;
    sadb_msg  msg;
    setSadbMsg ( & msg, SADB_FLUSH, SADB_SATYPE_UNSPEC, SADB_MSG_LEN64, pfKey.getSeq () );
    pfKey.send ( & msg, sizeof ( msg ) );
    pfKey.checkAnswer ();
  } catch ( Error err ) {
    TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SADB__flush: Leave (error)" );
    return err.result;
  }
  return TCCIPsec__Result::ok;
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SADB__flush: Leave (ok)" );
}

#if defined USE_KAME_IPSEC

inline unsigned short ipSecProtoToIPProto ( const TCCIPsec__Protocol & ipSecProto ) throw ( Error ) {
  switch ( (int) ipSecProto ) {
    case TCCIPsec__Protocol::esp:   return IPPROTO_ESP;
    case TCCIPsec__Protocol::ah:    return IPPROTO_AH;
    //case TCCIPsec__Protocol::ipComp: return IPPROTO_IPCOMP
    default: throw Error ( TCCIPsec__Result::parameterInvalid );
  }
}

inline unsigned char ipSecRuleLevelToLevel ( const TCCIPsec__RuleLevel & ipSecRuleLevel,
                                             int * id ) throw ( Error ) {
  switch ( ipSecRuleLevel.get_selection () ) {
    case TCCIPsec__RuleLevel::ALT_defaultLevel: return IPSEC_LEVEL_DEFAULT;
    case TCCIPsec__RuleLevel::ALT_use: return IPSEC_LEVEL_USE;
    case TCCIPsec__RuleLevel::ALT_require: return IPSEC_LEVEL_REQUIRE;
    case TCCIPsec__RuleLevel::ALT_unique:
      * id = ipSecRuleLevel.unique().id();
      return IPSEC_LEVEL_UNIQUE;
    default: throw Error ( TCCIPsec__Result::parameterInvalid );
  }
}

TCCIPsec__Result f__IPsec__SPDB__add (
    const CHARSTRING& srcAddress,
    const INTEGER& srcPrefixLen,
    const INTEGER& srcPort,
    const CHARSTRING& dstAddress,
    const INTEGER& dstPrefixLen,
    const INTEGER& dstPort,
    const TCCIPsec__TranspProto& transpProto,
    const TCCIPsec__PolicyDirection& dir,
    const TCCIPsec__PolicyRule& rule )
{
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SPDB__add: Enter" );
  try {
    PfKey           pfKey;
    unsigned char   msg[1024]={0};
    int             len = sizeof ( sadb_msg );
    len += setAddressPart ( msg + len, SADB_EXT_ADDRESS_SRC, srcAddress, srcPrefixLen, transpProto, srcPort );
    len += setAddressPart ( msg + len, SADB_EXT_ADDRESS_DST, dstAddress, dstPrefixLen, transpProto, dstPort );
    sadb_x_policy   * policyExt = (sadb_x_policy*) ( msg + len );
    memset ( policyExt, 0, sizeof ( * policyExt ) ); // for better portability
    policyExt->sadb_x_policy_len = sizeof ( * policyExt ) / 8;
    policyExt->sadb_x_policy_exttype = SADB_X_EXT_POLICY;
    policyExt->sadb_x_policy_dir = dir; // Depends on enum values of TCCIPsec_PolicyDirection
    //policyExt->sadb_x_policy_reserved = 0;
    //policyExt->sadb_x_policy_id = 0;
    //policyExt->sadb_x_policy_priority = 0; from kernel version 2.6.6
    len += sizeof ( * policyExt );
    switch ( rule.get_selection () ) {
      case TCCIPsec__PolicyRule::ALT_discard:
        policyExt->sadb_x_policy_type = IPSEC_POLICY_DISCARD; break;
      case TCCIPsec__PolicyRule::ALT_noneRule:
        policyExt->sadb_x_policy_type = IPSEC_POLICY_NONE; break;
      case TCCIPsec__PolicyRule::ALT_ipSec: {
        policyExt->sadb_x_policy_type = IPSEC_POLICY_IPSEC;
        int nRules = rule.ipSec().size_of ();
        if ( nRules < 1 || nRules > 2 ) {
          throw Error ( TCCIPsec__Result::parameterInvalid );
        }
        for ( int i = 0; i < nRules; ++i ) {
          const TCCIPsec__Rule &    ipSecRule = rule.ipSec()[i];
          if (!(ipSecRule.mode().get_selection () == TCCIPsec__Mode::ALT_transport || 
               ipSecRule.mode().get_selection () == TCCIPsec__Mode::ALT_tunnel))
            throw Error ( TCCIPsec__Result::parameterInvalid );
          sadb_x_ipsecrequest   * policyExt2 = (sadb_x_ipsecrequest*) ( msg + len );
          memset ( policyExt2, 0, sizeof ( * policyExt2 ) ); // for better portability
          policyExt2->sadb_x_ipsecrequest_len = sizeof ( *policyExt2 );
          policyExt2->sadb_x_ipsecrequest_proto = ipSecProtoToIPProto ( (int) ipSecRule.protocol() );
          if ( ipSecRule.mode().get_selection () == TCCIPsec__Mode::ALT_transport){ 
             policyExt2->sadb_x_ipsecrequest_mode = IPSEC_MODE_TRANSPORT;
          }
          else {  
             policyExt2->sadb_x_ipsecrequest_mode = IPSEC_MODE_TUNNEL;
          }   
          int reqId = 0;
          unsigned char level = ipSecRuleLevelToLevel ( rule.ipSec()[i].level (), & reqId );
          policyExt2->sadb_x_ipsecrequest_level = level;
          //policyExt2->sadb_x_ipsecrequest_reserved1 = 0;
          policyExt2->sadb_x_ipsecrequest_reqid = reqId;
          //policyExt2->sadb_x_ipsecrequest_reserved2 = 0;
          policyExt->sadb_x_policy_len += sizeof ( *policyExt2 ) / 8;
          len += sizeof ( *policyExt2 );
          if ( ipSecRule.mode().get_selection () == TCCIPsec__Mode::ALT_tunnel){
            TCCIPsec__Tunnel tunnel = ipSecRule.mode().tunnel(); 
            len += set_saEndPoint( msg+len, tunnel.srcAddr(), tunnel.srcPort());
            len += set_saEndPoint( msg+len, tunnel.dstAddr(), tunnel.dstPort());
          }
        }
        break;
      }
      default:
        throw Error ( TCCIPsec__Result::parameterInvalid );
    }
    setSadbMsg ( & msg, SADB_X_SPDADD, SADB_SATYPE_UNSPEC, len/8, pfKey.getSeq () );
    pfKey.send ( & msg, len );
    pfKey.checkAnswer ();
  } catch ( Error err ) {
    TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SPDB__add: Leave (error)" );
    return err.result;
  }
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SPDB__add: Leave (ok)" );
  return TCCIPsec__Result::ok;
}

TCCIPsec__Result f__IPsec__SPDB__delete (
    const CHARSTRING& srcAddress,
    const INTEGER& srcPrefixLen,
    const INTEGER& srcPort,
    const CHARSTRING& dstAddress,
    const INTEGER& dstPrefixLen,
    const INTEGER& dstPort,
    const TCCIPsec__TranspProto& transpProto,
    const TCCIPsec__PolicyDirection& dir )
{
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SPDB__delete: Enter" );
  try {
    PfKey           pfKey;
    unsigned char   msg[1024];
    int             len = sizeof ( sadb_msg );
    len += setAddressPart ( msg + len, SADB_EXT_ADDRESS_SRC, srcAddress, srcPrefixLen, transpProto, srcPort );
    len += setAddressPart ( msg + len, SADB_EXT_ADDRESS_DST, dstAddress, dstPrefixLen, transpProto, dstPort );
    sadb_x_policy   * policyExt = (sadb_x_policy*) ( msg + len );
    memset ( policyExt, 0, sizeof ( * policyExt ) ); // for better portability
    policyExt->sadb_x_policy_len = sizeof ( * policyExt ) / 8;
    policyExt->sadb_x_policy_exttype = SADB_X_EXT_POLICY;
    //policyExt->sadb_x_policy_type = 0;
    policyExt->sadb_x_policy_dir = dir; // Depends on enum values of TCCIPsec_PolicyDirection
    //policyExt->sadb_x_policy_reserved = 0;
    //policyExt->sadb_x_policy_id = 0;
    //policyExt->sadb_x_policy_priority = 0; from kernel version 2.6.6
    len += sizeof ( * policyExt );
    setSadbMsg ( & msg, SADB_X_SPDDELETE, SADB_SATYPE_UNSPEC, len / 8, pfKey.getSeq () );
    pfKey.send ( & msg, len );
    pfKey.checkAnswer ();
  } catch ( Error err ) {
    TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SPDB__delete: Leave (error)" );
    return err.result;
  }
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SPDB__delete: Leave (ok)" );
  return TCCIPsec__Result::ok;
}

TCCIPsec__Result f__IPsec__SPDB__flush ()
{
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SPDB__flush: Enter" );
  try {
    PfKey     pfKey;
    sadb_msg  msg;
    setSadbMsg ( & msg, SADB_X_SPDFLUSH, SADB_SATYPE_UNSPEC, SADB_MSG_LEN64, pfKey.getSeq () );
    pfKey.send ( & msg, sizeof ( msg ) );
    pfKey.checkAnswer ();
  } catch ( Error err ) {
    TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SPDB__flush: Leave (error)" );
    return err.result;
  }
  TTCN_Logger::log ( TTCN_DEBUG, "TCCIPsec: f__IPsec__SPDB__flush: Leave (ok)" );
  return TCCIPsec__Result::ok;
}
#endif // defined USE_KAME_IPSEC

#endif // defined USE_IPSEC || defined USE_KAME_IPSEC

#if ! defined USE_IPSEC && ! defined USE_KAME_IPSEC
TCCIPsec__Result f__IPsec__SADB__add (
    const CHARSTRING& srcAddress,
    const CHARSTRING& dstAddress,
    const TCCIPsec__Protocol& protocol,
    const INTEGER& spi,
    const TCCIPsec__ExtensionList& extensionList,
    const TCCIPsec__Algorithm& alg,
    const BOOLEAN& setparitybit = 0,
    const BOOLEAN& useNatt = 0,
    const TCCIPsec__IPsecMode& ipSecMode = TCCIPsec__IPsecMode::anyMode)
{
  static bool first = true;
  if ( first ) {
    TTCN_Logger::log ( TTCN_WARNING, "TCCIPsec: f__IPsec__SADB__add: IPsec support was not specified during compilation" );
    first = false;
  }
  return TCCIPsec__Result::notImplemented;
}
TCCIPsec__Result f__IPsec__SADB__delete (
    const CHARSTRING& srcAddress,
    const CHARSTRING& dstAddress,
    const TCCIPsec__Protocol& protocol,
    const INTEGER& spi )
{
  static bool first = true;
  if ( first ) {
    TTCN_Logger::log ( TTCN_WARNING, "TCCIPsec: f__IPsec__SADB__delete: IPsec support was not specified during compilation" );
    first = false;
  }
  return TCCIPsec__Result::notImplemented;
}
TCCIPsec__Result f__IPsec__SADB__flush ()
{
  static bool first = true;
  if ( first ) {
    TTCN_Logger::log ( TTCN_WARNING, "TCCIPsec: f__IPsec__SADB__flush: IPsec support was not specified during compilation" );
    first = false;
  }
  return TCCIPsec__Result::notImplemented;
}
#endif // ! defined USE_IPSEC && ! defined USE_KAME_IPSEC

#if ! defined USE_KAME_IPSEC
TCCIPsec__Result f__IPsec__SPDB__add (
    const CHARSTRING& srcAddress,
    const INTEGER& srcPrefixLen,
    const INTEGER& srcPort,
    const CHARSTRING& dstAddress,
    const INTEGER& dstPrefixLen,
    const INTEGER& dstPort,
    const TCCIPsec__TranspProto& transpProto,
    const TCCIPsec__PolicyDirection& dir,
    const TCCIPsec__PolicyRule& rule )
{
  static bool first = true;
  if ( first ) {
    TTCN_Logger::log ( TTCN_WARNING, "TCCIPsec: f__IPsec__SPDB__add: IPsec SPDB support was not specified during compilation" );
    first = false;
  }
  return TCCIPsec__Result::notImplemented;
}
TCCIPsec__Result f__IPsec__SPDB__delete (
    const CHARSTRING& srcAddress,
    const INTEGER& srcPrefixLen,
    const INTEGER& srcPort,
    const CHARSTRING& dstAddress,
    const INTEGER& dstPrefixLen,
    const INTEGER& dstPort,
    const TCCIPsec__TranspProto& transpProto,
    const TCCIPsec__PolicyDirection& dir )
{
  static bool first = true;
  if ( first ) {
    TTCN_Logger::log ( TTCN_WARNING, "TCCIPsec: f__IPsec__SPDB__delete: IPsec SPDB support was not specified during compilation" );
    first = false;
  }
  return TCCIPsec__Result::notImplemented;
}
TCCIPsec__Result f__IPsec__SPDB__flush ()
{
  static bool first = true;
  if ( first ) {
    TTCN_Logger::log ( TTCN_WARNING, "TCCIPsec: f__IPsec__SPDB__flush: IPsec SPDB support was not specified during compilation" );
    first = false;
  }
  return TCCIPsec__Result::notImplemented;
}
#endif //! defined USE_KAME_IPSEC

}
