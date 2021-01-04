/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               IPL4asp_PT.hh
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 531
//  Updated:            2013-01-08
//  Contact:            http://ttcn.ericsson.se
//  Reference:


#ifndef IPL4asp__PT_HH
#define IPL4asp__PT_HH

#include <netinet/in.h>
#include <netdb.h>

#ifndef NO_IPV6
  #define USE_IPV6
#endif

#if defined LKSCTP_1_0_7 || defined LKSCTP_1_0_9
#ifndef USE_SCTP
  #define USE_SCTP
#endif
#endif

#ifdef LKSCTP_MULTIHOMING_ENABLED
#ifndef USE_SCTP
  #define USE_SCTP
#endif
#endif

#ifdef USE_SCTP
#include <netinet/sctp.h>

#ifdef SCTP_ADAPTION_LAYER
  #ifdef LKSCTP_1_0_7
    #undef LKSCTP_1_0_7
    #error LKSCTP_1_0_7 defined but the lksctp older than 1.0.7. Use only -DUSE_SCTP, version is automatically selected
  #endif  
  #ifdef LKSCTP_1_0_9
    #error LKSCTP_1_0_9 defined but the lksctp older than 1.0.7. Use only -DUSE_SCTP, version is automatically selected
    #undef LKSCTP_1_0_9
  #endif  
#else 
// 1.0.7 or newer
  #ifdef SCTP_AUTH_CHUNK 
    // 1.0.9 or newer
    #ifdef LKSCTP_1_0_7
      #undef LKSCTP_1_0_7
      #error LKSCTP_1_0_7 defined but the lksctp newer than 1.0.7. Use only -DUSE_SCTP, version is automatically selected
    #endif
    #ifndef LKSCTP_1_0_9
      #define LKSCTP_1_0_9
    #endif  
  #else
  // 1.0.7
    #ifdef LKSCTP_1_0_9
      #undef LKSCTP_1_0_9
      #error LKSCTP_1_0_9 defined but the lksctp older than 1.0.9. Use only -DUSE_SCTP, version is automatically selected
    #endif
    #ifndef LKSCTP_1_0_7
      #define LKSCTP_1_0_7
    #endif  
    
  #endif

#endif

#endif
#ifdef USE_IPL4_EIN_SCTP

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#include "ss7cp.h"
#include "00sctpapi.h"

#ifdef TRUE
#undef TRUE
#endif
#define TRUE true

#ifdef FALSE
#undef FALSE
#endif
#define FALSE false

#ifdef UNKNOWN
#undef UNKNOWN
#endif

#ifdef SUCCESS
#undef SUCCESS
#endif

#endif


#include <map>
#include <TTCN3.hh>
#include "IPL4asp_Types.hh"
#include "Socket_API_Definitions.hh"


//SSL
#ifdef IPL4_USE_SSL
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#endif

