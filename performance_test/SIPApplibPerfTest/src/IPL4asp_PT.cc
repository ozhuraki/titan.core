/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               IPL4asp_PT.cc
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 531
//  Updated:            2012-08-16
//  Contact:            http://ttcn.ericsson.se
//  Reference:

#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <limits.h>
#include <time.h>
#include "IPL4asp_PT.hh"
#include "IPL4asp_PortType.hh"
#include "Socket_API_Definitions.hh"

#define  IPL4_SCTP_WHOLE_MESSAGE_RECEIVED 0
#define  IPL4_SCTP_EOF_RECEIVED 1
#define  IPL4_SCTP_ERROR_RECEIVED 2
#define  IPL4_SCTP_PARTIAL_RECEIVE 3

#define  IPL4_IPV4_ANY_ADDR "0.0.0.0"
#define  IPL4_IPV6_ANY_ADDR "::"


//SSL
#ifdef IPL4_USE_SSL
#define AS_SSL_CHUNCK_SIZE 16384
// character buffer length to store temporary SSL informations, 256 is usually enough
#define SSL_CHARBUF_LENGTH 256
// number of bytes to read from the random devices
#define SSL_PRNG_LENGTH 1024
#endif

#ifdef USE_IPL4_EIN_SCTP
#include "SS7Common.hh"
#include <pthread.h>

//const IPL4asp__Types::OptionList empty_IPL4asp__Types::OptionList(NULL_VALUE);

#endif

using namespace IPL4asp__Types;

namespace IPL4asp__PortType {

#define SET_OS_ERROR_CODE (result.os__error__code()() = errno)
#define RETURN_ERROR(code) {              \
  result.errorCode()() = PortError::code; \
  if (result.os__error__code().ispresent()) \
    result.os__error__text()() = strerror((int)result.os__error__code()()); \
  ASP__Event event; \
  event.result() = result; \
  if (portRef.globalConnOpts.extendedPortEvents == GlobalConnOpts::YES) \
    portRef.incoming_message(event);  \
  return result;  \
}
#define RETURN_ERROR_STACK(code) {              \
  result.errorCode()() = PortError::code; \
  if (result.os__error__code().ispresent()) \
    result.os__error__text()() = strerror((int)result.os__error__code()()); \
  ASP__Event event; \
  event.result() = result; \
  if (globalConnOpts.extendedPortEvents == GlobalConnOpts::YES) \
    incoming_message(event);  \
  return result;  \
}

#ifdef LKSCTP_MULTIHOMING_ENABLED
int my_sctp_connectx(int sd, struct sockaddr * addrs, int addrcnt){

#ifdef SCTP_SOCKOPT_CONNECTX_OLD
  return sctp_connectx(sd, addrs, addrcnt,NULL);
#else
  return sctp_connectx(sd, addrs, addrcnt);
#endif
}
#endif

inline void IPL4asp__PT_PROVIDER::testIfInitialized() const {
  if (!mapped) {
    TTCN_error("IPL4 Test Port not mapped");
  }
}


INTEGER simpleGetMsgLen(const OCTETSTRING& stream, ro__integer& args)
{
  return stream.lengthof();
} // simpleGetMsgLen


int SetLocalSockAddr(const char* debug_str, IPL4asp__PT_PROVIDER& portRef,
                    int def_addr_family,
                    const char *locName, int locPort,
                    SockAddr& sockAddr, socklen_t& sockAddrLen){
  int hp=0;
  bool locName_empty=(strlen(locName)==0);
  const char* def_loc_host;
  if(strlen(portRef.defaultLocHost)==0){
    if(def_addr_family==AF_INET6){
      def_loc_host= IPL4_IPV6_ANY_ADDR;
    } else {
      def_loc_host= IPL4_IPV4_ANY_ADDR;
    }
  } else {
    def_loc_host=portRef.defaultLocHost;
  }
    portRef.debug("SetLocalSockAddr: locName: %s loc_port %d def_loc_host %s, add_family %s", locName, locPort,def_loc_host, 
    def_addr_family==AF_INET6?"AF_INET6":"AF_INET");
  
  if (locPort != -1 && !locName_empty)
    hp = SetSockAddr(locName, locPort, sockAddr, sockAddrLen);
  else if (locPort == -1 && locName_empty) { // use default host and port
    portRef.debug("%s: use defaults: %s:%d", debug_str,
      def_loc_host, portRef.defaultLocPort);
    hp = SetSockAddr(def_loc_host,
            portRef.defaultLocPort, sockAddr, sockAddrLen);
  } else if (locPort == -1) { // use default port
    portRef.debug("%s: use default port: %s:%d",  debug_str,
      locName, portRef.defaultLocPort);
    hp = SetSockAddr(locName, portRef.defaultLocPort,
            sockAddr, sockAddrLen);
  } else { // use default host
    portRef.debug("%s: use default host: %s:%d", debug_str,
      def_loc_host, locPort);
    hp = SetSockAddr(def_loc_host,
                     locPort, sockAddr, sockAddrLen);
  }
  return hp;
}

int SetSockAddr(const char *name, int port,
                    SockAddr& sa, socklen_t& saLen) {

  int err = 0;
  int addrtype = -1;
#if defined LINUX || defined SOLARIS || defined SOLARIS8
  struct sockaddr_in saddr;
#ifdef USE_IPV6
  struct sockaddr_in6 saddr6;
#endif
  if(inet_pton(AF_INET, name, &(saddr.sin_addr))) {
    memset(&sa.v4, 0, sizeof(sa.v4));
    saLen = sizeof(sa.v4);
    sa.v4.sin_family = AF_INET;
    sa.v4.sin_port = htons(port);
    memcpy(&sa.v4.sin_addr, &(saddr.sin_addr), sizeof(saddr.sin_addr));
    addrtype = AF_INET;
  }
#ifdef USE_IPV6
  else if(inet_pton(AF_INET6, name, &(saddr6.sin6_addr))) {
    memset(&sa.v6, 0, sizeof(sa.v6));
    saLen = sizeof(sa.v6);
    sa.v6.sin6_family = AF_INET6;
    sa.v6.sin6_port = htons(port);
    memcpy(&sa.v6.sin6_addr, &(saddr6.sin6_addr), sizeof(saddr6.sin6_addr));
    addrtype = AF_INET6;
  }
#endif
  else {

    struct addrinfo myaddr, *res;
    memset(&myaddr,0,sizeof(myaddr));
    myaddr.ai_flags = AI_ADDRCONFIG|AI_PASSIVE;
    myaddr.ai_socktype = SOCK_STREAM;
    myaddr.ai_protocol = 0;

    if ((err = getaddrinfo(name, NULL, &myaddr, &res)) != 0) {
          //printf("SetSockAddr: getaddrinfo error: %i, %s", err, gai_strerror(err));
          return -1;
    }

    if (res->ai_addr->sa_family == AF_INET) { // IPv4
      struct sockaddr_in *saddr = (struct sockaddr_in *) res->ai_addr;
      memset(&sa.v4, 0, sizeof(sa.v4));
      saLen = sizeof(sa.v4);
      sa.v4.sin_family = AF_INET;
      sa.v4.sin_port = htons(port);
      memcpy(&sa.v4.sin_addr, &(saddr->sin_addr), sizeof(saddr->sin_addr));
      addrtype = AF_INET;
    }
#ifdef USE_IPV6
    else if (res->ai_addr->sa_family == AF_INET6){ // IPv6
      struct sockaddr_in6 *saddr = (struct sockaddr_in6 *) res->ai_addr;
      memset(&sa.v6, 0, sizeof(sa.v6));
      saLen = sizeof(sa.v6);
      memcpy(&sa.v6,saddr,saLen);
     sa.v6.sin6_port = htons(port);
      addrtype = AF_INET6;
    }
#endif
    else
    {
      //printf("sa_family not handled!");
    }
    freeaddrinfo(res);
  }
#else // Cygwin
  struct hostent *hp = gethostbyname(name);
  if (hp == NULL)
    return -1;
  if (hp->h_addrtype == AF_INET) { // IPv4
    memset(&sa.v4, 0, sizeof(sa.v4));
    saLen = sizeof(sa.v4);
    sa.v4.sin_family = AF_INET;
    sa.v4.sin_port = htons(port);
    memcpy(&sa.v4.sin_addr.s_addr, hp->h_addr_list[0], hp->h_length);
    addrtype = AF_INET;
  } else {
    return -1;
  }
#endif
  return addrtype;

}



bool SetNameAndPort(SockAddr *sa, socklen_t saLen,
        HostName& name, PortNumber& port) {
  sa_family_t af;
  void *src;
  int vport;

  if (saLen == sizeof(sockaddr_in)) { // IPv4
    af = AF_INET;
    src = &sa->v4.sin_addr.s_addr;
    vport = sa->v4.sin_port;

    char dst[INET_ADDRSTRLEN];
    if (inet_ntop(af, src, dst, INET_ADDRSTRLEN) == NULL) {
      name = "?";
      port = -1;
      return false;
    } else
      name = CHARSTRING(dst);
    port = ntohs(vport);
    return true;
  }
#ifdef USE_IPV6
  else
  { // IPv6
    af = AF_INET6;
    src = &sa->v6.sin6_addr.s6_addr;
    vport = sa->v6.sin6_port;

    char dst[INET6_ADDRSTRLEN];
    if (inet_ntop(af, src, dst, INET6_ADDRSTRLEN) == NULL) {
      name = "?";
      port = -1;
      return false;
    } else
      name = CHARSTRING(dst);
    port = ntohs(vport);
    return true;
  }
#endif
  name = "?";
  port = -1;
  return false;
} // SetNameAndPort



IPL4asp__PT_PROVIDER::IPL4asp__PT_PROVIDER(const char *par_port_name)
	  : PORT(par_port_name) {
  debug("IPL4asp__PT_PROVIDER::IPL4asp__PT_PROVIDER: enter");
  debugAllowed = false;
  mapped = false;
  sockListSize = SOCK_LIST_SIZE_MIN;
  defaultLocHost = "";
  defaultLocPort = 9999;
  backlog = SOMAXCONN;
  defaultGetMsgLen = simpleGetMsgLen;
  defaultMsgLenArgs = new ro__integer(NULL_VALUE);
  pureNonBlocking = false;
  poll_timeout = -1;
  max_num_of_poll =-1;
  lonely_conn_id = -1;
  lazy_conn_id_level = 0;
  broadcast = false;
  send_extended_result = false;
#ifdef USE_SCTP
  (void) memset(&initmsg, 0, sizeof(struct sctp_initmsg));
  initmsg.sinit_num_ostreams = 64;
  initmsg.sinit_max_instreams = 64;
  initmsg.sinit_max_attempts = 0;
  initmsg.sinit_max_init_timeo = 0;
  (void) memset(&events, 0, sizeof (events));
  events.sctp_data_io_event = TRUE;
  events.sctp_association_event = TRUE;
  events.sctp_address_event = TRUE;
  events.sctp_send_failure_event = TRUE;
  events.sctp_peer_error_event = TRUE;
  events.sctp_shutdown_event = TRUE;
  events.sctp_partial_delivery_event = TRUE;
#ifdef LKSCTP_1_0_7
  events.sctp_adaptation_layer_event = TRUE;
#elif defined LKSCTP_1_0_9
  events.sctp_adaptation_layer_event = TRUE;
  events.sctp_authentication_event = FALSE;
#else
  events.sctp_adaption_layer_event = TRUE;
#endif
#endif
#ifdef USE_IPL4_EIN_SCTP
  native_stack = TRUE;
  sctpInstanceId=0;
  userInstanceId=0;
  userInstanceIdSpecified=false;
  sctpInstanceIdSpecified=false;
  userId=USER01_ID;
  cpManagerIPA="";
#endif
  //SSL
#ifdef IPL4_USE_SSL
  ssl_initialized=false;
  ssl_key_file=NULL;
  ssl_certificate_file=NULL;
  ssl_trustedCAlist_file=NULL;
  ssl_cipher_list=NULL;
  ssl_verify_certificate=false;
  ssl_use_session_resumption=true;
  ssl_session=NULL;
  ssl_password=NULL;
  ssl_ctx = NULL;
  ssl_reconnect_attempts = 5;
  ssl_reconnect_delay = 10000; //in milisec, so by default 0.01sec
#endif
} // IPL4asp__PT_PROVIDER::IPL4asp__PT_PROVIDER



IPL4asp__PT_PROVIDER::~IPL4asp__PT_PROVIDER()
{
  debug("IPL4asp__PT_PROVIDER::~IPL4asp__PT_PROVIDER: enter");
  delete defaultMsgLenArgs;
  // now SSL context can be removed
#ifdef IPL4_USE_SSL
  if (ssl_ctx!=NULL) {
    SSL_CTX_free(ssl_ctx);
  }
  delete [] ssl_key_file;
  delete [] ssl_certificate_file;
  delete [] ssl_trustedCAlist_file;
  delete [] ssl_cipher_list;
  delete [] ssl_password;
#endif
} // IPL4asp__PT_PROVIDER::~IPL4asp__PT_PROVIDER



void IPL4asp__PT_PROVIDER::debug(const char *fmt, ...) {
  if (debugAllowed) {
		TTCN_Logger::begin_event(TTCN_DEBUG);
		TTCN_Logger::log_event("%s: ", get_name());
		va_list args;
		va_start(args, fmt);
		TTCN_Logger::log_event_va_list(fmt, args);
		va_end(args);
		TTCN_Logger::end_event();
  }
} // IPL4asp__PT_PROVIDER::debug



void IPL4asp__PT_PROVIDER::set_parameter(const char *parameter_name,
                                         const char *parameter_value)
{
  debug("IPL4asp__PT_PROVIDER::set_parameter: enter (name: %s, value: %s)",
    parameter_name, parameter_value);
  if (!strcmp(parameter_name, "debug")) {
    if (!strcasecmp(parameter_value,"YES")) {
      debugAllowed = true;
      ipDiscConfig.debugAllowed = true;
    }
  debug("IPL4asp__PT_PROVIDER::set_parameter: enter (name: %s, value: %s)",
      parameter_name, parameter_value);
  } else if (!strcmp(parameter_name, "max_num_of_poll")) {
    max_num_of_poll = atoi(parameter_value);
  } else if (!strcmp(parameter_name, "poll_timeout")) {
    poll_timeout = atoi(parameter_value);
  } else if (!strcmp(parameter_name, "defaultListeningPort")) {
    defaultLocPort = atoi(parameter_value);
  } else if (!strcmp(parameter_name, "defaultListeningHost")) {
    defaultLocHost = parameter_value;
  } else if (!strcmp(parameter_name, "backlog")) {
    backlog = atoi(parameter_value);
    if (backlog <= 0) {
      backlog = SOMAXCONN;
      TTCN_warning("IPL4asp__PT_PROVIDER::set_parameter: invalid "
                   "backlog value set to %d", backlog);
    }
  } else if (!strcmp(parameter_name, "sockListSizeInit")) {
    sockListSize = atoi(parameter_value);
    if (sockListSize < SOCK_LIST_SIZE_MIN) {
      sockListSize = SOCK_LIST_SIZE_MIN;
      TTCN_warning("IPL4asp__PT_PROVIDER::set_parameter: invalid "
                   "sockListSizeInit value set to %d", sockListSize);
    }
  } else if (!strcmp(parameter_name, "pureNonBlocking")) {
    if (!strcasecmp(parameter_value,"YES"))
      pureNonBlocking = true;
  } else if (!strcmp(parameter_name, "useExtendedResult")) {
    if (!strcasecmp(parameter_value,"YES"))
      send_extended_result = true;
  } else if (!strcmp(parameter_name, "lazy_conn_id_handling")) {
    if (!strcasecmp(parameter_value,"YES")){
      lazy_conn_id_level = 1;
      } else {
      lazy_conn_id_level = 0;
      }
  } else if (!strcmp(parameter_name, "ipAddressDiscoveryType")) {
    if (!strcasecmp(parameter_value,"DHCP_OR_ARP"))
      ipDiscConfig.type = IPDiscConfig::DHCP_OR_ARP;
    else if (!strcasecmp(parameter_value,"DHCP"))
      ipDiscConfig.type = IPDiscConfig::DHCP;
    else if (!strcasecmp(parameter_value,"ARP"))
      ipDiscConfig.type = IPDiscConfig::ARP;
  } else if (!strcmp(parameter_name, "interfaceName")) {
      ipDiscConfig.expIfName = parameter_value;
  } else if (!strcmp(parameter_name, "interfaceIpAddress")) {
      ipDiscConfig.expIfIpAddress = parameter_value;
  } else if (!strcmp(parameter_name, "excludedInterfaceIpAddress")) {
      ipDiscConfig.exclIfIpAddress = parameter_value;
  } else if (!strcmp(parameter_name, "ethernetAddressStart")) {
      ipDiscConfig.ethernetAddress = parameter_value;
  } else if (!strcmp(parameter_name, "leaseTime")) {
    ipDiscConfig.leaseTime = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "leaseFile")) {
      ipDiscConfig.leaseFile = parameter_value;
  } else if (!strcmp(parameter_name, "numberOfIpAddressesToFind")){
    ipDiscConfig.nOfAddresses = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "dhcpMsgRetransmitCount")){
    ipDiscConfig.dhcpMsgRetransmitCount = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "dhcpMsgRetransmitPeriodInms")){
    ipDiscConfig.dhcpMsgRetransmitPeriodInms = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "dhcpMaxParallelRequestCount")){
    ipDiscConfig.dhcpMaxParallelRequestCount = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "dhcpTimeout")){
    ipDiscConfig.dhcpTimeout = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "arpMsgRetransmitCount")){
    ipDiscConfig.arpMsgRetransmitCount = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "arpMsgRetransmitPeriodInms")){
    ipDiscConfig.arpMsgRetransmitPeriodInms = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "arpMaxParallelRequestCount")){
    ipDiscConfig.arpMaxParallelRequestCount = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "tcpReuseAddress")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.tcpReuseAddr = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.tcpReuseAddr = GlobalConnOpts::NO;
  }else if (!strcmp(parameter_name, "sslReuseAddress")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.tcpReuseAddr = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.tcpReuseAddr = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "udpReuseAddress")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.udpReuseAddr = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.udpReuseAddr = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sctpReuseAddress")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.sctpReuseAddr = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.sctpReuseAddr = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "tcpKeepAlive")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.tcpKeepAlive = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.tcpKeepAlive = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "tcpKeepCount")){
    globalConnOpts.tcpKeepCnt = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "tcpKeepIdle")){
    globalConnOpts.tcpKeepIdle = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "tcpKeepInterval")){
    globalConnOpts.tcpKeepIntvl = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "sslKeepAlive")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.tcpKeepAlive = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.tcpKeepAlive = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sslKeepCount")){
    globalConnOpts.tcpKeepCnt = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "sslKeepIdle")){
    globalConnOpts.tcpKeepIdle = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "sslKeepInterval")){
    globalConnOpts.tcpKeepIntvl = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "extendedPortEvents")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.extendedPortEvents = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.extendedPortEvents = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sinit_num_ostreams")){ //sctp specific params starts here
    globalConnOpts.sinit_num_ostreams = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "sinit_max_instreams")){
    globalConnOpts.sinit_max_instreams = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "sinit_max_attempts")){
    globalConnOpts.sinit_max_attempts = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "sinit_max_init_timeo")){
    globalConnOpts.sinit_max_init_timeo = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "sctp_data_io_event")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.sctp_data_io_event = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.sctp_data_io_event = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sctp_association_event")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.sctp_association_event = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.sctp_association_event = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sctp_address_event")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.sctp_address_event = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.sctp_address_event = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sctp_send_failure_event")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.sctp_send_failure_event = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.sctp_send_failure_event = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sctp_peer_error_event")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.sctp_peer_error_event = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.sctp_peer_error_event = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sctp_shutdown_event")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.sctp_shutdown_event = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.sctp_shutdown_event = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sctp_partial_delivery_event")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.sctp_partial_delivery_event = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.sctp_partial_delivery_event = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sctp_adaptation_layer_event")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.sctp_adaptation_layer_event = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.sctp_adaptation_layer_event = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sctp_authentication_event")){
    if (!strcasecmp(parameter_value,"YES"))
      globalConnOpts.sctp_authentication_event = GlobalConnOpts::YES;
    else if (!strcasecmp(parameter_value,"NO"))
      globalConnOpts.sctp_authentication_event = GlobalConnOpts::NO;
  } else if (!strcmp(parameter_name, "sctp_connection_method")){
    if (!strcasecmp(parameter_value,"METHOD_0"))
      globalConnOpts.connection_method = GlobalConnOpts::METHOD_ZERO;
    else if (!strcasecmp(parameter_value,"METHOD_1"))
      globalConnOpts.connection_method = GlobalConnOpts::METHOD_ONE;
    else if (!strcasecmp(parameter_value,"METHOD_2"))
      globalConnOpts.connection_method = GlobalConnOpts::METHOD_TWO;
  }
  else if (!strcmp(parameter_name, "sctp_stack")){
    if (!strcasecmp(parameter_value,"kernel"))
      native_stack=TRUE;
    else if (!strcasecmp(parameter_value,"EIN"))
      native_stack=FALSE;
  }
  else if(!strcmp(parameter_name,"broadcast")){
    if (!strcasecmp(parameter_value,"enabled"))
      broadcast = true;
    else if (!strcasecmp(parameter_value,"disabled"))
      broadcast = false;    
    else {
      broadcast = false;
      TTCN_warning("IPL4asp__PT::set_parameter(): Unsupported Test Port parameter value: %s", parameter_value);
    }  
  }       
#ifdef USE_IPL4_EIN_SCTP
  else if(!strcasecmp("cpManagerIPA", parameter_name))
  {
    cpManagerIPA = parameter_value;
  } 
  else if (!strcmp(parameter_name, "USERID")){
  if(!SS7Common::handle_parameter(parameter_name, parameter_value, &userId)) {
      TTCN_error("IPL4asp__PT::set_parameter(): Invalid user ID.");
    }
  }
  else if(strcasecmp("userInstanceId", parameter_name) == 0) 
  {
    int uinst = atoi(parameter_value);
    if(uinst < 0 || uinst > 255) 
    {
      log_warning("Wrong userInstanceId parameter value: %s, Must be an integer between 0 and 255 ", parameter_value);
    }
    else 
    {
      userInstanceId = uinst;
      userInstanceIdSpecified = true;
      if(!sctpInstanceIdSpecified)
          sctpInstanceId = userInstanceId;
    }
  }
  else if(strcasecmp("sctpInstanceId", parameter_name) == 0) 
  {
    int tinst = atoi(parameter_value);
    if(tinst < 0 || tinst > 255) 
    {
      log_warning("Wrong sctpInstanceId parameter value: %s, Must be an integer between 0 and 255 ", parameter_value);
    }
    else 
    {
      sctpInstanceId = tinst;
      sctpInstanceIdSpecified = true;
    }
  }

#endif


  //SSL params
#ifdef IPL4_USE_SSL
    else if(strcmp(parameter_name, ssl_use_session_resumption_name()) == 0) {
    if(strcasecmp(parameter_value, "yes") == 0) ssl_use_session_resumption = true;
    else if(strcasecmp(parameter_value, "no") == 0) ssl_use_session_resumption = false;
    else log_warning("Parameter value '%s' not recognized for parameter '%s'", parameter_value, ssl_use_session_resumption_name());
  } else if(strcmp(parameter_name, ssl_private_key_file_name()) == 0) {
    delete [] ssl_key_file;
    ssl_key_file=new char[strlen(parameter_value)+1];
    strcpy(ssl_key_file, parameter_value);
  } else if(strcmp(parameter_name, ssl_trustedCAlist_file_name()) == 0) {
    delete [] ssl_trustedCAlist_file;
    ssl_trustedCAlist_file=new char[strlen(parameter_value)+1];
    strcpy(ssl_trustedCAlist_file, parameter_value);
  } else if(strcmp(parameter_name, ssl_certificate_file_name()) == 0) {
    delete [] ssl_certificate_file;
    ssl_certificate_file=new char[strlen(parameter_value)+1];
    strcpy(ssl_certificate_file, parameter_value);
  } else if(strcmp(parameter_name, ssl_cipher_list_name()) == 0) {
    delete [] ssl_cipher_list;
    ssl_cipher_list=new char[strlen(parameter_value)+1];
    strcpy(ssl_cipher_list, parameter_value);
  } else if(strcmp(parameter_name, ssl_password_name()) == 0) {
    ssl_password=new char[strlen(parameter_value)+1];
    strcpy(ssl_password, parameter_value);
  } else if(strcmp(parameter_name, ssl_verifycertificate_name()) == 0) {
    if(strcasecmp(parameter_value, "yes") == 0) ssl_verify_certificate = true;
    else if(strcasecmp(parameter_value, "no") == 0) ssl_verify_certificate = false;
    else log_warning("Parameter value '%s' not recognized for parameter '%s'", parameter_value, ssl_verifycertificate_name());
  } else if (!strcmp(parameter_name, "ssl_reconnect_attempts")){
	  ssl_reconnect_attempts = atoi ( parameter_value );
  } else if (!strcmp(parameter_name, "ssl_reconnect_delay")){
	  ssl_reconnect_delay = atoi ( parameter_value );
  }
#endif
  // else if ( next param ) ...
} // IPL4asp__PT_PROVIDER::set_parameter


void IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable(int fd)
{
  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: fd: %i", fd);
  std::map<int,int>::iterator it = fd2IndexMap.find(fd);
  if (pureNonBlocking) {
    if (it != fd2IndexMap.end()) {
      //Add SSL layer
#ifdef IPL4_USE_SSL
    if(sockList[it->second].type == IPL4asp_SSL)
	  {
		  switch(sockList[it->second].sslState){
		  case STATE_WAIT_FOR_RECEIVE_CALLBACK:
			    sockList[it->second].sslState = STATE_NORMAL;
			    Handler_Remove_Fd_Write(fd);
			  return;
		  case STATE_PNBRECONNECTATTEMPT:
		  case STATE_CONNECTING:
			  switch(perform_ssl_handshake(it->second))
			  {
			    case 0:
				  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: SSL mapping failed for client socket: %d", sockList[it->second].sock);
				  if (ConnDel(it->second) == -1) debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: unable to close socket");
				  sendError(PortError::ERROR__SOCKET, it->second);
				  return;

			    case -1:
			      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: SSL Reconnect attempt for client socket: %d", sockList[it->second].sock);
				  sockList[it->second].sslState = STATE_PNBRECONNECTATTEMPT;
				  return; //in this case keep the handler running & return to try again later
			  }
			  sockList[it->second].sslState = STATE_NORMAL;
			  break;
		  case STATE_SERVER_ACCEPTING:
		  {
			  switch(perform_ssl_handshake(it->second))
		      {
			    case 0://connection failed, need to close the port
				  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: SSL mapping failed for client socket: %d", sockList[it->second].sock);
				  if (ConnDel(it->second) == -1) debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: unable to close socket");
				  sendError(PortError::ERROR__SOCKET, it->second);
				  return;

			    case -1://in this case keep the handler running & return to try again later
			      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: Need SSL Accept re-attempt for client socket: %d", sockList[it->second].sock);
				  return;

			    case 1: //if sucess continue
			    default:
			    	break;
			  }

			  sockList[it->second].sslState = STATE_NORMAL;
			  Handler_Remove_Fd_Write(fd);

			  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: Client connection Accepted.");

			  ASP__Event event;
			  SockAddr sa;
			  socklen_t saLen = sizeof(SockAddr);
			  if (getpeername(sockList[it->second].sock, (struct sockaddr *)&sa, &saLen)
			      == -1) {
			      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: getpeername failed: %s", strerror(errno));
			      sendError(PortError::ERROR__HOSTNAME, it->second, errno);
			    } else if (!SetNameAndPort(&sa, saLen, event.connOpened().remName(), event.connOpened().remPort())) {
			      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: SetNameAndPort failed");
			      sendError(PortError::ERROR__HOSTNAME, it->second);
			    }


		      if (getsockname(sockList[it->second].sock, (struct sockaddr *)&sa, &saLen) == -1) {
			      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: getsockname error: %s",
			            strerror(errno));
			      sendError(PortError::ERROR__HOSTNAME, it->second, errno);
			    } else if (!SetNameAndPort(&sa, saLen, event.connOpened().locName(),
			                               event.connOpened().locPort())) {
			      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: SetNameAndPort failed");
			      sendError(PortError::ERROR__HOSTNAME, it->second);
			    }

			    event.connOpened().proto().ssl() = SslTuple(null_type());
			    event.connOpened().connId() = it->second;
			    event.connOpened().userData() = sockList[it->second].userData;
			    incoming_message(event);
			    debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: Leave.");
			  return;
		  }
		  default:
  			  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: Unhandled SSL State %d . Client: %d", sockList[it->second].sslState, it->second);
  			  Handler_Remove_Fd_Write(fd);
  			  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: Leave.");
  			  return;
		  }
	  }
#endif
	  Handler_Remove_Fd_Write(fd);
      sendError(PortError::ERROR__AVAILABLE, it->second);

    } else
      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: Error: fd not found");
  } else
    debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: Error: pureNonBlocking is FALSE");

  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Writable: Leave.");
}

void IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable(int fd)
{
  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: enter, fd: %i", fd);

#ifdef USE_IPL4_EIN_SCTP
  if(!native_stack && (fd == pipe_to_TTCN_thread_fds[0] || fd == pipe_to_TTCN_thread_log_fds[0])){
    handle_message_from_ein(fd);
    return;
  }
#endif


  std::map<int,int>::iterator it = fd2IndexMap.find(fd);
  if (it == fd2IndexMap.end()) {
    debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: Error: fd not found");
    return;
  }

  int i = it->second;
  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: connId: %d READABLE   sock: %i, type: %i",
    i, sockList[i].sock, sockList[i].type);

  ASP__RecvFrom asp;
  asp.connId() = i;
  asp.userData() = sockList[i].userData;
  asp.remName() = *(sockList[i].remoteaddr);
  asp.remPort() = *(sockList[i].remoteport);
  asp.locName() = *(sockList[i].localaddr);
  asp.locPort() = *(sockList[i].localport);

  // Identify local socket
  SockAddr sa;
  socklen_t saLen = sizeof(SockAddr);

  // Handle active socket
  int len;
  unsigned char buf[RECV_MAX_LEN];
  saLen = sizeof(SockAddr);
  switch (sockList[i].type) {
  case IPL4asp_UDP:
    debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: udp message received");
    asp.proto().udp() = UdpTuple(null_type());
    len = recvfrom(sockList[i].sock, buf, RECV_MAX_LEN,
                   0, (struct sockaddr *)&sa, &saLen);
    if (len == -1) {
      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: udp recvfrom error: %s",
            strerror(errno));
      sendError(PortError::ERROR__SOCKET, i, errno);
      break;
    }
    if (!SetNameAndPort(&sa, saLen, asp.remName(), asp.remPort()))
      sendError(PortError::ERROR__HOSTNAME, i);

    asp.msg() = OCTETSTRING(len, buf);
    incoming_message(asp);
    break;
  case IPL4asp_TCP_LISTEN: {
    debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: tcp connection requested");
    ASP__Event event;
    int sock = accept(sockList[i].sock, (struct sockaddr *)&sa, &saLen);
    if (sock == -1) {
      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: tcp accept error: %s",
            strerror(errno));
      sendError(PortError::ERROR__SOCKET, i, errno);
      break;
    }
    int k = ConnAdd(IPL4asp_TCP, sock, i);
    if ( k == -1) {
      sendError(PortError::ERROR__INSUFFICIENT__MEMORY, i);
      break;
    }

    event.connOpened().remName() = *(sockList[k].remoteaddr);
    event.connOpened().remPort() = *(sockList[k].remoteport);
    event.connOpened().locName() = *(sockList[k].localaddr);
    event.connOpened().locPort() = *(sockList[k].localport);
    event.connOpened().proto().tcp() = TcpTuple(null_type());
    event.connOpened().connId() = k;
    event.connOpened().userData() = sockList[k].userData;
    incoming_message(event);
    break;
  } // IPL4asp_TCP_LISTEN
  case IPL4asp_TCP: {
    asp.proto().tcp() = TcpTuple(null_type());
    memset(&sa, 0, saLen);
    len = recv(sockList[i].sock, buf, RECV_MAX_LEN, 0);

    if (len == -1) {
      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: tcp recvfrom error: %s",
            strerror(errno));
      sendError(PortError::ERROR__SOCKET, i, errno);
      break;
    }

    if (len == 0) {
      closingPeer = sa;
      closingPeerLen = saLen;
      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: "
            "peer closed connection %d, fd: %d", i, sockList[i].sock);
      if (i == dontCloseConnectionId) {
        close(sockList[i].sock);
        debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: closing connection %d"
              " postponed due to nonblocking send operation", i);
        break;
      }
      ASP__Event event;
      event.connClosed() =
        ConnectionClosedEvent(i,
          asp.remName(), asp.remPort(), asp.locName(), asp.locPort(),
          asp.proto(), asp.userData());
      if (ConnDel(i) == -1) {
        debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: ConnDel failed");
        sendError(PortError::ERROR__SOCKET, i);
      }