namespace IPL4asp__PortType {

struct thread_log{
  TTCN_Logger::Severity severity;
  char *msg;
};

typedef int sctp_assoc_t;

#if (defined(SOLARIS) && !defined(_SOCKLEN_T)) || defined(WIN32)
  typedef int socklen_t;
#endif

#define RECV_MAX_LEN 65535
#define ADDR_LEN_MAX 64
#define N_RECENTLY_CLOSED 10 //Number of recently closed sockets not to assign
#define SOCK_LIST_SIZE_MIN 16

enum SSL_STATES {STATE_DONT_RECEIVE, STATE_WAIT_FOR_RECEIVE_CALLBACK, STATE_BLOCK_FOR_SENDING, STATE_DONT_CLOSE, STATE_NORMAL, STATE_CONNECTING, STATE_PNBRECONNECTATTEMPT, STATE_SERVER_ACCEPTING};

typedef enum {
  IPL4asp_UDP,
  IPL4asp_TCP_LISTEN,
  IPL4asp_TCP,
  IPL4asp_SCTP_LISTEN,
  IPL4asp_SCTP,
  IPL4asp_SSL_LISTEN,
  IPL4asp_SSL
} SockType;

typedef struct {
  enum { SOCK_NONEX = -1, SOCK_CLOSED = -2, SOCK_NOT_KNOWN = -3 };
  enum { ACTION_NONE = 0, ACTION_BIND = 1, ACTION_CONNECT = 2 , ACTION_DELETE = 3};
  SockType type;
  int sock; // -1: nonexistent, -2: closed
  TTCN_Buffer **buf;
  sctp_assoc_t *assocIdList;
  unsigned int cnt;
  int userData;
  Socket__API__Definitions::f__getMsgLen getMsgLen;
  Socket__API__Definitions::ro__integer *msgLenArgs;
  int msgLen; // -1 or the message length returned by getMsgLen
  int nextFree; // -1 or index of next free element
  int parentIdx; // parent index (-1 if no)
#ifdef IPL4_USE_SSL
  SSL* sslObj;
#endif
  SSL_STATES sslState;
  Socket__API__Definitions::PortNumber *localport;
  CHARSTRING *localaddr;
  Socket__API__Definitions::PortNumber *remoteport;
  CHARSTRING *remoteaddr;
#ifdef USE_IPL4_EIN_SCTP
  int next_action;
  int endpoint_id;
  int ref_count;
  int remote_addr_index;
  int maxOs;
  Socket__API__Definitions::SocketList remote_addr_list;
#endif
  void clear();
} SockDesc;

typedef union {
  struct sockaddr_in v4;
#ifdef USE_IPV6
  struct sockaddr_in6 v6;
#endif
} SockAddr;

int SetSockAddr(const char *name, int port,
                    SockAddr& sa, socklen_t& saLen);

struct IPDiscConfig {
  enum Type { NONE = 0, DHCP, DHCP_OR_ARP, ARP };
  Type          type;
  CHARSTRING    expIfName;
  CHARSTRING    expIfIpAddress;
  CHARSTRING    exclIfIpAddress;
  CHARSTRING    ethernetAddress;
  unsigned int  leaseTime;
  CHARSTRING    leaseFile;
  unsigned int  nOfAddresses;
  bool          debugAllowed;
  unsigned int  dhcpMsgRetransmitCount;
  unsigned int  dhcpMsgRetransmitPeriodInms;
  unsigned int  dhcpMaxParallelRequestCount;
  unsigned int  dhcpTimeout;
  unsigned int  arpMsgRetransmitCount;
  unsigned int  arpMsgRetransmitPeriodInms;
  unsigned int  arpMaxParallelRequestCount;
  IPDiscConfig () :
    type ( NONE ),
    leaseTime ( 0 ),
    nOfAddresses ( 0 ),
    debugAllowed ( false ),
    dhcpMsgRetransmitCount ( 5 ),
    dhcpMsgRetransmitPeriodInms ( 3000 ),
    dhcpMaxParallelRequestCount ( 25 ),
    dhcpTimeout ( 1000000000 ),
    arpMsgRetransmitCount ( 3 ),
    arpMsgRetransmitPeriodInms ( 1000 ),
    arpMaxParallelRequestCount ( 50 )
 {}
};

struct IPAddrLease {
  CHARSTRING    ifName;
  CHARSTRING    leaseFile;
};

struct GlobalConnOpts {
  enum { NOT_SET = -1 };
  enum { NO = 0, YES = 1 };
  enum { METHOD_ZERO = 0, METHOD_ONE= 1, METHOD_TWO = 2 };
  int connection_method; /* METHOD_ZERO, METHOD_ONE, METHOD_TWO*/
  int tcpReuseAddr; /* YES, NO, NOT_SET */
  int udpReuseAddr; /* YES, NO, NOT_SET */
  int sctpReuseAddr; /* YES, NO, NOT_SET */
  int sslReuseAddr; /* YES, NO, NOT_SET */
  int tcpKeepAlive; /* YES, NO, NOT_SET */
  int tcpKeepCnt; /* NOT_SET, 0.. */
  int tcpKeepIdle; /* NOT_SET, 0.. */
  int tcpKeepIntvl; /* NOT_SET, 0.. */
  int sslKeepAlive; /* YES, NO, NOT_SET */
  int sslKeepCnt; /* NOT_SET, 0.. */
  int sslKeepIdle; /* NOT_SET, 0.. */
  int sslKeepIntvl; /* NOT_SET, 0.. */
  int extendedPortEvents;/* YES, NO, NOT_SET */
  int sinit_num_ostreams; /* 64, 0.. */ //sctp specific params starts here
  int sinit_max_instreams; /* 64, 0.. */
  int sinit_max_attempts; /* 0, 0.. */
  int sinit_max_init_timeo; /* 0, 0.. */
  int sctp_data_io_event; /* YES, NO, NOT_SET */
  int sctp_association_event; /* YES, NO, NOT_SET */
  int sctp_address_event; /* YES, NO, NOT_SET */
  int sctp_send_failure_event; /* YES, NO, NOT_SET */
  int sctp_peer_error_event; /* YES, NO, NOT_SET */
  int sctp_shutdown_event; /* YES, NO, NOT_SET */
  int sctp_partial_delivery_event; /* YES, NO, NOT_SET */
  int sctp_adaptation_layer_event; /* YES, NO, NOT_SET */
  int sctp_authentication_event; /* YES, NO, NOT_SET */
  GlobalConnOpts () :
    connection_method ( METHOD_ZERO ),
    tcpReuseAddr ( YES ),
#ifdef LINUX
    udpReuseAddr ( YES ),
    sctpReuseAddr ( YES ),
#else
    udpReuseAddr ( NO ),
    sctpReuseAddr ( NO ),
#endif
    sslReuseAddr(YES),
    tcpKeepAlive ( NOT_SET ),
    tcpKeepCnt ( NOT_SET ),
    tcpKeepIdle ( NOT_SET ),
    tcpKeepIntvl ( NOT_SET ),
    sslKeepAlive ( NOT_SET ),
    sslKeepCnt ( NOT_SET ),
    sslKeepIdle ( NOT_SET ),
    sslKeepIntvl ( NOT_SET ),
    extendedPortEvents ( NO ),
    sinit_num_ostreams ( 64 ),
    sinit_max_instreams ( 64 ),
    sinit_max_attempts ( 0 ),
    sinit_max_init_timeo ( 0 ),
    sctp_data_io_event ( YES ),
    sctp_association_event ( YES ),
    sctp_address_event ( YES ),
    sctp_send_failure_event ( YES ),
    sctp_peer_error_event ( YES ),
    sctp_shutdown_event ( YES ),
    sctp_partial_delivery_event ( YES ),
    sctp_adaptation_layer_event ( YES ),
    sctp_authentication_event ( NO )
  {}
};

class IPL4asp__PT_PROVIDER : public PORT {
public:
  IPL4asp__PT_PROVIDER(const char *par_port_name = NULL);
  ~IPL4asp__PT_PROVIDER();

  void set_parameter(const char *parameter_name, const char *parameter_value);

  IPDiscConfig ipDiscConfig;
  IPAddrLease ipAddrLease;

  void debug(const char *fmt, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
  int outgoing_send_core(const IPL4asp__Types::ASP__Send& asp, Socket__API__Definitions::Result& result);
  int outgoing_send_core(const IPL4asp__Types::ASP__SendTo& asp, Socket__API__Definitions::Result& result);

  const char *defaultLocHost;
  int defaultLocPort;
protected:
   void user_map(const char *system_port);
   void user_unmap(const char *system_port);

  void user_start();
  void user_stop();

  void outgoing_send(const IPL4asp__Types::ASP__Send& asp);
  void outgoing_send(const IPL4asp__Types::ASP__SendTo& asp);

  virtual void incoming_message(const IPL4asp__Types::ASP__Event& incoming_par) = 0;
  virtual void incoming_message(const IPL4asp__Types::ASP__RecvFrom& incoming_par) = 0;

#ifdef IPL4_USE_SSL
  //increase buffer size
  bool increase_send_buffer(int fd, int &old_size, int& new_size);

  //SSL
  // Called after a TCP connection is established (client side or server accepted a connection).
  // It will create a new SSL conenction on the top of the TCP connection.
  virtual int perform_ssl_handshake(int client_id);
  // Called after a TCP connection is closed.
  // It will delete the SSL conenction.
  virtual bool perform_ssl_shutdown(int client_id);
  // Called from all_mandatory_configparameters_present() function
  // during map() operation to check mandatory parameter presents.
  virtual bool user_all_mandatory_configparameters_present(int clientId);
  // Called after an SSL connection is established (handshake finished) for further
  // authentication. Shall return 'true' if verification
  // is OK, otherwise 'false'. If return value was 'true', the connection is kept, otherwise
  // the connection will be shutted down.
  virtual bool  ssl_verify_certificates();
  // Call during SSL handshake (and rehandshake as well) by OpenSSL
  // Return values:
  // ==1: user authentication is passed, go on with handshake
  // ==0: user authentication failed, refuse the connection to the other peer
  // <0 : user don't care, go on with default basic checks
  virtual int   ssl_verify_certificates_at_handshake(int preverify_ok, X509_STORE_CTX *ssl_ctx);
  // Called to receive from the socket if data is available (select()).
  // Shall return with 0 if the peer is disconnected or with the number of bytes read.
  // If error occured, execution shall stop in the function by calling log_error()
  virtual int  receive_ssl_message_on_fd(int client_id);
  // Called to send a message on the socket.
  // Shall return with 0 if the peer is disconnected or with the number of bytes written.
  // If error occured, execution shall stop in the function by calling log_error()
  //virtual int  send_message_on_fd(int client_id, const unsigned char * message_buffer, int length_of_message);
  //virtual int  send_message_on_nonblocking_fd(int client_id, const unsigned char * message_buffer, int length_of_message);

  // The following members can be called to fetch the current values
  //bool         get_ssl_use_ssl() const                {return ssl_use_ssl;}
  bool         get_ssl_verifycertificate() const      {return ssl_verify_certificate;}
  bool         get_ssl_use_session_resumption() const {return ssl_use_session_resumption;}
  bool         get_ssl_initialized() const            {return ssl_initialized;}
  char       * get_ssl_key_file() const               {return ssl_key_file;}
  char       * get_ssl_certificate_file() const       {return ssl_certificate_file;}
  char       * get_ssl_trustedCAlist_file() const     {return ssl_trustedCAlist_file;}
  char       * get_ssl_cipher_list() const            {return ssl_cipher_list;}
  char       * get_ssl_password() const;
  const unsigned char * get_ssl_server_auth_session_id_context() const {return ssl_server_auth_session_id_context;}
//  const SSL_METHOD * get_current_ssl_method() const         {return ssl_method;}
//  const SSL_CIPHER * get_current_ssl_cipher() const         {return ssl_cipher;}
  SSL_SESSION* get_current_ssl_session() const        {return ssl_session;}
  SSL_CTX    * get_current_ssl_ctx() const            {return ssl_ctx;}
  SSL        * get_current_ssl() const                {return ssl_current_ssl;}

  bool getSslObj(int connId, SSL*& sslObj);
  bool setSslObj(int connId, SSL* sslObj);

  // The following members can be called to set the current values
  // NOTE that in case the parameter_value is a char *pointer, the old character
  // array is deleted by these functions automatically.
  void         set_ssl_verifycertificate(bool parameter_value);
  void         set_ssl_use_session_resumption(bool parameter_value);
  void         set_ssl_key_file(char * parameter_value);
  void         set_ssl_certificate_file(char * parameter_value);
  void         set_ssl_trustedCAlist_file(char * parameter_value);
  void         set_ssl_cipher_list(char * parameter_value);
  void         set_ssl_server_auth_session_id_context(const unsigned char * parameter_value);

  // The following members can be called to fetch the default test port parameter names
  virtual const char* ssl_use_session_resumption_name();
  virtual const char* ssl_private_key_file_name();
  virtual const char* ssl_trustedCAlist_file_name();
  virtual const char* ssl_certificate_file_name();
  virtual const char* ssl_password_name();
  virtual const char* ssl_cipher_list_name();
  virtual const char* ssl_verifycertificate_name();
#endif
public:
  // Logging functions
  void log_debug(const char *fmt, ...) const
    __attribute__ ((__format__ (__printf__, 2, 3)));
  void log_warning(const char *fmt, ...) const
    __attribute__ ((__format__ (__printf__, 2, 3)));
  void log_hex(const char *prompt, const unsigned char *msg, size_t length) const;

private:
  void Handle_Fd_Event_Writable(int fd);
  void Handle_Fd_Event_Readable(int fd);
  void handle_event(int fd, int connId, const void *buf);
  int getmsg(int fd, int connId, struct msghdr *msg,void *buf, size_t *buflen, ssize_t *nrp, size_t cmsglen);
  int ConnAdd(SockType type, int sock, int parentIdx = -1);
  int ConnDel(int connId);
  int setUserData(int id, int userData);
  int getUserData(int id, int& userData);
  int getConnectionDetails(int id, IPL4asp__Types::IPL4__Param IPL4param, IPL4asp__Types::IPL4__ParamResult& IPL4paramResult);
  void sendError(Socket__API__Definitions::PortError code, const Socket__API__Definitions::ConnectionId& id,
    int os_error_code = 0);
  int sendNonBlocking(const Socket__API__Definitions::ConnectionId& id, sockaddr *sa,
    socklen_t saLen, SockType type, const OCTETSTRING& msg, Socket__API__Definitions::Result& result, const Socket__API__Definitions::ProtoTuple& protoTuple = (const Socket__API__Definitions::ProtoTuple&)Socket__API__Definitions::ProtoTuple().unspecified());
  bool getAndCheckSockType(int connId,
    Socket__API__Definitions::ProtoTuple::union_selection_type proto, SockType& type);
  bool setOptions(const IPL4asp__Types::OptionList& options,
    int sock, const Socket__API__Definitions::ProtoTuple& proto, bool beforeBind = false);
  inline void testIfInitialized() const;
  inline bool isConnIdValid(int connId) const {
    return ((unsigned int)connId < sockListSize && connId > 0 &&
            sockList != 0 && sockList[connId].sock > 0);
  }

  void setResult(Socket__API__Definitions::Result& result, Socket__API__Definitions::PortError code, const Socket__API__Definitions::ConnectionId& id, int os_error_code = 0);

  int backlog;
  bool pureNonBlocking;
  bool send_extended_result;
  int poll_timeout;
  int max_num_of_poll;
  GlobalConnOpts globalConnOpts;
  Socket__API__Definitions::f__getMsgLen defaultGetMsgLen;
  Socket__API__Definitions::ro__integer *defaultMsgLenArgs;

  SockDesc *sockList;
  unsigned int sockListCnt;
  unsigned int sockListSize;
  int lonely_conn_id;
  int firstFreeSock;
  int lastFreeSock;
  bool broadcast;
  
  int lazy_conn_id_level;
public:
#ifdef USE_SCTP
  struct sctp_initmsg initmsg;
  struct sctp_event_subscribe events;
#endif
  bool native_stack;
#ifdef USE_IPL4_EIN_SCTP
  CHARSTRING cpManagerIPA;
  int pipe_to_TTCN_thread_fds[2];
  int pipe_to_TTCN_thread_log_fds[2];
  int pipe_to_EIN_thread_fds[2];
  void create_pipes();
  void destroy_pipes();
  void read_pipe(int pipe_fds[]);
  void write_pipe(int pipe_fds[]);
  void log_msg(const char *header, const MSG_T *msg);
  void log_thread(TTCN_Logger::Severity severity, const char *fmt, ...)
    __attribute__ ((__format__ (__printf__, 3, 4)));
  void log_thread_msg(const char *header, const MSG_T *msg);
  
  static IPL4asp__PT_PROVIDER *port_ptr;
  
  pthread_t thread;
  void start_thread();
  void do_bind();
  void do_unbind();

  //Internal commmunication between threads

  static void *thread_main(void *arg);
  void ein_receive_loop();

  // State indicators
  boolean ein_connected;
  boolean thread_started;
  boolean unlinkingGT;
  boolean exiting;
  UCHAR_T bindResult;

  UCHAR_T sctpInstanceId;
  UCHAR_T userInstanceId;
  boolean userInstanceIdSpecified;
  boolean sctpInstanceIdSpecified;
  enum code_set {API_RETURN_CODES, CONF_RETURNCODE} ;
  const char *get_ein_sctp_error_message(const USHORT_T value, const code_set code_set_spec); 
  USHORT_T userId;

  void handle_message_from_ein(int fd);
  std::map<int,int> ep2IndexMap;

  Socket__API__Definitions::Result Listen_einsctp(const IPL4asp__Types::HostName& locName,
    const IPL4asp__Types::PortNumber& locPort, int next_action , const IPL4asp__Types::HostName& remName,
    const IPL4asp__Types::PortNumber& remPort, const IPL4asp__Types::OptionList& options, 
    const IPL4asp__Types::SocketList &sock_list);
  int ConnAddEin(SockType type,
    int assoc_enpoint, int parentIdx, const IPL4asp__Types::HostName& locName,
    const IPL4asp__Types::PortNumber& locPort,const IPL4asp__Types::HostName& remName,
    const IPL4asp__Types::PortNumber& remPort, int next_action);
  int ConnDelEin(int connId);

USHORT_T  SctpInitializeConf(
    UCHAR_T returnCode,
    ULONG_T sctpEndpointId,
    USHORT_T assignedMis,
    USHORT_T assignedOsServerMode,
    USHORT_T maxOs,
    ULONG_T pmtu,
    ULONG_T mappingKey,
    USHORT_T localPort
);
USHORT_T  SctpAssociateConf(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T ulpKey
);
USHORT_T  SctpCommUpInd(
    ULONG_T sctpEndpointId,
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T origin,
    USHORT_T outboundStreams,
    USHORT_T inboundStreams,
    USHORT_T remotePort,
    UCHAR_T numOfRemoteIpAddrs,
    IPADDRESS_T * remoteIpAddrList_sp
);
USHORT_T  SctpDataArriveInd(
    ULONG_T assocId,
    USHORT_T streamId,
    ULONG_T ulpKey,
    ULONG_T payloadProtId,
    BOOLEAN_T unorderFlag,
    USHORT_T streamSequenceNumber,
    UCHAR_T partialDeliveryFlag,
    ULONG_T dataLength,
    UCHAR_T * data_p
);
USHORT_T  SctpCommLostInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T eventType,
    UCHAR_T origin
);
USHORT_T  SctpAssocRestartInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    USHORT_T outboundStreams,
    USHORT_T inboundStreams,
    UCHAR_T numOfRemoteIpAddrs,
    IPADDRESS_T * remoteIpAddrList_sp
);
USHORT_T  SctpCommErrorInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T errorCode
);
USHORT_T  SctpShutdownConf(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T ulpKey
);
USHORT_T  SctpCongestionCeaseInd(
    ULONG_T assocId,
    ULONG_T ulpKey
);
USHORT_T  SctpSendFailureInd(
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
);
USHORT_T  SctpNetworkStatusChangeInd(
    ULONG_T assocId,
    ULONG_T ulpKey,
    UCHAR_T newStatus,
    IPADDRESS_T remoteIpAddr_s
);
USHORT_T  SctpIndError(
    USHORT_T errorCode,
    MSG_T * msg_sp
);