//      sockList[i].sock = SockDesc::SOCK_CLOSED;
      incoming_message(event);
      closingPeerLen = 0;
      break;
    }

    (*sockList[i].buf)->put_s(len, buf);

    bool msgFound = false;
    do {
      if (sockList[i].getMsgLen != simpleGetMsgLen) {
        if (sockList[i].msgLen == -1)
          sockList[i].msgLen = sockList[i].getMsgLen.invoke(
            OCTETSTRING((*sockList[i].buf)->get_len(), (*sockList[i].buf)->get_data()),
            *sockList[i].msgLenArgs);
        len = sockList[i].msgLen;
      } else {
        len = (*sockList[i].buf)->get_len();
      }
      msgFound = len != -1 && len <= (int)sockList[i].buf[0]->get_len();
      if (msgFound) {
        debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: message length: (%d/%d bytes)\n",
          len, (int)sockList[i].buf[0]->get_len());
        asp.msg() = OCTETSTRING(len, sockList[i].buf[0]->get_data());
        sockList[i].buf[0]->set_pos((size_t)len);
        sockList[i].buf[0]->cut();
        if(lazy_conn_id_level && sockListCnt==1 && lonely_conn_id!=-1){
          asp.connId()=-1;
        }
        incoming_message(asp);
        sockList[i].msgLen = -1;
      }
    } while (msgFound && sockList[i].buf[0]->get_len() != 0);
    if (sockList[i].buf[0]->get_len() != 0)
      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: incomplete message (%d bytes)\n",
        (int)sockList[i].buf[0]->get_len());
    break;
  } // IPL4asp_TCP
  case IPL4asp_SCTP_LISTEN: {
#ifdef USE_SCTP
    debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: sctp connection requested");
    ASP__Event event;
    int sock = accept(sockList[i].sock, (struct sockaddr *)&sa, &saLen);
    if (sock == -1) {
      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: sctp accept error: %s",
            strerror(errno));
      sendError(PortError::ERROR__SOCKET, i, errno);
      break;
    }
    int k = ConnAdd(IPL4asp_SCTP, sock, i);
    if ( k == -1) {
      sendError(PortError::ERROR__INSUFFICIENT__MEMORY, i);
      break;
    }

    event.connOpened().remName() = *(sockList[k].remoteaddr);
    event.connOpened().remPort() = *(sockList[k].remoteport);
    event.connOpened().locName() = *(sockList[k].localaddr);
    event.connOpened().locPort() = *(sockList[k].localport);
    event.connOpened().proto().sctp() = SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
    event.connOpened().connId() = k;
    event.connOpened().userData() = sockList[k].userData;
    incoming_message(event);
#else
    sendError(PortError::ERROR__UNSUPPORTED__PROTOCOL,
              sockList[i].sock);
#endif
    break;
  } // IPL4asp_SCTP_LISTEN
  case IPL4asp_SCTP: {
#ifdef USE_SCTP
    debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: sctp message received");
    memset(&sa, 0, saLen);

    debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: sctp peername and sockname obtained");


    ASP__Event event;
    int sock = sockList[i].sock;
    ssize_t n = 0;
    size_t buflen = RECV_MAX_LEN;
    struct msghdr  msg[1];
    struct iovec  iov[1];
    struct cmsghdr  *cmsg;
    struct sctp_sndrcvinfo *sri;
    char   cbuf[sizeof (*cmsg) + sizeof (*sri)];
    size_t cmsglen = sizeof (*cmsg) + sizeof (*sri);

    /* Initialize the message header for receiving */
    memset(msg, 0, sizeof (*msg));
    msg->msg_control = cbuf;
    msg->msg_controllen = sizeof (*cmsg) + sizeof (*sri);
    msg->msg_flags = 0;
    memset(cbuf, 0, sizeof (*cmsg) + sizeof (*sri));
    cmsg = (struct cmsghdr *)cbuf;
    sri = (struct sctp_sndrcvinfo *)(cmsg + 1);
    iov->iov_base = buf;
    iov->iov_len = RECV_MAX_LEN;
    msg->msg_iov = iov;
    msg->msg_iovlen = 1;

    int getmsg_retv=getmsg(sock, i, msg, buf, &buflen, &n, cmsglen);
    switch(getmsg_retv){
      case IPL4_SCTP_WHOLE_MESSAGE_RECEIVED:
      {
        (*sockList[i].buf)->put_s(n, buf);
        const unsigned char* atm=(*sockList[i].buf)->get_data();
        if (msg->msg_flags & MSG_NOTIFICATION) {
          debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: Notification received");
          handle_event(sock, i, atm);
        } else {
          debug("PL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: Incoming data (%ld bytes): stream = %hu, ssn = %hu, flags = %hx, ppid = %u \n", n,
          sri->sinfo_stream,(unsigned int)sri->sinfo_ssn, sri->sinfo_flags, sri->sinfo_ppid);

          INTEGER i_ppid;
					if (ntohl(sri->sinfo_ppid) <= (unsigned long)INT_MAX)
						i_ppid = ntohl(sri->sinfo_ppid);
					else {
						char sbuf[16];
						sprintf(sbuf, "%u", ntohl(sri->sinfo_ppid));
						i_ppid = INTEGER(sbuf);
					}

          asp.proto().sctp() = SctpTuple(sri->sinfo_stream, i_ppid, OMIT_VALUE, OMIT_VALUE);

          (*sockList[i].buf)->get_string(asp.msg());
          if(lazy_conn_id_level && sockListCnt==1 && lonely_conn_id!=-1){
            asp.connId()=-1;
          }
          incoming_message(asp);
        }
        if(sockList[i].buf) (*sockList[i].buf)->clear();
      }
        break;
      case IPL4_SCTP_PARTIAL_RECEIVE:
        debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: partial receive: %ld bytes", n);
        (*sockList[i].buf)->put_s(n, buf);
        break;
      case IPL4_SCTP_ERROR_RECEIVED:
        debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: SCTP error, Socket is closed.");
      case IPL4_SCTP_EOF_RECEIVED:
        debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: Socket is closed.");
        asp.proto().sctp()=SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
        event.connClosed() =
          ConnectionClosedEvent(i,
            asp.remName(), asp.remPort(), asp.locName(), asp.locPort(),
            asp.proto(), asp.userData());
        if (ConnDel(i) == -1) {
          debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: ConnDel failed");
          sendError(PortError::ERROR__SOCKET, i);
        }
        incoming_message(event);
        closingPeerLen = 0;
        break;
      default:

        break;
    }

#else
    sendError(PortError::ERROR__UNSUPPORTED__PROTOCOL,
              sockList[i].sock);
#endif
    break;
  } // IPL4asp_SCTP
  case IPL4asp_SSL_LISTEN: {
#ifdef IPL4_USE_SSL
	debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: ssl connection requested");
    ASP__Event event;

    if (ssl_verify_certificate && ssl_trustedCAlist_file==NULL)
    {
      debug("%s is not defined in the configuration file altough %s=yes", ssl_trustedCAlist_file_name(), ssl_verifycertificate_name());
      sendError(PortError::ERROR__SOCKET, i);
      break;
    }

    int sock = accept(sockList[i].sock, (struct sockaddr *)&sa, &saLen);
    if (sock == -1) {
      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: accept error: %s",
            strerror(errno));
      sendError(PortError::ERROR__SOCKET, i, errno);
      break;
    }

    int k = ConnAdd(IPL4asp_SSL, sock, i);
    if ( k == -1) {
      sendError(PortError::ERROR__INSUFFICIENT__MEMORY, i);
      break;
    }


    switch(perform_ssl_handshake(k))
    {
      case 1:
    	  sockList[k].sslState = STATE_NORMAL;
    	  break; //sucess
      case -1:
		  if(pureNonBlocking)
		  {
		  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: leave");
		  return;
		  }
		  else debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: PureNonBlocking is false : programming error.");
      case 0:
      default:
    	debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: SSL mapping failed for client socket: %d", sockList[k].sock);
        if (ConnDel(k) == -1)
          debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: unable to close socket");

    	 sendError(PortError::ERROR__SOCKET, i, errno);
    	 debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: leave");
    	 return;
    }

    event.connOpened().remName() = *(sockList[k].remoteaddr);
    event.connOpened().remPort() = *(sockList[k].remoteport);
    event.connOpened().locName() = *(sockList[k].localaddr);
    event.connOpened().locPort() = *(sockList[k].localport);
    event.connOpened().proto().ssl() = SslTuple(null_type());
    event.connOpened().connId() = k;
    event.connOpened().userData() = sockList[k].userData;
    incoming_message(event);
#else
    sendError(PortError::ERROR__UNSUPPORTED__PROTOCOL,
              sockList[i].sock);
#endif
    break;
  } // IPL4asp_SSL_LISTEN
  case IPL4asp_SSL: {
#ifdef IPL4_USE_SSL
	debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: SSL recvfrom enter:");
    asp.proto().ssl() = SslTuple(null_type());
    memset(&sa, 0, saLen);

    if (!isConnIdValid(i)) {
      debug("IPL4asp__PT_PROVIDER::receive_message_on_fd: invalid connId: %i", i);
      break;
    }

    log_debug("receiving data");
    len = receive_ssl_message_on_fd(i);

    if (len == -1) {
      debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: ssl recvfrom error: %s",
            strerror(errno));
      sendError(PortError::ERROR__SOCKET, i);
      break;
    }


    if (len == 0) { // peer disconnected
      log_debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: Client %d closed connection.", fd);
      switch (sockList[i].sslState) {
        case STATE_BLOCK_FOR_SENDING:
          log_debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: state is STATE_BLOCK_FOR_SENDING, don't close connection.");
          Handler_Remove_Fd_Read(i);
          sockList[i].sslState = STATE_DONT_CLOSE;
          log_debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: setting socket state to STATE_DONT_CLOSE");
          break;
        case STATE_DONT_CLOSE:
          log_debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: state is STATE_DONT_CLOSE, don't close connection.");
          break;
        default:
            closingPeer = sa;
            closingPeerLen = saLen;
            debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: "
                  "peer closed connection %d, fd: %d", i, sockList[i].sock);
            if (i == dontCloseConnectionId) {
              close(sockList[i].sock);
              debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: closing connection %d"
                    " postponed due to nonblocking send operation", i);
              break;
            }
            ASP__Event event;
            event.connClosed() =
              ConnectionClosedEvent(i,
                asp.remName(), asp.remPort(), asp.locName(), asp.locPort(),
                asp.proto(), asp.userData());
            if (ConnDel(i) == -1) {
              debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: ConnDel failed");
              sendError(PortError::ERROR__SOCKET, i);
            }
            incoming_message(event);
            closingPeerLen = 0;
      } // switch (client_data->reading_state)
    } else if (len > 0) {
        bool msgFound = false;
        do {
          if (sockList[i].getMsgLen != simpleGetMsgLen) {
            if (sockList[i].msgLen == -1)
              sockList[i].msgLen = sockList[i].getMsgLen.invoke(
                OCTETSTRING((*sockList[i].buf)->get_len(), (*sockList[i].buf)->get_data()),
                *sockList[i].msgLenArgs);
            len = sockList[i].msgLen;
          } else {
            len = (*sockList[i].buf)->get_len();
          }
          msgFound = len != -1 && len <= (int)sockList[i].buf[0]->get_len();
          if (msgFound) {
            debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: message length: (%d/%d bytes)\n",
              len, (int)sockList[i].buf[0]->get_len());
            asp.msg() = OCTETSTRING(len, sockList[i].buf[0]->get_data());
            sockList[i].buf[0]->set_pos((size_t)len);
            sockList[i].buf[0]->cut();
            if(lazy_conn_id_level && sockListCnt==1 && lonely_conn_id!=-1){
              asp.connId()=-1;
            }
            incoming_message(asp);
            sockList[i].msgLen = -1;
          }
        } while (msgFound && sockList[i].buf[0]->get_len() != 0);
        if (sockList[i].buf[0]->get_len() != 0)
          debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: incomplete message (%d bytes)\n",
            (int)sockList[i].buf[0]->get_len());
    } /* else if (len == -2) =>
          means that reading would bloc.
          in this case I stop receiving message on the file descriptor, do nothing */
#else
    sendError(PortError::ERROR__UNSUPPORTED__PROTOCOL,
              sockList[i].sock);
#endif
    break;
  } // IPL4asp_SSL
  default:
    sendError(PortError::ERROR__UNSUPPORTED__PROTOCOL,
              sockList[i].sock);
    break;
  } // switch
  debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: leave");
} // IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable


int IPL4asp__PT_PROVIDER::getmsg(int fd, int connId, struct msghdr *msg, void *buf, size_t *buflen,
    ssize_t *nrp, size_t cmsglen)
{
#ifdef USE_SCTP
  *nrp = recvmsg(fd, msg, 0);
  debug("IPL4asp__PT_PROVIDER::getmsg: nr: %ld",*nrp);
  if (*nrp < 0) {
   /* EOF or error */
   debug("IPL4asp__PT_PROVIDER::getmsg: error: %d, %s",errno,strerror(errno));
   return IPL4_SCTP_ERROR_RECEIVED;
  } else if (*nrp==0){
   debug("IPL4asp__PT_PROVIDER::getmsg: connection closed");
   return IPL4_SCTP_EOF_RECEIVED;
  }
  /* Whole message is received, return it. */
  if (msg->msg_flags & MSG_EOR) {
   return IPL4_SCTP_WHOLE_MESSAGE_RECEIVED;
  }
#endif
  return IPL4_SCTP_PARTIAL_RECEIVE;
}


void IPL4asp__PT_PROVIDER::handle_event(int fd, int connId, const void *buf)
{
#ifdef USE_SCTP
  ASP__Event event;
  union sctp_notification  *snp;
  snp = (sctp_notification *)buf;
  switch (snp->sn_header.sn_type)
  {
    case SCTP_ASSOC_CHANGE:
      debug("IPL4asp__PT_PROVIDER::handle_event: incoming SCTP_ASSOC_CHANGE event.");
      struct sctp_assoc_change *sac;
      sac = &snp->sn_assoc_change;
      if (events.sctp_association_event) {
        event.sctpEvent().sctpAssocChange().clientId() = connId;
        event.sctpEvent().sctpAssocChange().proto().sctp() = SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
        switch(sac->sac_state){
          case SCTP_COMM_UP:
            event.sctpEvent().sctpAssocChange().sac__state() = IPL4asp__Types::SAC__STATE::SCTP__COMM__UP;
            break;

          case SCTP_COMM_LOST:
            event.sctpEvent().sctpAssocChange().sac__state() = IPL4asp__Types::SAC__STATE::SCTP__COMM__LOST;
            break;

          case SCTP_RESTART:
            event.sctpEvent().sctpAssocChange().sac__state() = IPL4asp__Types::SAC__STATE::SCTP__RESTART;
            break;

          case SCTP_SHUTDOWN_COMP:
            event.sctpEvent().sctpAssocChange().sac__state() = IPL4asp__Types::SAC__STATE::SCTP__SHUTDOWN__COMP;
            break;

          case SCTP_CANT_STR_ASSOC:
            event.sctpEvent().sctpAssocChange().sac__state() = IPL4asp__Types::SAC__STATE::SCTP__CANT__STR__ASSOC;
            break;

         default:
            event.sctpEvent().sctpAssocChange().sac__state() = IPL4asp__Types::SAC__STATE::SCTP__UNKNOWN__SAC__STATE;
            TTCN_warning("IPL4asp__PT_PROVIDER::handle_event: Unexpected sac_state value received %d", sac->sac_state);
            break;
        }
        
        incoming_message(event);
        if(sac->sac_state == SCTP_COMM_LOST) {
          debug("IPL4asp__PT_PROVIDER::handle_event: SCTP_ASSOC_CHANGE event SCTP_COMM_LOST, closing Sock.");
          ASP__Event event_close;
          ProtoTuple proto_close;
          proto_close.sctp()=SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
          event_close.connClosed() = ConnectionClosedEvent(connId,
                                    *(sockList[connId].remoteaddr),
                                    *(sockList[connId].remoteport),
                                    *(sockList[connId].localaddr),
                                    *(sockList[connId].localport),
                                    proto_close, sockList[connId].userData);
          if (ConnDel(connId) == -1) {
            debug("IPL4asp__PT_PROVIDER::handle_event: ConnDel failed");
            sendError(PortError::ERROR__SOCKET, connId);
          }
          closingPeerLen = 0;
          incoming_message(event_close);

        }
      }
      break;
    case SCTP_PEER_ADDR_CHANGE:
      debug("IPL4asp__PT_PROVIDER::handle_event: incoming SCTP_PEER_ADDR_CHANGE event.");
      struct sctp_paddr_change *spc;
      spc = &snp->sn_paddr_change;
      if (events.sctp_address_event)
      {
        event.sctpEvent().sctpPeerAddrChange().clientId() = connId;
        switch(spc->spc_state)
        {
         case SCTP_ADDR_AVAILABLE:
           event.sctpEvent().sctpPeerAddrChange().spc__state() = IPL4asp__Types::SPC__STATE::SCTP__ADDR__AVAILABLE;
           break;

         case SCTP_ADDR_UNREACHABLE:
           event.sctpEvent().sctpPeerAddrChange().spc__state() = IPL4asp__Types::SPC__STATE::SCTP__ADDR__UNREACHABLE;
           break;

         case SCTP_ADDR_REMOVED:
           event.sctpEvent().sctpPeerAddrChange().spc__state() = IPL4asp__Types::SPC__STATE::SCTP__ADDR__REMOVED;
           break;

         case SCTP_ADDR_ADDED:
           event.sctpEvent().sctpPeerAddrChange().spc__state() = IPL4asp__Types::SPC__STATE::SCTP__ADDR__ADDED;
           break;

         case SCTP_ADDR_MADE_PRIM:
           event.sctpEvent().sctpPeerAddrChange().spc__state() = IPL4asp__Types::SPC__STATE::SCTP__ADDR__MADE__PRIM;
           break;

#if  defined(LKSCTP_1_0_7) || defined(LKSCTP_1_0_9)
         case SCTP_ADDR_CONFIRMED:
           event.sctpEvent().sctpPeerAddrChange().spc__state() = IPL4asp__Types::SPC__STATE::SCTP__ADDR__CONFIRMED;
           break;
#endif
        default:
           event.sctpEvent().sctpPeerAddrChange().spc__state() = IPL4asp__Types::SPC__STATE::SCTP__UNKNOWN__SPC__STATE;
           TTCN_warning("IPL4asp__PT_PROVIDER::handle_event: Unexpected spc_state value received %d", spc->spc_state);
           break;
        }
        incoming_message(event);
      }
      break;
    case SCTP_REMOTE_ERROR:
      debug("IPL4asp__PT_PROVIDER::handle_event: incoming SCTP_REMOTE_ERROR event.");
      struct sctp_remote_error *sre;
      sre = &snp->sn_remote_error;
      if (events.sctp_peer_error_event) {
        event.sctpEvent().sctpRemoteError().clientId() = connId;
        incoming_message(event);
      }
      break;
    case SCTP_SEND_FAILED:
      debug("IPL4asp__PT_PROVIDER::handle_event: incoming SCTP_SEND_FAILED event.");
      struct sctp_send_failed *ssf;
      ssf = &snp->sn_send_failed;
      if (events.sctp_send_failure_event) {
        event.sctpEvent().sctpSendFailed().clientId() = connId;
        incoming_message(event);
      }
      break;
    case SCTP_SHUTDOWN_EVENT:
      debug("IPL4asp__PT_PROVIDER::handle_event: incoming SCTP_SHUTDOWN_EVENT event.");
      struct sctp_shutdown_event *sse;
      sse = &snp->sn_shutdown_event;
      if (events.sctp_shutdown_event) {
        event.sctpEvent().sctpShutDownEvent().clientId() = connId;
        incoming_message(event);
      }
      break;
#if  defined(LKSCTP_1_0_7) || defined(LKSCTP_1_0_9)
    case SCTP_ADAPTATION_INDICATION:
      debug("IPL4asp__PT_PROVIDER::handle_event: incoming SCTP_ADAPTATION_INDICATION event.");
      struct sctp_adaptation_event *sai;
      sai = &snp->sn_adaptation_event;
      if (events.sctp_adaptation_layer_event) {
        event.sctpEvent().sctpAdaptationIndication().clientId() = connId;
        incoming_message(event);
      }
      break;
#else
    case SCTP_ADAPTION_INDICATION:
      debug("IPL4asp__PT_PROVIDER::handle_event: incoming SCTP_ADAPTION_INDICATION event.");
      struct sctp_adaption_event *sai;
      sai = &snp->sn_adaption_event;
      if (events.sctp_adaption_layer_event) {
        event.sctpEvent().sctpAdaptationIndication().clientId() = connId;
        incoming_message(event);
      }
      break;
#endif
    case SCTP_PARTIAL_DELIVERY_EVENT:
      debug("IPL4asp__PT_PROVIDER::handle_event: incoming SCTP_PARTIAL_DELIVERY_EVENT event.");
      struct sctp_pdapi_event *pdapi;
      pdapi = &snp->sn_pdapi_event;
      if (events.sctp_partial_delivery_event) {
        event.sctpEvent().sctpPartialDeliveryEvent().clientId() = connId;
        incoming_message(event);
      }
      break;
    default:
      debug("IPL4asp__PT_PROVIDER::handle_event: Unknown notification type!");
      break;
  }
#endif
}



void IPL4asp__PT_PROVIDER::user_map(const char *system_port)
{
  debug("IPL4asp__PT_PROVIDER::user_map: enter");
  sockList = NULL;
  sockListCnt = 0;
  firstFreeSock = -1;
  lastFreeSock = -1;
  dontCloseConnectionId = -1;
  closingPeerLen = 0;
  lonely_conn_id = -1;
#ifdef USE_IPL4_EIN_SCTP
  if(!native_stack) do_bind();
#endif
  
  mapped = true;
} // IPL4asp__PT_PROVIDER::user_map


void IPL4asp__PT_PROVIDER::user_unmap(const char *system_port)
{
  debug("IPL4asp__PT_PROVIDER::user_unmap: enter");
  mapped = false;
  if (sockListCnt > 0) {
    debug("IPL4asp__PT_PROVIDER::user_unmap: There are %i open connections",
      sockListCnt);
  }
  if (sockList != 0) {
    for (unsigned int i = 1; i < sockListSize; ++i) {
#ifdef USE_IPL4_EIN_SCTP
  if(!native_stack && sockList[i].sock != SockDesc::SOCK_NONEX && (sockList[i].type==IPL4asp_SCTP_LISTEN || sockList[i].type==IPL4asp_SCTP)){
      sockList[i].next_action=SockDesc::ACTION_DELETE;
      if(sockList[i].type == IPL4asp_SCTP){
        EINSS7_00SctpShutdownReq(sockList[i].sock);
      } else {
        if(sockList[i].ref_count==0){
          EINSS7_00SctpDestroyReq(sockList[i].endpoint_id);
          ConnDelEin(i);
        }
      }

  }
  else
#endif
      
      if (sockList[i].sock > 0)
      ConnDel(i);
    }
  }
  sockListCnt = 0;
  firstFreeSock = -1;
  lastFreeSock = -1;
#ifdef USE_IPL4_EIN_SCTP
  if(!native_stack) do_unbind();
#endif
  Free(sockList); sockList = 0;
  lonely_conn_id = -1;
  Uninstall_Handler();
} // IPL4asp__PT_PROVIDER::user_unmap



void IPL4asp__PT_PROVIDER::user_start()
{
  debug("IPL4asp__PT_PROVIDER::user_start: enter");
} // IPL4asp__PT_PROVIDER::user_start



void IPL4asp__PT_PROVIDER::user_stop()
{
  debug("IPL4asp__PT_PROVIDER::user_stop: enter");
} // IPL4asp__PT_PROVIDER::user_stop


bool IPL4asp__PT_PROVIDER::getAndCheckSockType(int connId,
  ProtoTuple::union_selection_type proto, SockType &type)
{
  if (!isConnIdValid(connId)) {
    debug("IPL4asp__PT_PROVIDER::getAndCheckSockType: invalid connId: %i", connId);
    return false;
  }
  type = sockList[connId].type;
  if (proto != ProtoTuple::UNBOUND_VALUE &&
      proto != ProtoTuple::ALT_unspecified) {
    /* Proto is specified. It is used for checking only. */
    if ((type == IPL4asp_UDP && proto != ProtoTuple::ALT_udp) ||
        ((type == IPL4asp_TCP_LISTEN || type == IPL4asp_TCP) && proto != ProtoTuple::ALT_tcp) ||
        ((type == IPL4asp_SCTP_LISTEN || type == IPL4asp_SCTP) && proto != ProtoTuple::ALT_sctp)||
        ((type == IPL4asp_SSL_LISTEN || type == IPL4asp_SSL) && proto != ProtoTuple::ALT_ssl)) {
      return false;
    }
  }
  return true;
}

void IPL4asp__PT_PROVIDER::outgoing_send(const ASP__Send& asp)
{
  debug("IPL4asp__PT_PROVIDER::outgoing_send: ASP Send: enter");
  testIfInitialized();
  Socket__API__Definitions::Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
  outgoing_send_core(asp, result);
  if(result.errorCode().ispresent()){
    ASP__Event event;
    if(send_extended_result){
      event.extended__result().errorCode() =result.errorCode();
      event.extended__result().connId() =result.connId();
      event.extended__result().os__error__code() =result.os__error__code();
      event.extended__result().os__error__text() =result.os__error__text();
      event.extended__result().msg() = asp.msg();
    } else {
      event.result()=result;
    }
    incoming_message(event);
  }
} // IPL4asp__PT_PROVIDER::outgoing_send

int IPL4asp__PT_PROVIDER::outgoing_send_core(const ASP__Send& asp,  Socket__API__Definitions::Result& result)
{
  debug("IPL4asp__PT_PROVIDER::outgoing_send_core: ASP Send: enter");
  testIfInitialized();
  SockType type;
  int local_conn_id=asp.connId();
  if(lazy_conn_id_level && asp.connId()==-1){
    local_conn_id=lonely_conn_id;
  }
  ProtoTuple::union_selection_type proto = ProtoTuple::ALT_unspecified;
  if (asp.proto().ispresent())
    proto = asp.proto()().get_selection();
  if (getAndCheckSockType(local_conn_id, proto, type)) {
    if (asp.proto().ispresent()) {
      debug("IPL4asp__PT_PROVIDER::outgoing_send_core: ASP Send: calling sendNonBlocking with proto");
      return sendNonBlocking(local_conn_id, (sockaddr *)NULL, (socklen_t)0, type, asp.msg(), result, asp.proto());
    } else {
      return sendNonBlocking(local_conn_id, (sockaddr *)NULL, (socklen_t)0, type, asp.msg(), result);
    }
  }
  else
    setResult(result, PortError::ERROR__INVALID__INPUT__PARAMETER, asp.connId());
  return -1;
} // IPL4asp__PT_PROVIDER::outgoing_send_core

void IPL4asp__PT_PROVIDER::outgoing_send(const ASP__SendTo& asp)
{
  debug("IPL4asp__PT_PROVIDER::outgoing_send: ASP Send: enter");
  testIfInitialized();
  Socket__API__Definitions::Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
  outgoing_send_core(asp, result);
  if(result.errorCode().ispresent()){
    ASP__Event event;
    if(send_extended_result){
      event.extended__result().errorCode() =result.errorCode();
      event.extended__result().connId() =result.connId();
      event.extended__result().os__error__code() =result.os__error__code();
      event.extended__result().os__error__text() =result.os__error__text();
      event.extended__result().msg() = asp.msg();
    } else {
      event.result()=result;
    }
    incoming_message(event);
  }
}
int IPL4asp__PT_PROVIDER::outgoing_send_core(const ASP__SendTo& asp,  Socket__API__Definitions::Result& result)
{
  debug("IPL4asp__PT_PROVIDER::outgoing_send_core: ASP SendTo: enter");
  testIfInitialized();
  SockType type;
  ProtoTuple::union_selection_type proto = ProtoTuple::ALT_unspecified;
  if (asp.proto().ispresent())
    proto = asp.proto()().get_selection();
  if (getAndCheckSockType(asp.connId(), proto, type)) {
    if (asp.remPort() < 0 || asp.remPort() > 65535){
      setResult(result, PortError::ERROR__INVALID__INPUT__PARAMETER, asp.connId());
      return -1;
    }
    SockAddr to;
    socklen_t toLen;
    switch (type) {
      case IPL4asp_UDP:
      case IPL4asp_TCP_LISTEN:
      case IPL4asp_TCP:
#ifdef IPL4_USE_SSL
      case IPL4asp_SSL_LISTEN:
      case IPL4asp_SSL:
#endif
    	if (SetSockAddr(asp.remName(), asp.remPort(), to, toLen) == -1) {
          setResult(result,PortError::ERROR__HOSTNAME, asp.connId());
          return -1;
        }
        break;
      case IPL4asp_SCTP_LISTEN:
      case IPL4asp_SCTP:
#ifdef USE_SCTP
        if (SetSockAddr(asp.remName(), asp.remPort(), to, toLen) == -1) {
          setResult(result,PortError::ERROR__HOSTNAME, asp.connId());
          return -1;
        }
#endif
      default:
        setResult(result,PortError::ERROR__UNSUPPORTED__PROTOCOL, asp.connId());
        return -1;
	  }
    if (asp.proto().ispresent()) {
      debug("IPL4asp__PT_PROVIDER::outgoing_send_core: ASP SendTo: calling sendNonBlocking with proto");
      return sendNonBlocking(asp.connId(), (sockaddr *)&to, toLen, type, asp.msg(), result, asp.proto());
    } else {
      return sendNonBlocking(asp.connId(), (sockaddr *)&to, toLen, type, asp.msg(), result);
    }
  } else
    setResult(result, PortError::ERROR__INVALID__INPUT__PARAMETER, asp.connId());
  return -1;
} // IPL4asp__PT_PROVIDER::outgoing_send_core



int IPL4asp__PT_PROVIDER::sendNonBlocking(const ConnectionId& connId, sockaddr *sa,
       socklen_t saLen, SockType type, const OCTETSTRING& msg, Socket__API__Definitions::Result& result, const Socket__API__Definitions::ProtoTuple& protoTuple)
{
  debug("IPL4asp__PT_PROVIDER::sendNonBlocking: enter: connId: %d", (int)connId);
  int sock = sockList[(int)connId].sock;
  debug("IPL4asp__PT_PROVIDER::sendNonBlocking: fd: %d", sock);
  if (sock < 0) {
    debug("IPL4asp__PT_PROVIDER::sendNonBlocking: Connection %s",
      (sock == SockDesc::SOCK_CLOSED) ? "closed" : "does not exist");
    setResult(result,PortError::ERROR__SOCKET, (int)connId);
    return -1;
  }
  int rem = msg.lengthof();
  int sent_octets=0;
  const unsigned char *ptr = (const unsigned char *)msg;

#ifdef IPL4_USE_SSL
  if(type == IPL4asp_SSL)
  {
	  if (ssl_current_client!=NULL) log_warning("Warning: race condition while setting current client object pointer");
	  ssl_current_client=(IPL4asp__PT_PROVIDER *)this;

      if (sockList[(int)connId].sslState!=STATE_NORMAL)
      {
        // The socket is not writeable, so we subscribe to the event that notifies us when it becomes writable again
        // and we pass up a TEMPORARILY_UNAVAILABLE ASP to inform the user.
        // TODO: This functionality can be improved by buffering the message that we couldn't send thus the user doesn't have to
        // buffer it himself. It would be more efficient to buffer it here anyway (it would mean less ASP traffic)
        Handler_Add_Fd_Write(sock);
        setResult(result,PortError::ERROR__TEMPORARILY__UNAVAILABLE, (int)connId, EAGAIN);
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: leave (TEMPORARILY UNAVAILABLE)   "
          "connId: %i, fd: %i", (int)connId, sock);
        ssl_current_client=NULL;
        return 0;
      }


	  if(!getSslObj((int)connId, ssl_current_ssl))
	  {
		 log_debug("IPL4asp__PT_PROVIDER::sendNonBlocking: current SSL invalid for client: %d", (int)connId);
	     debug("IPL4asp__PT_PROVIDER::sendNonBlocking: Connection %s",
	        (sock == SockDesc::SOCK_CLOSED) ? "closed" : "does not exist");
	     setResult(result,PortError::ERROR__SOCKET, (int)connId);
       ssl_current_client=NULL;
	     return -1;
	  }

	  if (ssl_current_ssl==NULL)
	  {
		 log_debug("PL4asp__PT_PROVIDER::sendNonBlocking:No SSL data available for client %d",(int)connId);
		 setResult(result,PortError::ERROR__SOCKET, (int)connId);
     ssl_current_client=NULL;
		 return -1;
	  }
  }
#endif

  while (rem != 0) {
    int ret=-1;
    switch (type) {
#ifdef IPL4_USE_SSL
    case IPL4asp_SSL:
    	  //ret = sendNonBlocking(connId, ptr, rem);

    	  log_debug("Client ID = %d", (int)connId);
    	  //while (true) {
    	    int res;

	        // check if client exists
	        if (!isConnIdValid((int)connId)){
	            log_debug("IPL4asp__PT_PROVIDER::sendNonBlocking, Client ID %d does not exist.", (int)connId);
	            ret = -2;
	            break;
	        }

    	    log_debug("  one write cycle started");
    	    if (sockList[connId].sslState == STATE_DONT_CLOSE){
    	        //goto client_closed_connection;
    	    	//process postponed connection close
    	    	log_debug("IPL4asp__PT_PROVIDER::sendNonBlocking: Process postponed connection close.");
        	    SSL_set_quiet_shutdown(ssl_current_ssl, 1);
        	    log_debug("Setting SSL SHUTDOWN mode to QUIET");
        	    ssl_current_client=NULL;
        	    log_debug("leaving IPL4asp__PT_PROVIDER::sendNonBlocking()");
        	    log_debug("IPL4asp__PT_PROVIDER::sendNonBlocking: setting socket state to STATE_NORMAL");
        	    sockList[(int)connId].sslState = STATE_NORMAL;
        	    errno = EPIPE;
    	    	ret = -1;
    	    	break;
    	    }else res = ssl_getresult(SSL_write(ssl_current_ssl, ptr, rem));

    	    switch (res) {
    	    case SSL_ERROR_NONE:
    	      ssl_current_client=NULL;
    	      log_debug("leaving IPL4asp__PT_PROVIDER::sendNonBlocking() %d bytes is sent.", rem);
    	      log_debug("IPL4asp__PT_PROVIDER::sendNonBlocking: setting socket state to STATE_NORMAL");
    	      sockList[(int)connId].sslState = STATE_NORMAL;
    	      ret = rem; //all bytes are sent without any problem
    	      break;
    	    case SSL_ERROR_WANT_WRITE:
				int old_bufsize, new_bufsize;
				if (increase_send_buffer((int)connId, old_bufsize, new_bufsize)) {
					log_debug("Sending data on on file descriptor %d",(int)connId);
				    log_debug("The sending operation would block execution. The "
					  "size of the outgoing buffer was increased from %d to "
							  "%d bytes.",old_bufsize,
							  new_bufsize);
				    //retry to send
				    ret = 0;
				} else {
					log_warning("Sending data on file descriptor %d", (int)connId);
					log_warning("The sending operation would block execution and it "
								"is not possible to further increase the size of the "
					"outgoing buffer. Trying to process incoming data to "
					"avoid deadlock.");
					ssl_current_client=NULL;
					log_debug("IPL4asp__PT_PROVIDER::sendNonBlocking: setting socket state to STATE_BLOCK_FOR_SENDING");
					sockList[(int)connId].sslState = STATE_BLOCK_FOR_SENDING;
					//continue with EAGAIN
					errno = EAGAIN;
	    	        ret = -1;
				}
;    	        break;
    	    case SSL_ERROR_WANT_READ:
    	      //receiving buffer is probably empty thus reading would block execution
    	      if(!pureNonBlocking)
    	      {
			    log_debug("SSL_write cannot read data from socket %d. Trying to process data to avoid deadlock.", (int)connId);
				log_debug("IPL4asp__PT_PROVIDER::sendNonBlocking: setting socket state to STATE_DONT_RECEIVE");
				sockList[(int)connId].sslState = STATE_DONT_RECEIVE; //don't call receive_message_on_fd() to this socket
				for (;;) {
				  TTCN_Snapshot::take_new(TRUE);
				  pollfd pollClientFd = {sockList[(int)connId].sock, POLLIN, 0 };
				  int nEvents = poll(&pollClientFd, 1, 0);
				  if (nEvents == 1 && (pollClientFd.revents & (POLLIN | POLLHUP)) != 0)
					break;
			      if (nEvents < 0 && errno != EINTR)
			      {
					log_debug("System call poll() failed on file descriptor %d", sockList[(int)connId].sock);
					SSL_set_quiet_shutdown(ssl_current_ssl, 1);
					log_debug("Setting SSL SHUTDOWN mode to QUIET");
					log_debug("leaving IPL4asp__PT_PROVIDER::sendNonBlocking()");
					log_debug("IPL4asp__PT_PROVIDER::sendNonBlocking: setting socket state to STATE_NORMAL");
					sockList[(int)connId].sslState = STATE_NORMAL;
					ret = -1;
					break;
			      }
			    }
				log_debug("Deadlock resolved");
    	      }
    	      else
    	      {
    	    	  errno = EAGAIN;
    	    	  ret = -1;
    	      }
    	      break;
    	    case SSL_ERROR_ZERO_RETURN:
        	    log_warning("IPL4asp__PT_PROVIDER::sendNonBlocking: SSL connection was interrupted by the other side");
        	    SSL_set_quiet_shutdown(ssl_current_ssl, 1);
        	    log_debug("Setting SSL SHUTDOWN mode to QUIET");
        	    ssl_current_client=NULL;
        	    log_debug("leaving IPL4asp__PT_PROVIDER::sendNonBlocking()");
        	    log_debug("IPL4asp__PT_PROVIDER::sendNonBlocking: setting socket state to STATE_NORMAL");
        	    sockList[(int)connId].sslState = STATE_NORMAL;
        	    errno = EPIPE;
        	    ret = -1;
              break;
    	    default:
    	      log_debug("SSL error occured");
    	      SSL_set_quiet_shutdown(ssl_current_ssl, 1);
    	      log_debug("Setting SSL SHUTDOWN mode to QUIET");
    	      log_debug("leaving IPL4asp__PT_PROVIDER::sendNonBlocking()");
    	      log_debug("IPL4asp__PT_PROVIDER::sendNonBlocking: setting socket state to STATE_NORMAL");
    	      sockList[(int)connId].sslState = STATE_NORMAL;
    	      errno = EPIPE;
    	      ret = -1;
    	    }//end switch res
    	  //}//end while
    	  break;
#endif
      case IPL4asp_UDP:
      case IPL4asp_TCP_LISTEN:
      case IPL4asp_TCP:
        if (sa != NULL)
          ret = sendto(sock, ptr, rem, 0, sa, saLen);
        else
          ret = ::send(sock, ptr, rem, 0);
        break;
      case IPL4asp_SCTP_LISTEN:
      case IPL4asp_SCTP:

#ifdef USE_IPL4_EIN_SCTP
        if(!native_stack){
  	          ULONG_T streamID=0;
  	          ULONG_T payloadProtId=0;

              if (protoTuple.get_selection()==ProtoTuple::ALT_sctp) {

                    if(protoTuple.sctp().sinfo__stream().ispresent()) {
                      streamID = (int)(protoTuple.sctp().sinfo__stream()());
	            }
	            if(protoTuple.sctp().sinfo__ppid().ispresent()) {
		            payloadProtId=protoTuple.sctp().sinfo__ppid()().get_long_long_val();
	            }
	          }

            USHORT_T einretval=EINSS7_00SctpSendReq(sock,payloadProtId,rem,(unsigned char*)ptr,streamID,0,NULL,false);
            if(RETURN_OK != einretval){
              setResult(result,PortError::ERROR__SOCKET, (int)connId, einretval);
              return -1;
            } else {
              return rem;
            }
          }
#endif

#ifdef USE_SCTP
        struct cmsghdr   *cmsg;
        struct sctp_sndrcvinfo  *sri;
        char cbuf[sizeof (*cmsg) + sizeof (*sri)];
        struct msghdr   msg;
        struct iovec   iov;

        iov.iov_len = rem;

        memset(&msg, 0, sizeof (msg));
        iov.iov_base = (char *)ptr;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = cbuf;
        msg.msg_controllen = sizeof (*cmsg) + sizeof (*sri);

        memset(cbuf, 0, sizeof (*cmsg) + sizeof (*sri));
        cmsg = (struct cmsghdr *)cbuf;
        sri = (struct sctp_sndrcvinfo *)(cmsg + 1);

        cmsg->cmsg_len = sizeof (*cmsg) + sizeof (*sri);
        cmsg->cmsg_level = IPPROTO_SCTP;
        cmsg->cmsg_type  = SCTP_SNDRCV;

	sri->sinfo_stream = 0;
	sri->sinfo_ppid = 0;
	if (protoTuple.get_selection()==ProtoTuple::ALT_sctp) {

          if(protoTuple.sctp().sinfo__stream().ispresent()) {
            sri->sinfo_stream = (int)(protoTuple.sctp().sinfo__stream()());
	  }
	  if(protoTuple.sctp().sinfo__ppid().ispresent()) {
			unsigned int ui;
			INTEGER in = protoTuple.sctp().sinfo__ppid()();
			if (in.get_val().is_native() && in > 0)
				ui = (unsigned int)(int)in;
			else {
				OCTETSTRING os = int2oct(in, 4);
				unsigned char* p = (unsigned char*)&ui;
				*(p++) = os[3].get_octet();
				*(p++) = os[2].get_octet();
				*(p++) = os[1].get_octet();
				*(p++) = os[0].get_octet();
							    }
	    sri->sinfo_ppid = htonl(ui);
	  }
	}
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: sctp sinfo: %d, ppid: %d", sri->sinfo_stream,sri->sinfo_ppid);

        ret = ::sendmsg(sock, &msg, 0);

        break;
#endif
      default:
        setResult(result,PortError::ERROR__UNSUPPORTED__PROTOCOL, (int)connId);
        return -1;
    }

#ifdef IPL4_USE_SSL
    //In case of ssl it is used if the client id does not exist
    //if so, cannot do anything, send an error report and return;
    if(ret == -2)
    {
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: Connection %s",
          (sock == SockDesc::SOCK_CLOSED) ? "closed" : "does not exist");
        setResult(result,PortError::ERROR__SOCKET, (int)connId);
        ssl_current_client=NULL;
        return -1;
    }
#endif

    if (ret != -1) {
      rem -= ret;
      ptr += ret;
      sent_octets += ret;
      continue; // try sending the remaning octets
    }


    debug("IPL4asp__PT_PROVIDER::sendNonBlocking: error %s", strerror(errno));

    switch (errno) { // error handling
    case EINTR:
       // interrupted signal: try again
      break;
    case EPIPE: { //client closed connection
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: "
              "client closed connection, send event ASP and remove socket");

      int ii = connId;
      
      ASP__Event event;
      event.connClosed().connId() = connId;
      event.connClosed().remName() = *(sockList[ii].remoteaddr);
      event.connClosed().remPort() = *(sockList[ii].remoteport);
      event.connClosed().locName() = *(sockList[ii].localaddr);
      event.connClosed().locPort() = *(sockList[ii].localport);
      event.connClosed().proto().tcp() = TcpTuple(null_type());
      event.connClosed().userData() = 0;
      int l_ud; getUserData((int)connId, l_ud); event.connClosed().userData() = l_ud;

      switch (type) {
        case IPL4asp_SSL:
        	event.connClosed().proto().ssl() = SslTuple(null_type());
        	break;
        case IPL4asp_UDP:
          // no such operation in UDP
          break;
        case IPL4asp_TCP_LISTEN:
        case IPL4asp_TCP:
          event.connClosed().proto().tcp() = TcpTuple(null_type());
          break;
        case IPL4asp_SCTP_LISTEN:
        case IPL4asp_SCTP:
          event.connClosed().proto().sctp() =
            SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
        default:
          setResult(result,PortError::ERROR__UNSUPPORTED__PROTOCOL, (int)connId);
          break;
      }
      debug("IPL4asp__PT_PROVIDER::sendNonBlocking");
/*      if (getsockname(sock, (struct sockaddr *)&sa, &saLen) == -1) {
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: getsockname error: %s",
              strerror(errno));
        setResult(result,PortError::ERROR__HOSTNAME, (int)connId, errno);
      } else if (!SetNameAndPort((SockAddr *)&sa, saLen,
                                 event.connClosed().locName(),
                                 event.connClosed().locPort())) {
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: SetNameAndPort failed");
        setResult(result,PortError::ERROR__HOSTNAME, (int)connId);
      }*/
      if (ConnDel((int)connId) == -1) {
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: ConnDel failed");
        setResult(result,PortError::ERROR__SOCKET, (int)connId);
      }
/*      if (!SetNameAndPort(&closingPeer, closingPeerLen,
                          event.connClosed().remName(),
                          event.connClosed().remPort())) {
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: SetNameAndPort failed");
        setResult(result,PortError::ERROR__HOSTNAME, (int)connId);
      }*/
      incoming_message(event);
      return -1;
    }

    case EAGAIN: // same as case EWOULDBLOCK:
      if (pureNonBlocking)
      {
        // The socket is not writeable, so we subscribe to the event that notifies us when it becomes writable again
        // and we pass up a TEMPORARILY_UNAVAILABLE ASP to inform the user.
        // TODO: This functionality can be improved by buffering the message that we couldn't send thus the user doesn't have to
        // buffer it himself. It would be more efficient to buffer it here anyway (it would mean less ASP traffic)
        Handler_Add_Fd_Write(sock);
        setResult(result,PortError::ERROR__TEMPORARILY__UNAVAILABLE, (int)connId, EAGAIN);
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: leave (TEMPORARILY UNAVAILABLE)   "
          "connId: %i, fd: %i", (int)connId, sock);
        return sent_octets;
      }
      // If we don't use purenonBlocking mode, we let TITAN work (and block) until the message can be sent:
      dontCloseConnectionId = (int)connId;
      closingPeerLen = 0;
      debug("IPL4asp__PT_PROVIDER::sendNonBlocking: waits in TTCN_Snapshot::block_for_sending");
      TTCN_Snapshot::block_for_sending((int)sock);
      debug("IPL4asp__PT_PROVIDER::sendNonBlocking: TITAN returned"
        " to send on connection %d, socket %d", (int)connId, sock);
      if (closingPeerLen > 0) {
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: "
              "postponed close of connection %d", (int)connId);
        ASP__Event event;
        event.connClosed().connId() = connId;
      int ii = connId;

      event.connClosed().remName() = *(sockList[ii].remoteaddr);
      event.connClosed().remPort() = *(sockList[ii].remoteport);
      event.connClosed().locName() = *(sockList[ii].localaddr);
      event.connClosed().locPort() = *(sockList[ii].localport);
        event.connClosed().proto().tcp() = TcpTuple(null_type());
        event.connClosed().userData() = 0;
        int l_ud; getUserData((int)connId, l_ud); event.connClosed().userData() = l_ud;

        switch (type) {
		  case IPL4asp_SSL:
        	event.connClosed().proto().ssl() = SslTuple(null_type());
        	break;
          case IPL4asp_UDP:
            // no such operation in UDP
            break;
          case IPL4asp_TCP_LISTEN:
          case IPL4asp_TCP:
            event.connClosed().proto().tcp() = TcpTuple(null_type());
            break;
          case IPL4asp_SCTP_LISTEN:
          case IPL4asp_SCTP:
            event.connClosed().proto().sctp() =
              SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
          default:
            setResult(result,PortError::ERROR__UNSUPPORTED__PROTOCOL, (int)connId);
            break;
        }
/*        if (getsockname(sock, (struct sockaddr *)&sa, &saLen) == -1) {
          debug("IPL4asp__PT_PROVIDER::sendNonBlocking: getsockname error: %s",
                strerror(errno));
          setResult(result,PortError::ERROR__HOSTNAME, (int)connId, errno);
        } else if (!SetNameAndPort((SockAddr *)&sa, saLen,
                                   event.connClosed().locName(),
                                   event.connClosed().locPort())) {
          debug("IPL4asp__PT_PROVIDER::sendNonBlocking: SetNameAndPort failed");
          setResult(result,PortError::ERROR__HOSTNAME, (int)connId);
        }*/
        if (ConnDel((int)connId) == -1) {
          debug("IPL4asp__PT_PROVIDER::sendNonBlocking: ConnDel failed");
          setResult(result,PortError::ERROR__SOCKET, (int)connId);
        }
/*        if (!SetNameAndPort(&closingPeer, closingPeerLen,
                            event.connClosed().remName(),
                            event.connClosed().remPort())) {
          debug("IPL4asp__PT_PROVIDER::sendNonBlocking: SetNameAndPort failed");
          setResult(result,PortError::ERROR__HOSTNAME, (int)connId);
        }*/
        incoming_message(event);
        closingPeerLen = 0;
      }
      dontCloseConnectionId = -1;
      break;
    case ENOBUFS:
      // try again as in EINTR
      break;
    case EBADF: // invalid file descriptor
      debug("IPL4asp__PT_PROVIDER::sendNonBlocking: "
            "invalid fd: %d", (int)sock);
      setResult(result,PortError::ERROR__INVALID__CONNECTION, (int)connId, EBADF);
      return -1;
    default:
      setResult(result,PortError::ERROR__SOCKET, (int)connId, errno);
      return -1;
    } // switch (errno)
  } // while
  debug("IPL4asp__PT_PROVIDER::sendNonBlocking: leave");
  return sent_octets;
} // IPL4asp__PT::sendNonBlocking


bool IPL4asp__PT_PROVIDER::setOptions(const OptionList& options,
  int sock, const Socket__API__Definitions::ProtoTuple& proto, bool beforeBind)
{
  debug("IPL4asp__PT_PROVIDER::setOptions: enter, number of options: %i",
    options.size_of());
  debug("IPL4asp__PT_PROVIDER::setOptions: sock: %i", sock);
  bool allProto = proto.get_selection() == ProtoTuple::ALT_unspecified;
  bool udpProto = proto.get_selection() == ProtoTuple::ALT_udp || allProto;
  bool tcpProto = proto.get_selection() == ProtoTuple::ALT_tcp || allProto;
  bool sslProto = proto.get_selection() == ProtoTuple::ALT_ssl || allProto;
  bool sctpProto = proto.get_selection() == ProtoTuple::ALT_sctp || allProto;
//  if (options.size_of() > 2 ||
//      (options.size_of() == 2 && options[0].get_selection() == options[1].get_selection())) {
//    debug("IPL4asp__PT_PROVIDER::setOptions: Invalid options");
//    return false;
//  }
  int iR = -1, iK = -1, iM = -1, iS = -1;
  for (int i = 0; i < options.size_of(); ++i) {
    switch (options[i].get_selection()) {
      case Option::ALT_reuseAddress: iR = i; break;
      case Option::ALT_tcpKeepAlive: iK = i; break;
      case Option::ALT_sctpEventHandle: iM = i; break;
      case Option::ALT_sslKeepAlive: iS = i; break;
      default: break;
     }
   }
  /* Setting reuse address */
  int enable = GlobalConnOpts::NOT_SET;
  if (iR != -1) {
    if (!tcpProto && !udpProto && !sctpProto && !sslProto) {
      debug("IPL4asp__PT_PROVIDER::setOptions: Unsupported protocol for reuse address");
      return false;
    }
    enable = GlobalConnOpts::YES;
    if (options[iR].reuseAddress().enable().ispresent() &&
        options[iR].reuseAddress().enable()() == FALSE)
        enable = GlobalConnOpts::NO;
    if (sock == -1) {
      if (tcpProto) globalConnOpts.tcpReuseAddr = enable;
      if (udpProto) globalConnOpts.udpReuseAddr = enable;
      if (sctpProto) globalConnOpts.sctpReuseAddr = enable;
      if (sslProto) globalConnOpts.sctpReuseAddr = enable;
    }
  }
  if (sock != -1 && (iR != -1 || beforeBind)) {
    if (enable == GlobalConnOpts::NOT_SET) {
      if (allProto)
        return false;
      if (tcpProto) enable = globalConnOpts.tcpReuseAddr;
      else if (udpProto) enable = globalConnOpts.udpReuseAddr;
      else if (sctpProto) enable = globalConnOpts.sctpReuseAddr;
      else if (sslProto) enable = globalConnOpts.sslReuseAddr;
    }
    if (enable == GlobalConnOpts::YES) {
      int r = 1;
      if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                     (const char*)&r, sizeof(r)) < 0) {
        debug("IPL4asp__PT_PROVIDER::setOptions: setsockopt REUSEADDR on "
              "socket %d failed: %s", sock, strerror(errno));
        return false;
      }
      debug("IPL4asp__PT_PROVIDER::setOptions: Socket option REUSEADDR on "
            "socket %d is set to: %i", sock, r);
    }
  }
  
  // Set broadcast for UDP   
  if(sock != -1 && udpProto )
  {
    if(broadcast){
    int on=1;
    if( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on) ) < 0 )
      {
        TTCN_error("Setsockopt error: SO_BROADCAST");
      } else {
        debug("IPL4asp__PT_PROVIDER::sendNonBlocking: Socket option SO_BROADCAST on ");
      }
    }
  }  
  
  /* Setting keep alive TCP*/
  if (iK != -1 && !tcpProto) {
    debug("IPL4asp__PT_PROVIDER::setOptions: Unsupported protocol for tcp keep alive");
    return false;
  }
  if (tcpProto) {
    enable = globalConnOpts.tcpKeepAlive;
    int count = globalConnOpts.tcpKeepCnt;
    int idle = globalConnOpts.tcpKeepIdle;
    int interval = globalConnOpts.tcpKeepIntvl;
    if (iK != -1) {
      if (options[iK].tcpKeepAlive().enable().ispresent()) {
        enable = GlobalConnOpts::NO;
        if (options[iK].tcpKeepAlive().enable()() == TRUE)
          enable = GlobalConnOpts::YES;
      }
      if (options[iK].tcpKeepAlive().count().ispresent())
        count = options[iK].tcpKeepAlive().count()();
      if (options[iK].tcpKeepAlive().idle().ispresent())
        idle = options[iK].tcpKeepAlive().idle()();
      if (options[iK].tcpKeepAlive().interval().ispresent())
        interval = options[iK].tcpKeepAlive().interval()();
      if (count < 0 || idle < 0 || interval < 0) {
         debug("IPL4asp__PT_PROVIDER::setOptions: Invalid tcp keep alive parameter");
         return false;
      }
      if (sock == -1) {
        globalConnOpts.tcpKeepAlive = enable;
        globalConnOpts.tcpKeepCnt = count;
        globalConnOpts.tcpKeepIdle = idle;
        globalConnOpts.tcpKeepIntvl = interval;
      }
    }
    if (sock != -1 && (iK != -1 || beforeBind)) {
#ifdef LINUX
      if (count != GlobalConnOpts::NOT_SET) {
        if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT,
                       (const char*)&count, sizeof(count)) < 0) {
          debug("f__IPL4__PROVIDER__connect: setsockopt TCP_KEEPCNT on "
                        "socket %d failed: %s", sock, strerror(errno));
          return false;
        }
        debug("IPL4asp__PT_PROVIDER::setOptions: TCP option TCP_KEEPCNT on "
                        "socket %d is set to: %i", sock, count);
      }
      if (idle != GlobalConnOpts::NOT_SET) {
        if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE,
                       (const char*)&idle, sizeof(idle)) < 0) {
          debug("IPL4asp__PT_PROVIDER::setOptions: setsockopt TCP_KEEPIDLE on "
                        "socket %d failed: %s", sock, strerror(errno));
          return false;
        }
        debug("IPL4asp__PT_PROVIDER::setOptions: TCP option TCP_KEEPIDLE on "
                        "socket %d is set to: %i", sock, idle);
      }
      if (interval != GlobalConnOpts::NOT_SET) {
        if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL,
                       (const char*)&interval, sizeof(interval)) < 0) {
          debug("IPL4asp__PT_PROVIDER::setOptions: setsockopt TCP_KEEPINTVL on "
                        "socket %d failed: %s", sock, strerror(errno));
          return false;
        }
        debug("IPL4asp__PT_PROVIDER::setOptions: TCP option TCP_KEEPINTVL on "
                        "socket %d is set to: %i", sock, interval);
      }
#endif
      if (enable != GlobalConnOpts::NOT_SET) {
        int r = (enable == GlobalConnOpts::YES) ? 1 : 0;
        if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE,
                       (const char*)&r, sizeof(r)) < 0) {
          debug("IPL4asp__PT_PROVIDER::setOptions: setsockopt SO_KEEPALIVE on "
                        "socket %d failed: %s", sock, strerror(errno));
          return false;
        }
        debug("IPL4asp__PT_PROVIDER::setOptions: socket option SO_KEEPALIVE on "
                        "socket %d is set to: %i", sock, r);
      }
    }
  }

#ifdef IPL4_USE_SSL
    /* Setting keep alive SSL*/
  if (iS != -1 && !sslProto) {
    debug("IPL4asp__PT_PROVIDER::setOptions: Unsupported protocol for ssl keep alive");
    return false;
  }
  if (sslProto) {
    enable = globalConnOpts.sslKeepAlive;
    int count = globalConnOpts.sslKeepCnt;
    int idle = globalConnOpts.sslKeepIdle;
    int interval = globalConnOpts.sslKeepIntvl;
    if (iS != -1) {
      if (options[iS].sslKeepAlive().enable().ispresent()) {
        enable = GlobalConnOpts::NO;
        if (options[iS].sslKeepAlive().enable()() == TRUE)
          enable = GlobalConnOpts::YES;
      }
      if (options[iS].sslKeepAlive().count().ispresent())
        count = options[iS].sslKeepAlive().count()();
      if (options[iS].sslKeepAlive().idle().ispresent())
        idle = options[iS].sslKeepAlive().idle()();
      if (options[iS].sslKeepAlive().interval().ispresent())
        interval = options[iS].sslKeepAlive().interval()();
      if (count < 0 || idle < 0 || interval < 0) {
         debug("IPL4asp__PT_PROVIDER::setOptions: Invalid ssl keep alive parameter");
         return false;
      }
      if (sock == -1) {
        globalConnOpts.sslKeepAlive = enable;
        globalConnOpts.sslKeepCnt = count;
        globalConnOpts.sslKeepIdle = idle;
        globalConnOpts.sslKeepIntvl = interval;
      }
    }
    if (sock != -1 && (iS != -1 || beforeBind)) {
#ifdef LINUX
      if (count != GlobalConnOpts::NOT_SET) {
        if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT,
                       (const char*)&count, sizeof(count)) < 0) {
          debug("f__IPL4__PROVIDER__connect: setsockopt TCP_KEEPCNT on "
                        "socket %d failed: %s", sock, strerror(errno));
          return false;
        }
        debug("IPL4asp__PT_PROVIDER::setOptions: TCP option TCP_KEEPCNT on "
                        "socket %d is set to: %i", sock, count);
      }
      if (idle != GlobalConnOpts::NOT_SET) {
        if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE,
                       (const char*)&idle, sizeof(idle)) < 0) {
          debug("IPL4asp__PT_PROVIDER::setOptions: setsockopt TCP_KEEPIDLE on "
                        "socket %d failed: %s", sock, strerror(errno));
          return false;
        }
        debug("IPL4asp__PT_PROVIDER::setOptions: TCP option TCP_KEEPIDLE on "
                        "socket %d is set to: %i", sock, idle);
      }
      if (interval != GlobalConnOpts::NOT_SET) {
        if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL,
                       (const char*)&interval, sizeof(interval)) < 0) {
          debug("IPL4asp__PT_PROVIDER::setOptions: setsockopt TCP_KEEPINTVL on "
                        "socket %d failed: %s", sock, strerror(errno));
          return false;
        }
        debug("IPL4asp__PT_PROVIDER::setOptions: TCP option TCP_KEEPINTVL on "
                        "socket %d is set to: %i", sock, interval);
      }
#endif
      if (enable != GlobalConnOpts::NOT_SET) {
        int r = (enable == GlobalConnOpts::YES) ? 1 : 0;
        if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE,
                       (const char*)&r, sizeof(r)) < 0) {
          debug("IPL4asp__PT_PROVIDER::setOptions: setsockopt SO_KEEPALIVE on "
                        "socket %d failed: %s", sock, strerror(errno));
          return false;
        }
        debug("IPL4asp__PT_PROVIDER::setOptions: socket option SO_KEEPALIVE on "
                        "socket %d is set to: %i", sock, r);
      }
    }
  }