USHORT_T  SctpBindConf(
    EINSS7INSTANCE_T sctpInstanceId,
    UCHAR_T error
);
USHORT_T SctpStatusConf(
    UCHAR_T returnCode,
    ULONG_T mappingKey,
    ULONG_T sctpEndpointId,
    ULONG_T numOfAssociations,
    ASSOC_T * assocStatusList_sp
);
USHORT_T  SctpTakeoverConf(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T sctpEndpointId
);
USHORT_T  SctpTakeoverInd(
    UCHAR_T returnCode,
    ULONG_T assocId,
    ULONG_T sctpEndpointId
);
USHORT_T  SctpCongestionInd(
    ULONG_T assocId,
    ULONG_T ulpKey
);


#endif

  std::map<int,int> fd2IndexMap;

  bool mapped;
  bool debugAllowed;
  Socket__API__Definitions::ConnectionId dontCloseConnectionId;
  SockAddr closingPeer;
  socklen_t closingPeerLen;

  friend void f__IPL4__PROVIDER__setGetMsgLen(
    IPL4asp__PT_PROVIDER& portRef,
    const Socket__API__Definitions::ConnectionId& connId,
    Socket__API__Definitions::f__getMsgLen& f,
    const Socket__API__Definitions::ro__integer& msgLenArgs);

  friend Socket__API__Definitions::Result f__IPL4__PROVIDER__listen(
    IPL4asp__PT_PROVIDER& portRef,
    const Socket__API__Definitions::HostName& locName,
    const Socket__API__Definitions::PortNumber& locPort,
    const Socket__API__Definitions::ProtoTuple& proto,
    const IPL4asp__Types::OptionList& options);

  friend Socket__API__Definitions::Result f__IPL4__PROVIDER__connect(
    IPL4asp__PT_PROVIDER& portRef,
    const Socket__API__Definitions::HostName& remName,
    const Socket__API__Definitions::PortNumber& remPort,
    const Socket__API__Definitions::HostName& locName,
    const Socket__API__Definitions::PortNumber& locPort,
    const Socket__API__Definitions::ConnectionId& connId,
    const Socket__API__Definitions::ProtoTuple& proto,
    const IPL4asp__Types::OptionList& options);

  friend Socket__API__Definitions::Result f__IPL4__PROVIDER__setOpt(
    IPL4asp__PT_PROVIDER& portRef,
    const IPL4asp__Types::OptionList& options,
    const Socket__API__Definitions::ConnectionId& connId,
    const Socket__API__Definitions::ProtoTuple& proto);

  friend Socket__API__Definitions::Result f__IPL4__PROVIDER__close(
    IPL4asp__PT_PROVIDER& portRef,
    const Socket__API__Definitions::ConnectionId& id,
    const Socket__API__Definitions::ProtoTuple& proto);

  friend Socket__API__Definitions::Result f__IPL4__PROVIDER__setUserData(
    IPL4asp__PT_PROVIDER& portRef,
    const Socket__API__Definitions::ConnectionId& id,
    const Socket__API__Definitions::UserData& userData);

  friend Socket__API__Definitions::Result f__IPL4__PROVIDER__getUserData(
    IPL4asp__PT_PROVIDER& portRef,
    const Socket__API__Definitions::ConnectionId& id,
    Socket__API__Definitions::UserData& userData);

  friend Socket__API__Definitions::Result f__IPL4__PROVIDER__getConnectionDetails(
    IPL4asp__PT_PROVIDER& portRef,
    const Socket__API__Definitions::ConnectionId& id,
    const IPL4asp__Types::IPL4__Param& IPL4param,
    IPL4asp__Types::IPL4__ParamResult& IPL4paramResult);

  //SSL

  bool ssl_verify_certificate;     // verify other part's certificate or not
  bool ssl_initialized;            // whether SSL already initialized or not
  bool ssl_use_session_resumption; // use SSL sessions or not
  int ssl_reconnect_attempts;// maximum reconnect attempts, by default 5 (used only if pureNonBlocking is NOT used)
  int ssl_reconnect_delay; // delay between reconnect attempts, by default 1 (used only if pureNonBlocking is NOT used)

  char *ssl_key_file;              // private key file
  char *ssl_certificate_file;      // own certificate file
  char *ssl_trustedCAlist_file;    // trusted CA list file
  char *ssl_cipher_list;           // ssl_cipher list restriction to apply
  char *ssl_password;              // password to decode the private key
  static const unsigned char * ssl_server_auth_session_id_context;