#endif

  /* Setting sctp events & inits */
  if (iM != -1 && !sctpProto) {
    debug("IPL4asp__PT_PROVIDER::setOptions: Unsupported protocol for sctp events");
    return false;
  }
  if (sctpProto) {
#ifdef USE_SCTP
      debug("IPL4asp__PT_PROVIDER::setOptions: Setting sctp options sinit_num_ostreams:%d,"
      			"sinit_max_instreams:%d, sinit_max_attempts:%d, sinit_max_init_timeo:%d",
			(int) globalConnOpts.sinit_num_ostreams, (int) globalConnOpts.sinit_max_instreams,
			(int) globalConnOpts.sinit_max_attempts, (int) globalConnOpts.sinit_max_init_timeo);
      struct sctp_initmsg initmsg;
      (void) memset(&initmsg, 0, sizeof(struct sctp_initmsg));
      initmsg.sinit_num_ostreams = (int) globalConnOpts.sinit_num_ostreams;
      initmsg.sinit_max_instreams = (int) globalConnOpts.sinit_max_instreams;
      initmsg.sinit_max_attempts = (int) globalConnOpts.sinit_max_attempts;
      initmsg.sinit_max_init_timeo = (int) globalConnOpts.sinit_max_init_timeo;

      if(sock!=-1){
        if (setsockopt(sock, IPPROTO_SCTP, SCTP_INITMSG, &initmsg,
          sizeof(struct sctp_initmsg)) < 0)
        {
           debug("IPL4asp__PT_PROVIDER::setOptions: setsockopt: SCTP init on "
                          "socket %d failed: %s", sock, strerror(errno));
			  }
      }
      if (iM != -1) {
        if (options[iM].sctpEventHandle().sctp__data__io__event().ispresent()) {
          globalConnOpts.sctp_data_io_event = GlobalConnOpts::NO;
        if (options[iM].sctpEventHandle().sctp__data__io__event()() == TRUE)
          globalConnOpts.sctp_data_io_event = GlobalConnOpts::YES;
        }
        if (options[iM].sctpEventHandle().sctp__association__event().ispresent()) {
          globalConnOpts.sctp_association_event = GlobalConnOpts::NO;
        if (options[iM].sctpEventHandle().sctp__association__event()() == TRUE)
          globalConnOpts.sctp_association_event = GlobalConnOpts::YES;
        }
        if (options[iM].sctpEventHandle().sctp__address__event().ispresent()) {
          globalConnOpts.sctp_address_event = GlobalConnOpts::NO;
        if (options[iM].sctpEventHandle().sctp__address__event()() == TRUE)
          globalConnOpts.sctp_address_event = GlobalConnOpts::YES;
        }
        if (options[iM].sctpEventHandle().sctp__send__failure__event().ispresent()) {
          globalConnOpts.sctp_send_failure_event = GlobalConnOpts::NO;
        if (options[iM].sctpEventHandle().sctp__send__failure__event()() == TRUE)
          globalConnOpts.sctp_send_failure_event = GlobalConnOpts::YES;
        }
        if (options[iM].sctpEventHandle().sctp__peer__error__event().ispresent()) {
          globalConnOpts.sctp_peer_error_event = GlobalConnOpts::NO;
        if (options[iM].sctpEventHandle().sctp__peer__error__event()() == TRUE)
          globalConnOpts.sctp_peer_error_event = GlobalConnOpts::YES;
        }
        if (options[iM].sctpEventHandle().sctp__shutdown__event().ispresent()) {
          globalConnOpts.sctp_shutdown_event = GlobalConnOpts::NO;
        if (options[iM].sctpEventHandle().sctp__shutdown__event()() == TRUE)
          globalConnOpts.sctp_shutdown_event = GlobalConnOpts::YES;
        }
        if (options[iM].sctpEventHandle().sctp__partial__delivery__event().ispresent()) {
          globalConnOpts.sctp_partial_delivery_event = GlobalConnOpts::NO;
        if (options[iM].sctpEventHandle().sctp__partial__delivery__event()() == TRUE)
          globalConnOpts.sctp_partial_delivery_event = GlobalConnOpts::YES;
        }
        if (options[iM].sctpEventHandle().sctp__adaptation__layer__event().ispresent()) {
          globalConnOpts.sctp_adaptation_layer_event = GlobalConnOpts::NO;
        if (options[iM].sctpEventHandle().sctp__adaptation__layer__event()() == TRUE)
          globalConnOpts.sctp_adaptation_layer_event = GlobalConnOpts::YES;
        }
	if (options[iM].sctpEventHandle().sctp__authentication__event().ispresent()) {
          globalConnOpts.sctp_authentication_event = GlobalConnOpts::NO;
        if (options[iM].sctpEventHandle().sctp__authentication__event()() == TRUE)
          globalConnOpts.sctp_authentication_event = GlobalConnOpts::YES;
        }
      }

      events.sctp_association_event = (boolean) globalConnOpts.sctp_association_event;
      events.sctp_address_event = (boolean) globalConnOpts.sctp_address_event;
      events.sctp_send_failure_event = (boolean) globalConnOpts.sctp_send_failure_event;
      events.sctp_peer_error_event = (boolean) globalConnOpts.sctp_peer_error_event;
      events.sctp_shutdown_event = (boolean) globalConnOpts.sctp_shutdown_event;
      events.sctp_partial_delivery_event = (boolean) globalConnOpts.sctp_partial_delivery_event;
#ifdef LKSCTP_1_0_7
      events.sctp_adaptation_layer_event = (boolean) globalConnOpts.sctp_adaptation_layer_event;
#elif defined LKSCTP_1_0_9
      events.sctp_adaptation_layer_event = (boolean) globalConnOpts.sctp_adaptation_layer_event;
      events.sctp_authentication_event = (boolean) globalConnOpts.sctp_authentication_event;
#else
      events.sctp_adaption_layer_event = (boolean) globalConnOpts.sctp_adaptation_layer_event;
#endif
      if(sock!=-1){
        if (setsockopt(sock, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof (events)) < 0)
        {
         TTCN_warning("Setsockopt error!");
          errno = 0;
        }
      }
#endif
  }
  debug("IPL4asp__PT_PROVIDER::setOptions: leave");
  return true;
}


int IPL4asp__PT_PROVIDER::ConnAdd(SockType type, int sock, int parentIdx)
{
  debug("IPL4asp__PT_PROVIDER: enter: sock: %d, parentIx: %d",
    sock, parentIdx);
  testIfInitialized();
  if (sockListCnt + N_RECENTLY_CLOSED >= sockListSize - 1 || sockList == NULL) {
    unsigned int sz = sockListSize;
    if (sockList != NULL) sz *= 2;
    SockDesc *newSockList =
      (SockDesc *)Realloc(sockList, sizeof(SockDesc) * sz);
    int i0 = (sockList == 0) ? 1 : sockListSize;
    sockList = newSockList;
    sockListSize = sz;
    debug("IPL4asp__PT_PROVIDER::ConnAdd: new sockListSize: %d", sockListSize);
    int j = firstFreeSock;
    for ( int i = sockListSize - 1; i >= i0; --i ) {
      memset(sockList + i, 0, sizeof (sockList[i]));
      sockList[i].sock = SockDesc::SOCK_NONEX;
      sockList[i].nextFree = j;
      j = i;
    }
    firstFreeSock = j;
    if (lastFreeSock == -1) lastFreeSock = sockListSize - 1;
  }

  int i = firstFreeSock;
  debug("IPL4asp__PT_PROVIDER::ConnAdd: connId: %d", i);

  if (parentIdx != -1) { // inherit the listener's properties
    sockList[i].userData = sockList[parentIdx].userData;
    sockList[i].getMsgLen = sockList[parentIdx].getMsgLen;
    sockList[i].parentIdx = parentIdx;
    sockList[i].msgLenArgs =
      new ro__integer(*sockList[parentIdx].msgLenArgs);
  } else { // otherwise initialize to defaults
    sockList[i].userData = 0;
    sockList[i].getMsgLen = defaultGetMsgLen;
    sockList[i].parentIdx = -1;
    sockList[i].msgLenArgs = new ro__integer(*defaultMsgLenArgs);
  }
  if (sockList[i].msgLenArgs == NULL)
    return -1;
  sockList[i].msgLen = -1;

  fd2IndexMap[sock] = i;
  sockList[i].type = type;
  sockList[i].localaddr=new CHARSTRING("");
  sockList[i].localport=new PortNumber(-1);
  sockList[i].remoteaddr=new CHARSTRING("");
  sockList[i].remoteport=new PortNumber(-1);

// Set local socket details
  SockAddr sa;
  socklen_t saLen = sizeof(SockAddr);
  if (getsockname(sock,(struct sockaddr *)&sa, &saLen) == -1) {
    debug("IPL4asp__PT_PROVIDER::ConnAdd: getsockname error: %s",
          strerror(errno));
    return -1;
  } else if (!SetNameAndPort(&sa, saLen,
             *(sockList[i].localaddr), *(sockList[i].localport))) {
    debug("IPL4asp__PT_PROVIDER::ConnAdd: SetNameAndPort failed");
    return -1;
  }



  switch (type) {
  case IPL4asp_UDP:
  case IPL4asp_TCP_LISTEN:
  case IPL4asp_SCTP_LISTEN:
    sockList[i].buf = NULL;
    sockList[i].assocIdList = NULL;
    sockList[i].cnt = 0;
    break;
  case IPL4asp_SSL_LISTEN:
    sockList[i].buf = NULL;
    sockList[i].assocIdList = NULL;
    sockList[i].cnt = 0;
    sockList[i].sslState = STATE_NORMAL;
    break;
  case IPL4asp_TCP:
      sockList[i].buf = (TTCN_Buffer **)Malloc(sizeof(TTCN_Buffer *));
    *sockList[i].buf = new TTCN_Buffer;
    if (*sockList[i].buf == NULL) {
      debug("IPL4asp__PT_PROVIDER::ConnAdd: failed to add socket %d", sock);
      Free(sockList[i].buf); sockList[i].buf = 0;
      return -1;
    }
    sockList[i].assocIdList = NULL;
    sockList[i].cnt = 1;
    if (getpeername(sock, (struct sockaddr *)&sa, &saLen) == -1) {
      debug("IPL4asp__PT_PROVIDER::ConnAdd: getpeername failed: %s", strerror(errno));
//      sendError(PortError::ERROR__HOSTNAME, i, errno);
    } else if (!SetNameAndPort(&sa, saLen, *(sockList[i].remoteaddr), *(sockList[i].remoteport))) {
      debug("IPL4asp__PT_PROVIDER::ConnAdd: SetNameAndPort failed");
      sendError(PortError::ERROR__HOSTNAME, i);
    }
    break;
  case IPL4asp_SSL:
    sockList[i].buf = (TTCN_Buffer **)Malloc(sizeof(TTCN_Buffer *));
    *sockList[i].buf = new TTCN_Buffer;
    if (*sockList[i].buf == NULL) {
      debug("IPL4asp__PT_PROVIDER::ConnAdd: failed to add socket %d", sock);
      Free(sockList[i].buf); sockList[i].buf = 0;
      return -1;
    }
    sockList[i].assocIdList = NULL;
    sockList[i].cnt = 1;
    sockList[i].sslState = STATE_CONNECTING;
#ifdef IPL4_USE_SSL
    sockList[i].sslObj = NULL;
#endif
    if (getpeername(sock, (struct sockaddr *)&sa, &saLen) == -1) {
      debug("IPL4asp__PT_PROVIDER::ConnAdd: getpeername failed: %s", strerror(errno));
//      sendError(PortError::ERROR__HOSTNAME, i, errno);
    } else if (!SetNameAndPort(&sa, saLen,*(sockList[i].remoteaddr), *(sockList[i].remoteport))) {
      debug("IPL4asp__PT_PROVIDER::ConnAdd: SetNameAndPort failed");
      sendError(PortError::ERROR__HOSTNAME, i);
    }
    break;
  case IPL4asp_SCTP:
    sockList[i].buf = (TTCN_Buffer **)Malloc(sizeof(TTCN_Buffer *));
    *sockList[i].buf = new TTCN_Buffer;
    if (*sockList[i].buf == NULL) {
      debug("IPL4asp__PT_PROVIDER::ConnAdd: failed to add socket %d", sock);
      Free(sockList[i].buf); sockList[i].buf = 0;
      return -1;
    }
    sockList[i].assocIdList = (sctp_assoc_t *)Malloc(sizeof(sctp_assoc_t));
    sockList[i].cnt = 1;
    if (getpeername(sock, (struct sockaddr *)&sa, &saLen) == -1) {
      debug("IPL4asp__PT_PROVIDER::ConnAdd: getpeername failed: %s", strerror(errno));
//      sendError(PortError::ERROR__HOSTNAME, i, errno);
    } else if (!SetNameAndPort(&sa, saLen, *(sockList[i].remoteaddr), *(sockList[i].remoteport))) {
      debug("IPL4asp__PT_PROVIDER::ConnAdd: SetNameAndPort failed");
      sendError(PortError::ERROR__HOSTNAME, i);
    }
    break;
  }
  Handler_Add_Fd_Read(sock);

  sockList[i].sock = sock;
  firstFreeSock = sockList[i].nextFree;
  sockList[i].nextFree = -1;
  ++sockListCnt;
  if(sockListCnt==1) {lonely_conn_id=i;}
  else {lonely_conn_id=-1;}
  debug("IPL4asp__PT_PROVIDER::ConnAdd: leave: sockListCnt: %i", sockListCnt);

  return i;
} // IPL4asp__PT::ConnAdd

void SockDesc::clear()
{
  for (unsigned int i = 0; i < cnt; ++i) delete buf[i];
  cnt = 0;
  if (buf != 0) { Free(buf); buf = 0; }
  if (assocIdList != 0) { Free(assocIdList); assocIdList = 0; }
  if (msgLenArgs != 0) { delete msgLenArgs; msgLenArgs = 0; }
  if (localaddr != 0) { delete localaddr; localaddr = 0; }
  if (localport != 0) { delete localport; localport = 0; }
  if (remoteaddr != 0) { delete remoteaddr; remoteaddr = 0; }
  if (remoteport != 0) { delete remoteport; remoteport = 0; }

  sock = SOCK_NONEX;
  msgLen = -1;
  nextFree = -1;
}

int IPL4asp__PT_PROVIDER::ConnDel(int connId)
{
  debug("IPL4asp__PT_PROVIDER::ConnDel: enter: connId: %d", connId);
  int sock = sockList[connId].sock;
  debug("IPL4asp__PT_PROVIDER::ConnDel: fd: %d", sock);
  if (sock <= 0)
    return -1;
  Handler_Remove_Fd(sock, EVENT_ALL);

#ifdef IPL4_USE_SSL
  if((sockList[connId].type == IPL4asp_SSL) && mapped) perform_ssl_shutdown(connId);
#endif

  if (close(sock) == -1) {
    TTCN_warning("IPL4asp__PT_PROVIDER::ConnDel: failed to close socket"
      " %d: %s, connId: %d", sock, strerror(errno), connId);
  }

  fd2IndexMap.erase(sockList[connId].sock);

  sockList[connId].clear();
  sockList[lastFreeSock].nextFree = connId;
  lastFreeSock = connId;
  sockListCnt--;

  return connId;
} // IPL4asp__PT_PROVIDER::ConnDel


int IPL4asp__PT_PROVIDER::setUserData(int connId, int userData)
{
  debug("IPL4asp__PT_PROVIDER::setUserData enter: connId %d userdata %d",
        connId, userData);
  testIfInitialized();
  if (!isConnIdValid(connId)) {
    debug("IPL4asp__PT_PROVIDER::setUserData: invalid connId: %i", connId);
    return -1;
  }
  sockList[connId].userData = userData;
  return connId;
} // IPL4asp__PT_PROVIDER::setUserData



int IPL4asp__PT_PROVIDER::getUserData(int connId, int& userData)
{
  debug("IPL4asp__PT_PROVIDER::getUserData enter: socket %d", connId);
  testIfInitialized();
  if (!isConnIdValid(connId)) {
    debug("IPL4asp__PT_PROVIDER::getUserData: invalid connId: %i", connId);
    return -1;
  }
  userData = sockList[connId].userData;
  return connId;
} // IPL4asp__PT_PROVIDER::getUserData



int IPL4asp__PT_PROVIDER::getConnectionDetails(int connId, IPL4__Param IPL4param, IPL4__ParamResult& IPL4paramResult)
{
  debug("IPL4asp__PT_PROVIDER::getConnectionDetails enter: socket %d", connId);
  testIfInitialized();
  if (!isConnIdValid(connId)) {
    debug("IPL4asp__PT_PROVIDER::getConnectionDetails: invalid connId: %i", connId);
    return -1;
  }
  SockAddr sa;
  socklen_t saLen = sizeof(SockAddr);
  SockType type;
  switch (IPL4param) {
    case IPL4__Param::IPL4__LOCALADDRESS:
      if (getsockname(sockList[connId].sock,
          (struct sockaddr *)&sa, &saLen) == -1) {
        IPL4paramResult.local().hostName() = "?";
        IPL4paramResult.local().portNumber() = -1;
        debug("IPL4asp__PT_PROVIDER::getConnectionDetails: getsockname error: %s",
              strerror(errno));
        return -1;
      }
      if (!SetNameAndPort(&sa, saLen,
                            IPL4paramResult.local().hostName(),
                            IPL4paramResult.local().portNumber())) {
        debug("IPL4asp__PT_PROVIDER::getConnectionDetails: SetNameAndPort failed");
        return -1;
      }
      break;
    case IPL4__Param::IPL4__REMOTEADDRESS:
      if (getpeername(sockList[connId].sock,
          (struct sockaddr *)&sa, &saLen) == -1) {
        IPL4paramResult.remote().hostName() = "?";
        IPL4paramResult.remote().portNumber() = -1;
        debug("IPL4asp__PT_PROVIDER::getConnectionDetails: getpeername error: %s",
              strerror(errno));
        return -1;
      }
      if (!SetNameAndPort(&sa, saLen,
                            IPL4paramResult.remote().hostName(),
                            IPL4paramResult.remote().portNumber())) {
        debug("IPL4asp__PT_PROVIDER::getConnectionDetails: SetNameAndPort failed");
        return -1;
      }
      break;
    case IPL4__Param::IPL4__PROTO:
      type = sockList[connId].type;
      switch (type) {
        case IPL4asp_UDP:
          IPL4paramResult.proto().udp() = UdpTuple(null_type());
          break;
        case IPL4asp_TCP_LISTEN:
        case IPL4asp_TCP:
          IPL4paramResult.proto().tcp() = TcpTuple(null_type());
          break;
        case IPL4asp_SCTP_LISTEN:
        case IPL4asp_SCTP:
          IPL4paramResult.proto().sctp() = SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
          break;
        case IPL4asp_SSL_LISTEN:
        case IPL4asp_SSL:
          IPL4paramResult.proto().ssl() = SslTuple(null_type());
          break;
        default: break;
      }
      break;
    case IPL4__Param::IPL4__USERDATA:
      IPL4paramResult.userData() = sockList[connId].userData;
      break;
    case IPL4__Param::IPL4__PARENTIDX:
      IPL4paramResult.parentIdx() = sockList[connId].parentIdx;
      break;
    default: break;
  }

  return connId;
} // IPL4asp__PT_PROVIDER::getConnectionDetails


void IPL4asp__PT_PROVIDER::sendError(PortError code, const ConnectionId& id,
  int os_error_code)
{
  ASP__Event event;
  event.result().errorCode() = code;
  event.result().connId() = id;
  if (os_error_code != 0) {
    event.result().os__error__code() = os_error_code;
    event.result().os__error__text() = strerror(os_error_code);
  } else {
    event.result().os__error__code() = OMIT_VALUE;
    event.result().os__error__text() = OMIT_VALUE;
  }
  incoming_message(event);
} // IPL4asp__PT_PROVIDER::sendError

void IPL4asp__PT_PROVIDER::setResult(Socket__API__Definitions::Result& result, PortError code, const ConnectionId& id,
  int os_error_code)
{
  result.errorCode() = code;
  result.connId() = id;
  if (os_error_code != 0) {
    result.os__error__code() = os_error_code;
    result.os__error__text() = strerror(os_error_code);
  } else {
    result.os__error__code() = OMIT_VALUE;
    result.os__error__text() = OMIT_VALUE;
  }
} // IPL4asp__PT_PROVIDER::setResult


void f__IPL4__PROVIDER__setGetMsgLen(IPL4asp__PT_PROVIDER& portRef,
                  const ConnectionId& connId, f__IPL4__getMsgLen& f,
                  const ro__integer& msgLenArgs)
{
  portRef.testIfInitialized();
  if ((int)connId == -1) {
    portRef.defaultGetMsgLen = f;
    delete portRef.defaultMsgLenArgs;
    portRef.defaultMsgLenArgs = new Socket__API__Definitions::ro__integer(msgLenArgs);
    portRef.debug("f__IPL4__PROVIDER__setGetMsgLen: "
                  "The default getMsgLen fn is modified");
  } else {
    if (!portRef.isConnIdValid(connId)) {
      portRef.debug("IPL4asp__PT_PROVIDER::f__IPL4__PROVIDER__setGetMsgLen: "
                    "invalid connId: %i", (int)connId);
      return;
    }
    portRef.debug("f__IPL4__PROVIDER__setGetMsgLen: "
                  "getMsgLen fn for connection %d is modified", (int)connId);
    portRef.sockList[(int)connId].getMsgLen = f;
    delete portRef.sockList[(int)connId].msgLenArgs;
    portRef.sockList[(int)connId].msgLenArgs = new Socket__API__Definitions::ro__integer(msgLenArgs);
  }
} // f__IPL4__PROVIDER__setGetMsgLen



Result f__IPL4__PROVIDER__listen(IPL4asp__PT_PROVIDER& portRef, const HostName& locName,
  const PortNumber& locPort, const ProtoTuple& proto, const OptionList& options)
{
  Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
  SockAddr sockAddr;
  socklen_t sockAddrLen;
  //struct hostent *hp = NULL;
  int hp = 0;

  portRef.debug("f__IPL4__PROVIDER__listen: enter %s %d", (const char *) locName, (int) locPort);
  portRef.testIfInitialized();

  //check if all mandatory SSL config params are present for a listening socket
#ifdef IPL4_USE_SSL
  if(proto.get_selection() == ProtoTuple::ALT_ssl)
  {
	  if (portRef.ssl_certificate_file==NULL)
	    {
	  	  portRef.debug("%s is not defined in the configuration file", portRef.ssl_certificate_file_name());
	  	  RETURN_ERROR(ERROR__GENERAL);
	    }
	    if (portRef.ssl_trustedCAlist_file==NULL)
	    {
	  	  portRef.debug("%s is not defined in the configuration file", portRef.ssl_trustedCAlist_file_name());
	  	  RETURN_ERROR(ERROR__GENERAL);
	    }
	    if (portRef.ssl_key_file==NULL)
	    {
	  	  portRef.debug("%s is not defined in the configuration file", portRef.ssl_private_key_file_name());
	  	  RETURN_ERROR(ERROR__GENERAL);
	    }
  }
#endif

  if (locPort < -1 || locPort > 65535)
    RETURN_ERROR(ERROR__INVALID__INPUT__PARAMETER);

  hp=SetLocalSockAddr("f__IPL4__PROVIDER__listen",portRef,AF_INET,locName, locPort, sockAddr, sockAddrLen);
//  if (locPort != -1 && locName != "")
//    hp = SetSockAddr(locName, locPort, sockAddr, sockAddrLen);
//  else if (locPort == -1 && locName == "") { // use default host and port
//    portRef.debug("f__IPL4__PROVIDER__listen: use defaults: %s:%d",
//      portRef.defaultLocHost, portRef.defaultLocPort);
//    hp = SetSockAddr(HostName(portRef.defaultLocHost),
//            portRef.defaultLocPort, sockAddr, sockAddrLen);
//  } else if (locPort == -1) { // use default port
//    portRef.debug("f__IPL4__PROVIDER__listen: use default port: %s:%d",
//      (const char *)locName, portRef.defaultLocPort);
//    hp = SetSockAddr(locName, portRef.defaultLocPort,
//            sockAddr, sockAddrLen);
//  } else { // use default host
//    portRef.debug("f__IPL4__PROVIDER__listen: use default host: %s:%d",
//      portRef.defaultLocHost, (int)locPort);
//    hp = SetSockAddr(HostName(portRef.defaultLocHost),
//                     locPort, sockAddr, sockAddrLen);
//  }

  if (hp == -1) {
    SET_OS_ERROR_CODE;
    RETURN_ERROR(ERROR__HOSTNAME);
  }
#ifdef LKSCTP_MULTIHOMING_ENABLED
      int addr_index=-1;
      int num_of_addr=0;
      unsigned char* sarray=NULL;
#ifdef USE_IPL4_EIN_SCTP
    if(portRef.native_stack){
#endif
      for(int i=0; i<options.size_of();i++){
        if(options[i].get_selection()==Option::ALT_sctpAdditionalLocalAddresses){
          addr_index=i;
          num_of_addr=options[i].sctpAdditionalLocalAddresses().size_of();
          break;
        }
      }
      if(num_of_addr){
        sarray=(unsigned char*)Malloc(num_of_addr*
#ifdef USE_IPV6
          sizeof(struct sockaddr_in6)
#else 
          sizeof(struct sockaddr_in)
#endif
        );
//          SockAddr saLoc2;
//          socklen_t saLoc2Len;
        int used_bytes=0;
portRef.debug("f__IPL4__PROVIDER__listen: addr family main: %s ",hp==-1?"Error":hp==AF_INET?"AF_INET":"AF_INET6");
        int final_hp=hp;
        for(int i=0; i<num_of_addr;i++){
          SockAddr saLoc2;
          socklen_t saLoc2Len;
          int hp3 = SetLocalSockAddr("f__IPL4__PROVIDER__connect",portRef,hp,options[addr_index].sctpAdditionalLocalAddresses()[i], locPort, saLoc2, saLoc2Len);
portRef.debug("f__IPL4__PROVIDER__listen: addr added Family: %s ",hp3==-1?"Error":hp3==AF_INET?"AF_INET":"AF_INET6");
          if (hp3 == -1) {
            SET_OS_ERROR_CODE;
            Free(sarray);
            RETURN_ERROR(ERROR__HOSTNAME);
          }
          if(hp3==AF_INET){
            memcpy (sarray + used_bytes, &saLoc2, sizeof (struct sockaddr_in));
            used_bytes += sizeof (struct sockaddr_in);          
          }
#ifdef USE_IPV6
          else{
            final_hp=hp3;
            memcpy (sarray + used_bytes, &saLoc2, sizeof (struct sockaddr_in6));
            used_bytes += sizeof (struct sockaddr_in6);          
          }
#endif
          
        }
        hp=final_hp;
portRef.debug("f__IPL4__PROVIDER__listen: addr family final: %s ",hp==-1?"Error":hp==AF_INET?"AF_INET":"AF_INET6");
      }
#ifdef USE_IPL4_EIN_SCTP
    }
#endif
#endif

  // create socket based on the transport protocol
  int fd = -1;
  switch (proto.get_selection()) {
  case ProtoTuple::ALT_udp:
    fd = socket(hp, SOCK_DGRAM, 0);
    break;
#ifdef IPL4_USE_SSL
  case ProtoTuple::ALT_ssl:
#endif
  case ProtoTuple::ALT_tcp:
    fd = socket(hp, SOCK_STREAM, 0);
    break;
  case ProtoTuple::ALT_sctp:
#ifdef USE_IPL4_EIN_SCTP
    if(!portRef.native_stack){
      break;
    }
#endif
#ifdef USE_SCTP
    fd = socket(hp, SOCK_STREAM, IPPROTO_SCTP);
    break;
#endif
  default: case ProtoTuple::UNBOUND_VALUE:
    RETURN_ERROR(ERROR__UNSUPPORTED__PROTOCOL);
  }
#ifdef USE_IPL4_EIN_SCTP
    if(portRef.native_stack || proto.get_selection() != ProtoTuple::ALT_sctp){
#endif
  if (fd == -1) {
    portRef.debug("f__IPL4__PROVIDER__listen: failed to create new socket");
#ifdef LKSCTP_MULTIHOMING_ENABLED
        if(sarray) {Free(sarray);}
#endif
    SET_OS_ERROR_CODE;
    RETURN_ERROR(ERROR__SOCKET);
  }

  // set socket properties...
  if (!portRef.setOptions(options, fd, proto, true)) {
    portRef.debug("f__IPL4__PROVIDER__listen: Setting options on "
                  "socket %d failed: %s", fd, strerror(errno));
    SET_OS_ERROR_CODE;
    close(fd);
#ifdef LKSCTP_MULTIHOMING_ENABLED
        if(sarray) {Free(sarray);}
#endif
    RETURN_ERROR(ERROR__SOCKET);
  }
  if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
    portRef.debug("f__IPL4__PROVIDER__listen: fcntl O_NONBLOCK on "
                  "socket %d failed: %s", fd, strerror(errno));
    SET_OS_ERROR_CODE;
    close(fd);
#ifdef LKSCTP_MULTIHOMING_ENABLED
        if(sarray) {Free(sarray);}
#endif
    RETURN_ERROR(ERROR__SOCKET);
  }
#ifdef USE_IPL4_EIN_SCTP
    }
#endif

  // bind and listen
  int connId = -1;
  switch (proto.get_selection()) {
  case ProtoTuple::ALT_udp:
    if (bind(fd, (struct sockaddr*)&sockAddr, sockAddrLen) == -1) {
      portRef.debug("f__IPL4__PROVIDER__listen: bind on socket %d failed: %s",
                    fd, strerror(errno));
      SET_OS_ERROR_CODE;
      close(fd);
      RETURN_ERROR(ERROR__SOCKET);
    }
    connId = portRef.ConnAdd(IPL4asp_UDP, fd);
    if (connId == -1)
      RETURN_ERROR(ERROR__INSUFFICIENT__MEMORY);
    result.connId()() = connId;
    break;
#ifdef IPL4_USE_SSL
  case ProtoTuple::ALT_ssl:
#endif
  case ProtoTuple::ALT_tcp:
    if (bind(fd, (struct sockaddr*)&sockAddr, sockAddrLen) == -1) {
      portRef.debug("f__IPL4__PROVIDER__listen: bind on socket %d failed: %s",fd,
                    strerror(errno));
      SET_OS_ERROR_CODE;
      close(fd);
      RETURN_ERROR(ERROR__SOCKET);
    }
    if (listen(fd, portRef.backlog) == -1) {
      portRef.debug("f__IPL4__PROVIDER__listen: "
                    "listen on socket %d failed: %s", fd, strerror(errno));
      SET_OS_ERROR_CODE;
      close(fd);
      RETURN_ERROR(ERROR__SOCKET);
    }
    connId = portRef.ConnAdd((proto.get_selection() == ProtoTuple::ALT_tcp ? IPL4asp_TCP_LISTEN : IPL4asp_SSL_LISTEN), fd);
    if (connId == -1)
      RETURN_ERROR(ERROR__INSUFFICIENT__MEMORY);
    result.connId()() = connId;
    break;
  case ProtoTuple::ALT_sctp:
  {
#ifdef USE_IPL4_EIN_SCTP
    if(!portRef.native_stack){
      result=portRef.Listen_einsctp(locName, locPort,SockDesc::ACTION_NONE,"",-1,options,IPL4asp__Types::SocketList(NULL_VALUE));
      break;
    }
#endif
    
#ifdef USE_SCTP
    if (bind(fd, (struct sockaddr*)&sockAddr, sockAddrLen) == -1) {
      portRef.debug("f__IPL4__PROVIDER__listen: bind on socket %d failed: %s",fd,
                    strerror(errno));
      SET_OS_ERROR_CODE;
      close(fd);
#ifdef LKSCTP_MULTIHOMING_ENABLED
        if(sarray) {Free(sarray);}
#endif
      RETURN_ERROR(ERROR__SOCKET);
    }
#ifdef LKSCTP_MULTIHOMING_ENABLED
      if(num_of_addr){
          portRef.debug("f__IPL4__PROVIDER__connect: "
                        "sctp_bindx on socket %d num_addr: %d", fd, num_of_addr);
        
        if (sctp_bindx(fd, (struct sockaddr*)sarray, num_of_addr,  SCTP_BINDX_ADD_ADDR ) == -1) {
          portRef.debug("f__IPL4__PROVIDER__connect: "
                        "sctp_bindx on socket %d failed: %s", fd, strerror(errno));
          SET_OS_ERROR_CODE;
          Free(sarray);
          close(fd);
          RETURN_ERROR(ERROR__SOCKET);
        }
        Free(sarray);
       
        
      }
#endif
    if (listen(fd, portRef.backlog) == -1) {
      portRef.debug("f__IPL4__PROVIDER__listen: "
                    "listen on socket %d failed: %s", fd, strerror(errno));
      SET_OS_ERROR_CODE;
      close(fd);
      RETURN_ERROR(ERROR__SOCKET);
    }
    connId = portRef.ConnAdd(IPL4asp_SCTP_LISTEN, fd);
    if (connId == -1)
      RETURN_ERROR(ERROR__INSUFFICIENT__MEMORY);
    result.connId()() = connId;
    break;
#endif
  }
  default: case ProtoTuple::UNBOUND_VALUE:
    RETURN_ERROR(ERROR__UNSUPPORTED__PROTOCOL);
  } // switch(proto.get_selection())

  portRef.debug("f__IPL4__PROVIDER__listen: leave: "
                "socket created, connection ID: %d, fd: %d", connId, fd);
  if(portRef.globalConnOpts.extendedPortEvents == GlobalConnOpts::YES) {
    ASP__Event event;
    event.result() = result;
    portRef.incoming_message(event);
  }
  return result;
} // f__IPL4__PROVIDER__listen



Result f__IPL4__PROVIDER__connect(IPL4asp__PT_PROVIDER& portRef, const HostName& remName,
                        const PortNumber& remPort, const HostName& locName,
                        const PortNumber& locPort, const ConnectionId& connId,
			const ProtoTuple& proto, const OptionList& options)
{
  bool einprog = false;
  SockAddr saRem;
  socklen_t saRemLen;
  //struct hostent *hp = NULL;
  int hp = 0;
  Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,OMIT_VALUE);

	portRef.debug("f__IPL4__PROVIDER__connect: enter");
  portRef.testIfInitialized();

  if (remName == "")
    RETURN_ERROR(ERROR__HOSTNAME);

  if (remPort < 0 || remPort > 65535 || locPort < -1 || locPort > 65535)
    RETURN_ERROR(ERROR__INVALID__INPUT__PARAMETER);

  if ((hp = SetSockAddr(remName, remPort, saRem, saRemLen)) == -1) {
    SET_OS_ERROR_CODE;
    RETURN_ERROR(ERROR__HOSTNAME);
  }

#ifdef LKSCTP_MULTIHOMING_ENABLED
      int num_of_addr_rem=0;
      unsigned char* sarray_rem=NULL;
#ifdef USE_IPL4_EIN_SCTP
    if(portRef.native_stack){
#endif
      
      if(proto.ischosen(ProtoTuple::ALT_sctp) && proto.sctp().remSocks().ispresent()){
        num_of_addr_rem=proto.sctp().remSocks()().size_of();
      }
      if(num_of_addr_rem){
        sarray_rem=(unsigned char*)Malloc((num_of_addr_rem+1)*
#ifdef USE_IPV6
          sizeof(struct sockaddr_in6)
#else 
          sizeof(struct sockaddr_in)
#endif
        );
//          SockAddr saLoc2;
//          socklen_t saLoc2Len;
        int used_bytes=0;
          if(hp==AF_INET){
            memcpy (sarray_rem , &saRem, sizeof (struct sockaddr_in));
            used_bytes += sizeof (struct sockaddr_in);          
          }
#ifdef USE_IPV6
          else{
            memcpy (sarray_rem , &saRem, sizeof (struct sockaddr_in6));
            used_bytes += sizeof (struct sockaddr_in6);          
          }
#endif
portRef.debug("f__IPL4__PROVIDER__listen: addr family main: %s ",hp==-1?"Error":hp==AF_INET?"AF_INET":"AF_INET6");
        int final_hp_rem=hp;
        for(int i=0; i<num_of_addr_rem;i++){
          SockAddr saLoc2;
          socklen_t saLoc2Len;
          int hp3 = SetLocalSockAddr("f__IPL4__PROVIDER__connect",portRef,hp,proto.sctp().remSocks()()[i].hostName(), remPort, saLoc2, saLoc2Len);
portRef.debug("f__IPL4__PROVIDER__listen: addr added Family: %s ",hp3==-1?"Error":hp3==AF_INET?"AF_INET":"AF_INET6");
          if (hp3 == -1) {
            SET_OS_ERROR_CODE;
            Free(sarray_rem);
            RETURN_ERROR(ERROR__HOSTNAME);
          }
          if(hp3==AF_INET){
            memcpy (sarray_rem + used_bytes, &saLoc2, sizeof (struct sockaddr_in));
            used_bytes += sizeof (struct sockaddr_in);          
          }
#ifdef USE_IPV6
          else{
            final_hp_rem=hp3;
            memcpy (sarray_rem + used_bytes, &saLoc2, sizeof (struct sockaddr_in6));
            used_bytes += sizeof (struct sockaddr_in6);          
          }
#endif
          
        }
        hp=final_hp_rem;
        num_of_addr_rem++;
portRef.debug("f__IPL4__PROVIDER__listen: addr family final: %s ",hp==-1?"Error":hp==AF_INET?"AF_INET":"AF_INET6");
      }
#ifdef USE_IPL4_EIN_SCTP
    }
#endif
#endif


  int sock = -1;
  if (proto.get_selection() == ProtoTuple::ALT_udp && (int)connId > 0) {
    if (!portRef.isConnIdValid(connId)) {
      portRef.debug("f__IPL4__PROVIDER__connect:: invalid connId: %i", (int)connId);
      RETURN_ERROR(ERROR__INVALID__INPUT__PARAMETER);
    }
    result.connId()() = connId;
    sock = portRef.sockList[(int)connId].sock;
  } else {
		portRef.debug("f__IPL4__PROVIDER__connect: "
                  "create new socket: %s:%d -> %s:%d",
                  (const char *)locName, (int)locPort,
                  (const char *)remName, (int)remPort);
    switch(proto.get_selection()) {
    case ProtoTuple::ALT_udp:
      result = f__IPL4__PROVIDER__listen(portRef, locName, locPort, proto, options);
      if (result.errorCode().ispresent())
        return result;
      sock = portRef.sockList[(int)result.connId()()].sock;
      break;
#ifdef IPL4_USE_SSL
    case ProtoTuple::ALT_ssl:
#endif
    case ProtoTuple::ALT_tcp: {
      SockAddr saLoc;
      socklen_t saLocLen;
      //struct hostent *hp = NULL;
        int hp2 = SetLocalSockAddr("f__IPL4__PROVIDER__connect",portRef,hp,locName, locPort, saLoc, saLocLen);
//      if (locPort != -1 && locName != "")
//        hp = SetSockAddr(locName, locPort, saLoc, saLocLen);
//      else if (locName == "" && locPort == -1) { // use default host and port
//        portRef.debug("f__IPL4__PROVIDER__connect: use defaults: %s:%d",
//          portRef.defaultLocHost, portRef.defaultLocPort);
//        hp = SetSockAddr(HostName(portRef.defaultLocHost),
//                portRef.defaultLocPort, saLoc, saLocLen);
//      } else if (locPort == -1) { // use default port
//        portRef.debug("f__IPL4__PROVIDER__connect: use default port: %s:%d",
//          (const char *)locName, portRef.defaultLocPort);
//        hp = SetSockAddr(locName, portRef.defaultLocPort, saLoc, saLocLen);
//      } else { // use default host
//        portRef.debug("f__IPL4__PROVIDER__connect: use default host: %s:%d",
//          portRef.defaultLocHost, (int)locPort);
//        hp = SetSockAddr(HostName(portRef.defaultLocHost),
//                         locPort, saLoc, saLocLen);
//      }
      if (hp2 == -1) {
        SET_OS_ERROR_CODE;
        RETURN_ERROR(ERROR__HOSTNAME);
      }

      int fd = socket(hp2, SOCK_STREAM, 0);
      if (fd == -1) {
        portRef.debug("f__IPL4__PROVIDER__connect: "
                      "failed to create new socket");
        SET_OS_ERROR_CODE;
        RETURN_ERROR(ERROR__SOCKET);
      }

      // set socket properties
      if (!portRef.setOptions(options, fd, proto, true)) {
        portRef.debug("f__IPL4__PROVIDER__connect: Setting options on "
                      "socket %d failed: %s", fd, strerror(errno));
        SET_OS_ERROR_CODE;
        close(fd);
        RETURN_ERROR(ERROR__SOCKET);
      }

      if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        portRef.debug("f__IPL4__PROVIDER__connect: fcntl O_NONBLOCK on "
                      "socket %d failed: %s", fd, strerror(errno));
        SET_OS_ERROR_CODE;
        close(fd);
        RETURN_ERROR(ERROR__SOCKET);
      }

      if (bind(fd, (struct sockaddr*)&saLoc, saLocLen) == -1) {
        portRef.debug("f__IPL4__PROVIDER__connect: "
                      "bind on socket %d failed: %s", fd, strerror(errno));
        SET_OS_ERROR_CODE;
        close(fd);
        RETURN_ERROR(ERROR__SOCKET);
      }
      
      sock = fd;
      break;
    } // ProtoTuple_alt_tcp
    case ProtoTuple::ALT_sctp: {
#ifdef USE_IPL4_EIN_SCTP
    if(!portRef.native_stack){
      result=portRef.Listen_einsctp(locName, locPort,SockDesc::ACTION_CONNECT,remName,remPort,options,
         proto.sctp().remSocks().ispresent()?proto.sctp().remSocks()():IPL4asp__Types::SocketList(NULL_VALUE));
      if(portRef.globalConnOpts.extendedPortEvents == GlobalConnOpts::YES) {
        ASP__Event event;
        event.result() = result;
        portRef.incoming_message(event);
      }

      return result;
      break;
    }
#endif
#ifdef USE_SCTP
      SockAddr saLoc;
      socklen_t saLocLen;
      //struct hostent *hp = NULL;
      int hp2 = SetLocalSockAddr("f__IPL4__PROVIDER__connect",portRef,hp,locName, locPort, saLoc, saLocLen);

//      if (locPort != -1 && locName != "")
//        hp = SetSockAddr(locName, locPort, saLoc, saLocLen);
//      else if (locName == "" && locPort == -1) { // use default host and port
//        portRef.debug("f__IPL4__PROVIDER__connect: use defaults: %s:%d",
//          portRef.defaultLocHost, portRef.defaultLocPort);
//        hp = SetSockAddr(HostName(portRef.defaultLocHost),
//                portRef.defaultLocPort, saLoc, saLocLen);
//      } else if (locPort == -1) { // use default port
//        portRef.debug("f__IPL4__PROVIDER__connect: use default port: %s:%d",
//          (const char *)locName, portRef.defaultLocPort);
//        hp = SetSockAddr(locName, portRef.defaultLocPort, saLoc, saLocLen);
//      } else { // use default host
//        portRef.debug("f__IPL4__PROVIDER__connect: use default host: %s:%d",
//          portRef.defaultLocHost, (int)locPort);
//        hp = SetSockAddr(HostName(portRef.defaultLocHost),
//                         locPort, saLoc, saLocLen);
//      }
      if (hp2 == -1) {
        SET_OS_ERROR_CODE;
#ifdef LKSCTP_MULTIHOMING_ENABLED
        if(sarray_rem) {Free(sarray_rem);}
#endif
        RETURN_ERROR(ERROR__HOSTNAME);
      }
#ifdef LKSCTP_MULTIHOMING_ENABLED
      int addr_index=-1;
      int num_of_addr=0;
      unsigned char* sarray=NULL;
      for(int i=0; i<options.size_of();i++){
        if(options[i].get_selection()==Option::ALT_sctpAdditionalLocalAddresses){
          addr_index=i;
          num_of_addr=options[i].sctpAdditionalLocalAddresses().size_of();
          break;
        }
      }
      if(num_of_addr){
        sarray=(unsigned char*)Malloc(num_of_addr*
#ifdef USE_IPV6
          sizeof(struct sockaddr_in6)
#else 
          sizeof(struct sockaddr_in)
#endif
        );
//          SockAddr saLoc2;
//          socklen_t saLoc2Len;
        int used_bytes=0;
portRef.debug("f__IPL4__PROVIDER__listen: addr family main: %s ",hp2==-1?"Error":hp2==AF_INET?"AF_INET":"AF_INET6");
        int final_hp=hp2;
        for(int i=0; i<num_of_addr;i++){
          SockAddr saLoc2;
          socklen_t saLoc2Len;
          int hp3 = SetLocalSockAddr("f__IPL4__PROVIDER__connect",portRef,hp2,options[addr_index].sctpAdditionalLocalAddresses()[i], locPort, saLoc2, saLoc2Len);
portRef.debug("f__IPL4__PROVIDER__listen: addr added Family: %s ",hp3==-1?"Error":hp3==AF_INET?"AF_INET":"AF_INET6");
          if (hp3 == -1) {
            SET_OS_ERROR_CODE;
            Free(sarray);
            RETURN_ERROR(ERROR__HOSTNAME);
          }
          if(hp3==AF_INET){
            memcpy (sarray + used_bytes, &saLoc2, sizeof (struct sockaddr_in));
            used_bytes += sizeof (struct sockaddr_in);          
          }
#ifdef USE_IPV6
          else{
            final_hp=hp3;
            memcpy (sarray + used_bytes, &saLoc2, sizeof (struct sockaddr_in6));
            used_bytes += sizeof (struct sockaddr_in6);          
          }
#endif
          
        }
        hp2=final_hp;
portRef.debug("f__IPL4__PROVIDER__listen: addr family final: %s ",hp2==-1?"Error":hp2==AF_INET?"AF_INET":"AF_INET6");
      }
#endif

      int fd = socket(hp2, SOCK_STREAM, IPPROTO_SCTP);
      if (fd == -1) {
        portRef.debug("f__IPL4__PROVIDER__connect: "
                      "failed to create new socket");
        SET_OS_ERROR_CODE;
#ifdef LKSCTP_MULTIHOMING_ENABLED
        if(sarray_rem) {Free(sarray_rem);}
        if(sarray) {Free(sarray);}
#endif
        RETURN_ERROR(ERROR__SOCKET);
      }

      // set socket properties
      if (!portRef.setOptions(options, fd, proto, true)) {
        portRef.debug("f__IPL4__PROVIDER__connect: Setting options on "
                      "socket %d failed: %s", fd, strerror(errno));
        SET_OS_ERROR_CODE;
        close(fd);
#ifdef LKSCTP_MULTIHOMING_ENABLED
        if(sarray_rem) {Free(sarray_rem);}
        if(sarray) {Free(sarray);}
#endif
        RETURN_ERROR(ERROR__SOCKET);
      }

      if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        portRef.debug("f__IPL4__PROVIDER__connect: fcntl O_NONBLOCK on "
                      "socket %d failed: %s", fd, strerror(errno));
        SET_OS_ERROR_CODE;
        close(fd);
#ifdef LKSCTP_MULTIHOMING_ENABLED
        if(sarray_rem) {Free(sarray_rem);}
        if(sarray) {Free(sarray);}
#endif
        RETURN_ERROR(ERROR__SOCKET);
      }

      if (bind(fd, (struct sockaddr*)&saLoc, saLocLen) == -1) {
        portRef.debug("f__IPL4__PROVIDER__connect: "
                      "bind on socket %d failed: %s", fd, strerror(errno));
        SET_OS_ERROR_CODE;
        close(fd);
#ifdef LKSCTP_MULTIHOMING_ENABLED
        if(sarray_rem) {Free(sarray_rem);}
        if(sarray) {Free(sarray);}
#endif
        RETURN_ERROR(ERROR__SOCKET);
      }
#ifdef LKSCTP_MULTIHOMING_ENABLED
      if(num_of_addr){

        if (sctp_bindx(fd, (struct sockaddr*)sarray, num_of_addr,  SCTP_BINDX_ADD_ADDR ) == -1) {
          portRef.debug("f__IPL4__PROVIDER__connect: "
                        "sctp_bindx on socket %d failed: %s", fd, strerror(errno));
          SET_OS_ERROR_CODE;
          Free(sarray);
          if(sarray_rem) {Free(sarray_rem);}
          close(fd);
          RETURN_ERROR(ERROR__SOCKET);
        }
        Free(sarray);
       
        
      }
#endif
      sock = fd;
      break;
#endif
    }
    default: case ProtoTuple::UNBOUND_VALUE:
			RETURN_ERROR(ERROR__UNSUPPORTED__PROTOCOL);
    } // switch(proto.get_selection())
  }

  switch(proto.get_selection()) {
  case ProtoTuple::ALT_udp:
#ifdef IPL4_USE_SSL
  case ProtoTuple::ALT_ssl:
#endif
  case ProtoTuple::ALT_tcp:
    portRef.debug("f__IPL4__PROVIDER__connect: sock: %d", sock);
    if (connect(sock, (struct sockaddr *)&saRem, saRemLen) == -1) {
      int l_errno = errno;
      SET_OS_ERROR_CODE;
      einprog = (l_errno == EINPROGRESS);
      portRef.debug("f__IPL4__PROVIDER__connect: error: %s", strerror(errno));
      if (!einprog) {
        if ((proto.get_selection() == ProtoTuple::ALT_tcp) || (proto.get_selection() == ProtoTuple::ALT_ssl)) {
          close(sock);
          portRef.debug("f__IPL4__PROVIDER__connect: socket %d is closed.", sock);
        } else {
          // The UDP socket has already been added to sockList and even if it
          // cannot be connected to some remote destination, it can still be used
          // as a listening socket or with the SendTo ASP. Therefore, it is not
          // removed from connList in case of error. But it is going to be removed,
          // if the socket has been created in this operation.
          if ((int)connId == -1) {
            if (portRef.ConnDel(result.connId()()) == -1)
              portRef.debug("f__IPL4__PROVIDER__connect: unable to close socket %d (udp): %s",
                sock, strerror(errno));
            result.connId() = OMIT_VALUE;
          }
        }
        RETURN_ERROR(ERROR__SOCKET);
      }
    }
    if (proto.get_selection() == ProtoTuple::ALT_tcp) {
      int l_connId = portRef.ConnAdd(IPL4asp_TCP, sock);
      if (l_connId == -1)
        RETURN_ERROR(ERROR__INSUFFICIENT__MEMORY);
      result.connId()() = l_connId;
      if((*portRef.sockList[l_connId].remoteport)==-1){
        *portRef.sockList[l_connId].remoteaddr=remName;
        *portRef.sockList[l_connId].remoteport=remPort;
      }

#ifdef IPL4_USE_SSL
    }
    else  if (proto.get_selection() == ProtoTuple::ALT_ssl) {
      int l_connId = portRef.ConnAdd(IPL4asp_SSL, sock);
      if (l_connId == -1)
        RETURN_ERROR(ERROR__INSUFFICIENT__MEMORY);
      result.connId()() = l_connId;
#endif
    } else {
      portRef.debug("f__IPL4__PROVIDER__connect: udp socket connected, connection ID: %d",
        (int)result.connId()());
      break;
    }
    if (einprog) {
      if (portRef.pureNonBlocking)
      {
        // The socket is not writeable yet
        portRef.Handler_Add_Fd_Write(sock);
        portRef.debug("f__IPL4__PROVIDER__connect: leave (TEMPORARILY UNAVAILABLE)   fd: %i", sock);
        RETURN_ERROR(ERROR__TEMPORARILY__UNAVAILABLE);
      }
      result.os__error__code() = OMIT_VALUE;
      pollfd    pollFd;
      portRef.debug("f__IPL4__PROVIDER__connect: EAGAIN: waiting in poll: fd %d   connId: %d",
        sock, (int)result.connId()());
      int nEvents = -1;
      for (int kk=1;;kk++) {
        memset(&pollFd, 0, sizeof(pollFd));
        pollFd.fd = sock;
        pollFd.events = POLLOUT | POLLIN;
        nEvents = poll(&pollFd, 1, portRef.poll_timeout); // infinite
        if (nEvents > 0) break;
        if (nEvents < 0 && errno != EINTR) break;
        if (portRef.max_num_of_poll>0 && portRef.max_num_of_poll<=kk) break;
      }
      portRef.debug("f__IPL4__PROVIDER__connect: EAGAIN: poll returned: %i", nEvents);
      portRef.debug("f__IPL4__PROVIDER__connect: EAGAIN: revents: 0x%04X", pollFd.revents);
      bool socketError = false;
      if (nEvents > 0) {
        if ((pollFd.revents & POLLOUT) != 0)
          portRef.debug("f__IPL4__PROVIDER__connect: EAGAIN: writable");
        if ((proto.get_selection() == ProtoTuple::ALT_tcp) || (proto.get_selection() == ProtoTuple::ALT_ssl)) {
          int sendRes = ::send(sock, "", 0, 0);
          portRef.debug("f__IPL4__PROVIDER__connect: Probing connection, result: %d", sendRes);
          if (sendRes < 0) {
            SET_OS_ERROR_CODE;
            portRef.debug("f__IPL4__PROVIDER__connect: error: %s", strerror(errno));
            socketError = true;
          }
        }
      } else {
        SET_OS_ERROR_CODE;
        socketError = true;
      }
      if (socketError) {
        if (proto.get_selection() != ProtoTuple::ALT_udp || (int)connId == -1) {
          if (portRef.ConnDel(result.connId()()) == -1)
            portRef.debug("f__IPL4__PROVIDER__connect: unable to close socket");
          result.connId() = OMIT_VALUE;
        }
        RETURN_ERROR(ERROR__SOCKET);
      } // socketError
    } // einprog
#ifdef IPL4_USE_SSL
    //Add SSL layer
	if(proto.get_selection() == ProtoTuple::ALT_ssl)
	{
		switch(portRef.perform_ssl_handshake((int)result.connId()()))
		{
		  case 1:
			  break;
		  case -1:
		    portRef.sockList[result.connId()()].sslState = STATE_PNBRECONNECTATTEMPT;
	        portRef.Handler_Add_Fd_Write(sock);
	        portRef.debug("f__IPL4__PROVIDER__connect: leave (TEMPORARILY UNAVAILABLE)   fd: %i", sock);
	        RETURN_ERROR(ERROR__TEMPORARILY__UNAVAILABLE);
	        break;
		  case 0:
		  default: //value
			portRef.debug("f__IPL4__PROVIDER__connect: SSL mapping failed for client socket: %d", portRef.sockList[(int)result.connId()()].sock);
			SET_OS_ERROR_CODE;
			if (portRef.ConnDel(result.connId()()) == -1)portRef.debug("f__IPL4__PROVIDER__connect: unable to close socket");
			result.connId() = OMIT_VALUE;
			RETURN_ERROR(ERROR__SOCKET);
			break;
		}
	}
#endif
    break;
  case ProtoTuple::ALT_sctp: {
#ifdef USE_SCTP
    portRef.debug("f__IPL4__PROVIDER__connect: sock: %d", sock);
#ifdef LKSCTP_MULTIHOMING_ENABLED
        if(sarray_rem) {
    portRef.debug("f__IPL4__PROVIDER__connectx: sock: %d, num_of_addr %d", sock, num_of_addr_rem);
          if (my_sctp_connectx(sock, (struct sockaddr *)sarray_rem, num_of_addr_rem) == -1) {
            int l_errno = errno;
            SET_OS_ERROR_CODE;
            einprog = (l_errno == EINPROGRESS);
            if (!einprog) {
              portRef.debug("f__IPL4__PROVIDER__connect: error: %s", strerror(errno));
              close(sock);
              Free(sarray_rem);
              RETURN_ERROR(ERROR__SOCKET);
            }
          }
          Free(sarray_rem);
          
        } else
#endif
    if (connect(sock, (struct sockaddr *)&saRem, saRemLen) == -1) {
      int l_errno = errno;
      SET_OS_ERROR_CODE;
      einprog = (l_errno == EINPROGRESS);
      if (!einprog) {
        portRef.debug("f__IPL4__PROVIDER__connect: error: %s", strerror(errno));
        close(sock);
        RETURN_ERROR(ERROR__SOCKET);
      }
    }

    int l_connId = portRef.ConnAdd(IPL4asp_SCTP, sock);
    if (l_connId == -1) {
      RETURN_ERROR(ERROR__INSUFFICIENT__MEMORY);
    }
    result.connId()() = l_connId;
      if((*portRef.sockList[l_connId].remoteport)==-1){
        *portRef.sockList[l_connId].remoteaddr=remName;
        *portRef.sockList[l_connId].remoteport=remPort;
      }

    if (einprog) {
      if (portRef.pureNonBlocking)
      {
        // The socket is not writeable yet
        portRef.Handler_Add_Fd_Write(sock);
        portRef.debug("f__IPL4__PROVIDER__connect: leave (TEMPORARILY UNAVAILABLE)   fd: %i", sock);
        RETURN_ERROR(ERROR__TEMPORARILY__UNAVAILABLE);
      }
      result.os__error__code() = OMIT_VALUE;
      pollfd    pollFd;
      portRef.debug("f__IPL4__PROVIDER__connect: EAGAIN: waiting in poll: fd %d   connId: %d",
        sock, (int)result.connId()());
      int nEvents = -1;
      for (int kk=1;;kk++) {
        memset(&pollFd, 0, sizeof(pollFd));
        pollFd.fd = sock;
        pollFd.events = POLLOUT | POLLIN;
        nEvents = poll(&pollFd, 1, portRef.poll_timeout); // infinite
        if (nEvents > 0) break;
        if (nEvents < 0 && errno != EINTR) break;
        if (portRef.max_num_of_poll>0 && portRef.max_num_of_poll<=kk) break;
      }
      portRef.debug("f__IPL4__PROVIDER__connect: EAGAIN: poll returned: %i", nEvents);
      portRef.debug("f__IPL4__PROVIDER__connect: EAGAIN: revents: 0x%04X", pollFd.revents);
      bool socketError = false;
      if (nEvents > 0) {
        if ((pollFd.revents & POLLOUT) != 0)
          portRef.debug("f__IPL4__PROVIDER__connect: EAGAIN: writable");
        if (connect(sock, (struct sockaddr *)&saRem, saRemLen) == -1) {
          if(errno == EISCONN){
             portRef.debug("f__IPL4__PROVIDER__connect: Probing connection, result: sucessfull");
          } else {
             portRef.debug("f__IPL4__PROVIDER__connect: Probing connection, result: unsucessfull");
             SET_OS_ERROR_CODE;
             socketError = true;
          }
        }
      } else {
        SET_OS_ERROR_CODE;
        socketError = true;
      }
      if (socketError) {
        if (portRef.ConnDel(result.connId()()) == -1)
          portRef.debug("f__IPL4__PROVIDER__connect: unable to close socket");
        result.connId() = OMIT_VALUE;
        RETURN_ERROR(ERROR__SOCKET);
      } // socketError
    } // einprog
#endif
    break;
  }
  default: case ProtoTuple::UNBOUND_VALUE:
    RETURN_ERROR(ERROR__UNSUPPORTED__PROTOCOL);
  } // switch(proto.get_selection())
  portRef.debug("f__IPL4__PROVIDER__connect: leave");

  if(portRef.globalConnOpts.extendedPortEvents == GlobalConnOpts::YES) {
    ASP__Event event;
    event.result() = result;
    portRef.incoming_message(event);
  }

  return result;
} // f__IPL4__PROVIDER__connect



Result f__IPL4__PROVIDER__setOpt(IPL4asp__PT_PROVIDER& portRef, const OptionList& options,
  const ConnectionId& connId, const ProtoTuple& proto)
{
  portRef.testIfInitialized();
  Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,OMIT_VALUE);
  ProtoTuple protocol = proto;
  int sock = -1;
  if ((int)connId != -1) {
    if (!portRef.isConnIdValid(connId)) {
      RETURN_ERROR(ERROR__INVALID__INPUT__PARAMETER);
    }
    sock = portRef.sockList[(int)connId].sock;
    SockType type = (SockType)-1;
    if (!portRef.getAndCheckSockType(connId, proto.get_selection(), type))
      RETURN_ERROR(ERROR__SOCKET);
      switch (type) {
        case IPL4asp_TCP_LISTEN:
        case IPL4asp_TCP:
        	protocol.tcp() = TcpTuple(null_type()); break;
        case IPL4asp_UDP:
        	protocol.udp() = UdpTuple(null_type()); break; //TCP<->UDP SWITCHED VALUES FIXED -- ETHNBA
        case IPL4asp_SCTP_LISTEN:
        case IPL4asp_SCTP:
          protocol.sctp() = SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE); break;
        case IPL4asp_SSL_LISTEN:
        case IPL4asp_SSL:
          protocol.ssl() = SslTuple(null_type()); break;
        default: break;
      }
  }
  if (!portRef.setOptions(options, sock, protocol))
    RETURN_ERROR(ERROR__SOCKET);
  return result;
}



Result f__IPL4__PROVIDER__close(IPL4asp__PT_PROVIDER& portRef,
              const ConnectionId& connId, const ProtoTuple& proto)
{
	portRef.debug("f__IPL4__PROVIDER__close: enter: connId: %d", (int)connId);
  portRef.testIfInitialized();
  Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,OMIT_VALUE);
  if (!portRef.isConnIdValid(connId)) {
    RETURN_ERROR(ERROR__INVALID__INPUT__PARAMETER);
  }
  SockType type;
  if (!portRef.getAndCheckSockType(connId, proto.get_selection(), type)) {
    RETURN_ERROR(ERROR__INVALID__INPUT__PARAMETER);
  }

  if (type == IPL4asp_SCTP_LISTEN || type == IPL4asp_SCTP) {
    // Close SCTP associations if any, but the socket is not closed.
#ifdef USE_IPL4_EIN_SCTP
    if(!portRef.native_stack){
      portRef.sockList[connId].next_action=SockDesc::ACTION_DELETE;
      if(type == IPL4asp_SCTP){
        EINSS7_00SctpShutdownReq(portRef.sockList[connId].sock);
      } else {
        if(portRef.sockList[connId].ref_count==0){
          EINSS7_00SctpDestroyReq(portRef.sockList[connId].endpoint_id);
          portRef.ConnDelEin(connId);
        }
      }
    
    } else {
#endif

      if (portRef.ConnDel(connId) == -1)
        RETURN_ERROR(ERROR__SOCKET);
#ifdef USE_IPL4_EIN_SCTP

    }
#endif
  } else {
    if (portRef.ConnDel(connId) == -1)
      RETURN_ERROR(ERROR__SOCKET);
  }
  result.connId()() = connId;
  if(portRef.globalConnOpts.extendedPortEvents == GlobalConnOpts::YES) {
    ASP__Event event;
    event.result() = result;
    portRef.incoming_message(event);
  }
  return result;
} // f__IPL4__PROVIDER__close



Result f__IPL4__PROVIDER__setUserData(
  IPL4asp__PT_PROVIDER& portRef,
  const ConnectionId& id,
  const UserData& userData)
{
  Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,OMIT_VALUE);
  if (portRef.setUserData((int)id, (int)userData) == -1) {
    RETURN_ERROR(ERROR__GENERAL);
  }
  return result;
} // f__IPL4__PROVIDER__setUserData



Result f__IPL4__PROVIDER__getUserData(
  IPL4asp__PT_PROVIDER& portRef,
  const ConnectionId& connId,
  UserData& userData)
{
  Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,OMIT_VALUE);
  int userDataTemp;
  if (portRef.getUserData((int)connId, userDataTemp) == -1) {
    RETURN_ERROR(ERROR__GENERAL);
  }
  userData = userDataTemp;
  return result;
} // f__IPL4__PROVIDER__getUserData



Result f__IPL4__PROVIDER__getConnectionDetails(
  IPL4asp__PT_PROVIDER& portRef,
  const ConnectionId& connId,
  const IPL4__Param& IPL4param,
  IPL4__ParamResult& IPL4paramResult)
{
  Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,OMIT_VALUE);
  IPL4__ParamResult paramResult;
  if (portRef.getConnectionDetails((int)connId, IPL4param, paramResult) == -1) {
    RETURN_ERROR(ERROR__GENERAL);
  }
  IPL4paramResult = paramResult;
  return result;
} // f__IPL4__PROVIDER__getConnectionDetails

Result f__IPL4__listen(
  IPL4asp__PT& portRef,
  const HostName& locName,
  const PortNumber& locPort,
  const ProtoTuple& proto,
  const OptionList& options)
{
  return f__IPL4__PROVIDER__listen(portRef, locName, locPort, proto, options);
} // f__IPL4__listen



Result f__IPL4__connect(
  IPL4asp__PT& portRef,
  const HostName& remName,
  const PortNumber& remPort,
  const HostName& locName,
  const PortNumber& locPort,
  const ConnectionId& connId,
  const ProtoTuple& proto,
  const OptionList& options)
{
  return f__IPL4__PROVIDER__connect(portRef, remName, remPort,
                                    locName, locPort, connId, proto, options);
} // f__IPL4__connect



Result f__IPL4__setOpt(
  IPL4asp__PT& portRef,
  const OptionList& options,
  const ConnectionId& connId,
  const ProtoTuple& proto)
{
  return f__IPL4__PROVIDER__setOpt(portRef, options, connId, proto);
} // f__IPL4__setOpt



Result f__IPL4__close(
  IPL4asp__PT& portRef,
  const ConnectionId& connId,
  const ProtoTuple& proto)
{
  return f__IPL4__PROVIDER__close(portRef, connId, proto);
} // f__IPL4__close



Result f__IPL4__setUserData(
  IPL4asp__PT& portRef,
  const ConnectionId& connId,
  const UserData& userData)
{
  return f__IPL4__PROVIDER__setUserData(portRef, connId, userData);
} // f__IPL4__setUserData



Result f__IPL4__getUserData(
  IPL4asp__PT& portRef,
  const ConnectionId& connId,
  UserData& userData)
{
  return f__IPL4__PROVIDER__getUserData(portRef, connId, userData);
} // f__IPL4__getUserData



Result f__IPL4__getConnectionDetails(
  IPL4asp__PT& portRef,
  const ConnectionId& connId,
  const IPL4__Param& IPL4param,
  IPL4__ParamResult& IPL4paramResult)
{
  return f__IPL4__PROVIDER__getConnectionDetails(portRef, connId, IPL4param, IPL4paramResult);
} // f__IPL4__getConnectionDetails