#ifdef IPL4_USE_SSL
//  const SSL_METHOD  *ssl_method;         // SSL context method
  SSL_CTX     *ssl_ctx;            // SSL context
//  const SSL_CIPHER  *ssl_cipher;         // used SSL ssl_cipher
  SSL_SESSION *ssl_session;        // SSL ssl_session
  SSL         *ssl_current_ssl;    // currently used SSL object
  static void *ssl_current_client; // current SSL object, used only during authentication

  bool ssl_actions_to_seed_PRNG(); // Seed the PRNG with enough random data
  bool ssl_init_SSL();             // Initialize SSL libraries and create the SSL context
  void ssl_log_SSL_info();         // Log the currently used SSL setting (debug)
  int  ssl_getresult(int result_code); // Fetch and log the SSL error code from I/O operation result codes
  // Callback function to pass the password to OpenSSL. Called by OpenSSL
  // during SSL handshake.
  static int ssl_password_cb(char * password_buffer, int length_of_password, int rw_flag, void * user_data);
  // Callback function to perform authentication during SSL handshake. Called by OpenSSL.
  // NOTE: for further authentication, use ssl_verify_certificates().
  static int ssl_verify_callback(int preverify_status, X509_STORE_CTX * ssl_context);
#endif
};
#ifdef EIN_R3B