void f__IPL4__setGetMsgLen(
  IPL4asp__PT& portRef,
  const ConnectionId& connId,
  f__IPL4__getMsgLen& f,
  const ro__integer& msgLenArgs)
{
  f__IPL4__PROVIDER__setGetMsgLen(portRef, connId, f, msgLenArgs);
} // f__IPL4__setGetMsgLen

Result f__IPL4__send(
  IPL4asp__PT& portRef,
  const ASP__Send& asp,
  INTEGER& sent__octets)
{
  if(TTCN_Logger::log_this_event(TTCN_PORTEVENT)){
    TTCN_Logger::begin_event(TTCN_PORTEVENT);
    TTCN_Logger::log_event("%s: f_IPL4_send: ", portRef.get_name());
    asp.log();
    TTCN_Logger::end_event();
  }
  Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
  sent__octets=portRef.outgoing_send_core(asp,result);
  return result;
}

Result f__IPL4__sendto(
  IPL4asp__PT& portRef,
  const ASP__SendTo& asp,
  INTEGER& sent__octets)
{
  if(TTCN_Logger::log_this_event(TTCN_PORTEVENT)){
    TTCN_Logger::begin_event(TTCN_PORTEVENT);
    TTCN_Logger::log_event("%s: f_IPL4_sendto: ", portRef.get_name());
    asp.log();
    TTCN_Logger::end_event();
  }
  Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
  sent__octets=portRef.outgoing_send_core(asp,result);
  return result;
}

//************
//SSL
//************
#ifdef IPL4_USE_SSL
// ssl_session ID context of the server
static unsigned char ssl_server_context_name[] = "McHalls&EduardWasHere";
const unsigned char * IPL4asp__PT_PROVIDER::ssl_server_auth_session_id_context = ssl_server_context_name;
// Password pointer
void  *IPL4asp__PT_PROVIDER::ssl_current_client = NULL;

// Data set/get functions
char       * IPL4asp__PT_PROVIDER::get_ssl_password() const {return ssl_password;}
void         IPL4asp__PT_PROVIDER::set_ssl_verifycertificate(bool par) {ssl_verify_certificate=par;}
void         IPL4asp__PT_PROVIDER::set_ssl_use_session_resumption(bool par) {ssl_use_session_resumption=par;}
void         IPL4asp__PT_PROVIDER::set_ssl_key_file(char * par) {
  delete [] ssl_key_file;
  ssl_key_file=par;
}
void         IPL4asp__PT_PROVIDER::set_ssl_certificate_file(char * par) {
  delete [] ssl_certificate_file;
  ssl_certificate_file=par;
}
void         IPL4asp__PT_PROVIDER::set_ssl_trustedCAlist_file(char * par) {
  delete [] ssl_trustedCAlist_file;
  ssl_trustedCAlist_file=par;
}
void         IPL4asp__PT_PROVIDER::set_ssl_cipher_list(char * par) {
  delete [] ssl_cipher_list;
  ssl_cipher_list=par;
}
void         IPL4asp__PT_PROVIDER::set_ssl_server_auth_session_id_context(const unsigned char * par) {
	ssl_server_auth_session_id_context=par;
}

// Default parameter names
//const char* IPL4asp__PT_PROVIDER::ssl_use_ssl_name()                { return "ssl_use_ssl";}
const char* IPL4asp__PT_PROVIDER::ssl_use_session_resumption_name() { return "ssl_use_session_resumption";}
const char* IPL4asp__PT_PROVIDER::ssl_private_key_file_name()       { return "ssl_private_key_file";}
const char* IPL4asp__PT_PROVIDER::ssl_trustedCAlist_file_name()     { return "ssl_trustedCAlist_file";}
const char* IPL4asp__PT_PROVIDER::ssl_certificate_file_name()       { return "ssl_certificate_chain_file";}
const char* IPL4asp__PT_PROVIDER::ssl_password_name()               { return "ssl_private_key_password";}
const char* IPL4asp__PT_PROVIDER::ssl_cipher_list_name()            { return "ssl_allowed_ciphers_list";}
const char* IPL4asp__PT_PROVIDER::ssl_verifycertificate_name()      { return "ssl_verify_certificate";}


int IPL4asp__PT_PROVIDER::perform_ssl_handshake(int client_id) {

  log_debug("entering IPL4asp__PT_PROVIDER::perform_ssl_handshake() .");

  //in case of purenonblocking & client reconnect
  if(pureNonBlocking && getSslObj(client_id, ssl_current_ssl) && ssl_current_ssl!=NULL)
  {
	  debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Reconnection for client: %d ", client_id);
  }
  else
  {
	  if(!ssl_init_SSL())
	  {
		debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: SSL initialization failed during client: %d connection", client_id);
		return 0;
	  }

	  log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Create a new SSL object");
	  if (ssl_ctx==NULL)
	  {
		debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: No SSL CTX found, SSL not initialized for client: %d", client_id);
		return 0;
	  }
	  ssl_current_ssl=SSL_new(ssl_ctx);

	  if (ssl_current_ssl==NULL)
	  {
		debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Creation of SSL object failed for client: %d", client_id);
		return 0;
	  }

	  if(!setSslObj(client_id, ssl_current_ssl))
	  {
		debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: setSslObj failed for client: %d", client_id);
		return 0;
	  }

	  log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: New client added with key '%d'", client_id);
	  log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Binding SSL to the socket");
	  if (SSL_set_fd(ssl_current_ssl, sockList[client_id].sock)!=1)
	  {
		debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Binding of SSL object to socket failed. Client: %d", client_id);
		return 0;
	  }
  }
  // Conext change for SSL objects may come here in the
  // future.

  //server accepting
  if ((sockList[client_id].parentIdx != -1) && (sockList[sockList[client_id].parentIdx].type == IPL4asp_SSL_LISTEN))
  {
    log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Accept SSL connection request");
    if (ssl_current_client!=NULL) log_warning("Warning: IPL4asp__PT_PROVIDER::perform_ssl_handshake: race condition while setting current client object pointer");
    ssl_current_client=(IPL4asp__PT_PROVIDER *)this;

    /*
    if (ssl_getresult(SSL_accept(ssl_current_ssl))!=SSL_ERROR_NONE) {
      log_warning("Warning: IPL4asp__PT_PROVIDER::perform_ssl_handshake: Connection from client %d is refused", client_id);
      ssl_current_client=NULL;
      log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: leaving IPL4asp__PT_PROVIDER::perform_ssl_handshake()");
      return 0;
    }
   */
    int attempt = 0;
    while(true)
    {
    	log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: SSL attempt to Accept Client Connection... %d", attempt + 1);
    	switch(ssl_getresult(SSL_accept(ssl_current_ssl)))
    	{
    	   case SSL_ERROR_NONE:
    		   break;
    	   case SSL_ERROR_WANT_READ:
    	   case SSL_ERROR_WANT_WRITE:
       	      if(pureNonBlocking)
       		  {
       	    	sockList[client_id].sslState = STATE_SERVER_ACCEPTING;
       	    	Handler_Add_Fd_Write(sockList[client_id].sock);
       		 	ssl_current_client=NULL;
       		 	log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: leaving IPL4asp__PT_PROVIDER::perform_ssl_handshake()");
       		 	return -1;
       		  }

			  if(++attempt == ssl_reconnect_attempts)
			  {
				ssl_current_client=NULL;
				log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Connection accept failed");
				log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: leaving IPL4asp__PT_PROVIDER::perform_ssl_handshake()");
				return 0;
			  }
			  //usleep(ssl_reconnect_delay);
	          timespec tm_val;
		      tm_val.tv_sec=ssl_reconnect_delay/1000000;
			  tm_val.tv_nsec=(ssl_reconnect_delay%1000000)*1000;
	          nanosleep(&tm_val,NULL);
		      continue;
    	   case SSL_ERROR_SYSCALL:
    		   log_warning("Warning: IPL4asp__PT_PROVIDER::perform_ssl_handshake: SSL_ERROR_SYSCALL peer is disconnected");
    	   default:
			  log_warning("Warning: IPL4asp__PT_PROVIDER::perform_ssl_handshake: Connection from client %d is refused", client_id);
			  ssl_current_client=NULL;
			  log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: leaving IPL4asp__PT_PROVIDER::perform_ssl_handshake()");
			  return 0;
    	}
    	break;
    }

    sockList[client_id].sslState = STATE_NORMAL;
    debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Connection from client %d is accepted", client_id);
    ssl_current_client=NULL;

  } else {//client connecting
    if (ssl_use_session_resumption && ssl_session!=NULL) {
      log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Try to use ssl_session resumption");
      if (ssl_getresult(SSL_set_session(ssl_current_ssl, ssl_session))!=SSL_ERROR_NONE)
      {
        debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: SSL error occured during set session. Client: %d", client_id);
	    return 0;
      }
    }

    log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Connect to server");
    if (ssl_current_client!=NULL) log_warning("IPL4asp__PT_PROVIDER::perform_ssl_handshake: race condition while setting current client object pointer");
    ssl_current_client=(IPL4asp__PT_PROVIDER *)this;

    int attempt = 0;
    while(true)
    {
    	log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: SSL attempt to connect ... %d", attempt + 1);
    	switch(ssl_getresult(SSL_connect(ssl_current_ssl)))
    	{
    	case SSL_ERROR_NONE:
    		log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Connection Success.");
    		sockList[client_id].sslState = STATE_NORMAL;
    		break;
    	case SSL_ERROR_WANT_READ:
    	case SSL_ERROR_WANT_WRITE:
    		if(pureNonBlocking)
    		{
    			ssl_current_client=NULL;
    			log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: leaving IPL4asp__PT_PROVIDER::perform_ssl_handshake()");
    			return -1;
    		}

    		if(++attempt == ssl_reconnect_attempts)
    		{
    			ssl_current_client=NULL;
    			log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Connection failed");
    			log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: leaving IPL4asp__PT_PROVIDER::perform_ssl_handshake()");
    			return 0;
    		}
    		//usleep(ssl_reconnect_delay);
    		timespec tm_val;
			tm_val.tv_sec=ssl_reconnect_delay/1000000;
			tm_val.tv_nsec=(ssl_reconnect_delay%1000000)*1000;
            nanosleep(&tm_val,NULL);
    		continue;
    	default:
    	    ssl_current_client=NULL;
			log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Connection failed");
			log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: leaving IPL4asp__PT_PROVIDER::perform_ssl_handshake()");
    		return 0;
    	}
    	break;
    }

    ssl_current_client=NULL;
    if (ssl_use_session_resumption) {
      log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Connected, get new ssl_session");
      ssl_session=SSL_get1_session(ssl_current_ssl);
      if (ssl_session==NULL)
        log_warning("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Server did not send a session ID");
    }
  }

  if (ssl_use_session_resumption) {
    if (SSL_session_reused(ssl_current_ssl)) log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Session was reused");
    else log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Session was not reused");
  }

  if (!ssl_verify_certificates()) { // remove client
    log_warning("IPL4asp__PT_PROVIDER::perform_ssl_handshake: Verification failed");
    log_debug("IPL4asp__PT_PROVIDER::perform_ssl_handshake: leaving IPL4asp__PT_PROVIDER::perform_ssl_handshake()");
    return 0;

  }
  log_debug("leaving IPL4asp__PT_PROVIDER::perform_ssl_handshake()");
  return 1;
}


bool IPL4asp__PT_PROVIDER::perform_ssl_shutdown(int client_id) {

  log_debug("entering IPL4asp__PT_PROVIDER::perform_ssl_shutdown()");
  if (getSslObj(client_id, ssl_current_ssl) && ssl_current_ssl!=NULL) {
    if(sockList[client_id].sslState != STATE_CONNECTING) { SSL_shutdown(ssl_current_ssl);
    SSL_free(ssl_current_ssl);}
  } else
    log_warning("SSL object not found for client %d", client_id);
  log_debug("leaving IPL4asp__PT_PROVIDER::perform_ssl_shutdown()");
  return true;
}


//Currently not used (separated to listen/connect directly
bool IPL4asp__PT_PROVIDER::user_all_mandatory_configparameters_present(int clientId) {
  if (sockList[clientId].type == IPL4asp_SSL_LISTEN) {
    if (ssl_certificate_file==NULL)
    {
      debug("%s is not defined in the configuration file", ssl_certificate_file_name());
      return false;
    }
    if (ssl_trustedCAlist_file==NULL)
    {
      debug("%s is not defined in the configuration file", ssl_trustedCAlist_file_name());
      return false;
    }
    if (ssl_key_file==NULL)
    {
      debug("%s is not defined in the configuration file", ssl_private_key_file_name());
      return false;
    }
  } else if(sockList[clientId].type == IPL4asp_SSL){
    if (ssl_verify_certificate && ssl_trustedCAlist_file==NULL)
    {
      debug("%s is not defined in the configuration file altough %s=yes", ssl_trustedCAlist_file_name(), ssl_verifycertificate_name());
      return false;
    }
  }
  return true;
}



//STATE_WAIT_FOR_RECEIVE_CALLBACK: if the SSL_read operation would
//                                  block because the socket is not ready for writing,
//                                  I set the socket state to this state and add the file
//                                  descriptor to the Event_Handler. The Event_Handler will
//                                  wake up and call the receive_message_on_fd operation
//                                  if the socket is ready to write.
//If the SSL_read operation would block because the socket is not ready for
//reading, I do nothing
int IPL4asp__PT_PROVIDER::receive_ssl_message_on_fd(int client_id)
{
  log_debug("entering IPL4asp__PT_PROVIDER::receive_message_on_fd()");

 if (ssl_current_client!=NULL) log_warning("Warning: race condition while setting current client object pointer");
  ssl_current_client=(IPL4asp__PT_PROVIDER *)this;

  TTCN_Buffer* recv_tb = *sockList[client_id].buf;

  if(sockList[client_id].sslState != STATE_NORMAL)
  {
	  log_debug("IPL4asp__PT_PROVIDER::receive_message_on_fd: leave - Client is Connecting: %d", client_id);
    ssl_current_client=NULL;
	  return -2;
  }

  if(!getSslObj(client_id, ssl_current_ssl))
  {
	  log_debug("IPL4asp__PT_PROVIDER::receive_message_on_fd: current SSL invalid for client: %d", client_id);
    ssl_current_client=NULL;
	  return 0;
  }
 
  int messageLength=0;
  size_t end_len=AS_SSL_CHUNCK_SIZE;
  unsigned char *end_ptr;
  while (messageLength<=0) {
    log_debug("  one read cycle started");
    recv_tb->get_end(end_ptr, end_len);
    messageLength = SSL_read(ssl_current_ssl, end_ptr, end_len);
    if (messageLength <= 0) {
      int res=ssl_getresult(messageLength);
      switch (res) {
      case SSL_ERROR_ZERO_RETURN:
        log_debug("IPL4asp__PT_PROVIDER::receive_message_on_fd: SSL connection was interrupted by the other side");
        SSL_set_quiet_shutdown(ssl_current_ssl, 1);
        log_debug("SSL_ERROR_ZERO_RETURN is received, setting SSL SHUTDOWN mode to QUIET");
        ssl_current_client=NULL;
        log_debug("leaving IPL4asp__PT_PROVIDER::receive_message_on_fd() with SSL_ERROR_ZERO_RETURN");
        return 0;
      case SSL_ERROR_WANT_WRITE://writing would block
        if (pureNonBlocking){
        	Handler_Add_Fd_Write(sockList[client_id].sock);
            log_debug("IPL4asp__PT_PROVIDER::receive_message_on_fd: setting socket state to STATE_WAIT_FOR_RECEIVE_CALLBACK");
            sockList[client_id].sslState = STATE_WAIT_FOR_RECEIVE_CALLBACK;

            ssl_current_client=NULL;
            log_debug("leaving IPL4asp__PT_PROVIDER::receive_message_on_fd()");
            return -2;
        }
      case SSL_ERROR_WANT_READ: //reading would block, continue processing data
        if (pureNonBlocking){
            log_debug("IPL4asp__PT_PROVIDER::receive_message_on_fd: reading would block, leaving IPL4asp__PT_PROVIDER::receive_message_on_fd()");
            ssl_current_client = NULL;
            log_debug("leaving IPL4asp__PT_PROVIDER::receive_message_on_fd()");
            return -2;
        }
        log_debug("repeat the read operation to finish the pending SSL handshake");
        break;
      default:
        ssl_current_client=NULL;
        log_debug("SSL error occured");
        return -1;
      }
    } else {
    	recv_tb->increase_length(messageLength);
    }
  }
  ssl_current_client=NULL;
  log_debug("leaving IPL4asp__PT_PROVIDER::receive_message_on_fd() with number of bytes read: %d", messageLength);

  return messageLength;
}

bool IPL4asp__PT_PROVIDER::increase_send_buffer(int fd,
     int &old_size, int& new_size)
{
    int set_size;
#if defined LINUX || defined FREEBSD || defined SOLARIS8
    socklen_t
#else /* SOLARIS or WIN32 */
    int
#endif
	optlen = sizeof(old_size);
    // obtaining the current buffer size first
    if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&old_size, &optlen))
	goto getsockopt_failure;
    if (old_size <= 0) {
	log_warning("System call getsockopt(SO_SNDBUF) "
	    "returned invalid buffer size (%d) on file descriptor %d.",
	    old_size, fd);
	return false;
    }
    // trying to double the buffer size
    set_size = 2 * old_size;
    if (set_size > old_size) {
	if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&set_size,
	    sizeof(set_size))) {
	    // the operation failed
	    switch (errno) {
	    case ENOMEM:
	    case ENOBUFS:
		errno = 0;
		break;
	    default:
		// other error codes indicate a fatal error
		goto setsockopt_failure;
	    }
	} else {
	    // the operation was successful
	    goto success;
	}
    }
    // trying to perform a binary search to determine the maximum buffer size
    set_size = old_size;
    for (int size_step = old_size / 2; size_step > 0; size_step /= 2) {
	int tried_size = set_size + size_step;
	if (tried_size > set_size) {
	    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&tried_size,
		sizeof(tried_size))) {
		// the operation failed
		switch (errno) {
		case ENOMEM:
		case ENOBUFS:
		    errno = 0;
		    break;
		default:
		    // other error codes indicate a fatal error
		    goto setsockopt_failure;
		}
	    } else {
		// the operation was successful
		set_size = tried_size;
	    }
	}
    }
    if (set_size <= old_size) return false;
success:
    // querying the new effective buffer size (it might be smaller
    // than set_size but should not be smaller than old_size)
    optlen = sizeof(new_size);
    if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&new_size,
	&optlen)) goto getsockopt_failure;
    if (new_size > old_size) return true;
    else {
	if (new_size < old_size)
            log_warning("System call getsockopt(SO_SNDBUF) returned unexpected buffer size "
	    "(%d, after increasing it from %d to %d) on file descriptor %d.",
	    new_size, old_size, set_size, fd);
	return false;
    }
getsockopt_failure:
    log_warning("System call getsockopt(SO_SNDBUF) failed on file "
	"descriptor %d. (%s)", fd, strerror(errno));
    return false;
setsockopt_failure:
    log_warning("System call setsockopt(SO_SNDBUF) failed on file "
	"descriptor %d. (%s)", fd, strerror(errno));
    return false;
}

bool IPL4asp__PT_PROVIDER::ssl_verify_certificates()
{
  char str[SSL_CHARBUF_LENGTH];

  log_debug("entering IPL4asp__PT_PROVIDER::ssl_verify_certificates()");

  ssl_log_SSL_info();

  // Get the other side's certificate
  log_debug("Check certificate of the other party");
  X509 *cert = SSL_get_peer_certificate (ssl_current_ssl);
  if (cert != NULL) {

    {
      log_debug("Certificate information:");
      X509_NAME_oneline (X509_get_subject_name (cert), str, SSL_CHARBUF_LENGTH);
      log_debug("  subject: %s", str);
    }

    // We could do all sorts of certificate verification stuff here before
    // deallocating the certificate.

    // Just a basic check that the certificate is valid
    // Other checks (e.g. Name in certificate vs. hostname) shall be
    // done on application level
    if (ssl_verify_certificate)
      log_debug("Verification state is: %s", X509_verify_cert_error_string(SSL_get_verify_result(ssl_current_ssl)));
    X509_free (cert);

  } else
    log_warning("Other side does not have certificate.");

  log_debug("leaving IPL4asp__PT_PROVIDER::ssl_verify_certificates()");
  return true;
}

bool IPL4asp__PT_PROVIDER::ssl_actions_to_seed_PRNG() {
  struct stat randstat;

  if(RAND_status()) {
    log_debug("PRNG already initialized, no action needed");
    return true;
  }
  log_debug("Seeding PRND");
  // OpenSSL tries to use random devives automatically
  // these would not be necessary
  if (!stat("/dev/urandom", &randstat)) {
    log_debug("Using installed random device /dev/urandom for seeding the PRNG with %d bytes.", SSL_PRNG_LENGTH);
    if (RAND_load_file("/dev/urandom", SSL_PRNG_LENGTH)!=SSL_PRNG_LENGTH)
	{
	  debug("Could not read from /dev/urandom");
	  return false;
	}
  } else if (!stat("/dev/random", &randstat)) {
    log_debug("Using installed random device /dev/random for seeding the PRNG with %d bytes.", SSL_PRNG_LENGTH);
    if (RAND_load_file("/dev/random", SSL_PRNG_LENGTH)!=SSL_PRNG_LENGTH)
    {
      debug("Could not read from /dev/random");
      return false;
    }
  } else {
    /* Neither /dev/random nor /dev/urandom are present, so add
       entropy to the SSL PRNG a hard way. */
    log_warning("Solaris patches to provide random generation devices are not installed.\nSee http://www.openssl.org/support/faq.html \"Why do I get a \"PRNG not seeded\" error message?\"\nA workaround will be used.");
    for (int i = 0; i < 10000  &&  !RAND_status(); ++i) {
      char buf[4];
      struct timeval tv;
      gettimeofday(&tv, 0);
      buf[0] = tv.tv_usec & 0xF;
      buf[2] = (tv.tv_usec & 0xF0) >> 4;
      buf[3] = (tv.tv_usec & 0xF00) >> 8;
      buf[1] = (tv.tv_usec & 0xF000) >> 12;
      RAND_add(buf, sizeof buf, 0.1);
    }
    return true;
  }

  if(!RAND_status()) {
    debug("Could not seed the Pseudo Random Number Generator with enough data.");
    return false;
  } else {
    log_debug("PRNG successfully initialized.");
  }

  return true;
}


bool IPL4asp__PT_PROVIDER::ssl_init_SSL()
{
  if (ssl_initialized) {
    log_debug("SSL already initialized, no action needed");
    return true;
  }

  log_debug("Init SSL started");
  log_debug("Using %s (%lx)", SSLeay_version(SSLEAY_VERSION), OPENSSL_VERSION_NUMBER);


  SSL_library_init();          // initialize library
  SSL_load_error_strings();    // readable error messages

  // Create SSL method: both server and client understanding SSLv2, SSLv3, TLSv1
//  ssl_method = SSLv23_method();
//  if (ssl_method==NULL)
//  {
//    debug("SSL method creation failed.");
//    return false;
//  }
  // Create context
  ssl_ctx = SSL_CTX_new (SSLv23_method());
  if (ssl_ctx==NULL)
  {
    debug("SSL context creation failed.");
    return false;
  }

  // valid for all SSL objects created from this context afterwards
  if(ssl_certificate_file!=NULL) {
    log_debug("Loading certificate file");
    if(SSL_CTX_use_certificate_chain_file(ssl_ctx, ssl_certificate_file)!=1)
    {
      debug("Can't read certificate file ");
      return false;
    }
  }

  // valid for all SSL objects created from this context afterwards
  if(ssl_key_file!=NULL) {
    log_debug("Loading key file");
    if (ssl_current_client!=NULL) log_warning("Warning: race condition while setting current client object pointer");
    ssl_current_client=(IPL4asp__PT_PROVIDER *)this;
    if(ssl_password!=NULL)
      SSL_CTX_set_default_passwd_cb(ssl_ctx, ssl_password_cb);
    if(SSL_CTX_use_PrivateKey_file(ssl_ctx, ssl_key_file, SSL_FILETYPE_PEM)!=1)
    {
      debug("Can't read key file ");
      return false;
    }

    ssl_current_client=NULL;
  }

  if (ssl_trustedCAlist_file!=NULL) {
    log_debug("Loading trusted CA list file");
    if (SSL_CTX_load_verify_locations(ssl_ctx, ssl_trustedCAlist_file, NULL)!=1)
    {
      debug("Can't read trustedCAlist file ");
      return false;
    }
  }

  if (ssl_certificate_file!=NULL && ssl_key_file!=NULL) {
    log_debug("Check for consistency between private and public keys");
    if (SSL_CTX_check_private_key(ssl_ctx)!=1)
      log_warning("Private key does not match the certificate public key");
  }

  // check the other side's certificates
  if (ssl_verify_certificate) {
    log_debug("Setting verification behaviour: verification required and do not allow to continue on failure");
    SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, ssl_verify_callback);
  } else {
    log_debug("Setting verification behaviour: verification not required and do allow to continue on failure");
    SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, ssl_verify_callback);
  }

  if (ssl_cipher_list!=NULL) {
    log_debug("Setting ssl_cipher list restrictions");
    if (SSL_CTX_set_cipher_list(ssl_ctx, ssl_cipher_list)!=1)
    {
      debug("Cipher list restriction failed for %s", ssl_cipher_list);
      return false;
    }
  }

  if(!ssl_actions_to_seed_PRNG())
  {
    debug("Can't seed PRNG.");
    return false;
  }

  //Session id context can be set at any time but will be useful only for server(s)

  log_debug("Activate ssl_session resumption");
  log_debug("Context is: %s; length = %lu", ssl_server_auth_session_id_context, (unsigned long)strlen((const char*)ssl_server_auth_session_id_context));
  if (SSL_CTX_set_session_id_context(ssl_ctx, ssl_server_auth_session_id_context, strlen((const char*)ssl_server_auth_session_id_context))!=1)
  {
	debug("Activation of SSL ssl_session resumption failed");
	return false;
  }

  ssl_initialized=true;
  log_debug("Init SSL successfully finished");
  return true;
}


void IPL4asp__PT_PROVIDER::ssl_log_SSL_info()
{
  char str[SSL_CHARBUF_LENGTH];

  log_debug("Check SSL description");
//  ssl_cipher=SSL_get_current_cipher(ssl_current_ssl);
//  if (ssl_cipher!=NULL) {
    SSL_CIPHER_description(SSL_get_current_cipher(ssl_current_ssl), str, SSL_CHARBUF_LENGTH);
    {
      log_debug("SSL description:");
      log_debug("%s", str);
    }
//  }
}



// Log the SSL error and flush the error queue
// Can be used after the followings:
// SSL_connect(), SSL_accept(), SSL_do_handshake(),
// SSL_read(), SSL_peek(), or SSL_write()
int IPL4asp__PT_PROVIDER::ssl_getresult(int res)
{
  log_debug("SSL operation result:");
  int err = SSL_get_error(ssl_current_ssl, res);

  switch(err) {
  case SSL_ERROR_NONE:
    log_debug("SSL_ERROR_NONE");
    break;
  case SSL_ERROR_ZERO_RETURN:
    log_debug("SSL_ERROR_ZERO_RETURN");
    break;
  case SSL_ERROR_WANT_READ:
    log_debug("SSL_ERROR_WANT_READ");
    break;
  case SSL_ERROR_WANT_WRITE:
    log_debug("SSL_ERROR_WANT_WRITE");
    break;
  case SSL_ERROR_WANT_CONNECT:
    log_debug("SSL_ERROR_WANT_CONNECT");
    break;
  case SSL_ERROR_WANT_ACCEPT:
    log_debug("SSL_ERROR_WANT_ACCEPT");
    break;
  case SSL_ERROR_WANT_X509_LOOKUP:
    log_debug("SSL_ERROR_WANT_X509_LOOKUP");
    break;
  case SSL_ERROR_SYSCALL:
    log_debug("SSL_ERROR_SYSCALL");
    log_debug("EOF was observed that violates the protocol, peer disconnected; treated as a normal disconnect");
    return SSL_ERROR_ZERO_RETURN;
    break;
  case SSL_ERROR_SSL:
    log_debug("SSL_ERROR_SSL");
    break;
  default:
    debug("Unknown SSL error code: %d", err);
    //--ethnba -may need to return to skip reading the error string
  }
  // get the copy of the error string in readable format
  unsigned long e=ERR_get_error();
  while (e) {
    log_debug("SSL error queue content:");
    log_debug("  Library:  %s", ERR_lib_error_string(e));
    log_debug("  Function: %s", ERR_func_error_string(e));
    log_debug("  Reason:   %s", ERR_reason_error_string(e));
    e=ERR_get_error();
  }
  //It does the same but more simple:
  // ERR_print_errors_fp(stderr);
  return err;
}

int   IPL4asp__PT_PROVIDER::ssl_verify_certificates_at_handshake(int preverify_ok, X509_STORE_CTX *ssl_ctx) {
   // don't care by default
   return -1;
}

// Callback function used by OpenSSL.
// Called when a password is needed to decrypt the private key file.
// NOTE: not thread safe
int IPL4asp__PT_PROVIDER::ssl_password_cb(char *buf, int num, int rwflag,void *userdata) {

  if (ssl_current_client!=NULL) {
     char *ssl_client_password;
     ssl_client_password=((IPL4asp__PT_PROVIDER *)ssl_current_client)->get_ssl_password();
     if(ssl_client_password==NULL) return 0;
     const char* pass = (const char*) ssl_client_password;
     int pass_len = strlen(pass) + 1;
     if (num < pass_len) return 0;

     strcpy(buf, pass);
     return(strlen(pass));
  } else { // go on with no password set
     fprintf(stderr, "Warning: no current SSL object found but ssl_password_cb is called, programming error\n");
     return 0;
  }
}

// Callback function used by OpenSSL.
// Called during SSL handshake with a pre-verification status.
int IPL4asp__PT_PROVIDER::ssl_verify_callback(int preverify_ok, X509_STORE_CTX *ssl_ctx)
{
  SSL     *ssl_pointer;
  SSL_CTX *ctx_pointer;
  int user_result;

  ssl_pointer = (SSL *)X509_STORE_CTX_get_ex_data(ssl_ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
  ctx_pointer = SSL_get_SSL_CTX(ssl_pointer);

  if (ssl_current_client!=NULL) {
     user_result=((IPL4asp__PT_PROVIDER *)ssl_current_client)->ssl_verify_certificates_at_handshake(preverify_ok, ssl_ctx);
     if (user_result>=0) return user_result;
  } else { // go on with default authentication
     fprintf(stderr, "Warning: no current SSL object found but ssl_verify_callback is called, programming error\n");
  }

  // if ssl_verifiycertificate == "no", then always accept connections
  if (SSL_CTX_get_verify_mode(ctx_pointer) && SSL_VERIFY_NONE)
    return 1;
  // if ssl_verifiycertificate == "yes", then accept connections only if the
  // certificate is valid
  else if (SSL_CTX_get_verify_mode(ctx_pointer) && SSL_VERIFY_PEER)
    return preverify_ok;
  // something went wrong
  else
    return 0;
}

bool IPL4asp__PT_PROVIDER::setSslObj(int connId, SSL* sslObj)
{
  debug("IPL4asp__PT_PROVIDER::setSslObj enter: connId %d",
        connId);
  testIfInitialized();
  if (!isConnIdValid(connId)) {
    debug("IPL4asp__PT_PROVIDER::setSslObj: invalid connId: %i", connId);
    return false;
  }
  sockList[connId].sslObj = sslObj;
  return true;
} // IPL4asp__PT_PROVIDER::setSslObj

bool IPL4asp__PT_PROVIDER::getSslObj(int connId, SSL*& sslObj)
{
  debug("IPL4asp__PT_PROVIDER::getSslObj enter: socket %d", connId);
  testIfInitialized();
  if (!isConnIdValid(connId)) {
    debug("IPL4asp__PT_PROVIDER::getSslObj: invalid connId: %i", connId);
    return false;
  }
  sslObj = sockList[connId].sslObj;
  return true;
} // IPL4asp__PT_PROVIDER::getSslObj

#endif
////////////////////////////////////////////////////////////////////////
/////    Default SSL log functions
////////////////////////////////////////////////////////////////////////
void IPL4asp__PT_PROVIDER::log_debug(const char *fmt, ...) const
{
  if (debugAllowed) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event_str("IPL4asp__PT_PROVIDER - SSL socket: ");
    va_list args;
    va_start(args, fmt);
    TTCN_Logger::log_event_va_list(fmt, args);
    va_end(args);
    TTCN_Logger::end_event();
  }
}