USHORT_T  EINSS7_00SCTPSETEPALIASCONF(
    UCHAR_T returnCode,
    ULONG_T sctpEndpointId,
    ULONG_T epAlias
);

USHORT_T EINSS7_00SCTPSETUSERCONGESTIONLEVELCONF (
    UCHAR_T returnCode,
    ULONG_T epAlias,
    UCHAR_T congestionLevel,
    ULONG_T numberOfAffectedEndpoints
);

USHORT_T EINSS7_00SCTPGETUSERCONGESTIONLEVELCONF (
    UCHAR_T returnCode,
    ULONG_T epAlias,
    UCHAR_T congestionLevel
);

UINT16_T EINSS7_00SCTPREDIRECTIND (
    UINT8_T action,
    UINT16_T oldSctpFeInstance,
    UINT16_T newSctpFeInstance
);

UINT16_T EINSS7_00SCTPINFOCONF (
    UINT8_T result,
    UINT8_T reservedByte,
    UINT8_T partialDeliveryFlag,
    UINT32_T mappingKey,
    EINSS7_00SCTP_INFO_TAG_T * topTag
);

UINT16_T EINSS7_00SCTPINFOIND (
    UINT8_T reservedByte1,
    UINT8_T reservedByte2,
    UINT8_T partialDeliveryFlag,
    UINT32_T mappingKey,
    EINSS7_00SCTP_INFO_TAG_T * topTag
);

UINT16_T EINSS7_00SCTPUPDATECONF (
    UINT8_T result,
    UINT8_T reservedByte,
    UINT8_T partialDeliveryFlag,
    UINT32_T mappingKey,
    EINSS7_00SCTP_INFO_TAG_T * topTag
);


#endif

int SetLocalSockAddr(const char* debug_str, IPL4asp__PT_PROVIDER& portRef,
                    int def_addr_family,
                    const char *locName, int locPort,
                    SockAddr& sockAddr, socklen_t& sockAddrLen);
} /*end of namespace*/
#endif