void IPL4asp__PT_PROVIDER::log_warning(const char *fmt, ...) const
{
  TTCN_Logger::begin_event(TTCN_WARNING);
  TTCN_Logger::log_event_str("IPL4asp__PT_PROVIDER - SSL socket Warning: ");
  va_list args;
  va_start(args, fmt);
  TTCN_Logger::log_event_va_list(fmt, args);
  va_end(args);
  TTCN_Logger::end_event();
}


void IPL4asp__PT_PROVIDER::log_hex(const char *prompt, const unsigned char *msg,
  size_t length) const
{
  if (debugAllowed) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event_str("IPL4asp__PT_PROVIDER - SSL socket: ");
    if (prompt != NULL) TTCN_Logger::log_event_str(prompt);
    TTCN_Logger::log_event("Size: %lu, Msg:", (unsigned long)length);
    for (size_t i = 0; i < length; i++) TTCN_Logger::log_event(" %02x", msg[i]);
    TTCN_Logger::end_event();
  }
}

#ifdef USE_IPL4_EIN_SCTP

IPL4asp__PT_PROVIDER *IPL4asp__PT_PROVIDER::port_ptr=NULL;

void IPL4asp__PT_PROVIDER::do_bind()
{
  unsigned char bindRequestCounter = 0;
  exiting = FALSE;
  int otherId = SCTP_ID;

  log_debug("entering IPL4asp__PT_PROVIDER::do_bind() userId: %d, otherId: %d, userInstanceId: %d, sctpInstanceId: %d, cpManagerIPA %s", userId, otherId, userInstanceId, sctpInstanceId,(const char*)cpManagerIPA);
  
  SS7Common::setDebug(debugAllowed);
  
  SS7Common::connect_ein(userId, otherId, (const TEXT_T*)cpManagerIPA, userInstanceId, sctpInstanceId);

  if (userId == GENERATE_BY_CP_ID) {
    userId = SS7Common::getUsedMPOwner();  
    userInstanceId = SS7Common::getUsedMPInstance();
  }

#ifdef EIN_R3B

EINSS7_00SCTPFUNCNEW_T newFunc;
memset(&newFunc,0,sizeof(newFunc));
newFunc.EINSS7_00SctpSetEpAliasConf= EINSS7_00SCTPSETEPALIASCONF;
newFunc.EINSS7_00SctpSetUserCongestionLevelConf= EINSS7_00SCTPSETUSERCONGESTIONLEVELCONF;
newFunc.EINSS7_00SctpGetUserCongestionLevelConf= EINSS7_00SCTPGETUSERCONGESTIONLEVELCONF;
newFunc.EINSS7_00SctpRedirectInd= EINSS7_00SCTPREDIRECTIND;
newFunc.EINSS7_00SctpInfoConf= EINSS7_00SCTPINFOCONF;
newFunc.EINSS7_00SctpInfoInd= EINSS7_00SCTPINFOIND;
newFunc.EINSS7_00SctpUpdateConf= EINSS7_00SCTPUPDATECONF;
EINSS7_00SCTPRegFuncNew(&newFunc);

#endif
  USHORT_T ret_val;
  MSG_T msg;
  msg.receiver = userId;


    do
    {
    bindRequestCounter++;
    /* Bind to stack*/
      // hardcoded value: ANSI 1996
      log_debug("Sending:\n"
        "EINSS7_00SctpBindReq(\n"
         "userId=%d\n"
         "sctpInstanceId=%d\n"
         "api_type=%d"
         "bind_type=%d)\n",
         userId,
         sctpInstanceId,
         EINSS7_00SCTP_APITYPE_API,
         EINSS7_00SCTP_BIND_IF_NOT_BOUND);
      ret_val = EINSS7_00SctpBindReq( userId, 
                                      sctpInstanceId,
                                      EINSS7_00SCTP_APITYPE_API,
                                      EINSS7_00SCTP_BIND_IF_NOT_BOUND
                                      );

      if (ret_val != RETURN_OK)
        TTCN_error("EINSS7_00SctpBindReq failed: %d (%s)",
          ret_val, get_ein_sctp_error_message(ret_val,API_RETURN_CODES));
      else
        log_debug("EINSS7_00SctpBindReq was successful");

      log_debug("IPL4asp__PT_PROVIDER::do_bind(): Waiting for BindConf for user %d...", userId);

      do
      {
        ret_val = SS7Common::CallMsgRecv(&msg);
      }
      while(ret_val == MSG_TIMEOUT);

      if (ret_val != MSG_RECEIVE_OK)
        TTCN_error("IPL4asp__PT_PROVIDER::do_bind(): CallMsgRecv failed: %d (%s)",
          ret_val, SS7Common::get_ein_error_message(ret_val));
      /* Print the message into the log */
      log_msg("IPL4asp__PT_PROVIDER::do_bind(): Received message", &msg);

      /*Init the global variable*/
      port_ptr = this;
      ret_val = EINSS7_00SctpHandleInd(&msg);
      if (ret_val != RETURN_OK)
        TTCN_error("IPL4 test port (%s): EINSS7_00SctpHandleInd failed: "
          "%d (%s) Message is ignored.", get_name(), ret_val,
          SS7Common::get_ein_error_message(ret_val));
      else
        log_debug("PL4asp__PT_PROVIDER::do_bind(): message processing "
          "was successful");

      /* Release the buffer */

      ret_val = SS7Common::CallReleaseMsgBuffer(&msg);
      if (ret_val != RETURN_OK)
        TTCN_warning("IPL4 test port (%s): CallReleaseMsgBuffer "
          "failed: %d (%s)", get_name(), ret_val,
          SS7Common::get_ein_error_message(ret_val));
      else
        log_debug("IPL4asp__PT_PROVIDER::do_bind(): message was released "
          "successfully");
      /* Reset the global variable */
      port_ptr = NULL;

      if (bindResult != EINSS7_00SCTP_OK)
        TTCN_warning("Bind failed: %d, %s, tries: %d.", bindResult,
            get_ein_sctp_error_message(ret_val,API_RETURN_CODES), bindRequestCounter);
      else {
        log_debug("The bind was successful.");

     }
    } while (bindResult != EINSS7_00SCTP_NTF_OK
                                  && bindRequestCounter <= 3);


  create_pipes();

  log_debug("IPL4asp__PT_PROVIDER::do_bind() : starting the second thread.");
  start_thread();
}

void IPL4asp__PT_PROVIDER::create_pipes()
{
    if (pipe(pipe_to_TTCN_thread_fds))
        TTCN_error("IPL4asp__PT_PROVIDER::create_pipe() @place1: pipe system call failed");
    if (pipe(pipe_to_TTCN_thread_log_fds))
        TTCN_error("IPL4asp__PT_PROVIDER::create_pipe() @place1: pipe system call failed");


    Handler_Add_Fd_Read(pipe_to_TTCN_thread_fds[0]);
    Handler_Add_Fd_Read(pipe_to_TTCN_thread_log_fds[0]);

    if (pipe(pipe_to_EIN_thread_fds))
        TTCN_error("IPL4asp__PT_PROVIDER::create_pipe() @place2: pipe system call failed");
}

void IPL4asp__PT_PROVIDER::start_thread()
{
  if (thread_started)
    return;
  if (pthread_create(&thread, NULL, IPL4asp__PT_PROVIDER::thread_main, this))
    TTCN_error("TCAPasp_PT_EIN_Interface::launch_thread(): pthread_create failed.");
  thread_started = TRUE;
}

void *IPL4asp__PT_PROVIDER::thread_main(void *arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  static_cast<IPL4asp__PT_PROVIDER*>(arg)->ein_receive_loop();
  return NULL;
}

void IPL4asp__PT_PROVIDER::ein_receive_loop()
{
  for (; !exiting; )
  {
    MSG_T msg;
    USHORT_T ret_val;
    msg.receiver = userId;

    do {
      ret_val = SS7Common::CallMsgRecv(&msg);
    } while(!exiting && ret_val == MSG_TIMEOUT);

    //log_debug("A message arrived from the stack.");

    if(exiting)
      break;

    if (ret_val != MSG_RECEIVE_OK)
    {
      log_thread(TTCN_DEBUG,"IPL4asp__PT_PROVIDER::ein_receive_loop(): EINSS7CpMsgRecv_r failed: %d (%s)",
        ret_val, SS7Common::get_ein_error_message(ret_val));
      return;
    }

    write_pipe(pipe_to_TTCN_thread_fds);
    if(exiting)
      break;

    read_pipe(pipe_to_EIN_thread_fds);

    if(exiting)
      break;

    log_msg("IPL4asp__PT_PROVIDER::ein_receive_loop(): Received message", &msg);

    /* Set the global variable so that the EIN callback functions reach
     * our member functions */
    port_ptr = this;
    /* Pass the message to the EIN stack TCAP layer */
    log_thread(TTCN_DEBUG,"IPL4asp__PT_PROVIDER::ein_receive_loop(): passing incoming message to EIN");


    ret_val = EINSS7_00SctpHandleInd(&msg);
    if (ret_val != RETURN_OK)
      log_thread(TTCN_WARNING,"IPL4 test port (%s): EINSS7_00SctpHandleInd failed: "
        "%d (%s) Message is ignored.", get_name(), ret_val,
        SS7Common::get_ein_error_message(ret_val));
    else log_thread(TTCN_DEBUG,"IPL4asp__PT_PROVIDER::ein_receive_loop(): message processing "
        "was successful");

    port_ptr = NULL;
    write_pipe(pipe_to_TTCN_thread_fds);

    /* Release the buffer */
    ret_val = SS7Common::CallReleaseMsgBuffer(&msg);
    if (ret_val != RETURN_OK)
      log_thread(TTCN_WARNING,"IPL4 test port (%s): CallReleaseMsgBuffer "
        "failed: %d (%s)", get_name(), ret_val,
        SS7Common::get_ein_error_message(ret_val));
    else log_thread(TTCN_DEBUG,"IPL4asp__PT_PROVIDER::ein_receive_loop(): message was released "
        "successfully");
    /* Reset the global variable */
    port_ptr = NULL;
  }

  log_thread(TTCN_DEBUG,"IPL4asp__PT_PROVIDER::ein_receive_loop(): exiting... ");
}

void IPL4asp__PT_PROVIDER::destroy_pipes()
{
    Handler_Remove_Fd_Read(pipe_to_TTCN_thread_fds[0]);
    Handler_Remove_Fd_Read(pipe_to_TTCN_thread_log_fds[0]);

    close(pipe_to_TTCN_thread_fds[0]);
    pipe_to_TTCN_thread_fds[0] = -1;
    close(pipe_to_TTCN_thread_fds[1]);
    pipe_to_TTCN_thread_fds[1] = -1;
    close(pipe_to_TTCN_thread_log_fds[0]);
    pipe_to_TTCN_thread_log_fds[0] = -1;
    close(pipe_to_TTCN_thread_log_fds[1]);
    pipe_to_TTCN_thread_log_fds[1] = -1;
    close(pipe_to_EIN_thread_fds[0]);
    pipe_to_EIN_thread_fds[0] = -1;
    close(pipe_to_EIN_thread_fds[1]);
    pipe_to_EIN_thread_fds[1] = -1;
}


void IPL4asp__PT_PROVIDER::read_pipe(int pipe_fds[])
{
    //log_debug("TCAPasp_PT_EIN_Interface: Waiting in read_pipe()...");
    unsigned char buf;
    if (read(pipe_fds[0], &buf, 1) != 1){
      exiting=TRUE;
      TTCN_warning("IPL4asp__PT_PROVIDER::read_pipe(): read system call failed");
   }      
}


void IPL4asp__PT_PROVIDER::write_pipe(int pipe_fds[])
{
    //log_debug("TCAPasp_PT_EIN_Interface: Writing in the pipe...");
    unsigned char buf = '\0';
    if (write(pipe_fds[1], &buf, 1) != 1){
      exiting=TRUE;
      TTCN_warning("IPL4asp__PT_PROVIDER::write_pipe(): write system call failed");
    }
}

void IPL4asp__PT_PROVIDER::log_thread(TTCN_Logger::Severity severity, const char *fmt, ...){
  if(exiting) {return;}
  if (severity!=TTCN_DEBUG || debugAllowed)
  {
    thread_log *log_msg=(thread_log *)Malloc(sizeof(thread_log));
    va_list args;
    va_start(args, fmt);
    log_msg->severity=severity;
    log_msg->msg=mprintf_va_list(fmt, args);
    int len = write(pipe_to_TTCN_thread_log_fds[1], &log_msg, sizeof(thread_log*));
    if(exiting) {return;}
    if (len == 0) {
      TTCN_error("Internal queue shutdown");
    } else if (len < 0) {
      TTCN_error("Error while writing to internal queue (errno = %d)", errno);
    } else if (len != sizeof(thread_log*)){
      TTCN_error("Partial write to the queue: %d bytes written (errno = %d)",
          len, errno);
    }
    
  }
}

void IPL4asp__PT_PROVIDER::log_thread_msg(const char *header, const MSG_T *msg){
  if (debugAllowed)
  {
    char *msgstr=mprintf("TCAP test port (%s): ", get_name());
    if (header != NULL) msgstr=mputprintf(msgstr,"%s: ", header);
    msgstr=mputprintf(msgstr,"{");
    msgstr=mputprintf(msgstr," Sender: %d,", msg->sender);
    msgstr=mputprintf(msgstr," Receiver: %d,", msg->receiver);
    msgstr=mputprintf(msgstr," Primitive: %d,", msg->primitive);
    msgstr=mputprintf(msgstr," Size: %d,", msg->size);
    msgstr=mputprintf(msgstr," Message:");
    for (USHORT_T i = 0; i < msg->size; i++)
      msgstr=mputprintf(msgstr," %02X", msg->msg_p[i]);
    msgstr=mputprintf(msgstr," }");
    log_thread(TTCN_DEBUG,msgstr);
    Free(msgstr);
  }
}

void IPL4asp__PT_PROVIDER::do_unbind()
{
  USHORT_T ret_val;
  int otherId = SCTP_ID;

  /* UnBind */
  if (bindResult == EINSS7_00SCTP_NTF_OK)
  {
      exiting = TRUE;

      log_debug("Sending:\n"
        "EINSS7_00SctpBindReq(\n"
         "sctpInstanceId=%d)",
         sctpInstanceId);
      ret_val = EINSS7_00SctpUnbindReq(sctpInstanceId);
      switch (ret_val)
      {
        case RETURN_OK:
          log_debug("EINSS7_00SctpBindReq(%d) was successful", sctpInstanceId);
          break;
        case MSG_NOT_CONNECTED:
          TTCN_warning("IPL4 test port (%s): "
            "The EIN stack was not bound", get_name());
          break;
        default:
          TTCN_error("EINSS7_00SctpBindReq(%d) failed: %d (%s)",
            sctpInstanceId, ret_val,
            SS7Common::get_ein_error_message(ret_val));
      }
  }else
    TTCN_warning("IPL4 test port (%s): "
      "was not bound.", get_name());
  bindResult = EINSS7_00SCTP_NOT_BOUND;

  // wait 0.5 sec to unbind reach the stack
  // before closing connection
  // 0.5 sec was recommended by Ulf.Melin@tietoenator.com
  // TR ID: 6801
  usleep (500000);
  /* Disconnect from EIN stack */

  SS7Common::disconnect_ein(userId, otherId, userInstanceId, sctpInstanceId);

  /* Clean up resources */
  log_debug("IPL4asp__PT_PROVIDER::do_unbind() : destroying the pipes.");
  destroy_pipes();

  ein_connected = false;
  thread_started = false;
}


Socket__API__Definitions::Result IPL4asp__PT_PROVIDER::Listen_einsctp(const HostName& locName,
  const PortNumber& locPort, int next_action , const HostName& remName,
  const PortNumber& remPort,  const IPL4asp__Types::OptionList& options, const IPL4asp__Types::SocketList &sock_list){

  Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
  SockAddr sockAddr;
  socklen_t sockAddrLen;
  //struct hostent *hp = NULL;
  int hp = 0;

  debug("f__IPL4__PROVIDER__listen: enter %s %d", (const char *) locName, (int) locPort);
  testIfInitialized();

  int addr_index=-1;
  int num_of_addr=1;
  int config_group=0;
  for(int i=0; i<options.size_of();i++){
    if(options[i].get_selection()==Option::ALT_sctpAdditionalLocalAddresses){
      addr_index=i;
      num_of_addr+=options[i].sctpAdditionalLocalAddresses().size_of();
//      break;
    }
    if(options[i].get_selection()==Option::ALT_sctpEINConfigGroup){
      config_group=options[i].sctpEINConfigGroup();
    }    
  }

  
  
  if (locPort < -1 || locPort > 65535)
    RETURN_ERROR_STACK(PortError::ERROR__INVALID__INPUT__PARAMETER);
  
  IPADDRESS_T *ip_struct=(IPADDRESS_T *)Malloc(num_of_addr*sizeof(IPADDRESS_T));
  memset((void *)ip_struct,0,num_of_addr*sizeof(IPADDRESS_T));

  hp=SetLocalSockAddr("f__IPL4__PROVIDER__listen",*this,AF_INET,locName, locPort, sockAddr, sockAddrLen);

  if (hp == -1) {
    SET_OS_ERROR_CODE;
    Free(ip_struct);
    RETURN_ERROR_STACK(PortError::ERROR__HOSTNAME);
  }

  char ip_addr[46];
  memset((void *)ip_addr,0,46);
  if(hp == AF_INET){
    inet_ntop(AF_INET,&sockAddr.v4.sin_addr.s_addr,ip_addr,46);
    ip_struct[0].addrType=EINSS7_00SCTP_IPV4;
  }
#ifdef USE_IPV6
  else {
    inet_ntop(AF_INET6,sockAddr.v6.sin6_addr.s6_addr,ip_addr,46);
    ip_struct[0].addrType=EINSS7_00SCTP_IPV6;
  }
#endif  
  ip_struct[0].addr=(unsigned char*)mcopystr(ip_addr);
  ip_struct[0].addrLength=strlen(ip_addr)+1;

  for(int i=1; i<num_of_addr;i++){
    hp=SetLocalSockAddr("f__IPL4__PROVIDER__listen",*this,AF_INET,options[addr_index].sctpAdditionalLocalAddresses()[i-1], locPort, sockAddr, sockAddrLen);

    if (hp == -1) {
      SET_OS_ERROR_CODE;
      for(int k=0;k<i;k++){
        Free(ip_struct[k].addr);
      }
      Free(ip_struct);
      RETURN_ERROR_STACK(PortError::ERROR__HOSTNAME);
    }

    memset((void *)ip_addr,0,46);
    if(hp == AF_INET){
      inet_ntop(AF_INET,&sockAddr.v4.sin_addr.s_addr,ip_addr,46);
      ip_struct[i].addrType=EINSS7_00SCTP_IPV4;
    }
#ifdef USE_IPV6
    else {
      inet_ntop(AF_INET6,sockAddr.v6.sin6_addr.s6_addr,ip_addr,46);
      ip_struct[i].addrType=EINSS7_00SCTP_IPV6;
    }
#endif  
    ip_struct[i].addr=(unsigned char*)mcopystr(ip_addr);
    ip_struct[i].addrLength=strlen(ip_addr)+1;
  }
  
  int conn_id=ConnAddEin(next_action==SockDesc::ACTION_CONNECT?IPL4asp_SCTP:IPL4asp_SCTP_LISTEN,SockDesc::SOCK_NOT_KNOWN,-1,ip_addr,locPort,remName,remPort,next_action);
  sockList[conn_id].remote_addr_list=sock_list;
  
  
  USHORT_T init_result=EINSS7_00SctpInitializeGroupIdReq(
    userId,
    sctpInstanceId,
    next_action==SockDesc::ACTION_CONNECT,
    (int) globalConnOpts.sinit_max_instreams,
    (int) globalConnOpts.sinit_num_ostreams,
    conn_id,
    locPort,
    num_of_addr,
    ip_struct,
    0,
    config_group
  );
      for(int k=0;k<num_of_addr;k++){
        Free(ip_struct[k].addr);
      }
      Free(ip_struct);
  
  if(EINSS7_00SCTP_OK!=init_result){
    ConnDelEin(conn_id);
    result.errorCode()=PortError::ERROR__GENERAL;
    result.os__error__code()=init_result;
    result.os__error__text()=get_ein_sctp_error_message(init_result,API_RETURN_CODES);
    return result;
  }
  
    result.connId()=conn_id;
    result.errorCode()=PortError::ERROR__TEMPORARILY__UNAVAILABLE;
  if(globalConnOpts.extendedPortEvents == GlobalConnOpts::YES) {
    ASP__Event event;
    event.result() = result;
    incoming_message(event);
  }
  return result;
  
}  

int IPL4asp__PT_PROVIDER::ConnAddEin(SockType type,
  int assoc_enpoint, int parentIdx, const HostName& locName,
  const PortNumber& locPort,const HostName& remName,
  const PortNumber& remPort, int next_action)
{
  debug("IPL4asp__PT_PROVIDER::ConnAdd: enter: assoc_enpoint: %d, parentIx: %d",
    assoc_enpoint, parentIdx);
  testIfInitialized();
  if (sockListCnt + N_RECENTLY_CLOSED >= sockListSize - 1 || sockList == NULL) {
    unsigned int sz = sockListSize;
    if (sockList != NULL) sz *= 2;
    SockDesc *newSockList =
      (SockDesc *)Realloc(sockList, sizeof(SockDesc) * sz);
    int i0 = (sockList == 0) ? 1 : sockListSize;
    sockList = newSockList;
    sockListSize = sz;
    debug("IPL4asp__PT_PROVIDER::ConnAdd: new sockListSize: %d", sockListSize);
    int j = firstFreeSock;
    for ( int i = sockListSize - 1; i >= i0; --i ) {
      memset(sockList + i, 0, sizeof (sockList[i]));
      sockList[i].sock = SockDesc::SOCK_NONEX;
      sockList[i].nextFree = j;
      j = i;
    }
    firstFreeSock = j;
    if (lastFreeSock == -1) lastFreeSock = sockListSize - 1;
  }

  int i = firstFreeSock;
  debug("IPL4asp__PT_PROVIDER::ConnAdd: connId: %d", i);

  if (parentIdx != -1) { // inherit the listener's properties
    sockList[i].userData = sockList[parentIdx].userData;
    sockList[i].getMsgLen = sockList[parentIdx].getMsgLen;
    sockList[i].parentIdx = parentIdx;
    sockList[parentIdx].ref_count++;
    sockList[i].msgLenArgs =
      new ro__integer(*sockList[parentIdx].msgLenArgs);
  } else { // otherwise initialize to defaults
    sockList[i].userData = 0;
    sockList[i].getMsgLen = defaultGetMsgLen;
    sockList[i].parentIdx = -1;
    sockList[i].msgLenArgs = new ro__integer(*defaultMsgLenArgs);
  }
  if (sockList[i].msgLenArgs == NULL)
    return -1;
  sockList[i].msgLen = -1;

//  ae2IndexMap[assoc_enpoint] = i;
  sockList[i].ref_count=0;
  sockList[i].type = type;
  sockList[i].localaddr=new CHARSTRING(locName);
  sockList[i].localport=new PortNumber(locPort);
  sockList[i].remoteaddr=new CHARSTRING(remName);
  sockList[i].remoteport=new PortNumber(remPort);
  sockList[i].next_action=next_action;
  sockList[i].remote_addr_index=0;
  sockList[i].remote_addr_list=IPL4asp__Types::SocketList(NULL_VALUE);



  switch (type) {
  case IPL4asp_SCTP_LISTEN:
    sockList[i].buf = NULL;
    sockList[i].assocIdList = NULL;
    sockList[i].cnt = 0;
    break;
  case IPL4asp_SCTP:
    sockList[i].buf = (TTCN_Buffer **)Malloc(sizeof(TTCN_Buffer *));
    *sockList[i].buf = new TTCN_Buffer;
    if (*sockList[i].buf == NULL) {
      debug("IPL4asp__PT_PROVIDER::ConnAdd: failed to add socket %d", assoc_enpoint);
      Free(sockList[i].buf); sockList[i].buf = 0;
      return -1;
    }
    sockList[i].assocIdList = (sctp_assoc_t *)Malloc(sizeof(sctp_assoc_t));
    sockList[i].cnt = 1;
    break;
  default:
    break;
  }

  sockList[i].sock = assoc_enpoint;
  firstFreeSock = sockList[i].nextFree;
  sockList[i].nextFree = -1;
  ++sockListCnt;
  if(sockListCnt==1) {lonely_conn_id=i;}
  else {lonely_conn_id=-1;}
  debug("IPL4asp__PT_PROVIDER::ConnAdd: leave: sockListCnt: %i", sockListCnt);

  return i;
} // IPL4asp__PT::ConnAdd


int IPL4asp__PT_PROVIDER::ConnDelEin(int connId)
{
  debug("IPL4asp__PT_PROVIDER::ConnDel: enter: connId: %d", connId);

  if(sockList[connId].parentIdx!=-1){
    int parentIdx=sockList[connId].parentIdx;
    sockList[parentIdx].ref_count--;
    if(sockList[parentIdx].ref_count==0 && sockList[parentIdx].next_action==SockDesc::ACTION_DELETE){
      EINSS7_00SctpDestroyReq(sockList[parentIdx].endpoint_id);
      ep2IndexMap.erase(sockList[parentIdx].endpoint_id);
      ConnDelEin(parentIdx);
    }
  } else {
    if(sockList[connId].ref_count!=0) {
      sockList[connId].next_action=SockDesc::ACTION_DELETE;
      return connId;
    }
    EINSS7_00SctpDestroyReq(sockList[connId].endpoint_id);
  }

  sockList[connId].clear();
  sockList[lastFreeSock].nextFree = connId;
  lastFreeSock = connId;
  sockListCnt--;
  if(sockListCnt==1) {
    unsigned int i=0;
    while(i<sockListSize && sockList[i].sock!=SockDesc::SOCK_NONEX){
      i++;
    }
    lonely_conn_id=i;
  }
  else {lonely_conn_id=-1;}

  return connId;
} // IPL4asp__PT_PROVIDER::ConnDel


USHORT_T  IPL4asp__PT_PROVIDER::SctpInitializeConf(
    UCHAR_T returnCode,
    ULONG_T sctpEndpointId,
    USHORT_T assignedMis,
    USHORT_T assignedOsServerMode,
    USHORT_T maxOs,
    ULONG_T pmtu,
    ULONG_T mappingKey,
    USHORT_T localPort
){
  log_debug("SctpInitializeConf sctpEndpointId %ul  mappingKey %ul returnCode %d ",sctpEndpointId,mappingKey,returnCode);

  if(EINSS7_00SCTP_NTF_DUPLICATE_INIT==returnCode && sockList[mappingKey].next_action==SockDesc::ACTION_CONNECT){
    std::map<int,int>::iterator it = ep2IndexMap.find(sctpEndpointId);
    int parent_id=it->second;
    sockList[parent_id].ref_count++;
    sockList[mappingKey].parentIdx=parent_id;
    
  } else if(EINSS7_00SCTP_NTF_OK!=returnCode){
    Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
    ConnDelEin(mappingKey);
    EINSS7_00SctpDestroyReq(sctpEndpointId);
    result.errorCode()=PortError::ERROR__GENERAL;
    result.os__error__code()=returnCode;
    result.os__error__text()=get_ein_sctp_error_message(returnCode,CONF_RETURNCODE);
    result.connId()=mappingKey;
    ASP__Event event;
    event.result() = result;
    incoming_message(event);
    return RETURN_OK;
  }
  
  
  sockList[mappingKey].sock=sctpEndpointId;
  sockList[mappingKey].maxOs=maxOs;
  sockList[mappingKey].endpoint_id=sctpEndpointId;
  *(sockList[mappingKey].localport)=localPort;
  if(sockList[mappingKey].next_action==SockDesc::ACTION_CONNECT){
    if(EINSS7_00SCTP_NTF_OK==returnCode){
      sockList[mappingKey].ref_count=0;
      ep2IndexMap[sctpEndpointId] = mappingKey;
    }
    char rem_addr[46];
    memset((void *)rem_addr,0,46);
    IPADDRESS_T ip_struct;
    strcpy(rem_addr,(const char*)*(sockList[mappingKey].remoteaddr));
    if(!strchr(rem_addr,':')){
      ip_struct.addrType=EINSS7_00SCTP_IPV4;
    }
#ifdef USE_IPV6
    else {
      ip_struct.addrType=EINSS7_00SCTP_IPV6;
    }
#endif 
    ip_struct.addr=(unsigned char*)rem_addr;
    ip_struct.addrLength=strlen(rem_addr)+1;
    
    USHORT_T req_result=EINSS7_00SctpAssociateReq(
      sctpEndpointId,
      maxOs<(int) globalConnOpts.sinit_num_ostreams?maxOs:(int) globalConnOpts.sinit_num_ostreams,
      mappingKey,
      *(sockList[mappingKey].remoteport),
      ip_struct
    );

    if(EINSS7_00SCTP_OK!=req_result){
      Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
      ConnDelEin(mappingKey);
      EINSS7_00SctpDestroyReq(sctpEndpointId);
      result.errorCode()=PortError::ERROR__GENERAL;
      result.os__error__code()=returnCode;
      result.os__error__text()=get_ein_sctp_error_message(returnCode,API_RETURN_CODES);
      result.connId()=mappingKey;
      ASP__Event event;
      event.result() = result;
      incoming_message(event);
    }

  } else {
    Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
    sockList[mappingKey].next_action=SockDesc::ACTION_NONE;
    sockList[mappingKey].ref_count=0;
    ep2IndexMap[sctpEndpointId] = mappingKey;
    result.connId()=mappingKey;
    result.errorCode()=PortError::ERROR__AVAILABLE;
    ASP__Event event;
    event.result() = result;
    incoming_message(event);
  }
  return RETURN_OK;

}

USHORT_T  IPL4asp__PT_PROVIDER::SctpAssociateConf(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T ulpKey
){
  log_debug(" SctpAssociateConf assocId %ul  ulpKey %ul returnCode %d ",assocId,ulpKey,returnCode);

  if(EINSS7_00SCTP_NTF_OK!=returnCode){
    Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
    ConnDelEin(ulpKey);
    EINSS7_00SctpDestroyReq(sockList[ulpKey].endpoint_id);
    result.errorCode()=PortError::ERROR__GENERAL;
    result.os__error__code()=returnCode;
    result.os__error__text()=get_ein_sctp_error_message(returnCode,CONF_RETURNCODE);
    result.connId()=ulpKey;
    ASP__Event event;
    event.result() = result;
    incoming_message(event);
    return RETURN_OK;
  }
  sockList[ulpKey].sock=assocId;
    Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
//    sockList[ulpKey].next_action=SockDesc::ACTION_NONE;
    result.connId()=ulpKey;
    result.errorCode()=PortError::ERROR__AVAILABLE;
    ASP__Event event;
    event.result() = result;
    incoming_message(event);

  return RETURN_OK;

}

USHORT_T  IPL4asp__PT_PROVIDER::SctpCommUpInd(
    ULONG_T sctpEndpointId,
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T origin,
    USHORT_T outboundStreams,
    USHORT_T inboundStreams,
    USHORT_T remotePort,
    UCHAR_T numOfRemoteIpAddrs,
    IPADDRESS_T * remoteIpAddrList_sp
){
  log_debug(" SctpCommUpInd assocId %ul  ulpKey %ul sctpEndpointId %d origin %d ",assocId,ulpKey,sctpEndpointId,origin);
  if(isConnIdValid(ulpKey) && sockList[ulpKey].sock==(int)assocId
      && sockList[ulpKey].endpoint_id==(int)sctpEndpointId ){
    
    sockList[ulpKey].next_action=SockDesc::ACTION_NONE;
    ASP__Event event;
    event.sctpEvent().sctpAssocChange().clientId() = ulpKey;
    event.sctpEvent().sctpAssocChange().proto().sctp() = SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
    event.sctpEvent().sctpAssocChange().sac__state() = IPL4asp__Types::SAC__STATE::SCTP__COMM__UP;
    incoming_message(event);
   
  } else {
  // new connection
    std::map<int,int>::iterator it = ep2IndexMap.find(sctpEndpointId);
    int parent_id=it->second;
    int conn_id;
    if(sockList[parent_id].next_action==SockDesc::ACTION_DELETE){
      conn_id=ConnAddEin(IPL4asp_SCTP,assocId,parent_id,
       *(sockList[parent_id].localaddr),
       *(sockList[parent_id].localport),
       CHARSTRING(remoteIpAddrList_sp[0].addrLength-1,(const char*)remoteIpAddrList_sp[0].addr)
       ,remotePort,
       SockDesc::ACTION_DELETE);
      EINSS7_00SctpSetUlpKeyReq(assocId,conn_id);
      EINSS7_00SctpAbortReq(assocId);
      return RETURN_OK;
    }
    conn_id=ConnAddEin(IPL4asp_SCTP,assocId,parent_id,
     *(sockList[parent_id].localaddr),
     *(sockList[parent_id].localport),
     CHARSTRING(remoteIpAddrList_sp[0].addrLength-1,(const char*)remoteIpAddrList_sp[0].addr)
     ,remotePort,
     SockDesc::ACTION_NONE);
    
    ASP__Event event;
    
    event.connOpened().connId() = conn_id;
    event.connOpened().remName() =*(sockList[conn_id].remoteaddr);
    event.connOpened().remPort() =*(sockList[conn_id].remoteport);
    event.connOpened().locName() =*(sockList[conn_id].localaddr);
    event.connOpened().locPort() =*(sockList[conn_id].localport);
    event.connOpened().proto().sctp() = SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
    event.connOpened().userData() =sockList[conn_id].userData;
    incoming_message(event);
   
    EINSS7_00SctpSetUlpKeyReq(assocId,conn_id);
    
    event.sctpEvent().sctpAssocChange().clientId() = conn_id;
    event.sctpEvent().sctpAssocChange().proto().sctp() = SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
    event.sctpEvent().sctpAssocChange().sac__state() = IPL4asp__Types::SAC__STATE::SCTP__COMM__UP;
    incoming_message(event);
   
    
  }
  return RETURN_OK;
}


void IPL4asp__PT_PROVIDER::handle_message_from_ein(int fd){
  if(pipe_to_TTCN_thread_log_fds[0]== fd){
    thread_log *r;
    int len = read(pipe_to_TTCN_thread_log_fds[0], &r, sizeof(thread_log *));
    if (len == sizeof(thread_log *)) {
      TTCN_Logger::begin_event(r->severity);
      TTCN_Logger::log_event("%s", r->msg);
      TTCN_Logger::end_event();

      Free(r->msg);
      Free(r);
    }
    else if (len == 0) {
      TTCN_warning(get_name(), "Internal queue shutdown");
    }
    else if (len < 0) {
      TTCN_warning(get_name(), "Error while reading from internal queue (errno = %d)", errno);
    }
    else {
      TTCN_warning(get_name(),
          "Partial read from the queue: %d bytes read (errno = %d)", len, errno);
    }
  } else if(pipe_to_TTCN_thread_fds[0] == fd){
    read_pipe(pipe_to_TTCN_thread_fds);
    write_pipe(pipe_to_EIN_thread_fds);
    read_pipe(pipe_to_TTCN_thread_fds);
  }
  
}
USHORT_T  IPL4asp__PT_PROVIDER::SctpDataArriveInd(
    ULONG_T assocId,
    USHORT_T streamId,
    ULONG_T ulpKey,
    ULONG_T payloadProtId,
    BOOLEAN_T unorderFlag,
    USHORT_T streamSequenceNumber,
    UCHAR_T partialDeliveryFlag,
    ULONG_T dataLength,
    UCHAR_T * data_p
){

  log_debug("SctpDataArriveInd  assocId %ul  ulpKey %ul  streamId %ud  payloadProtId %ul dataLength %ul",assocId,ulpKey,streamId,payloadProtId,dataLength);
    switch(partialDeliveryFlag){
      case EINSS7_00SCTP_LAST_PARTIAL_DELIVERY:
      case EINSS7_00SCTP_NO_PARTIAL_DELIVERY:
      {
  ASP__RecvFrom asp;
  asp.connId() = ulpKey;
  asp.userData() = sockList[ulpKey].userData;
  asp.remName() = *(sockList[ulpKey].remoteaddr);
  asp.remPort() = *(sockList[ulpKey].remoteport);
  asp.locName() = *(sockList[ulpKey].localaddr);
  asp.locPort() = *(sockList[ulpKey].localport);
        sockList[ulpKey].buf[0]->put_s(dataLength, data_p);
//          debug("PL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: Incoming data (%ld bytes): stream = %hu, ssn = %hu, flags = %hx, ppid = %u \n", n,
//          sri->sinfo_stream,(unsigned int)sri->sinfo_ssn, sri->sinfo_flags, sri->sinfo_ppid);

          INTEGER i_ppid;
          i_ppid.set_long_long_val(payloadProtId);

          asp.proto().sctp() = SctpTuple(streamId, i_ppid, OMIT_VALUE, OMIT_VALUE);

          sockList[ulpKey].buf[0]->get_string(asp.msg());
          incoming_message(asp);
        if(sockList[ulpKey].buf) sockList[ulpKey].buf[0]->clear();
      }
        break;
      case EINSS7_00SCTP_FIRST_PARTIAL_DELIVERY:
      case EINSS7_00SCTP_MIDDLE_PARTIAL_DELIVERY:
        debug("IPL4asp__PT_PROVIDER::Handle_Fd_Event_Readable: partial receive: %ud bytes", dataLength);
        sockList[ulpKey].buf[0]->put_s(dataLength, data_p);
        break;
      default:

        break;
    }

  return RETURN_OK;
}

USHORT_T  IPL4asp__PT_PROVIDER::SctpCommLostInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T eventType,
    UCHAR_T origin
){

  log_debug(" SctpCommLostInd assocId %ul  ulpKey %ul eventType %d origin %d ",assocId,ulpKey,eventType,origin);
  if(sockList[ulpKey].next_action==SockDesc::ACTION_CONNECT &&
     globalConnOpts.connection_method==GlobalConnOpts::METHOD_ONE &&
     sockList[ulpKey].remote_addr_index<sockList[ulpKey].remote_addr_list.lengthof()){

    char rem_addr[46];
    memset((void *)rem_addr,0,46);
    IPADDRESS_T ip_struct;
    strcpy(rem_addr,(const char*)(sockList[ulpKey].remote_addr_list[sockList[ulpKey].remote_addr_index].hostName()));
    sockList[ulpKey].remote_addr_index++;
    if(!strchr(rem_addr,':')){
      ip_struct.addrType=EINSS7_00SCTP_IPV4;
    }
#ifdef USE_IPV6
    else {
      ip_struct.addrType=EINSS7_00SCTP_IPV6;
    }
#endif 
    ip_struct.addr=(unsigned char*)rem_addr;
    ip_struct.addrLength=strlen(rem_addr)+1;
    
    USHORT_T req_result=EINSS7_00SctpAssociateReq(
      sockList[ulpKey].endpoint_id,
      sockList[ulpKey].maxOs<(int) globalConnOpts.sinit_num_ostreams?sockList[ulpKey].maxOs:(int) globalConnOpts.sinit_num_ostreams,
      ulpKey,
      *(sockList[ulpKey].remoteport),
      ip_struct
    );

    if(EINSS7_00SCTP_OK!=req_result){
      Result result(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
      ConnDelEin(ulpKey);
      EINSS7_00SctpDestroyReq(sockList[ulpKey].endpoint_id);
      result.errorCode()=PortError::ERROR__GENERAL;
      result.os__error__code()=req_result;
      result.os__error__text()=get_ein_sctp_error_message(req_result,API_RETURN_CODES);
      result.connId()=ulpKey;
      ASP__Event event;
      event.result() = result;
      incoming_message(event);
    }
     
    return RETURN_OK;
  } 
  else if(sockList[ulpKey].next_action!=SockDesc::ACTION_DELETE){
    ASP__Event event;
    ProtoTuple proto;
    proto.sctp()=SctpTuple(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
        event.connClosed() =
          ConnectionClosedEvent(ulpKey,
            *(sockList[ulpKey].remoteaddr), 
            *(sockList[ulpKey].remoteport),
            *(sockList[ulpKey].localaddr), 
            *(sockList[ulpKey].localport),
            proto, sockList[ulpKey].userData);
    incoming_message(event);
  } else {
    sockList[ulpKey].next_action=SockDesc::ACTION_DELETE;
  }
  ConnDelEin(ulpKey);
  return RETURN_OK;

}

USHORT_T  IPL4asp__PT_PROVIDER::SctpAssocRestartInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    USHORT_T outboundStreams,
    USHORT_T inboundStreams,
    UCHAR_T numOfRemoteIpAddrs,
    IPADDRESS_T * remoteIpAddrList_sp
){
  log_debug("SctpAssocRestartInd  assocId %ul  ulpKey %ul ",assocId,ulpKey);

  return RETURN_OK;
}


USHORT_T  IPL4asp__PT_PROVIDER::SctpCommErrorInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T errorCode
){
  log_debug("SctpCommErrorInd  assocId %ul  ulpKey %ul  errorCode %d",assocId,ulpKey,errorCode);
  return RETURN_OK;
}

USHORT_T  IPL4asp__PT_PROVIDER::SctpShutdownConf(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T ulpKey
){
  log_debug(" SctpShutdownConf  assocId %ul  ulpKey %ul returnCode %d",assocId,ulpKey,returnCode);
  return RETURN_OK;
}


USHORT_T  IPL4asp__PT_PROVIDER::SctpCongestionCeaseInd(
    ULONG_T assocId,
    ULONG_T ulpKey
){
  log_debug(" SctpCongestionCeaseInd  assocId %ul  ulpKey %ul ",assocId,ulpKey);
  return RETURN_OK;
}

USHORT_T  IPL4asp__PT_PROVIDER::SctpSendFailureInd(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T ulpKey,
    ULONG_T dataLength,
    UCHAR_T * data_p,
    ULONG_T payloadProtId,
    USHORT_T streamId,
    ULONG_T userSequence,
    IPADDRESS_T * remoteIpAddr_s,
    BOOLEAN_T unorderFlag
){
  log_debug("SctpSendFailureInd   assocId %ul  ulpKey %ul returnCode %d",assocId,ulpKey,returnCode);
  return RETURN_OK;
}


USHORT_T  IPL4asp__PT_PROVIDER::SctpNetworkStatusChangeInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T newStatus,
    IPADDRESS_T remoteIpAddr_s
){
  log_debug(" SctpNetworkStatusChangeInd  assocId %ul  ulpKey %ul newStatus %d",assocId,ulpKey,newStatus);
  return RETURN_OK;
}

USHORT_T  IPL4asp__PT_PROVIDER::SctpIndError(
    USHORT_T errorCode,
    MSG_T * msg_sp
){
  log_debug(" SctpIndError  errorCode %d ",errorCode);
  return RETURN_OK;
}

USHORT_T  IPL4asp__PT_PROVIDER::SctpBindConf(
    EINSS7INSTANCE_T sctpInstanceId,
    UCHAR_T error
){
  log_debug(" SctpBindConf sctpInstanceId %ul, error %d ",sctpInstanceId, error);
  bindResult= error;
  return RETURN_OK;
}

USHORT_T IPL4asp__PT_PROVIDER::SctpStatusConf(
    UCHAR_T returnCode,
    ULONG_T mappingKey,
    ULONG_T sctpEndpointId,
    ULONG_T numOfAssociations,
    ASSOC_T * assocStatusList_sp
){
  log_debug(" SctpStatusConf returnCode %d mappingKey %ul, sctpEndpointId %ul",returnCode,mappingKey,sctpEndpointId);
  return RETURN_OK;
}

USHORT_T  IPL4asp__PT_PROVIDER::SctpTakeoverConf(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T sctpEndpointId
){
  log_debug("SctpTakeoverConf returnCode %d assocId %ul, sctpEndpointId %ul",returnCode,assocId,sctpEndpointId);
  return RETURN_OK;
}

USHORT_T  IPL4asp__PT_PROVIDER::SctpTakeoverInd(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T sctpEndpointId
){
  log_debug("SctpTakeoverInd returnCode %d assocId %ul, sctpEndpointId %ul",returnCode,assocId,sctpEndpointId);
  return RETURN_OK;
}

USHORT_T  IPL4asp__PT_PROVIDER::SctpCongestionInd(
    ULONG_T assocId,
    ULONG_T ulpKey
){
  log_debug("SctpCongestionInd assocId %ul, ulpKey %ul",assocId,ulpKey);
  return RETURN_OK;
}


#ifdef EIN_R3B

USHORT_T  EINSS7_00SCTPSETEPALIASCONF(
    UCHAR_T returnCode,
    ULONG_T sctpEndpointId,
    ULONG_T epAlias
){
  IPL4asp__PT_PROVIDER::port_ptr->log_debug("EINSS7_00SCTPSETEPALIASCONF");
  return RETURN_OK;
}

USHORT_T EINSS7_00SCTPSETUSERCONGESTIONLEVELCONF (
    UCHAR_T returnCode,
    ULONG_T epAlias,
    UCHAR_T congestionLevel,
    ULONG_T numberOfAffectedEndpoints
){
  IPL4asp__PT_PROVIDER::port_ptr->log_debug("EINSS7_00SCTPSETUSERCONGESTIONLEVELCONF");
  return RETURN_OK;
}

USHORT_T EINSS7_00SCTPGETUSERCONGESTIONLEVELCONF (
    UCHAR_T returnCode,
    ULONG_T epAlias,
    UCHAR_T congestionLevel
){
  IPL4asp__PT_PROVIDER::port_ptr->log_debug("EINSS7_00SCTPGETUSERCONGESTIONLEVELCONF");
  return RETURN_OK;
}

UINT16_T EINSS7_00SCTPREDIRECTIND (
    UINT8_T action,
    UINT16_T oldSctpFeInstance,
    UINT16_T newSctpFeInstance
){
  IPL4asp__PT_PROVIDER::port_ptr->log_debug("EINSS7_00SCTPREDIRECTIND");
  return RETURN_OK;
}

UINT16_T EINSS7_00SCTPINFOCONF (
    UINT8_T result,
    UINT8_T reservedByte,
    UINT8_T partialDeliveryFlag,
    UINT32_T mappingKey,
    EINSS7_00SCTP_INFO_TAG_T * topTag
){
  IPL4asp__PT_PROVIDER::port_ptr->log_debug("EINSS7_00SCTPINFOCONF");
  EINSS7_00SctpInfoTagRemove(topTag,true);
  return RETURN_OK;
}

UINT16_T EINSS7_00SCTPINFOIND (
    UINT8_T reservedByte1,
    UINT8_T reservedByte2,
    UINT8_T partialDeliveryFlag,
    UINT32_T mappingKey,
    EINSS7_00SCTP_INFO_TAG_T * topTag
){
  IPL4asp__PT_PROVIDER::port_ptr->log_debug("EINSS7_00SCTPINFOIND");
  EINSS7_00SctpInfoTagRemove(topTag,true);
  return RETURN_OK;
}

UINT16_T EINSS7_00SCTPUPDATECONF (
    UINT8_T result,
    UINT8_T reservedByte,
    UINT8_T partialDeliveryFlag,
    UINT32_T mappingKey,
    EINSS7_00SCTP_INFO_TAG_T * topTag
){
  IPL4asp__PT_PROVIDER::port_ptr->log_debug("EINSS7_00SCTPUPDATECONF");
  EINSS7_00SctpInfoTagRemove(topTag,true);
  return RETURN_OK;
}


#endif


const char *IPL4asp__PT_PROVIDER::get_ein_sctp_error_message(const USHORT_T value, const code_set code_set_spec)
{
    
    switch (code_set_spec){
      case API_RETURN_CODES:
        switch (value)
        {
           case EINSS7_00SCTP_OK: 
             return "EINSS7_00SCTP_OK";
           break;
           case EINSS7_00SCTP_INVALID_ASSOC_ID: 
             return "EINSS7_00SCTP_INVALID_ASSOC_ID";
           break;
           case EINSS7_00SCTP_INVALID_OS: 
             return "EINSS7_00SCTP_INVALID_OS";
           break;
           case EINSS7_00SCTP_INVALID_ADDR_TYPE: 
             return "EINSS7_00SCTP_INVALID_ADDR_TYPE";
           break;
           case EINSS7_00SCTP_INVALID_INSTANCE_ID: 
             return "EINSS7_00SCTP_INVALID_INSTANCE_ID";
           break;
           case EINSS7_00SCTP_INVALID_REQ_MIS: 
             return "EINSS7_00SCTP_INVALID_REQ_MIS";
           break;
           case EINSS7_00SCTP_INVALID_OS_SM: 
             return "EINSS7_00SCTP_INVALID_OS_SM";
           break;
           case EINSS7_00SCTP_NULL_IP_LIST: 
             return "EINSS7_00SCTP_NULL_IP_LIST";
           break;
           case EINSS7_00SCTP_NO_DATA: 
             return "EINSS7_00SCTP_NO_DATA";
           break;
           case EINSS7_00SCTP_CALLBACK_FUNC_ALREADY_SET: 
             return "EINSS7_00SCTP_CALLBACK_FUNC_ALREADY_SET";
           break;
           case EINSS7_00SCTP_CALLBACK_FUNC_NOT_SET: 
             return "EINSS7_00SCTP_CALLBACK_FUNC_NOT_SET";
           break;
           case EINSS7_00SCTP_MSG_ARG_VAL: 
             return "EINSS7_00SCTP_MSG_ARG_VAL";
           break;
           case EINSS7_00SCTP_MSG_OUT_OF_MEMORY: 
             return "EINSS7_00SCTP_MSG_OUT_OF_MEMORY";
           break;
           case EINSS7_00SCTP_MSG_GETBUF_FAIL: 
             return "EINSS7_00SCTP_MSG_GETBUF_FAIL";
           break;
           case EINSS7_00SCTP_UNKNOWN_ERROR: 
             return "EINSS7_00SCTP_UNKNOWN_ERROR";
           break;
           case EINSS7_00SCTP_WRONG_VERSION_ERROR: 
             return "EINSS7_00SCTP_WRONG_VERSION_ERROR";
           break;
           case EINSS7_00SCTP_NO_SCTP_INST_ERROR: 
             return "EINSS7_00SCTP_NO_SCTP_INST_ERROR";
           break;
           case EINSS7_00SCTP_NOT_BOUND: 
             return "EINSS7_00SCTP_NOT_BOUND";
           break;
           case EINSS7_00SCTP_NOT_CONNECTED: 
             return "EINSS7_00SCTP_NOT_CONNECTED";
           break;
#ifdef EIN_R3B
           case EINSS7_00SCTP_INVALID_TAG_POINTER: 
             return "EINSS7_00SCTP_INVALID_TAG_POINTER";
           break;
           case EINSS7_00SCTP_INETPTON_FAILED: 
             return "EINSS7_00SCTP_INETPTON_FAILED";
           break;
#endif
           case EINSS7_00SCTP_IND_MEMORY_ERROR: 
             return "EINSS7_00SCTP_IND_MEMORY_ERROR";
           break;
           case EINSS7_00SCTP_MSG_WOULD_BLOCK: 
             return "EINSS7_00SCTP_MSG_WOULD_BLOCK";
           break;
          default:
            return "Unknown error";
        }
        break;
      
      case CONF_RETURNCODE:
        switch (value)
        {
           case EINSS7_00SCTP_NTF_OK: 
             return "EINSS7_00SCTP_OK";
           break;
           case EINSS7_00SCTP_NTF_MAX_INSTANCES_REACHED: 
             return "EINSS7_00SCTP_NTF_MAX_INSTANCES_REACHED";
           break;
           case EINSS7_00SCTP_NTF_INVALID_TRANS_ADDR: 
             return "EINSS7_00SCTP_NTF_INVALID_TRANS_ADDR";
           break;
           case EINSS7_00SCTP_NTF_INVALID_ENDPOINT_ID: 
             return "EINSS7_00SCTP_NTF_INVALID_ENDPOINT_ID";
           break;
           case EINSS7_00SCTP_NTF_INCORRECT_IP_ADDR: 
             return "EINSS7_00SCTP_NTF_INCORRECT_IP_ADDR";
           break;
           case EINSS7_00SCTP_NTF_OS_INCORRECT: 
             return "EINSS7_00SCTP_NTF_OS_INCORRECT";
           break;
           case EINSS7_00SCTP_NTF_INVALID_ADDR_TYPE: 
             return "EINSS7_00SCTP_NTF_INVALID_ADDR_TYPE";
           break;
           case EINSS7_00SCTP_NTF_NO_BUFFER_SPACE: 
             return "EINSS7_00SCTP_NTF_NO_BUFFER_SPACE";
           break;
           case EINSS7_00SCTP_NTF_CONNECTION_CLOSING_DOWN: 
             return "EINSS7_00SCTP_NTF_CONNECTION_CLOSING_DOWN";
           break;
           case EINSS7_00SCTP_NTF_INVALIDS_ASSOCIATION_ID: 
             return "EINSS7_00SCTP_NTF_INVALIDS_ASSOCIATION_ID";
           break;
           case EINSS7_00SCTP_NTF_MAX_ASSOC_REACHED: 
             return "EINSS7_00SCTP_NTF_MAX_ASSOC_REACHED";
           break;
           case EINSS7_00SCTP_NTF_INTERNAL_ERROR: 
             return "EINSS7_00SCTP_NTF_INTERNAL_ERROR";
           break;
           case EINSS7_00SCTP_NTF_WRONG_PRIMITIVE_FORMAT: 
             return "EINSS7_00SCTP_NTF_WRONG_PRIMITIVE_FORMAT";
           break;
           case EINSS7_00SCTP_NTF_NO_USER_DATA: 
             return "EINSS7_00SCTP_NTF_NO_USER_DATA";
           break;
           case EINSS7_00SCTP_NTF_INV_STREAM_ID: 
             return "EINSS7_00SCTP_NTF_INV_STREAM_ID";
           break;
           case EINSS7_00SCTP_NTF_DATA_EXCEEDED_MAX: 
             return "EINSS7_00SCTP_NTF_DATA_EXCEEDED_MAX";
           break;
           case EINSS7_00SCTP_NTF_INVALID_MODULE_ID: 
             return "EINSS7_00SCTP_NTF_INVALID_MODULE_ID";
           break;
           case EINSS7_00SCTP_NTF_WRONG_USER_MODULE_ID: 
             return "EINSS7_00SCTP_NTF_WRONG_USER_MODULE_ID";
           break;
           case EINSS7_00SCTP_NTF_WRONG_USER_DISTRIBUTOR: 
             return "EINSS7_00SCTP_NTF_WRONG_USER_DISTRIBUTOR";
           break;
           case EINSS7_00SCTP_NTF_INVALID_CLIENT_MODE: 
             return "EINSS7_00SCTP_NTF_INVALID_CLIENT_MODE";
           break;
           case EINSS7_00SCTP_NTF_MAX_USER_INSTS_REACHED: 
             return "EINSS7_00SCTP_NTF_MAX_USER_INSTS_REACHED";
           break;
           case EINSS7_00SCTP_NTF_WRONG_DSCP: 
             return "EINSS7_00SCTP_NTF_WRONG_DSCP";
           break;
           case EINSS7_00SCTP_NTF_ALREADY_BOUND: 
             return "EINSS7_00SCTP_NTF_ALREADY_BOUND";
           break;
           case EINSS7_00SCTP_NTF_INVALID_LOCAL_IP: 
             return "EINSS7_00SCTP_NTF_INVALID_LOCAL_IP";
           break;
           case EINSS7_00SCTP_NTF_INVALID_IF_VERSION: 
             return "EINSS7_00SCTP_NTF_INVALID_IF_VERSION";
           break;
           case EINSS7_00SCTP_NTF_USER_NOT_BOUND: 
             return "EINSS7_00SCTP_NTF_USER_NOT_BOUND";
           break;
           case EINSS7_00SCTP_NTF_WRONG_STATE: 
             return "EINSS7_00SCTP_NTF_WRONG_STATE";
           break;
           case EINSS7_00SCTP_NTF_DUPLICATE_INIT: 
             return "EINSS7_00SCTP_NTF_DUPLICATE_INIT";
           break;
           case EINSS7_00SCTP_NTF_DUPLICATE_INIT_ANOTHER_INSTANCE: 
             return "EINSS7_00SCTP_NTF_DUPLICATE_INIT_ANOTHER_INSTANCE";
           break;
           case EINSS7_00SCTP_NTF_INVALID_CONFIGURATION_GROUP_ID: 
             return "EINSS7_00SCTP_NTF_INVALID_CONFIGURATION_GROUP_ID";
           break;
           case EINSS7_00SCTP_NTF_UNABLE_TO_ASSIGN_PORT_NUMBER: 
             return "EINSS7_00SCTP_NTF_UNABLE_TO_ASSIGN_PORT_NUMBER";
           break;
#ifdef EIN_R3B
           case EINSS7_00SCTP_NTF_UNLOADING: 
             return "EINSS7_00SCTP_NTF_UNLOADING";
           break;
           case EINSS7_00SCTP_NTF_BIG_IP_LIST: 
             return "EINSS7_00SCTP_NTF_BIG_IP_LIST";
           break;
           case EINSS7_00SCTP_NTF_ENDPOINT_IS_BEING_DELETED: 
             return "EINSS7_00SCTP_NTF_ENDPOINT_IS_BEING_DELETED";
           break;
#endif
          default:
            return "Unknown error";
        }
        break;
      
      
      default:
        return "Unknown error";
    }
}

void IPL4asp__PT_PROVIDER::log_msg(const char *header, const MSG_T *msg)
{
  if (debugAllowed)
  {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("IPL4 test port (%s): ", get_name());
    if (header != NULL) TTCN_Logger::log_event("%s: ", header);
    TTCN_Logger::log_event_str("{");
    TTCN_Logger::log_event(" Sender: %d,", msg->sender);
    TTCN_Logger::log_event(" Receiver: %d,", msg->receiver);
    TTCN_Logger::log_event(" Primitive: %d,", msg->primitive);
    TTCN_Logger::log_event(" Size: %d,", msg->size);
    TTCN_Logger::log_event_str(" Message:");
    for (USHORT_T i = 0; i < msg->size; i++)
      TTCN_Logger::log_event(" %02X", msg->msg_p[i]);
    TTCN_Logger::log_event_str(" }");
    TTCN_Logger::end_event();
  }
}
#endif


} // namespace IPL4asp__PortType

#ifdef USE_IPL4_EIN_SCTP

USHORT_T  EINSS7_00SctpAssocRestartInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    USHORT_T outboundStreams,
    USHORT_T inboundStreams,
    UCHAR_T numOfRemoteIpAddrs,
    IPADDRESS_T * remoteIpAddrList_sp
){

  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpAssocRestartInd(
    assocId,
    ulpKey,
    outboundStreams,
    inboundStreams,
    numOfRemoteIpAddrs,
    remoteIpAddrList_sp
  );
}

USHORT_T  EINSS7_00SctpCommUpInd(
    ULONG_T sctpEndpointId,
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T origin,
    USHORT_T outboundStreams,
    USHORT_T inboundStreams,
    USHORT_T remotePort,
    UCHAR_T numOfRemoteIpAddrs,
    IPADDRESS_T * remoteIpAddrList_sp
){

  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpCommUpInd(
    sctpEndpointId,
    assocId,
    ulpKey,
    origin,
    outboundStreams,
    inboundStreams,
    remotePort,
    numOfRemoteIpAddrs,
    remoteIpAddrList_sp
  );
}

USHORT_T  EINSS7_00SctpCommLostInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T eventType,
    UCHAR_T origin
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpCommLostInd(
    assocId,
    ulpKey,
    eventType,
    origin
  );
}

USHORT_T  EINSS7_00SctpCommErrorInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T errorCode
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpCommErrorInd(
    assocId,
    ulpKey,
    errorCode
  );
}

USHORT_T  EINSS7_00SctpDataArriveInd(
    ULONG_T assocId,
    USHORT_T streamId,
    ULONG_T ulpKey,
    ULONG_T payloadProtId,
    BOOLEAN_T unorderFlag,
    USHORT_T streamSequenceNumber,
    UCHAR_T partialDeliveryFlag,
    ULONG_T dataLength,
    UCHAR_T * data_p
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpDataArriveInd(
    assocId,
    streamId,
    ulpKey,
    payloadProtId,
    unorderFlag,
    streamSequenceNumber,
    partialDeliveryFlag,
    dataLength,
    data_p
  );
}

USHORT_T  EINSS7_00SctpShutdownConf(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T ulpKey
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpShutdownConf(
    returnCode,
    assocId,
    ulpKey
  );
}

USHORT_T  EINSS7_00SctpCongestionInd(
    ULONG_T assocId,
    ULONG_T ulpKey
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpCongestionInd(
    assocId,
    ulpKey
  );
}

USHORT_T  EINSS7_00SctpCongestionCeaseInd(
    ULONG_T assocId,
    ULONG_T ulpKey
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpCongestionCeaseInd(
    assocId,
    ulpKey
  );
}

USHORT_T  EINSS7_00SctpSendFailureInd(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T ulpKey,
    ULONG_T dataLength,
    UCHAR_T * data_p,
    ULONG_T payloadProtId,
    USHORT_T streamId,
    ULONG_T userSequence,
    IPADDRESS_T * remoteIpAddr_s,
    BOOLEAN_T unorderFlag
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpSendFailureInd(
    returnCode,
    assocId,
    ulpKey,
    dataLength,
    data_p,
    payloadProtId,
    streamId,
    userSequence,
    remoteIpAddr_s,
    unorderFlag
  );
}


USHORT_T  EINSS7_00SctpNetworkStatusChangeInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T newStatus,
    IPADDRESS_T remoteIpAddr_s
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpNetworkStatusChangeInd(
    assocId,
    ulpKey,
    newStatus,
    remoteIpAddr_s
  );
}

USHORT_T  EINSS7_00SctpIndError(
    USHORT_T errorCode,
    MSG_T * msg_sp
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpIndError(
    errorCode,
    msg_sp
  );
}

USHORT_T  EINSS7_00SctpBindConf(
    EINSS7INSTANCE_T sctpInstanceId,
    UCHAR_T error
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpBindConf(
    sctpInstanceId,
    error
  );
}

USHORT_T  EINSS7_00SctpStatusConf(
    UCHAR_T returnCode,
    ULONG_T mappingKey,
    ULONG_T sctpEndpointId,
    ULONG_T numOfAssociations,
    ASSOC_T * assocStatusList_sp
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpStatusConf(
    returnCode,
    mappingKey,
    sctpEndpointId,
    numOfAssociations,
    assocStatusList_sp
  );
}

USHORT_T  EINSS7_00SctpTakeoverConf(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T sctpEndpointId
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpTakeoverConf(
    returnCode,
    assocId,
    sctpEndpointId
  );
}

USHORT_T  EINSS7_00SctpTakeoverInd(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T sctpEndpointId
){
  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpTakeoverConf(
    returnCode,
    assocId,
    sctpEndpointId
  );
}

USHORT_T  EINSS7_00SctpInitializeConf(
    UCHAR_T returnCode,
    ULONG_T sctpEndpointId,
    USHORT_T assignedMis,
    USHORT_T assignedOsServerMode,
    USHORT_T maxOs,
    ULONG_T pmtu,
    ULONG_T mappingKey,
    USHORT_T localPort
){

  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpInitializeConf(
    returnCode,
    sctpEndpointId,
    assignedMis,
    assignedOsServerMode,
    maxOs,
    pmtu,
    mappingKey,
    localPort
  );
}

USHORT_T  EINSS7_00SctpAssociateConf(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T ulpKey
){

  return IPL4asp__PortType::IPL4asp__PT_PROVIDER::port_ptr->SctpAssociateConf(
    returnCode,
    assocId,
    ulpKey
  );
}
#endif
