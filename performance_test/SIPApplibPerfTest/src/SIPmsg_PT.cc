/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               SIPmsg_PT.cc
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 319
//  Updated:            2013-02-19
//  Contact:            http://ttcn.ericsson.se
//  Reference:          RFC3261, RFC2806, RFC2976, RFC3262, RFC3311, RFC3323,
//                      RFC3325, RFC3326, RFC3265, RFC3455, RFC4244, RFC4538,
//                      RFC6442, RFC6086, RFC6050 
//                      IETF Draft draft-ietf-dip-session-timer-15.txt,
//                      IETF Draft draft-levy-sip-diversion-08.txt, RFC5009
//                      IETF draft-ott-sip-serv-indication-notification-00
//                      IETF draft-holmberg-sipcore-proxy-feature-04,
//                      531/0363-FCP 101 5091

#include <memory.h>
#include "SIPmsg_PT.hh"

#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include "SIP_parse.h"
#include <errno.h>
#include "SIP_parse_.tab.h"

#define DEFAULT_LISTEN_PORT 5060    //we define this port for our default use
#define DEFAULT_DEST_PORT 5060    //remote port
#define DEFAULT_PROTO 0
#define PROTO_UDP 0
#define PROTO_TCP 1
#define BUF_SIZE 1901
#define CREATED 0
#define STARTED 1
#define LISTENING 2
#define CONNECTED 3
#define STOPPED 4

#ifdef SIPPORT_WITH_SIGCOMP

#include <string>
#include <SigComp_Compressor.hh>
#include <SigComp_Decompressor.hh>
#include <SigComp_Dumpers.hh>
#include <SigComp_State.hh>
#include <SigComp_ApplicationParameters.hh>

using namespace SigComp;
using namespace ELib;

#endif
using namespace SIPmsg__Types;
inline static MessageHeader *newMsgHdr() {
  MessageHeader *headerptr = new MessageHeader(
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
    OMIT_VALUE, OMIT_VALUE, OMIT_VALUE
  );
  return headerptr;
}

extern char myinput[BUF_SIZE];
extern RequestLine *rqlineptr;
extern StatusLine *stlineptr;
extern MessageHeader *headerptr;
extern int num_chars;
extern int errorind_loc;
extern int erro_handling_sip_parser;
extern void parsing(const char* buff, int len, bool);
extern int wildcarded_enabled_parser;
int body_mode;

namespace SIPmsg__PortType{

SIPmsg__PT::SIPmsg__PT(const char *SipPort_name):SIPmsg__PT_BASE(SipPort_name){
  local_port=DEFAULT_LISTEN_PORT;
  udportcp=DEFAULT_PROTO;
  listen_enabled=0; // enabled
  target_addr=NULL;   // empty string
  local_addr=NULL;   // empty string
  msg_in_buffer[0]='\0';
  target_port=DEFAULT_DEST_PORT;
  respmsg=NULL;
  reqmsg=NULL;
  status=CREATED;
  msg_encode_buff=NULL;
  msg_encode_buff_size=0;
  debug=false;
  header_mode=0;
  asp=false;
  auto_length=true;
  report_error=false;
  wait_msg_body=0;
  error_mode=0;
  listen_soc = -1;
  listen_soc_tcp = -1;
  comm_soc = -1;
  conn_list = NULL;
  size_conn_list=0;
  active_connections=0;
  last_conn=-1;
  mtu_size=BUF_SIZE;
  multiple_headers=0;
  random_udp=false;
  ipv6enabled = true;
  wildcarded_enabled_port=0;
  port_mode=0;
  raw_mode=0;
  close_soc=false;
#ifdef SIPPORT_WITH_SIGCOMP
    use_sigcomp = false;
    // ** USD not supported by SigComp Core as of 05.06.03 **
    // usd_file = 0;
    // ******************************************************
    config_file = 0;
    cid = strdup("default");
    lastCpmtId = 0;
#endif
}

SIPmsg__PT::~SIPmsg__PT(){
  if( listen_soc != -1 ) close(listen_soc);
  if( listen_soc_tcp != -1 ) close(listen_soc_tcp);
  if( comm_soc != -1 ) close(comm_soc);
  Free(target_addr);
  Free(local_addr);
  if(size_conn_list){
    for(int a=0;a<size_conn_list;a++){
      Free(conn_list[a].addr);
      Free(conn_list[a].buff);
      if(a) if(conn_list[a].fp !=-1) close(conn_list[a].fp);
      delete conn_list[a].respmsg;
      delete conn_list[a].reqmsg;
      delete conn_list[a].raw_msg;
    }
  }
  Free(conn_list);
#ifdef SIPPORT_WITH_SIGCOMP
    Free(config_file);
    // ** USD not supported by SigComp Core as of 05.06.03 **
    // Free(usd_file);
    // ******************************************************
    Free(cid);
    Free(lastCpmtId);
#endif
}

void SIPmsg__PT::user_map(const char *system_port){
  log("Map operation started.");
  FD_ZERO(&connections_read_fds);
  bool old_report_error=report_error;
  report_error=false;
  if(port_mode){
#ifdef SIPPORT_WITH_SIGCOMP
    if (use_sigcomp)
        TTCN_error("SigComp cannot be used with advanced port mode");
    else {
#endif
    conn_list=(connection_data *)Realloc(conn_list,10*sizeof(connection_data));
    size_conn_list=10;
    for(int conn_id=0;conn_id<size_conn_list;conn_id++){
      conn_list[conn_id].fp=-1;
      conn_list[conn_id].port=-1;
      conn_list[conn_id].addr=NULL;
      conn_list[conn_id].buff=NULL;
      conn_list[conn_id].msgsize=0;
      conn_list[conn_id].buffsize=0;
      conn_list[conn_id].errorind=0;
      conn_list[conn_id].wait_msg_body=0;
      conn_list[conn_id].respmsg=NULL;
      conn_list[conn_id].reqmsg=NULL;
      conn_list[conn_id].raw_msg=NULL;
    }
    conn_list[0].raw_msg= new CHARSTRING;
    if(listen_enabled==1 || listen_enabled==3){ // Open the UDP listening socket
      set_addr_struct(&listen_addr, local_port, local_addr);
      if((listen_soc=socket(PF_INET, SOCK_DGRAM, 0))<0)
           { TTCN_error("[start] Listening socket creation failed.");}
      if(fcntl(listen_soc, F_SETFD, O_NONBLOCK) == -1) TTCN_error("Fcntl error");
      if (bind(listen_soc,(struct sockaddr*)&listen_addr,sizeof(listen_addr))  < 0)
           { TTCN_error("[start] Listening socket bind failed. Used port number: %d",local_port);}
      FD_SET(listen_soc, &connections_read_fds);
      conn_list[0].fp=listen_soc;
      conn_list[0].port=target_port;
      if(target_addr){
        set_addr_struct(&remote_addr, target_port, target_addr);
        conn_list[0].addr=(char *)Realloc(conn_list[0].addr,strlen(target_addr) + 1);
        strcpy(conn_list[0].addr,target_addr);
      }

      log("UDP listening socket opened.");
    }
    if(listen_enabled==2 || listen_enabled==3){ // Open the TCP listening socket
      int reuse=1;
      set_addr_struct(&listen_addr, local_port, local_addr);
      if((listen_soc_tcp=socket(PF_INET, SOCK_STREAM, 0))<0)
           { TTCN_error("[start] Listening socket creation failed.");}
      if(setsockopt(listen_soc_tcp, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse,
                             sizeof(reuse)) < 0) TTCN_error("setsockopt error");
      if(fcntl(listen_soc_tcp, F_SETFD, O_NONBLOCK) == -1) TTCN_error("Fcntl error");
      if (bind(listen_soc_tcp,(struct sockaddr*)&listen_addr,sizeof(listen_addr))  < 0)
           { TTCN_error("[start] Listening socket bind failed. Used port number: %d",local_port);}
      if(listen(listen_soc_tcp,1)<0){TTCN_error("[start] Listen failed.");}
      FD_SET(listen_soc_tcp, &connections_read_fds);
      log("TCP listening socket opened.");
    }
    if(target_addr && udportcp){ // open a TCP connection to target host if needed
      add_conn(target_addr,target_port);
    }
    report_error=old_report_error;
#ifdef SIPPORT_WITH_SIGCOMP
   }
#endif
  } else {
    if(target_addr){                                  // open a connection
      set_addr_struct(&remote_addr, target_port, target_addr);
      comm_soc=open_comm_socket(remote_addr);
      FD_SET(comm_soc, &connections_read_fds);
      status=CONNECTED;
      dest_addr.host()()=inet_ntoa(remote_addr.sin_addr);
      dest_addr.portField()()=ntohs(remote_addr.sin_port);
      dest_addr.tcporudp()()=udportcp;
      log("Connection to host %s opened.",target_addr);
    }
    else if(listen_enabled){                           // open a listening socket
      int reuse=1;
      set_addr_struct(&listen_addr, local_port, local_addr);
      if((listen_soc=socket(PF_INET, udportcp?SOCK_STREAM:SOCK_DGRAM, 0))<0)
                      { TTCN_error("[start] Listening socket creation failed.");}
      if(udportcp)  if(setsockopt(listen_soc, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse,
                             sizeof(reuse)) < 0) TTCN_error("setsockopt error");
      if(fcntl(listen_soc, F_SETFD, O_NONBLOCK) == -1) TTCN_error("Fcntl error");

      if (bind(listen_soc,(struct sockaddr*)&listen_addr,sizeof(listen_addr))  < 0)
          { TTCN_error("[start] Listening socket bind failed. Used port number: %d",local_port);}
      if(udportcp){   // TCP
        if(listen(listen_soc,1)<0){TTCN_error("[start] Listen failed.");}
        FD_SET(listen_soc, &connections_read_fds);
        status=LISTENING;
      }
      else{    // UDP
        comm_soc=listen_soc;
        listen_soc=-1;
        FD_SET(comm_soc, &connections_read_fds);
        status=CONNECTED;
      }
      log("Listening socket opened.");
    }
    else {       // listen is not enabled and remote host name is not supplied
      status=STARTED;
    }
  if(old_report_error) TTCN_warning("Transport error reporting can be used only in advanced mode.");
#ifdef SIPPORT_WITH_SIGCOMP
  if (udportcp==PROTO_UDP && use_sigcomp) {
      // loadConfig(config_file, usd_file); // USD not supported by SigComp Core as of 05.06.03
      loadConfig(config_file, 0);
      if (cid==0) cid = strdup("default");
      openCompressor();
  }
  else if (use_sigcomp)
      TTCN_error("Sigcomp can only be used with UDP");
#endif
  }
  Install_Handler(&connections_read_fds,   NULL  ,   NULL  , 0.0);
  log("Map operation finished.");
}

void SIPmsg__PT::user_unmap(const char *system_port){
  log("Unmap operation started.");
  Uninstall_Handler();
  if( listen_soc != -1 ) close(listen_soc);
  if( listen_soc_tcp != -1 ) close(listen_soc_tcp);
  if( comm_soc != -1 ) close(comm_soc);
  if(respmsg!=NULL){delete respmsg; respmsg=NULL;}
  if(reqmsg!=NULL){delete reqmsg; reqmsg=NULL;}
  listen_soc = -1;
  listen_soc_tcp = -1;
  comm_soc = -1;
  wait_msg_body=0;
//  target_addr[0]='\0';   // empty string
  msg_in_buffer[0]='\0';
//  target_port=DEFAULT_DEST_PORT;
  if(size_conn_list){
    for(int a=0;a<size_conn_list;a++){
      Free(conn_list[a].addr);
      Free(conn_list[a].buff);
      if(a) if(conn_list[a].fp !=-1) close(conn_list[a].fp);
      delete conn_list[a].respmsg;
      delete conn_list[a].reqmsg;
      delete conn_list[a].raw_msg;
    }
  }
  Free(conn_list);
  conn_list=NULL;
  size_conn_list=0;
  active_connections=0;
  last_conn=-1;
#ifdef SIPPORT_WITH_SIGCOMP
    if(use_sigcomp)
        closeCompressor();
#endif
  status=STOPPED;
  log("Unmap operation finished.");
}

void SIPmsg__PT::set_parameter(const char *parameter_name,
                                                   const char *parameter_value){
  log("Setting of the parameter '%s' to value '%s' is requested.",parameter_name,parameter_value);
  if(!strcmp(parameter_name,"local_sip_port")){
    local_port=atoi(parameter_value);
    if(local_port<1){
      local_port=DEFAULT_LISTEN_PORT;
      TTCN_logger.log(TTCN_WARNING, "Incorrect default sip port in config file,"
                                  " default value \'%d\' is used.", local_port);
    } else log("The local port was set to:%i",local_port);
  }
  else if(!strcmp(parameter_name,"default_local_address")){
    local_addr=(char *)Realloc(local_addr,strlen(parameter_value)+1);
    strcpy(local_addr,parameter_value);
    log("The local address was set to: '%s'",local_addr);
  }
  else if(!strcmp(parameter_name,"default_sip_protocol")){
    if(!strcmp(parameter_value,"UDP")){udportcp=PROTO_UDP;log("The local protocol was set to: 'UDP'");}
    else if(!strcmp(parameter_value,"TCP")){udportcp=PROTO_TCP;log("The local protocol was set to: 'TCP'");}
    else {
      udportcp=DEFAULT_PROTO;
      TTCN_logger.log(TTCN_WARNING, "Incorrect default sip protocol in config"
                  " file, default value \'%s\' is used.", udportcp?"TCP":"UDP");
    }
  }
  else if(!strcmp(parameter_name,"default_dest_address")){
    target_addr=(char *)Realloc(target_addr,strlen(parameter_value)+1);
    strcpy(target_addr,parameter_value);
    log("The target address was set to: '%s'",target_addr);
  }
  else if(!strcmp(parameter_name,"default_dest_port")){
    target_port=atoi(parameter_value);
    if(target_port<1){
      target_port=DEFAULT_DEST_PORT;
      TTCN_logger.log(TTCN_WARNING, "Incorrect destination sip port in config"
                            " file, default value \'%d\' is used.", target_port);
    } else log("The target port was set to:%i",target_port);
  }
  else if(!strcmp(parameter_name,"listen_enabled")){
    if(!strcasecmp(parameter_value,"enabled")){listen_enabled=3;log("Listen enabled");}
    else if(!strcasecmp(parameter_value,"TCP_only")){listen_enabled=2;log("Listen enabled only on TCP");}
    else if(!strcasecmp(parameter_value,"UDP_only")){listen_enabled=1;log("Listen enabled only on UDP");}
    else if(!strcasecmp(parameter_value,"disabled")){listen_enabled=0;log("Listen disabled");}
    else {
      listen_enabled=0;
      TTCN_warning("Invalid value received: '%s'. Listen disabled.",parameter_value);
    }
  }
   else if(!strcmp(parameter_name,"length_calculation")){
    if(!strcasecmp(parameter_value,"enabled")){auto_length=true;log("Length calculation enabled.");}
    else if(!strcasecmp(parameter_value,"disabled")){auto_length=false;log("Length calculation disabled");}
    else {auto_length=true;TTCN_warning("Invalid value received: '%s'. Length calculation enabled.",parameter_value);}
  }
   else if(!strcmp(parameter_name,"debug")){
    if(!strcasecmp(parameter_value,"enabled")){debug=true;log("Debug enabled");}
    else if(!strcasecmp(parameter_value,"disabled")){debug=false;log("Debug disabled");}
    else {debug=false;TTCN_warning("Invalid value received: '%s'. Debug disabled.",parameter_value);}
  }
   else if(!strcmp(parameter_name,"transport_error_reporting")){
    if(!strcasecmp(parameter_value,"enabled")){report_error=true;log("Transport error reporting enabled");}
    else if(!strcasecmp(parameter_value,"disabled")){report_error=false;log("Transport error reporting disabled");}
    else {report_error=false;TTCN_warning("Invalid value received: '%s'. Transport error reporting disabled.",parameter_value);}
  }
   else if(!strcmp(parameter_name,"raw_mode")){
    if(!strcasecmp(parameter_value,"enabled")){raw_mode=1;log("Raw mode enabled");}
    else if(!strcasecmp(parameter_value,"disabled")){raw_mode=0;log("Raw mode disabled");}
    else {raw_mode=0;TTCN_warning("Invalid value received: '%s'. Raw mode disabled.",parameter_value);}
  }
   else if(!strcmp(parameter_name,"ASP_or_MSG")){
    if(!strcasecmp(parameter_value,"ASP")){asp=true;log("Port will use ASP interface.");}
    else if(!strcasecmp(parameter_value,"MSG")){asp=false;log("Port will use MSG interface.");}
    else {asp=false;TTCN_warning("Invalid value received: '%s'. Port will use MSG interface.",parameter_value);}
  }
  else if(!strcmp(parameter_name,"header_format")){
    if(!strcasecmp(parameter_value,"short")){header_mode=1;log("Port will use short header format.");}
    else if(!strcasecmp(parameter_value,"long")){header_mode=0;log("Port will use long header format.");}
    else {header_mode=0;TTCN_warning("Invalid value received: '%s'. Port will use long header format.",parameter_value);}
  }
  else if(!strcmp(parameter_name,"port_mode")){
    if(!strcasecmp(parameter_value,"basic")){port_mode=0;log("Port mode is basic.");}
    else if(!strcasecmp(parameter_value,"advanced")){port_mode=1;log("Port mode is advanced.");}
    else {port_mode=0;TTCN_warning("Invalid value received: '%s'. Port will work in basic mode.",parameter_value);}
  }
  else if(!strcmp(parameter_name,"multiple_headers")){
    if(!strcasecmp(parameter_value,"enabled")){multiple_headers=1;log("Multiple headers enabled");}
    else if(!strcasecmp(parameter_value,"disabled")){multiple_headers=0;log("Multiple headers  disabled");}
    else {multiple_headers=0;TTCN_warning("Invalid value received: '%s'. Multiple headers disabled.",parameter_value);}
  }
  else if(!strcmp(parameter_name,"wildcarded_uri")){
    if(!strcasecmp(parameter_value,"enabled")){wildcarded_enabled_port=1;log("Wildcarded uri enabled");}
    else if(!strcasecmp(parameter_value,"disabled")){wildcarded_enabled_port=0;log("Wildcarded uri  disabled");}
    else {wildcarded_enabled_port=0;TTCN_warning("Invalid value received: '%s'. Wildcarded uri disabled.",parameter_value);}
  }
  else if(!strcmp(parameter_name,"error_mode")){
     if(!strcasecmp(parameter_value,"ignore")){error_mode=2;log("Port will ignore any parsing error.");}
     else if(!strcasecmp(parameter_value,"warning")){error_mode=1;log("Error mode is set to: 'warning'");}
     else if(!strcasecmp(parameter_value,"error")){error_mode=0;log("Any parsing error will cause run time error.");}
    else {
      TTCN_logger.log(TTCN_WARNING, "Invalid error_mode in config"
                            " file, default value \'error\' is used.");
      error_mode=0;
    }
  }
  else if(!strcmp(parameter_name,"MTU_size")){
    if(!strcasecmp(parameter_value,"disabled")){mtu_size=0;log("MTU size checking disabled");}
    else{
      mtu_size=atoi(parameter_value);
      if(mtu_size<1){
        mtu_size=BUF_SIZE;
        TTCN_logger.log(TTCN_WARNING, "Incorrect MTU size in config"
                              " file, default value \'%d\' is used.", BUF_SIZE);
      } else log("The MTU size was set to:%i",mtu_size);
    }
  }
  else if(!strcmp(parameter_name,"random_udp_sending_port")){
    if(!strcasecmp(parameter_value,"enabled")){random_udp=true;log("Port will use random UDP source port for sending message.");}
    else if(!strcasecmp(parameter_value,"disabled")){random_udp=false;log("Port will use listening UDP port for sending message.");}
    else {random_udp=false;TTCN_warning("Invalid value received: '%s'. Port will use listening UDP port for sending message.",parameter_value);}
  }
  else if(!strcmp(parameter_name,"IPv6enabled")){
    if(!strcasecmp(parameter_value,"true")){ipv6enabled = true; log("IPv6 mode enabled.");}
    else if(!strcasecmp(parameter_value,"false")){ipv6enabled = false; log("IPv6 mode disabled.");}
    else {ipv6enabled = true; TTCN_warning("Invalid value received: '%s'. Port will operate in IPv6 mode.",parameter_value);}
  }
#ifdef SIPPORT_WITH_SIGCOMP
    else if(!strcmp(parameter_name,"use_sigcomp")){
        if(!strcasecmp(parameter_value,"true")){
            use_sigcomp=true;
        }
        else {
            use_sigcomp=false;
        }
    }
    else if(!strcmp(parameter_name,"sigcomp_cfg")){
        Free(config_file);
        config_file = strdup(parameter_value);
    }
    // ****** USD not supported by SigComp Core as of 05.06.03 ******
    // else if(!strcmp(parameter_name,"sigcomp_usd")){
    //     usd_file = strdup(parameter_value);
    // }
    else if(!strcmp(parameter_name,"sigcomp_compartment_id")){
        Free(cid);
        cid = strdup(parameter_value);
    }
#endif
  else TTCN_warning("Invalid parameter name: '%s'",parameter_name);
}

void SIPmsg__PT::user_start()
{
}

void SIPmsg__PT::user_stop()
{
}

void SIPmsg__PT::Event_Handler(const fd_set *r_fds,
        const fd_set *w_fds, const fd_set *e_fds,
        double time_since_last_call)
{
  log("Event_Handler started.");
  struct sockaddr_in source_addr;
#if defined LINUX || defined FREEBSD || defined SOLARIS8
  socklen_t
#else // SOLARIS or WIN32
  int
#endif
    sourcelen = sizeof (source_addr);
  int msg_in_len = strlen(msg_in_buffer);
  if(port_mode){
     // incoming TCP connection
    if(listen_soc_tcp!=-1 && FD_ISSET(listen_soc_tcp,r_fds)){
      int incoming_soc;
      struct sockaddr_in incoming_addr;
#if defined LINUX || defined FREEBSD || defined SOLARIS8
      socklen_t
#else // SOLARIS or WIN32
      int
#endif
        len=sizeof(incoming_addr);
      if((incoming_soc=accept(listen_soc_tcp,(struct sockaddr *) &incoming_addr
                              ,&len))<0){
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
          TTCN_warning("accept returned EAGAIN || EWOULDBLOCK");
        } else {
          if(report_error) {
            ADDRESS ad;
            ASP__SIP__error ec;
            ad.host()=OMIT_VALUE;
            ad.portField()=OMIT_VALUE;
            ad.tcporudp()=OMIT_VALUE;
            ec.error__code()=errno;
            ec.error__text()=strerror(errno);
            errno=0;
            ec.failed__operation()=SIP__operation::SIP__OP__ACCEPT;
            ec.addr()().remote__host()=OMIT_VALUE;
            ec.addr()().remote__port()=OMIT_VALUE;
            ec.addr()().protocol()=SIP__com__prot::TCP__E;
            if(local_addr)
              ec.addr()().local__host()()=local_addr;
            else ec.addr()().local__host()=OMIT_VALUE;
            ec.addr()().local__port()=local_port;
            ec.SIP__message()=OMIT_VALUE;
            incoming_message(ec,&ad);
          } else TTCN_error("Error during accepting connection request.");
        }
      } else {
        int index,port_num;
        char *remote_host;
        FD_SET(incoming_soc, &connections_read_fds);
        Install_Handler(&connections_read_fds, NULL, NULL, 0.0);
        remote_host=inet_ntoa(incoming_addr.sin_addr);
        port_num=ntohs(incoming_addr.sin_port);
        index=add_conn(remote_host,port_num,incoming_soc);
        log("TCP connection accepted from %s:%i",remote_host,port_num);
        log("Conn id:%i, host:%s:%i",index,conn_list[index].addr,conn_list[index].port);
      }
    }
    for(int a=0;a<size_conn_list;a++){
      ADDRESS remote;
      if(conn_list[a].fp!=-1 && FD_ISSET(conn_list[a].fp,r_fds)){
        int recvlength;
        recvlength = recvfrom(conn_list[a].fp,msg_in_buffer,MAX_IN_BUFFER,0
                       ,(struct sockaddr *)&source_addr,&sourcelen);
        log("recvlength=%d",recvlength);
        if ( recvlength < 0 ){
          if(report_error) {
            ASP__SIP__error ec;
            if(a){
              remote.host()()=conn_list[a].addr;
              remote.portField()()=conn_list[a].port;
              remote.tcporudp()()=1;
              close_conn(a);
            } else{
              remote.host()()=inet_ntoa(source_addr.sin_addr);
              remote.portField()()=ntohs(source_addr.sin_port);
              remote.tcporudp()()=0;
            }
            ec.error__code()=errno;
            ec.error__text()=strerror(errno);
            errno=0;
            ec.failed__operation()=SIP__operation::SIP__OP__RECEIVE;
            ec.addr()().remote__host()=remote.host();
            ec.addr()().remote__port()=remote.portField();
            ec.addr()().protocol()=a?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
            if(local_addr)
              ec.addr()().local__host()()=local_addr;
            else ec.addr()().local__host()=OMIT_VALUE;
            ec.addr()().local__port()=local_port;
            ec.SIP__message()=OMIT_VALUE;
            incoming_message(ec,&remote);
            continue;
          } else TTCN_error("[Event_Handler] UDP/TCP recvfrom failed.");
        }
        msg_in_buffer[recvlength]='\0';
        log("received buffer: %s",msg_in_buffer);
        if(a==0){  // incoming UDP packet
          log("UDP packet received.");
          last_conn=0;
          remote.host()()=inet_ntoa(source_addr.sin_addr);
          remote.portField()()=ntohs(source_addr.sin_port);
          remote.tcporudp()()=0;
//          int as=0;
          decode_messages(0,conn_list[a].wait_msg_body,recvlength,msg_in_buffer,conn_list[a].errorind,
            *conn_list[a].raw_msg,conn_list[a].respmsg,conn_list[a].reqmsg,remote);
        } else {  // incoming TCP messsage
          if(recvlength==0){ // TCP connection closed by peer
            log("TCP connection is closed by peer %s:%i",conn_list[a].addr,conn_list[a].port);
            close_conn(a);
          } else {
            log("TCP packet received. Conn id: %d, max: %d",a,size_conn_list);
            log("Remote host name: %s",conn_list[a].addr);
            log("Remote port: %d",conn_list[a].port);
            last_conn=a;
            remote.host()()=conn_list[a].addr;
            remote.portField()()=conn_list[a].port;
            remote.tcporudp()()=1;
            if((conn_list[a].buffsize-conn_list[a].msgsize)<(recvlength+1)){
              conn_list[a].buff=(char *)Realloc(conn_list[a].buff
                              ,(conn_list[a].buffsize+recvlength+1)*sizeof(char));
              conn_list[a].buffsize+=recvlength+1;
            }
            memcpy(conn_list[a].buff+conn_list[a].msgsize,msg_in_buffer,recvlength+1);
            conn_list[a].msgsize+=recvlength;
            decode_messages(1,conn_list[a].wait_msg_body,conn_list[a].msgsize,conn_list[a].buff,conn_list[a].errorind,
              *conn_list[a].raw_msg,conn_list[a].respmsg,conn_list[a].reqmsg,remote);
          }
        }
      }
    }
  } else { // basic mode
    if(status==LISTENING){      // incoming connection request
#if defined LINUX || defined FREEBSD || defined SOLARIS8
      socklen_t
#else // SOLARIS or WIN32
      int
#endif
        len=sizeof(remote_addr);
      if((comm_soc=accept(listen_soc,(struct sockaddr *) &remote_addr, &len))<0){
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
          TTCN_warning("accept returned EAGAIN || EWOULDBLOCK");
          return;
        } else {
          TTCN_error("Error during accepting connection request.");
        }
      }
      // Swith into connected state
      status = CONNECTED;
      fd_set readfdset;
      FD_ZERO(&readfdset);
      FD_SET(comm_soc, &readfdset);
      Install_Handler(&readfdset, NULL, NULL, 0.0);
      dest_addr.host()()=inet_ntoa(remote_addr.sin_addr);
      dest_addr.portField()()=ntohs(remote_addr.sin_port);
      dest_addr.tcporudp()()=udportcp;
      log("TCP connection accepted");
      return;
    }
    int recvlength;
#ifdef SIPPORT_WITH_SIGCOMP
    if(use_sigcomp)
        recvlength = receiveSigcompMessage(comm_soc, msg_in_buffer+msg_in_len, MAX_IN_BUFFER - msg_in_len, source_addr);
    else
#endif
    recvlength = recvfrom(comm_soc,msg_in_buffer+msg_in_len,MAX_IN_BUFFER-
                       msg_in_len ,0, (struct sockaddr *)&source_addr,&sourcelen);
    msg_in_len+=recvlength;
    msg_in_buffer[msg_in_len]='\0';

    if ( udportcp && (recvlength == 0) ){    // TCP connection closed by peer
      fd_set readfds;
      FD_ZERO(&readfds);
      TTCN_logger.log(TTCN_WARNING, "TCP connection closed by peer.");
      close(comm_soc);
      comm_soc = -1;

      wait_msg_body=0;      // clear buffer
      msg_in_buffer[0]='\0';
      if(respmsg!=NULL){delete respmsg; respmsg=NULL;}  // clear message
      if(reqmsg!=NULL){delete reqmsg; reqmsg=NULL;}
      if(listen_enabled){                           // open a listening socket
        if(listen_soc==-1){
          int reuse=1;
          set_addr_struct(&listen_addr, local_port, local_addr);
          if((listen_soc=socket(PF_INET, udportcp?SOCK_STREAM:SOCK_DGRAM, 0))<0)
                               { TTCN_error("Listening socket creation failed.");}
          if(udportcp)  if(setsockopt(listen_soc, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse,
                             sizeof(reuse)) < 0) TTCN_error("setsockopt error");
          if(fcntl(listen_soc, F_SETFD, O_NONBLOCK) == -1)
                                                        TTCN_error("fcntl error");
          if (bind(listen_soc,(struct sockaddr*)&listen_addr,sizeof(listen_addr))
                               < 0){ TTCN_error("Listening socket bind failed.");}
          if(listen(listen_soc,1)<0){TTCN_error("Listen failed.");}
        }
        FD_SET(listen_soc, &readfds);
        status=LISTENING;
        Install_Handler(&readfds,   NULL  ,   NULL  , 0.0);
      }else {Uninstall_Handler();}
      return;
    }
    if ( recvlength <= 0 ) TTCN_error("[Event_Handler] UDP/TCP recvfrom failed.");


    if(!udportcp){
      dest_addr.host()()=inet_ntoa(source_addr.sin_addr);
      dest_addr.portField()()=ntohs(source_addr.sin_port);
      dest_addr.tcporudp()()=udportcp;
    }
    decode_messages(udportcp,wait_msg_body,msg_in_len,msg_in_buffer,errorind,
      raw_msg,respmsg,reqmsg,dest_addr);
  }
}

void SIPmsg__PT::decode_messages(
    int udportcp_loc,
    int &wait_msg_body_loc,
    int &msg_in_len_loc,
    char *msg_in_buffer_loc,
    int &errorind_l,
    CHARSTRING &raw_msg_loc,
    PDU__SIP__Response *&respmsg_loc,
    PDU__SIP__Request *&reqmsg_loc,
    ADDRESS &dest_addr_loc
  ){
  errorind_loc=errorind_l;
  int more_msg=1;
  while(more_msg){
    if(udportcp_loc){ // In case of TCP check if the whole message present
                  // in the buffer or not
      char *atm=msg_in_buffer_loc;
      if(wait_msg_body_loc){     // waiting for message body
        log("waiting message found body, needed %d byte",wait_msg_body_loc);
        int a=wait_msg_body_loc;
        if(a>msg_in_len_loc){return;}
        if(errorind_loc){
          switch(error_mode){
            case 2:
              break;
            case 1:
              raw_msg_loc=raw_msg_loc+CHARSTRING(a,msg_in_buffer_loc);
              break;
            default:
              break;
          }
        } else if(raw_mode){
          raw_msg_loc=raw_msg_loc+CHARSTRING(a,msg_in_buffer_loc);
        } else if(reqmsg_loc!=NULL){
          reqmsg_loc->messageBody()()=CHARSTRING(a,msg_in_buffer_loc);
        }
        else if(respmsg_loc!=NULL){
          respmsg_loc->messageBody()()=CHARSTRING(a,msg_in_buffer_loc);
        }
        if(errorind_loc){
          switch(error_mode){
            case 2:
              break;
            case 1:
              if(asp){
                ASP__SIP__Raw req;
                req.raw()=raw_msg_loc;
                req.addr()().remote__host()=dest_addr_loc.host();
                req.addr()().remote__port()=dest_addr_loc.portField();
                req.addr()().protocol()=udportcp_loc?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
                if(local_addr)
                  req.addr()().local__host()()=local_addr;
                else req.addr()().local__host()=OMIT_VALUE;
                req.addr()().local__port()=local_port;
                incoming_message(req, &dest_addr_loc);
              }else incoming_message(raw_msg_loc, &dest_addr_loc);
              break;
            default:
              break;
          }
        }
        else{
          if(raw_mode){
            if(asp){
              ASP__SIP__Raw req;
              req.raw()=raw_msg_loc;
              req.addr()().remote__host()=dest_addr_loc.host();
              req.addr()().remote__port()=dest_addr_loc.portField();
              req.addr()().protocol()=udportcp_loc?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
              if(local_addr)
                req.addr()().local__host()()=local_addr;
              else req.addr()().local__host()=OMIT_VALUE;
              req.addr()().local__port()=local_port;
              incoming_message(req, &dest_addr_loc);
            }else incoming_message(raw_msg_loc, &dest_addr_loc);
          } else if(reqmsg_loc!=NULL){
            if(asp){
              ASP__SIP__Request req;
              req.request()=*reqmsg_loc;
              req.addr()().remote__host()=dest_addr_loc.host();
              req.addr()().remote__port()=dest_addr_loc.portField();
              req.addr()().protocol()=udportcp_loc?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
              if(local_addr)
                req.addr()().local__host()()=local_addr;
              else req.addr()().local__host()=OMIT_VALUE;
              req.addr()().local__port()=local_port;
              incoming_message(req, &dest_addr_loc);
            }else incoming_message(*reqmsg_loc, &dest_addr_loc);
            delete reqmsg_loc;
            reqmsg_loc=NULL;
          }
          else{
            if(asp){
              ASP__SIP__Response resp;
              resp.response()=*respmsg_loc;
              resp.addr()().remote__host()=dest_addr_loc.host();
              resp.addr()().remote__port()=dest_addr_loc.portField();
              resp.addr()().protocol()=udportcp_loc?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
              if(local_addr)
                resp.addr()().local__host()()=local_addr;
              else resp.addr()().local__host()=OMIT_VALUE;
              resp.addr()().local__port()=local_port;
              incoming_message(resp, &dest_addr_loc);
            }else incoming_message(*respmsg_loc, &dest_addr_loc);
            delete respmsg_loc;
            respmsg_loc=NULL;
          }
        }

        reduce_buff(a,msg_in_buffer_loc,msg_in_len_loc);
        msg_in_len_loc-=a;
        wait_msg_body_loc=0;
      }
      if(msg_in_len_loc==0)return;  // empty buffer
      while((*atm<=' ') &&(*atm>0)) {atm++;}  // skip leading blanks
      if(strstr(atm,"\n\r\n")==NULL){return;}  // search for alone crlf
    }

    rqlineptr=NULL;
    stlineptr=NULL;
    headerptr= newMsgHdr();
    num_chars=0;
    if (debug) {
      CHARSTRING ch=CHARSTRING(msg_in_len_loc,msg_in_buffer_loc);
      TTCN_Logger::begin_event(TTCN_DEBUG);
      TTCN_Logger::log_event("SIP test port (%s) received: ", get_name());
      ch.log();
      TTCN_Logger::end_event();
    }
    if(raw_mode){
      if(udportcp_loc){
        errorind_loc=0;
        const char *end;
        const char *atm=msg_in_buffer_loc;
        while((*atm<=' ') &&(*atm>0)) {atm++;}  // skip leading blanks
        end=strstr(atm,"\n\r\n");  // search for alone crlf
        if(end)
          num_chars=(end-msg_in_buffer_loc)+3;
        else num_chars=msg_in_len_loc;
        headerptr->contentLength()().fieldName()=FieldName::CONTENT__LENGTH__E;
        headerptr->contentLength()().len()=get_content_length(msg_in_buffer_loc,num_chars);
        log("Content-length: %i",(int)headerptr->contentLength()().len());
      } else num_chars=msg_in_len_loc;
    } else {
      erro_handling_sip_parser=error_mode;
      errorind_loc=7;   // 111
      wildcarded_enabled_parser=wildcarded_enabled_port;
      parsing(msg_in_buffer_loc,msg_in_len_loc, ipv6enabled);
//      SIP_parse_debug=1;
//      SIP_parse_parse();  // also sets appropriate fields of msg through pointers..
    }
    errorind_l=errorind_loc;
    if(errorind_loc){
      switch(error_mode){
        case 2:
          delete rqlineptr;
          delete stlineptr;
          log("Parse error, message dropped.");
          if(!udportcp_loc) {delete headerptr; return;}
          break;
        case 1:
          delete rqlineptr;
          delete stlineptr;
          raw_msg_loc=CHARSTRING(num_chars,msg_in_buffer_loc);
          break;
        default:
          TTCN_error("SIP parse error.");
          break;
      }
    } else if(rqlineptr!=NULL){
      reqmsg_loc= new PDU__SIP__Request;
      reqmsg_loc->requestLine()=*rqlineptr;
      reqmsg_loc->msgHeader()=*headerptr;
      reqmsg_loc->messageBody()=OMIT_VALUE;
      reqmsg_loc->payload()=OMIT_VALUE;
      delete rqlineptr;
    }
    else if(stlineptr!=NULL){
      respmsg_loc= new PDU__SIP__Response;
      respmsg_loc->statusLine()=*stlineptr;
      respmsg_loc->msgHeader()=*headerptr;
      respmsg_loc->messageBody()=OMIT_VALUE;
      respmsg_loc->payload()=OMIT_VALUE;
      delete stlineptr;
    } else {
      raw_msg_loc=CHARSTRING(num_chars,msg_in_buffer_loc);
    }

    if(udportcp_loc){   // TCP
      reduce_buff(num_chars,msg_in_buffer_loc,msg_in_len_loc);
      msg_in_len_loc-=num_chars;
      if(headerptr->contentLength().ispresent() &&
                                         headerptr->contentLength()().len()!=0){
         // Message body is present
        int a=(int)headerptr->contentLength()().len();

        if(a>msg_in_len_loc){wait_msg_body_loc=a; delete headerptr; log("Waiting for msg body");return;} // wait for the message body
        if(errorind_loc){
          switch(error_mode){
            case 2:
              break;
            case 1:
              raw_msg_loc=raw_msg_loc+CHARSTRING(a,msg_in_buffer_loc);
              break;
            default:
              break;
          }
        } else if(raw_mode){
          raw_msg_loc=raw_msg_loc+CHARSTRING(a,msg_in_buffer_loc);
        } else if(reqmsg_loc!=NULL){
          reqmsg_loc->messageBody()()=CHARSTRING(a,msg_in_buffer_loc);
        }
        else if(respmsg_loc!=NULL){
          respmsg_loc->messageBody()()=CHARSTRING(a,msg_in_buffer_loc);
        }
        reduce_buff(a,msg_in_buffer_loc,msg_in_len_loc);
        msg_in_len_loc-=a;
      }
    }
    else{           // UDP
      more_msg=0;    // no more posible message in buffer
      if(msg_in_len_loc>num_chars){    // Message body is present
        if(errorind_loc){
          switch(error_mode){
            case 2:
              break;
            case 1:
              raw_msg_loc=CHARSTRING(msg_in_len_loc,msg_in_buffer_loc);
              break;
            default:
              break;
          }
        } else if(reqmsg_loc!=NULL){
          reqmsg_loc->messageBody()()=CHARSTRING(msg_in_len_loc-num_chars,
                                             msg_in_buffer_loc+num_chars);
        }
        else{
          respmsg_loc->messageBody()()=CHARSTRING(msg_in_len_loc-num_chars,
                                             msg_in_buffer_loc+num_chars);
        }
      }
      else{
        if(reqmsg_loc!=NULL){    // Message body isn't present
          reqmsg_loc->messageBody()=OMIT_VALUE;
        }
        else if(respmsg_loc!=NULL){
          respmsg_loc->messageBody()=OMIT_VALUE;
        }
      }
      *msg_in_buffer_loc='\0';   // clear buffer
    }

    delete headerptr;
    if(errorind_loc){
      switch(error_mode){
        case 2:
          break;
        case 1:
          if(asp){
            ASP__SIP__Raw req;
            req.raw()=raw_msg_loc;
            req.addr()().remote__host()=dest_addr_loc.host();
            req.addr()().remote__port()=dest_addr_loc.portField();
            req.addr()().protocol()=udportcp_loc?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
            if(local_addr)
              req.addr()().local__host()()=local_addr;
            else req.addr()().local__host()=OMIT_VALUE;
            req.addr()().local__port()=local_port;
            incoming_message(req, &dest_addr_loc);
          }else incoming_message(raw_msg_loc, &dest_addr_loc);
          break;
        default:
          break;
      }
    }
    else{
      if(raw_mode){
        if(asp){
          ASP__SIP__Raw req;
          req.raw()=raw_msg_loc;
          req.addr()().remote__host()=dest_addr_loc.host();
          req.addr()().remote__port()=dest_addr_loc.portField();
          req.addr()().protocol()=udportcp_loc?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
          if(local_addr)
            req.addr()().local__host()()=local_addr;
          else req.addr()().local__host()=OMIT_VALUE;
          req.addr()().local__port()=local_port;
          incoming_message(req, &dest_addr_loc);
        }else incoming_message(raw_msg_loc, &dest_addr_loc);
      } else if(reqmsg_loc!=NULL){
        if(asp){
          ASP__SIP__Request req;
          req.request()=*reqmsg_loc;
          req.addr()().remote__host()=dest_addr_loc.host();
          req.addr()().remote__port()=dest_addr_loc.portField();
          req.addr()().protocol()=udportcp_loc?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
          if(local_addr)
            req.addr()().local__host()()=local_addr;
          else req.addr()().local__host()=OMIT_VALUE;
          req.addr()().local__port()=local_port;
          incoming_message(req, &dest_addr_loc);
        }else incoming_message(*reqmsg_loc, &dest_addr_loc);
        delete reqmsg_loc;
        reqmsg_loc=NULL;
      }
      else if(respmsg_loc!=NULL){
        if(asp){
          ASP__SIP__Response resp;
          resp.response()=*respmsg_loc;
          resp.addr()().remote__host()=dest_addr_loc.host();
          resp.addr()().remote__port()=dest_addr_loc.portField();
          resp.addr()().protocol()=udportcp_loc?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
          if(local_addr)
            resp.addr()().local__host()()=local_addr;
          else resp.addr()().local__host()=OMIT_VALUE;
          resp.addr()().local__port()=local_port;
          incoming_message(resp, &dest_addr_loc);
        }else incoming_message(*respmsg_loc, &dest_addr_loc);
        delete respmsg_loc;
        respmsg_loc=NULL;
      }
    }
  } //while (more_msg)
  log("Decode message finished");

}

int SIPmsg__PT::get_content_length(char *buff, int length){
  int current_state=0;
  int ret_val=0;
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

void SIPmsg__PT::outgoing_send(const ASP__SIP__close& send_par, const ADDRESS *destination_address){
  if(send_par.addr().remote__host().ispresent() && send_par.addr().remote__port().ispresent()){
    int conn_id=get_conn_id(send_par.addr().remote__host()(),send_par.addr().remote__port()());
    if(conn_id>0) {log("Connection to %s:%i is closed",conn_list[conn_id].addr,conn_list[conn_id].port);close_conn(conn_id);}
    if(conn_id<0) TTCN_warning("No connection found to close! Maybe it is already closed.");
  } else if(send_par.addr().protocol().ispresent() && send_par.addr().protocol()()==SIP__com__prot::TCP__E) {
    if(listen_soc_tcp!=-1){
      close(listen_soc_tcp);
      FD_CLR(listen_soc_tcp,&connections_read_fds);
      listen_soc_tcp=-1;
      Install_Handler(&connections_read_fds,   NULL  ,   NULL  , 0.0);
      log("TCP listening socket is closed");
    } else TTCN_warning("TCP listening socket is already closed!");
  } else if(send_par.addr().protocol().ispresent() && send_par.addr().protocol()()==SIP__com__prot::UDP__E) {
    if(listen_soc!=-1){
      close_conn(0);
      listen_soc=-1;
      log("UDP listening socket is closed");
    } else TTCN_warning("UDP listening socket is already closed!");
  } else TTCN_warning("Invalid close command!");
}

void SIPmsg__PT::outgoing_send(const ASP__SIP__open& send_par, const ADDRESS *destination_address){
  if(send_par.addr().remote__host().ispresent()){
    check_address(&send_par.addr());
    if (comm_soc==-1) return;
    if (close_soc) {close(comm_soc);comm_soc=-1;close_soc=false;}
    if(udportcp) log("TCP connection is opened.");
  } else if(send_par.addr().protocol().ispresent() && send_par.addr().protocol()()==SIP__com__prot::TCP__E){
    const char *loc_comm_addr=local_addr;
    int loc_com_port=local_port;
    if(send_par.addr().local__host().ispresent())
      loc_comm_addr=send_par.addr().local__host()();
    if(send_par.addr().local__port().ispresent())
      loc_com_port=send_par.addr().local__port()();
    if(listen_soc_tcp!=-1){
      close(listen_soc_tcp);
      FD_CLR(listen_soc_tcp,&connections_read_fds);
    }
    int reuse=1;
    set_addr_struct(&listen_addr, loc_com_port, loc_comm_addr);
    if((listen_soc_tcp=socket(PF_INET, SOCK_STREAM, 0))<0){
      if(report_error) {
        ADDRESS ad;
        ASP__SIP__error ec;
        ad.host()=OMIT_VALUE;
        ad.portField()=OMIT_VALUE;
        ad.tcporudp()=OMIT_VALUE;
        ec.error__code()=errno;
        ec.error__text()=strerror(errno);
        errno=0;
        ec.failed__operation()=SIP__operation::SIP__OP__CREATE__SOCKET;
        ec.addr()().remote__host()=OMIT_VALUE;
        ec.addr()().remote__port()=OMIT_VALUE;
        ec.addr()().protocol()=SIP__com__prot::TCP__E;
        if(loc_comm_addr)
          ec.addr()().local__host()()=loc_comm_addr;
        else ec.addr()().local__host()=OMIT_VALUE;
        ec.addr()().local__port()=loc_com_port;
        ec.SIP__message()=OMIT_VALUE;
        incoming_message(ec,&ad);
        return;
      } else TTCN_error("Listening socket creation failed.");
    }
    if(setsockopt(listen_soc_tcp, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse,
                           sizeof(reuse)) < 0){
      if(report_error) {
        close(listen_soc_tcp);
        listen_soc_tcp=-1;
        ADDRESS ad;
        ASP__SIP__error ec;
        ad.host()=OMIT_VALUE;
        ad.portField()=OMIT_VALUE;
        ad.tcporudp()=OMIT_VALUE;
        ec.error__code()=errno;
        ec.error__text()=strerror(errno);
        errno=0;
        ec.failed__operation()=SIP__operation::SIP__OP__SET__SOCKET__OPT;
        ec.addr()().remote__host()=OMIT_VALUE;
        ec.addr()().remote__port()=OMIT_VALUE;
        ec.addr()().protocol()=SIP__com__prot::TCP__E;
        if(loc_comm_addr)
          ec.addr()().local__host()()=loc_comm_addr;
        else ec.addr()().local__host()=OMIT_VALUE;
        ec.addr()().local__port()=loc_com_port;
        ec.SIP__message()=OMIT_VALUE;
        incoming_message(ec,&ad);
        return;
      } else  TTCN_error("setsockopt error");
    }
    if(fcntl(listen_soc_tcp, F_SETFD, O_NONBLOCK) == -1){
      if(report_error) {
        close(listen_soc_tcp);
        listen_soc_tcp=-1;
        ADDRESS ad;
        ASP__SIP__error ec;
        ad.host()=OMIT_VALUE;
        ad.portField()=OMIT_VALUE;
        ad.tcporudp()=OMIT_VALUE;
        ec.error__code()=errno;
        ec.error__text()=strerror(errno);
        errno=0;
        ec.failed__operation()=SIP__operation::SIP__OP__SOCKET__FCNTL;
        ec.addr()().remote__host()=OMIT_VALUE;
        ec.addr()().remote__port()=OMIT_VALUE;
        ec.addr()().protocol()=SIP__com__prot::TCP__E;
        if(loc_comm_addr)
          ec.addr()().local__host()()=loc_comm_addr;
        else ec.addr()().local__host()=OMIT_VALUE;
        ec.addr()().local__port()=loc_com_port;
        ec.SIP__message()=OMIT_VALUE;
        incoming_message(ec,&ad);
        return;
      } else TTCN_error("Fcntl error");
    }
    if (bind(listen_soc_tcp,(struct sockaddr*)&listen_addr,sizeof(listen_addr))  < 0){
      if(report_error) {
        close(listen_soc_tcp);
        listen_soc_tcp=-1;
        ADDRESS ad;
        ASP__SIP__error ec;
        ad.host()=OMIT_VALUE;
        ad.portField()=OMIT_VALUE;
        ad.tcporudp()=OMIT_VALUE;
        ec.error__code()=errno;
        ec.error__text()=strerror(errno);
        errno=0;
        ec.failed__operation()=SIP__operation::SIP__OP__SOCKET__BIND;
        ec.addr()().remote__host()=OMIT_VALUE;
        ec.addr()().remote__port()=OMIT_VALUE;
        ec.addr()().protocol()=SIP__com__prot::TCP__E;
        if(loc_comm_addr)
          ec.addr()().local__host()()=loc_comm_addr;
        else ec.addr()().local__host()=OMIT_VALUE;
        ec.addr()().local__port()=loc_com_port;
        ec.SIP__message()=OMIT_VALUE;
        incoming_message(ec,&ad);
        return;
      } else TTCN_error("Listening socket bind failed. Used port number: %d",local_port);
    }
    if(listen(listen_soc_tcp,1)<0){
      if(report_error) {
        close(listen_soc_tcp);
        listen_soc_tcp=-1;
        ADDRESS ad;
        ASP__SIP__error ec;
        ad.host()=OMIT_VALUE;
        ad.portField()=OMIT_VALUE;
        ad.tcporudp()=OMIT_VALUE;
        ec.error__code()=errno;
        ec.error__text()=strerror(errno);
        errno=0;
        ec.failed__operation()=SIP__operation::SIP__OP__SOCKET__LISTEN;
        ec.addr()().remote__host()=OMIT_VALUE;
        ec.addr()().remote__port()=OMIT_VALUE;
        ec.addr()().protocol()=SIP__com__prot::TCP__E;
        if(loc_comm_addr)
          ec.addr()().local__host()()=loc_comm_addr;
        else ec.addr()().local__host()=OMIT_VALUE;
        ec.addr()().local__port()=loc_com_port;
        ec.SIP__message()=OMIT_VALUE;
        incoming_message(ec,&ad);
        return;
      } else TTCN_error("Listen failed.");
    }
    FD_SET(listen_soc_tcp, &connections_read_fds);
    Install_Handler(&connections_read_fds,   NULL  ,   NULL  , 0.0);
    log("TCP listening socket is opened.");
  } else if(send_par.addr().protocol().ispresent() && send_par.addr().protocol()()==SIP__com__prot::UDP__E) {
    const char *loc_comm_addr=local_addr;
    const char *dest_comm_addr=target_addr;
    int loc_com_port=local_port;
    int dest_com_port=target_port;
    if(send_par.addr().remote__host().ispresent())
      dest_comm_addr=send_par.addr().remote__host()();
    if(send_par.addr().local__host().ispresent())
      loc_comm_addr=send_par.addr().local__host()();
    if(send_par.addr().remote__port().ispresent())
      dest_com_port=send_par.addr().remote__port()();
    if(send_par.addr().local__port().ispresent())
      loc_com_port=send_par.addr().local__port()();
    if(listen_soc!=-1){
      close(listen_soc);
      FD_CLR(listen_soc,&connections_read_fds);
    }
    set_addr_struct(&listen_addr, loc_com_port, loc_comm_addr);
    if((listen_soc=socket(PF_INET, SOCK_DGRAM, 0))<0){
      if(report_error) {
        ADDRESS ad;
        ASP__SIP__error ec;
        ad.host()=OMIT_VALUE;
        ad.portField()=OMIT_VALUE;
        ad.tcporudp()=OMIT_VALUE;
        ec.error__code()=errno;
        ec.error__text()=strerror(errno);
        errno=0;
        ec.failed__operation()=SIP__operation::SIP__OP__CREATE__SOCKET;
        ec.addr()().remote__host()=OMIT_VALUE;
        ec.addr()().remote__port()=OMIT_VALUE;
        ec.addr()().protocol()=SIP__com__prot::UDP__E;
        if(loc_comm_addr)
          ec.addr()().local__host()()=loc_comm_addr;
        else ec.addr()().local__host()=OMIT_VALUE;
        ec.addr()().local__port()=loc_com_port;
        ec.SIP__message()=OMIT_VALUE;
        incoming_message(ec,&ad);
        return;
      } else  TTCN_error("[start] Listening socket creation failed.");
    }
    if(fcntl(listen_soc, F_SETFD, O_NONBLOCK) == -1){
      if(report_error) {
        close(listen_soc);
        listen_soc=-1;
        ADDRESS ad;
        ASP__SIP__error ec;
        ad.host()=OMIT_VALUE;
        ad.portField()=OMIT_VALUE;
        ad.tcporudp()=OMIT_VALUE;
        ec.error__code()=errno;
        ec.error__text()=strerror(errno);
        errno=0;
        ec.failed__operation()=SIP__operation::SIP__OP__SOCKET__FCNTL;
        ec.addr()().remote__host()=OMIT_VALUE;
        ec.addr()().remote__port()=OMIT_VALUE;
        ec.addr()().protocol()=SIP__com__prot::UDP__E;
        if(loc_comm_addr)
          ec.addr()().local__host()()=loc_comm_addr;
        else ec.addr()().local__host()=OMIT_VALUE;
        ec.addr()().local__port()=loc_com_port;
        ec.SIP__message()=OMIT_VALUE;
        incoming_message(ec,&ad);
        return;
      } else TTCN_error("Fcntl error");
    }
    if (bind(listen_soc,(struct sockaddr*)&listen_addr,sizeof(listen_addr))< 0){
      if(report_error) {
        close(listen_soc);
        listen_soc=-1;
        ADDRESS ad;
        ASP__SIP__error ec;
        ad.host()=OMIT_VALUE;
        ad.portField()=OMIT_VALUE;
        ad.tcporudp()=OMIT_VALUE;
        ec.error__code()=errno;
        ec.error__text()=strerror(errno);
        errno=0;
        ec.failed__operation()=SIP__operation::SIP__OP__SOCKET__BIND;
        ec.addr()().remote__host()=OMIT_VALUE;
        ec.addr()().remote__port()=OMIT_VALUE;
        ec.addr()().protocol()=SIP__com__prot::UDP__E;
        if(loc_comm_addr)
          ec.addr()().local__host()()=loc_comm_addr;
        else ec.addr()().local__host()=OMIT_VALUE;
        ec.addr()().local__port()=loc_com_port;
        ec.SIP__message()=OMIT_VALUE;
        incoming_message(ec,&ad);
        return;
      } else TTCN_error("[start] Listening socket bind failed. Used port number: %d",local_port);
    }
    FD_SET(listen_soc, &connections_read_fds);
    conn_list[0].fp=listen_soc;
    conn_list[0].port=dest_com_port;
    if(dest_comm_addr){
      set_addr_struct(&remote_addr, dest_com_port, dest_comm_addr);
      conn_list[0].addr=(char *)Realloc(conn_list[0].addr,strlen(dest_comm_addr)+1);
      strcpy(conn_list[0].addr,dest_comm_addr);
    }
    Install_Handler(&connections_read_fds,   NULL  ,   NULL  , 0.0);

    log("UDP listening socket is opened.");
  }
}

void SIPmsg__PT::outgoing_send(const ASP__SIP__Response& send_par,
                                            const ADDRESS *destination_address){
  if(send_par.addr().ispresent()){
    check_address(&send_par.addr()());
  } else check_address((SIP__comm__adress*)NULL);
  if (comm_soc==-1) return;
  char tempsend[20];
  msg_encode_buff_size=mtu_size?mtu_size:BUF_SIZE;
  msg_encode_buff=(char *)Malloc(msg_encode_buff_size*sizeof(char));
  msgsize=0;
  msg_encode_buff[0]='\0';

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,send_par.response().statusLine().sipVersion());
  // SIP version

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );

  sprintf(tempsend, "%d" , (int)send_par.response().statusLine().statusCode());
  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, tempsend);

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, send_par.response().statusLine().reasonPhrase() );
   // Reason phrase

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );
  if (send_par.response().messageBody().ispresent() && auto_length){
    body_length=send_par.response().messageBody()().lengthof();
  } else body_length=-1;

  encode_headers(msg_encode_buff,msgsize,msg_encode_buff_size,body_length,header_mode,multiple_headers,ipv6enabled,&(send_par.response().msgHeader()));
  // Headers

  // MESSAGE BODY SECTION //

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );  // LONE CRLF
  if (send_par.response().messageBody().ispresent()){
    write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, send_par.response().messageBody()());
  }

  send_msg(msg_encode_buff);
  Free(msg_encode_buff);
  msg_encode_buff=NULL;
  msg_encode_buff_size=0;

}

void SIPmsg__PT::outgoing_send(const PDU__SIP__Response& send_par,
                                            const ADDRESS *destination_address){
  msg_encode_buff_size=mtu_size?mtu_size:BUF_SIZE;;
  msg_encode_buff=(char *)Malloc(msg_encode_buff_size*sizeof(char));
  char tempsend[20];
  check_address(destination_address);
  if (comm_soc==-1) return;
  msgsize=0;
  msg_encode_buff[0]='\0';

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,send_par.statusLine().sipVersion());
  // SIP version

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );

  sprintf(tempsend, "%d" , (int)send_par.statusLine().statusCode());
  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, tempsend);

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, send_par.statusLine().reasonPhrase() );
   // Reason phrase

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );
  if (send_par.messageBody().ispresent() && auto_length){
    body_length=send_par.messageBody()().lengthof();
  } else body_length=-1;

  encode_headers(msg_encode_buff,msgsize,msg_encode_buff_size,body_length,header_mode,multiple_headers,ipv6enabled,&(send_par.msgHeader()));
  // Headers

  // MESSAGE BODY SECTION //

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );  // LONE CRLF
  if (send_par.messageBody().ispresent()){
    write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, send_par.messageBody()());
  }

  send_msg(msg_encode_buff);
  Free(msg_encode_buff);
  msg_encode_buff=NULL;
  msg_encode_buff_size=0;

}

void SIPmsg__PT::outgoing_send(const ASP__SIP__Raw& send_par,
                                            const ADDRESS *destination_address){
  if(send_par.addr().ispresent()){
    check_address(&send_par.addr()());
  } else check_address((SIP__comm__adress*)NULL);
  if (comm_soc==-1) return;
  msgsize=((const CHARSTRING&)send_par.raw()).lengthof();
  send_msg((const CHARSTRING&)send_par.raw());

}

void SIPmsg__PT::outgoing_send(const PDU__SIP__Raw& send_par,
                                            const ADDRESS *destination_address){
  check_address(destination_address);
  if (comm_soc==-1) return;
  msgsize=((const CHARSTRING&)send_par).lengthof();
  send_msg((const CHARSTRING&)send_par);
}

void SIPmsg__PT::outgoing_send(const ASP__SIP__Request& send_par,
                                            const ADDRESS *destination_address){
  if(send_par.addr().ispresent()){
    check_address(&send_par.addr()());
  } else check_address((SIP__comm__adress*)NULL);
  if (comm_soc==-1) return;
  msg_encode_buff_size=mtu_size?mtu_size:BUF_SIZE;;
  msg_encode_buff=(char *)Malloc(msg_encode_buff_size*sizeof(char));
  // REQUEST LINE SECTION //

  msgsize=strlen(Method::enum_to_str(send_par.request().requestLine().method()))-2;
  strncpy(msg_encode_buff, Method::enum_to_str(send_par.request().requestLine().method()),msgsize);

  msg_encode_buff[msgsize]='\0';  // Method

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );
  print_url(msg_encode_buff,msgsize,msg_encode_buff_size,&send_par.request().requestLine().requestUri(), 0, ipv6enabled ); // Request URI

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size," " );

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,send_par.request().requestLine().sipVersion()); // SIP version

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,"\r\n" );

  if (send_par.request().messageBody().ispresent() && auto_length){
    body_length=send_par.request().messageBody()().lengthof();
  } else body_length=-1;
  encode_headers(msg_encode_buff,msgsize,msg_encode_buff_size,body_length,header_mode,multiple_headers,ipv6enabled,&(send_par.request().msgHeader()));
  // Headers

  // MESSAGE BODY SECTION //

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,"\r\n" );  // LONE CRLF
  if (send_par.request().messageBody().ispresent()){
    write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,send_par.request().messageBody()());
  }

   send_msg(msg_encode_buff);
  Free(msg_encode_buff);
  msg_encode_buff=NULL;
  msg_encode_buff_size=0;

}

void SIPmsg__PT::outgoing_send(const PDU__SIP__Request& send_par,
                                            const ADDRESS *destination_address){
  msg_encode_buff_size=mtu_size?mtu_size:BUF_SIZE;
  msg_encode_buff=(char *)Malloc(msg_encode_buff_size*sizeof(char));
  check_address(destination_address);
  if (comm_soc==-1) return;
  // REQUEST LINE SECTION //

  msgsize=strlen(Method::enum_to_str(send_par.requestLine().method()))-2;
  strncpy(msg_encode_buff, Method::enum_to_str(send_par.requestLine().method()),msgsize);

  msg_encode_buff[msgsize]='\0';  // Method

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );

  print_url(msg_encode_buff,msgsize,msg_encode_buff_size,&send_par.requestLine().requestUri(), 0, ipv6enabled ); // Request URI

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,send_par.requestLine().sipVersion()); // SIP version

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );

  if (send_par.messageBody().ispresent() && auto_length){
    body_length=send_par.messageBody()().lengthof();
  } else body_length=-1;
  encode_headers(msg_encode_buff,msgsize,msg_encode_buff_size,body_length,header_mode,multiple_headers,ipv6enabled,&(send_par.msgHeader()));
  // Headers

  // MESSAGE BODY SECTION //

  write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );  // LONE CRLF
  if (send_par.messageBody().ispresent()){
    write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, send_par.messageBody()());
  }

  send_msg(msg_encode_buff);
  Free(msg_encode_buff);
  msg_encode_buff=NULL;
  msg_encode_buff_size=0;
}


void SIPmsg__PT::encode_headers(char *& buff, int &m_size, int &b_size, int body_s, int h_mode, int m_header, bool ipv6enabled, const MessageHeader *header){
// Encodes the headers
  char tempsend[BUF_SIZE];
  const char *header_sep;

  if (header->accept().ispresent()){        // Accept header
    const Accept *accepthdr=& header->accept()();
    write_to_buff(buff,m_size,b_size, "Accept:");
    header_sep=!m_header?",":"\r\nAccept:";
    if(accepthdr->acceptArgs().ispresent()){
      const AcceptBody__List *acceptargsptr=&accepthdr->acceptArgs()();
      int paramnum=acceptargsptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size,(*acceptargsptr)[a].mediaRange());
        if((*acceptargsptr)[a].acceptParam().ispresent()){
          const char *atm[]={";",";","","="};
          print_list(buff,m_size,b_size,&((*acceptargsptr)[a].acceptParam()()), atm );
        }
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->accept__contact().ispresent()){     // Accept-Contact header
    write_to_buff(buff,m_size,b_size, h_mode?"a:":"Accept-Contact:");
    int contactnum=header->accept__contact()().ac__values().size_of();
    header_sep=!m_header?",":h_mode?"\r\na:":"\r\nAccept-Contact:";
    for(int a=0;a<contactnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size, "*");
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& header->accept__contact()().ac__values()[a], atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->acceptEncoding().ispresent()){  // Accept-Encoding header
    const AcceptEncoding *encodeptr=& header->acceptEncoding()();
    write_to_buff(buff,m_size,b_size, "Accept-Encoding:");
    header_sep=!m_header?",":"\r\nAccept-Encoding:";
    if(encodeptr->contentCoding().ispresent()){
      const ContentCoding__List *listptr=&encodeptr->contentCoding()();
      int paramnum=listptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size,(*listptr)[a]);
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->acceptLanguage().ispresent()){    // Accept Language header
    write_to_buff(buff,m_size,b_size, "Accept-Language:");
    header_sep=!m_header?",":"\r\nAccept-Language:";
    if(header->acceptLanguage()().languageBody().ispresent()){
      const LanguageBody__List *listptr=
                                   &header->acceptLanguage()().languageBody()();
      int paramnum=listptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size,(*listptr)[a].languageRange());
        if((*listptr)[a].acceptParam().ispresent()){
          const char *atm[]={";",";","","="};
          print_list(buff,m_size,b_size,&(*listptr)[a].acceptParam()(), atm );
        }
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->acceptResourcePriority().ispresent()){    // Accept-Resource-Priority header
    write_to_buff(buff,m_size,b_size, "Accept-Resource-Priority:");
    header_sep=!m_header?",":"\r\nAccept-Resource-Priority:";
    if(header->acceptResourcePriority()().rvalues().ispresent()){
      const Rvalue__List *listptr=
                                   &header->acceptResourcePriority()().rvalues()();
      int paramnum=listptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size,(*listptr)[a].namespace_());
        write_to_buff(buff,m_size,b_size, ".");
        write_to_buff(buff,m_size,b_size,(*listptr)[a].r__priority());
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }


  if (header->alertInfo().ispresent()){         // Alert Info header
    const AlertInfo *encodeptr=&header->alertInfo()();
    write_to_buff(buff,m_size,b_size, "Alert-Info:");
    header_sep=!m_header?",":"\r\nAlert-Info:";
    if(encodeptr->alertInfoBody().ispresent()){
      const AlertInfoBody__List *listptr=&encodeptr->alertInfoBody()();
      int paramnum=listptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size, "<");
        write_to_buff(buff,m_size,b_size,(*listptr)[a].url());
        write_to_buff(buff,m_size,b_size, ">");
        if((*listptr)[a].genericParams().ispresent()){
          const char *atm[]={";",";","","="};
          print_list(buff,m_size,b_size,&(*listptr)[a].genericParams()(), atm );
        }
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }


  if (header->alert__mode().ispresent()){       // Alert-Mode header
    write_to_buff(buff,m_size,b_size, "Alert-Mode:");
    write_to_buff(buff,m_size,b_size,header->alert__mode()().alert__mode());
    if(header->alert__mode()().alert__mode__param().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,
                         &header->alert__mode()().alert__mode__param()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->allow().ispresent()){           // Allow header
    write_to_buff(buff,m_size,b_size, "Allow:");
    header_sep=!m_header?",":"\r\nAllow:";
    if(header->allow()().methods().ispresent()){
      const Method__List *listptr=&header->allow()().methods()();
      int paramnum=listptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size,(*listptr)[a]);
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->allow__events().ispresent()){           // Allow-Events header
    write_to_buff(buff,m_size,b_size, h_mode?"u:":"Allow-Events:");
    header_sep=!m_header?",":h_mode?"\r\nu:":"\r\nAllow-Events:";
    const Event__type__list *listptr=&header->allow__events()().events();
    int paramnum=listptr->size_of();
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size,(*listptr)[a].event__package());
      if((*listptr)[a].event__templates().ispresent()){
        const Event__template__list *listptr2=&(*listptr)[a].event__templates()();
        for(int i=0;i<listptr2->size_of();i++){
          write_to_buff(buff,m_size,b_size, ".");
          write_to_buff(buff,m_size,b_size,(*listptr2)[i]);
        }
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->answer__mode().ispresent()){       // Answer-Mode header
    write_to_buff(buff,m_size,b_size, "Answer-Mode:");
    write_to_buff(buff,m_size,b_size,header->answer__mode()().answer__mode());
    if(header->answer__mode()().answer__mode__param().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,
                         &header->answer__mode()().answer__mode__param()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->authenticationInfo().ispresent()){ // Authentication-Info header
    const char *atm[]={"",", ","","="};
    write_to_buff(buff,m_size,b_size, "Authentication-Info:");
    print_list(buff,m_size,b_size,& header->authenticationInfo()().ainfo(), atm );

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->authorization().ispresent()){                // Authorization
    const GenericParam__List *listptr;
    const char *atm[]={"",", ","","="};
    write_to_buff(buff,m_size,b_size, "Authorization:");
    if(header->authorization()().body().get_selection()==
                                               Credentials::ALT_digestResponse){
      write_to_buff(buff,m_size,b_size, "Digest ");
      listptr = & header->authorization()().body().digestResponse();
    }
    else{
      const OtherAuth *otherptr=
                             & header->authorization()().body().otherResponse();
      write_to_buff(buff,m_size,b_size, (const CHARSTRING&)otherptr->authScheme());
      write_to_buff(buff,m_size,b_size, " ");
      listptr = & otherptr->authParams();
    }

    print_list(buff,m_size,b_size,listptr, atm );
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->callId().ispresent()){
    write_to_buff(buff,m_size,b_size, h_mode?"i:":"Call-ID:");        // Call-ID header
    write_to_buff(buff,m_size,b_size, header->callId()().callid());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->callInfo().ispresent()){            // Call Info header
    const CallInfo *encodeptr=& header->callInfo()();
    write_to_buff(buff,m_size,b_size, "Call-Info:");
    header_sep=!m_header?",":"\r\nCall-Info:";
    if(encodeptr->callInfoBody().ispresent()){
      const CallInfoBody__List *listptr=& encodeptr->callInfoBody()();
      int paramnum=listptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size, "<");
        write_to_buff(buff,m_size,b_size,(*listptr)[a].url());
        write_to_buff(buff,m_size,b_size, ">");
        if((*listptr)[a].infoParams().ispresent()){
          const char *atm[]={";",";","","="};
          print_list(buff,m_size,b_size,&(*listptr)[a].infoParams()(), atm );
        }
      }
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->contact().ispresent()){              // Contact header
    const ContactBody *contactbody = & header->contact()().contactBody();
    write_to_buff(buff,m_size,b_size, h_mode?"m:":"Contact:");        // nice look

    if (contactbody->get_selection() == ContactBody::ALT_wildcard){
      write_to_buff(buff,m_size,b_size, contactbody->wildcard());
    }
    else{
      const ContactAddress__List *listptr = & contactbody->contactAddresses();
      int listmax = listptr->size_of();
    header_sep=!m_header?",":h_mode?"\r\nm:":"\r\nContact:";
      for (int a = 0;a<listmax;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        print_addr_union(buff,m_size,b_size,&(*listptr)[a].addressField(),ipv6enabled);
        if((*listptr)[a].contactParams().ispresent()){
          const char *atm[]={";",";","","="};
          print_list(buff,m_size,b_size,&(*listptr)[a].contactParams()(), atm );
        }
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }


  if (header->contentDisposition().ispresent()){  // Content-Disposition header
    const ContentDisposition *disp=& header->contentDisposition()();
    write_to_buff(buff,m_size,b_size, "Content-Disposition:");
    write_to_buff(buff,m_size,b_size, disp->dispositionType());
    if(disp->dispositionParams().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& disp->dispositionParams()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->contentEncoding().ispresent()){     // Content-Encoding header
    const ContentCoding__List *listptr=
                                  & header->contentEncoding()().contentCoding();
    int paramnum=listptr->size_of();
    header_sep=!m_header?",":h_mode?"\r\ne:":"\r\nContent-Encoding:";
    write_to_buff(buff,m_size,b_size, h_mode?"e:":"Content-Encoding:");
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size,(*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->contentLanguage().ispresent()){       // Content-Language header
    const LanguageTag__List *listptr=&header->contentLanguage()().languageTag();
    int paramnum=listptr->size_of();
    header_sep=!m_header?",":"\r\nContent-Language:";
    write_to_buff(buff,m_size,b_size, "Content-Language:");
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size,(const CHARSTRING&)(*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->contentLength().ispresent()){          // Content-Length header
    write_to_buff(buff,m_size,b_size, h_mode?"l:":"Content-Length:");
    if(header->contentLength()().len()!=0){
      sprintf(tempsend, "%d" ,(int)header->contentLength()().len() );
    } else {
      if(body_s!=-1){
        sprintf(tempsend, "%d" ,body_s );
      } else {
        sprintf(tempsend, "%d" ,0 );
      }
    }
    write_to_buff(buff,m_size,b_size, tempsend);
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->contentType().ispresent()){           // Content-Type header
    write_to_buff(buff,m_size,b_size, h_mode?"c:":"Content-Type:");
    write_to_buff(buff,m_size,b_size,header->contentType()().mediaType());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->cSeq().ispresent()){
    write_to_buff(buff,m_size,b_size, "CSeq:");              // Cseq header
    write_to_buff(buff,m_size,b_size, int2str(header->cSeq()().seqNumber()));
    write_to_buff(buff,m_size,b_size, " ");
    write_to_buff(buff,m_size,b_size,header->cSeq()().method() );
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->date().ispresent()){                // Date header
    write_to_buff(buff,m_size,b_size, "Date:");
    write_to_buff(buff,m_size,b_size,header->date()().sipDate());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->diversion().ispresent()){                // Diversion header

    write_to_buff(buff,m_size,b_size, "Diversion:");
    const Diversion__params__list *listptr=
                                & header->diversion()().divParams();
    int paramnum=listptr->size_of();
    header_sep="\r\nDiversion:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      if((*listptr)[a].nameAddr().displayName().ispresent()){
        write_to_buff(buff,m_size,b_size, (*listptr)[a].nameAddr().displayName()());
        write_to_buff(buff,m_size,b_size," ");
      }
      print_url(buff,m_size,b_size,&(*listptr)[a].nameAddr().addrSpec(), 2 , ipv6enabled);
      if((*listptr)[a].div__params().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,& (*listptr)[a].div__params()(), atm );
      }
    }

    write_to_buff(buff,m_size,b_size, "\r\n");

  }

  if (header->errorInfo().ispresent()){          // Error-Info header
    write_to_buff(buff,m_size,b_size, "Error-Info:");
    header_sep=!m_header?",":"\r\nError-Info:";
    if(header->errorInfo()().errorInfo().ispresent()){
      const ErrorInfoBody__List *listptr = &header->errorInfo()().errorInfo()();
      int paramnum=listptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size, "<");
        write_to_buff(buff,m_size,b_size,(*listptr)[a].uri());
        write_to_buff(buff,m_size,b_size, ">");
        if((*listptr)[a].genericParams().ispresent()){
          const char *atm[]={";",";","","="};
          print_list(buff,m_size,b_size,&(*listptr)[a].genericParams()(), atm );
        }
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->event().ispresent()){            // Event header
    const Event *eventptr=&header->event()();
    write_to_buff(buff,m_size,b_size, h_mode?"o:":"Event:");
    write_to_buff(buff,m_size,b_size,eventptr->event__type().event__package());
    if(eventptr->event__type().event__templates().ispresent()){
      const Event__template__list *listptr=&eventptr->event__type().event__templates()();
      for(int i=0;i<listptr->size_of();i++){
        write_to_buff(buff,m_size,b_size, ".");
        write_to_buff(buff,m_size,b_size,(*listptr)[i]);
      }
    }
    if(eventptr->event__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,&eventptr->event__params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->expires().ispresent()){          // Expires header
    write_to_buff(buff,m_size,b_size, "Expires:");
    write_to_buff(buff,m_size,b_size,header->expires()().deltaSec());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

 if (header->feature__caps().ispresent()){
     write_to_buff(buff,m_size,b_size, h_mode?"fc:":"Feature-Caps:");
     const FC__Value__List *fcptr=
                                 & header->feature__caps()().fc__values(); 
     int paramnum=fcptr->size_of();
     header_sep=!m_header?",":h_mode?"\r\nfc:":"\r\nFeature-Caps:";
     for(int a=0;a<paramnum;a++){
       if(a){write_to_buff(buff,m_size,b_size, header_sep);}     
       write_to_buff(buff,m_size,b_size, (*fcptr)[a].fc__string());
       if((*fcptr)[a].feature__param().ispresent()){
          const char *atm[]={";",";","","="};  
          print_list(buff,m_size,b_size,& (*fcptr)[a].feature__param()(), atm );          
      }     
    }    
    write_to_buff(buff,m_size,b_size, "\r\n");
  }  
       
  if (header->fromField().ispresent()){                     // From header
    const From *fromptr=&header->fromField()();
    write_to_buff(buff,m_size,b_size, h_mode?"f:":"From:");
    print_addr_union(buff,m_size,b_size,& fromptr->addressField(),ipv6enabled);
    if(fromptr->fromParams().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& fromptr->fromParams()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->geolocation().ispresent()){     // Geolocation header
    write_to_buff(buff,m_size,b_size, "Geolocation:");
    const Location__value__list *listptr=
                                & header->geolocation()().location__values();
    int paramnum=listptr->size_of();
    header_sep=!m_header?",":"\r\nGeolocation:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      print_url(buff,m_size,b_size,&(*listptr)[a].location__uri(), 2 ,ipv6enabled);
      if((*listptr)[a].location__params().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,& (*listptr)[a].location__params()(), atm );
      }
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->geolocation__routing().ispresent()){     // Georouting header
    write_to_buff(buff,m_size,b_size, "Geolocation-Routing:");
    write_to_buff(buff,m_size,b_size, header->geolocation__routing()().georouting__param());
    if(header->geolocation__routing()().georouting__value().ispresent()){
      write_to_buff(buff,m_size,b_size, "=");
      write_to_buff(buff,m_size,b_size, header->geolocation__routing()().georouting__value()());
    }
    
    write_to_buff(buff,m_size,b_size, "\r\n");
  }
    
  if (header->geolocation__error().ispresent()){     // Geolocation-Error header
    write_to_buff(buff,m_size,b_size, "Geolocation-Error: ");
    write_to_buff(buff,m_size,b_size, int2str(header->geolocation__error()().location__error__code()));
    write_to_buff(buff,m_size,b_size, " ");
      if(header->geolocation__error()().location__error__params().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,& header->geolocation__error()().location__error__params()(), atm );
      }
    
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->historyInfo().ispresent()){                // History-Info header

    write_to_buff(buff,m_size,b_size, "History-Info:");
    const Hi__Entry__list *listptr=
                                & header->historyInfo()().hi__entries();
    int paramnum=listptr->size_of();
    header_sep=!m_header?",":"\r\nHistory-Info:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      if((*listptr)[a].nameAddr().displayName().ispresent()){
        write_to_buff(buff,m_size,b_size, (*listptr)[a].nameAddr().displayName()());
        write_to_buff(buff,m_size,b_size," ");
      }
      print_url(buff,m_size,b_size,&(*listptr)[a].nameAddr().addrSpec(), 2 ,ipv6enabled);
      if((*listptr)[a].hi__params().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,& (*listptr)[a].hi__params()(), atm );
      }
    }

    write_to_buff(buff,m_size,b_size, "\r\n");

  }
 
  if (header->info__Package().ispresent()){  // Info-Package header
    const Info__Package *infopacp=& header->info__Package()();
    write_to_buff(buff,m_size,b_size, "Info-Package:");
    
    write_to_buff(buff,m_size,b_size, infopacp->info__Package__Type().info__package__name());
    if(infopacp->info__Package__Type().info__package__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& infopacp->info__Package__Type().info__package__params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }   
     
  if (header->inReplyTo().ispresent()){         // In-Teply-To header
    const CallidString__List *listptr=& header->inReplyTo()().callids();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "In-Reply-To:");
    header_sep=!m_header?",":"\r\nIn-Reply-To:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size,(*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->join().ispresent()){     // Join header
    write_to_buff(buff,m_size,b_size, "Join:");
    write_to_buff(buff,m_size,b_size, header->join()().callid());
    if(header->join()().joinParams().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& header->join()().joinParams()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->maxForwards().ispresent()){       // Max-Forwards header
    write_to_buff(buff,m_size,b_size, "Max-Forwards:");
    sprintf(tempsend, "%d" ,(int)header->maxForwards()().forwards() );
    write_to_buff(buff,m_size,b_size, tempsend);
    write_to_buff(buff,m_size,b_size, "\r\n");
  }


  if (header->mimeVersion().ispresent()){       // Mime-Version header
    const MimeVersion *mimeptr=&header->mimeVersion()();
    write_to_buff(buff,m_size,b_size, "Mime-Version:");
    sprintf(tempsend, "%d" ,(int)mimeptr->majorNumber() );
    write_to_buff(buff,m_size,b_size, tempsend);
    write_to_buff(buff,m_size,b_size, ".");
    sprintf(tempsend, "%d" ,(int)mimeptr->minorNumber() );
    write_to_buff(buff,m_size,b_size, tempsend);
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->minExpires().ispresent()){          // Min-Expires header
    write_to_buff(buff,m_size,b_size, "Min-Expires:");
    write_to_buff(buff,m_size,b_size,header->minExpires()().deltaSec());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->min__SE().ispresent()){   // min-SE header
    const Min__SE *ptr=& header->min__SE()();
    write_to_buff(buff,m_size,b_size, "Min-SE:");
    write_to_buff(buff,m_size,b_size, ptr->deltaSec());
    if(ptr->params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& ptr->params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }


  if (header->organization().ispresent()){       // Organization header
    write_to_buff(buff,m_size,b_size, "Organization:");
    write_to_buff(buff,m_size,b_size,header->organization()().organization());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  
  if (header->p__access__network__info().ispresent()){     // P-Access-Network-Info header
    write_to_buff(buff,m_size,b_size, "P-Access-Network-Info:");
    const Access__net__spec__list *listptr=
                                & header->p__access__network__info()().access__net__specs();
    int paramnum=listptr->size_of();
    header_sep=!m_header?",":"\r\nP-Access-Network-Info:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size, (*listptr)[a].access__type());
      if((*listptr)[a].access__info().ispresent()){
          const char *atm[]={";",";","","="};
          print_list(buff,m_size,b_size,& (*listptr)[a].access__info()(), atm );          
      }     
    }    
    write_to_buff(buff,m_size,b_size, "\r\n");
  }
  
  if (header->p__alerting__mode().ispresent()){       // P-Alerting-Mode header
    write_to_buff(buff,m_size,b_size, "P-Alerting-Mode:");
    write_to_buff(buff,m_size,b_size,
                                header->p__alerting__mode()().alerting__type());
    if(header->p__alerting__mode()().alerting__info().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,
                       &header->p__alerting__mode()().alerting__info()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__answer__state().ispresent()){       // P-Answer-State header
    write_to_buff(buff,m_size,b_size, "P-Answer-State:");
    write_to_buff(buff,m_size,b_size,
                                header->p__answer__state()().answer__type());
    if(header->p__answer__state()().answer__info().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,
                       &header->p__answer__state()().answer__info()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }
  
  if (header->p__Area__Info().ispresent()){     // P-Area-Info header
    write_to_buff(buff,m_size,b_size, "P-Area-Info:");
    const char *atm[]={"",";","","="};
    print_list(buff,m_size,b_size,& header->p__Area__Info()().p__Area__Info__Value(), atm );

    write_to_buff(buff,m_size,b_size, "\r\n");
  }  
  
  if (header->passertedID().ispresent()){     // P-Asserted-Identity header
    const Identity__List *listptr=
                                  & header->passertedID()().ids();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "P-Asserted-Identity:");
    header_sep=!m_header?",":"\r\nP-Asserted-Identity:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      print_addr_union(buff,m_size,b_size,&(*listptr)[a],ipv6enabled);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }


  if (header->p__asserted__service().ispresent()){      // P-Asserted-Service header
    const Service__ID__List *listptr=& header->p__asserted__service()().p__as();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "P-Asserted-Service:");
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, ", ");}
      write_to_buff(buff,m_size,b_size,(*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }
 
  if (header->p__associated__uri().ispresent()){     // P-Associated-URI header
    write_to_buff(buff,m_size,b_size, "P-Associated-URI:");
    const P__Assoc__uri__spec__list *listptr=
                                & header->p__associated__uri()().p__assoc__uris();
    int paramnum=listptr->size_of();
    header_sep=!m_header?",":"\r\nP-Associated-URI:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      if((*listptr)[a].p__asso__uri().displayName().ispresent()){
        write_to_buff(buff,m_size,b_size, (*listptr)[a].p__asso__uri().displayName()());
        write_to_buff(buff,m_size,b_size," ");
      }
      print_url(buff,m_size,b_size,&(*listptr)[a].p__asso__uri().addrSpec(), 2 ,ipv6enabled);
      if((*listptr)[a].ai__params().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,& (*listptr)[a].ai__params()(), atm );
      }
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__called__party__id().ispresent()){     // P-Called-Party-ID header
    write_to_buff(buff,m_size,b_size, "P-Called-Party-ID:");
    if(header->p__called__party__id()().called__pty__id().displayName().ispresent()){
      write_to_buff(buff,m_size,b_size, header->p__called__party__id()().called__pty__id().displayName()());
      write_to_buff(buff,m_size,b_size," ");
    }
    print_url(buff,m_size,b_size,&header->p__called__party__id()().called__pty__id().addrSpec(), 2 ,ipv6enabled);
    if(header->p__called__party__id()().cpid__param().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& header->p__called__party__id()().cpid__param()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__charging__function__address().ispresent()){     // P-Charging-Function-Addresses header
    write_to_buff(buff,m_size,b_size, "P-Charging-Function-Addresses:");
    const char *atm[]={"",";","","="};
    print_list(buff,m_size,b_size,& header->p__charging__function__address()().charge__addr__params(), atm );

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__charging__vector().ispresent()){     // P-Charging-Vector header
    write_to_buff(buff,m_size,b_size, "P-Charging-Vector:icid-value=");
    write_to_buff(buff,m_size,b_size, header->p__charging__vector()().icid__value());
    if(header->p__charging__vector()().charge__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& header->p__charging__vector()().charge__params()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__media__auth().ispresent()){     // P-Media-Authorization header
    const Media__auth__token__list *listptr=& header->p__media__auth()().token__list();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "P-Media-Authorization:");
    header_sep=!m_header?",":"\r\nP-Media-Authorization:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size,hex2str((*listptr)[a]));
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__DCS__trace__pty__id().ispresent()){      // P-DCS-Trace-Party-ID
    write_to_buff(buff,m_size,b_size, "P-DCS-Trace-Party-ID:");
    const NameAddr *addr=& header->p__DCS__trace__pty__id()().name__addr();
    if(addr->displayName().ispresent()){
      write_to_buff(buff,m_size,b_size, addr->displayName()());
      write_to_buff(buff,m_size,b_size," ");
    }
    print_url(buff,m_size,b_size,& addr->addrSpec(), 2 ,ipv6enabled);

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__DCS__OSPS().ispresent()){      // P-DCS-OSPS
    write_to_buff(buff,m_size,b_size, "P-DCS-OSPS:");
    write_to_buff(buff,m_size,b_size, header->p__DCS__OSPS()().OSPS__tag());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__DCS__billing__info().ispresent()){     // P-DCS-Billing-Info
    write_to_buff(buff,m_size,b_size, "P-DCS-Billing-Info:");
    write_to_buff(buff,m_size,b_size, hex2str(header->p__DCS__billing__info()().billing__correlation__ID()));
    write_to_buff(buff,m_size,b_size, "/");
    write_to_buff(buff,m_size,b_size, hex2str(header->p__DCS__billing__info()().FEID__ID()));
    write_to_buff(buff,m_size,b_size, "@");
    print_host(buff,m_size,b_size, header->p__DCS__billing__info()().FEID__host(),ipv6enabled);
    if( header->p__DCS__billing__info()().billing__info__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& header->p__DCS__billing__info()().billing__info__params()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__DCS__LAES().ispresent()){     // P-DCS-LAES
    write_to_buff(buff,m_size,b_size, "P-DCS-LAES:");
    print_host(buff,m_size,b_size, header->p__DCS__LAES()().laes__sig().host()(),ipv6enabled);  // hostname
    if (header->p__DCS__LAES()().laes__sig().portField().ispresent()){
      char tempsend_loc[20];
      write_to_buff(buff,m_size,b_size, ":");
      sprintf(tempsend_loc,"%d",(int) header->p__DCS__LAES()().laes__sig().portField()());
      write_to_buff(buff,m_size,b_size, tempsend_loc);
    }
    if( header->p__DCS__LAES()().laes__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& header->p__DCS__LAES()().laes__params()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if(header->p__DCS__redirect().ispresent()){     // P-DCS-Redirect
    write_to_buff(buff,m_size,b_size, "P-DCS-Redirect:\"");
    print_url(buff,m_size,b_size,&header->p__DCS__redirect()().caller__ID(), 0,ipv6enabled);
    write_to_buff(buff,m_size,b_size, "\"");
    if(header->p__DCS__redirect()().redir__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,&header->p__DCS__redirect()().redir__params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }



  if (header->p__Early__Media().ispresent()){     // P-Early-Media
    write_to_buff(buff,m_size,b_size, "P-Early-Media:");
    header_sep=!m_header?",":"\r\nP-Early-Media:";
    if(header->p__Early__Media()().em__param__list().ispresent()){
      const Em__param__List *listptr=
                            & header->p__Early__Media()().em__param__list()();
      int paramnum=listptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size, (*listptr)[a]);
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }


  if (header->ppreferredID().ispresent()){     // P_Preferred_Identity header
    const Identity__List *listptr=
                                  & header->ppreferredID()().ids();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "P-Preferred-Identity:");
    header_sep=!m_header?",":"\r\nP-Preferred-Identity:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      print_addr_union(buff,m_size,b_size,&(*listptr)[a],ipv6enabled);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }
    
  if (header->p__preferred__service().ispresent()){      // P-Preferred-Service header
    const Service__ID__List *listptr=& header->p__preferred__service()().p__ps();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "P-Preferred-Service:");
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, ", ");}
      write_to_buff(buff,m_size,b_size,(*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }
  
  if(header->p__profile__key().ispresent()){     // P-Profile-Key
    write_to_buff(buff,m_size,b_size, "P-Profile-Key:");
    print_addr_union(buff,m_size,b_size,&header->p__profile__key()().profile__key(),ipv6enabled);
    if(header->p__profile__key()().profile__key__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,&header->p__profile__key()().profile__key__params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if(header->p__served__user().ispresent()){     // P-Served-User header
    write_to_buff(buff,m_size,b_size, "P-Served-User:");
    print_addr_union(buff,m_size,b_size,&header->p__served__user()().served__user(),ipv6enabled);
    if(header->p__served__user()().served__user__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,&header->p__served__user()().served__user__params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__Service__Indication().ispresent()){ //P-Service-Indication
    write_to_buff(buff,m_size,b_size, "P-Service-Indication:");
    write_to_buff(buff,m_size,b_size,header->p__Service__Indication()().service__indication());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->p__Service__Notification().ispresent()){ //P-Service-Notification
    write_to_buff(buff,m_size,b_size, "P-Service-Notification:");
    write_to_buff(buff,m_size,b_size,header->p__Service__Notification()().service__notification());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if(header->p__user__database().ispresent()){     // P-User-Database
    write_to_buff(buff,m_size,b_size, "P-User-Database:");
    print_url(buff,m_size,b_size,&header->p__user__database()().database(), 2,ipv6enabled);
    if(header->p__user__database()().params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,&header->p__user__database()().params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }


  if (header->p__visited__network__id().ispresent()){     // P-Visited-Network-ID header
    write_to_buff(buff,m_size,b_size, "P-Visited-Network-ID:");
    const Network__spec__list *listptr=
                            & header->p__visited__network__id()().vnetworks();
    int paramnum=listptr->size_of();
    header_sep=!m_header?",":"\r\nP-Visited-Network-ID:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size, (*listptr)[a].network__id());
      if((*listptr)[a].network__par().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,& (*listptr)[a].network__par()(), atm );
      }
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->path().ispresent()){      // Path header
    const RouteBody__List *listptr=& header->path()().routeBody();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Path:");
    header_sep=!m_header?",":"\r\nPath:";
    for(int a=0;a<paramnum;a++){
      const NameAddr *addr=& (*listptr)[a].nameAddr();
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      if(addr->displayName().ispresent()){
        write_to_buff(buff,m_size,b_size, addr->displayName()());
        write_to_buff(buff,m_size,b_size," ");
      }
      print_url(buff,m_size,b_size,& addr->addrSpec(), 2 ,ipv6enabled);

      if((*listptr)[a].rrParam().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,&(*listptr)[a].rrParam()(), atm );
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->priority().ispresent()){           // Priority header
    write_to_buff(buff,m_size,b_size, "Priority:");
    write_to_buff(buff,m_size,b_size,header->priority()().priorityValue());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->priv__answer__mode().ispresent()){       // Priv-Answer-Mode header
    write_to_buff(buff,m_size,b_size, "Priv-Answer-Mode:");
    write_to_buff(buff,m_size,b_size,
                                 header->priv__answer__mode()().answer__mode());
    if(header->priv__answer__mode()().answer__mode__param().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,
                 &header->priv__answer__mode()().answer__mode__param()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->privacy().ispresent()){      // Proxy-Require header
    const PrivacyValue__List *listptr=& header->privacy()().privacyValues();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Privacy:");
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, "; ");}
      write_to_buff(buff,m_size,b_size,(*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->proxyAuthenticate().ispresent()){  // Proxy-Authenticate header
    const GenericParam__List *listptr;
    const char *atm[]={"",", ","","="};
    int n_of_c=header->proxyAuthenticate()().challenge().size_of();
    for(int i=0;i<n_of_c;i++){
      const Challenge *ch=&header->proxyAuthenticate()().challenge()[i];
      write_to_buff(buff,m_size,b_size, "Proxy-Authenticate:");
      if( ch->get_selection()==Challenge::ALT_digestCln){
        write_to_buff(buff,m_size,b_size, "Digest ");
        listptr = & ch->digestCln();
      }
      else{
        const OtherAuth *otherptr= & ch->otherChallenge();
        write_to_buff(buff,m_size,b_size, otherptr->authScheme());
        write_to_buff(buff,m_size,b_size, " ");
        listptr = & otherptr->authParams();
      }

      print_list(buff,m_size,b_size,listptr, atm );

      write_to_buff(buff,m_size,b_size, "\r\n");
    }
  }

  if (header->proxyAuthorization().ispresent()){  // Proxy-Authorization header
    const GenericParam__List *listptr;
    const char *atm[]={"",", ","","="};
    write_to_buff(buff,m_size,b_size, "Proxy-Authorization:");
    if(header->proxyAuthorization()().credentials().get_selection()==
                                               Credentials::ALT_digestResponse){
      write_to_buff(buff,m_size,b_size, "Digest ");
      listptr = & header->proxyAuthorization()().credentials().digestResponse();
    }
    else{
      const OtherAuth *otherptr=
                 & header->proxyAuthorization()().credentials().otherResponse();
      write_to_buff(buff,m_size,b_size, otherptr->authScheme());
      write_to_buff(buff,m_size,b_size, " ");
      listptr = & otherptr->authParams();
    }

    print_list(buff,m_size,b_size,listptr, atm );
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->proxyRequire().ispresent()){      // Proxy-Require header
    const OptionTag__List *listptr=& header->proxyRequire()().optionsTags();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Proxy-Require:");
    header_sep=!m_header?",":"\r\nProxy-Require:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size,(*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->rack().ispresent()){ // RAck header
    write_to_buff(buff,m_size,b_size, "RAck:");
    sprintf(tempsend, "%d" ,(int)header->rack()().response__num());
    write_to_buff(buff,m_size,b_size, tempsend);
    write_to_buff(buff,m_size,b_size, " ");
    /*sprintf(tempsend, "%d" ,(int)header->rack()().seqNumber() );
    write_to_buff(buff,m_size,b_size, tempsend);*/
    write_to_buff(buff,m_size,b_size, int2str(header->rack()().seqNumber()));
    write_to_buff(buff,m_size,b_size, " ");
    write_to_buff(buff,m_size,b_size, header->rack()().method());
    write_to_buff(buff,m_size,b_size, "\r\n");

  }

  if (header->reason().ispresent()){  // Reason header
    const ReasonBody__List *listptr=&header->reason()().reasons();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Reason:");
    for(int a=0;a<paramnum;a++){
      write_to_buff(buff,m_size,b_size, (*listptr)[a].protocol());
      if((*listptr)[a].reasonValues().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,& (*listptr)[a].reasonValues()(), atm );
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->recordRoute().ispresent()){      // record-Route header
    const RouteBody__List *listptr=&header->recordRoute()().routeBody();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Record-Route:");
    header_sep=!m_header?",":"\r\nRecord-Route:";
    for(int a=0;a<paramnum;a++){
      const NameAddr *addr=&(*listptr)[a].nameAddr();
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      if(addr->displayName().ispresent()){
        write_to_buff(buff,m_size,b_size,addr->displayName()());
        write_to_buff(buff,m_size,b_size," ");
      }
      print_url(buff,m_size,b_size,& addr->addrSpec(), 2 ,ipv6enabled);

      if((*listptr)[a].rrParam().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,&(*listptr)[a].rrParam()(), atm );
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->recv__Info().ispresent()){        // Recv-Info header
    const Recv__Info *recvinfohdr=& header->recv__Info()();
    write_to_buff(buff,m_size,b_size, "Recv-Info:");
    header_sep=!m_header?",":"\r\nRecv-Info:";
    if(recvinfohdr->info__Package__List().ispresent()){
      const Info__Package__List *infopacklistptr=&recvinfohdr->info__Package__List()();
      int paramnum=infopacklistptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size,(*infopacklistptr)[a].info__package__name());
        if((*infopacklistptr)[a].info__package__params().ispresent()){
          const char *atm[]={";",";","","="};
          print_list(buff,m_size,b_size,&((*infopacklistptr)[a].info__package__params()()), atm );
        }
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }
    
  if (header->refer__sub().ispresent()){       // Refer-Sub header
    write_to_buff(buff,m_size,b_size, "Refer-Sub:");
    write_to_buff(buff,m_size,b_size,
                                    header->refer__sub()().refer__sub__value());
    if(header->refer__sub()().refer__sub__param().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,
                           &header->refer__sub()().refer__sub__param()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->referred__by().ispresent()){       // Referred-By header
    const Referred__by *reptr=& header->referred__by()();
    write_to_buff(buff,m_size,b_size, h_mode?"b:":"Referred-By:");
    print_addr_union(buff,m_size,b_size,& reptr->referer__uri(),ipv6enabled);
    if(reptr->refererParams().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,&reptr->refererParams()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->reject__contact().ispresent()){     // reject-Contact header
    write_to_buff(buff,m_size,b_size, h_mode?"j:":"Reject-Contact:");
    header_sep=!m_header?",":h_mode?"\r\nj:":"\r\nReject-Contact:";
    int contactnum=header->reject__contact()().rc__values().size_of();
    for(int a=0;a<contactnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size, "*");
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& header->reject__contact()().rc__values()[a], atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->replaces().ispresent()){     // Replaces header
    write_to_buff(buff,m_size,b_size, "Replaces:");
    write_to_buff(buff,m_size,b_size, header->replaces()().callid());
    if(header->replaces()().replacesParams().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& header->replaces()().replacesParams()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->replyTo().ispresent()){       // Reply-To header
    const ReplyTo *reptr=& header->replyTo()();
    write_to_buff(buff,m_size,b_size, "Reply-To:");
    print_addr_union(buff,m_size,b_size,& reptr->addressField(),ipv6enabled);
    if(reptr->replyToParams().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,&reptr->replyToParams()(), atm );
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if(header->refer__to().ispresent()){     // Refer-to header
    write_to_buff(buff,m_size,b_size, h_mode?"r:":"Refer-To:");
    print_addr_union(buff,m_size,b_size,&header->refer__to()().addr(),ipv6enabled);
    if(header->refer__to()().referToParams().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,&header->refer__to()().referToParams()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if(header->request__disp().ispresent()){ // Request-Disposition header
    write_to_buff(buff,m_size,b_size, h_mode?"d:":"Request-Disposition:");
    const Request__disp__directive__list *listptr=& header->request__disp()().directive__list();
    header_sep=!m_header?",":"\r\nRequest-Disposition:";
    int paramnum=listptr->size_of();
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size,(*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->require().ispresent()){     // Require header
    const OptionTag__List *listptr=& header->require()().optionsTags();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Require:");
    header_sep=!m_header?",":"\r\nRequire:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size,(*listptr)[a]);
    }

    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->resourcePriority().ispresent()){    // Resource-Priority header
    write_to_buff(buff,m_size,b_size, "Resource-Priority:");
    header_sep=!m_header?",":"\r\nResource-Priority:";
    if(header->resourcePriority()().rvalues().ispresent()){
      const Rvalue__List *listptr=
                                   &header->resourcePriority()().rvalues()();
      int paramnum=listptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size,(*listptr)[a].namespace_());
        write_to_buff(buff,m_size,b_size, ".");
        write_to_buff(buff,m_size,b_size,(*listptr)[a].r__priority());
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->retryAfter().ispresent()){   // Retry-After header
    const RetryAfter *retptr=& header->retryAfter()();
    write_to_buff(buff,m_size,b_size, "Retry-After:");
    write_to_buff(buff,m_size,b_size, retptr->deltaSec());
    if(retptr->comment().ispresent()){
      write_to_buff(buff,m_size,b_size, " (");
      write_to_buff(buff,m_size,b_size, retptr->comment()());
      write_to_buff(buff,m_size,b_size, ")");
    }
    if(retptr->retryParams().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& retptr->retryParams()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->route().ispresent()){      // Route header
    const RouteBody__List *listptr=& header->route()().routeBody();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Route:");
    header_sep=!m_header?",":"\r\nRoute:";
    for(int a=0;a<paramnum;a++){
      const NameAddr *addr=& (*listptr)[a].nameAddr();
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      if(addr->displayName().ispresent()){
        write_to_buff(buff,m_size,b_size, addr->displayName()());
        write_to_buff(buff,m_size,b_size," ");
      }
      print_url(buff,m_size,b_size,& addr->addrSpec(), 2,ipv6enabled );

      if((*listptr)[a].rrParam().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,&(*listptr)[a].rrParam()(), atm );
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->rseq().ispresent()){ // RSeq header
    write_to_buff(buff,m_size,b_size, "RSeq:");
    //sprintf(tempsend, "%d" ,(int)header->rseq()().response__num());
    //write_to_buff(buff,m_size,b_size, tempsend);
    write_to_buff(buff,m_size,b_size, int2str(header->rseq()().response__num()));
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->session__expires().ispresent()){   // Session-Expires header
    const Session__expires *ptr=& header->session__expires()();
    write_to_buff(buff,m_size,b_size, h_mode?"x:":"Session-Expires:");
    write_to_buff(buff,m_size,b_size, ptr->deltaSec());
    if(ptr->se__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& ptr->se__params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->session__id().ispresent()){   // Session-ID header
    const Session__ID *ptr=& header->session__id()();
    write_to_buff(buff,m_size,b_size,"Session-ID:");
    write_to_buff(buff,m_size,b_size, ptr->sessionID());
    if(ptr->se__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& ptr->se__params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }


  if (header->security__client().ispresent()){      // Security-Client header
    const Security__mechanism__list *listptr=& header->security__client()().sec__mechanism__list();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Security-Client:");
    header_sep=!m_header?",":"\r\nSecurity-Client:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size, (*listptr)[a].mechanism__name());
      if((*listptr)[a].mechanism__params().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,&(*listptr)[a].mechanism__params()(), atm );
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->security__server().ispresent()){      // Security-Server header
    const Security__mechanism__list *listptr=& header->security__server()().sec__mechanism__list();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Security-Server:");
    header_sep=!m_header?",":"\r\nSecurity-Server:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size, (*listptr)[a].mechanism__name());
      if((*listptr)[a].mechanism__params().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,&(*listptr)[a].mechanism__params()(), atm );
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->security__verify().ispresent()){      // Security-Verify header
    const Security__mechanism__list *listptr=& header->security__verify()().sec__mechanism__list();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Security-Verify:");
    header_sep=!m_header?",":"\r\nSecurity-Verify:";
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size, (*listptr)[a].mechanism__name());
      if((*listptr)[a].mechanism__params().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,&(*listptr)[a].mechanism__params()(), atm );
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->server().ispresent()){      // Server header
    const ServerVal__List *listptr=& header->server()().serverBody();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Server:");
    for(int a=0;a<paramnum;a++){
      write_to_buff(buff,m_size,b_size, " ");
      write_to_buff(buff,m_size,b_size, (*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->service__route().ispresent()){      // Service-Route header
    const RouteBody__List *listptr=& header->service__route()().routeBody();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "Service-Route:");
    header_sep=!m_header?",":"\r\nService-Route:";
    for(int a=0;a<paramnum;a++){
      const NameAddr *addr=& (*listptr)[a].nameAddr();
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      if(addr->displayName().ispresent()){
        write_to_buff(buff,m_size,b_size, addr->displayName()());
        write_to_buff(buff,m_size,b_size," ");
      }
      print_url(buff,m_size,b_size,& addr->addrSpec(), 2,ipv6enabled );

      if((*listptr)[a].rrParam().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,&(*listptr)[a].rrParam()(), atm );
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->sip__ETag().ispresent()){           // SIP-ETag header
    write_to_buff(buff,m_size,b_size, "SIP-ETag:");
    write_to_buff(buff,m_size,b_size,header->sip__ETag()().entity__tag());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->sip__If__Match().ispresent()){           // SIP-If-Match header
    write_to_buff(buff,m_size,b_size, "SIP-If-Match:");
    write_to_buff(buff,m_size,b_size,header->sip__If__Match()().entity__tag());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->subscription__state().ispresent()){ // Subscription-state header
    write_to_buff(buff,m_size,b_size, "Subscription-State:");
    write_to_buff(buff,m_size,b_size, header->subscription__state()().substate__value());
    if(header->subscription__state()().subexp__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,&header->subscription__state()().subexp__params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->subject().ispresent()){    // Subject header
    write_to_buff(buff,m_size,b_size, h_mode?"s:":"Subject:");
    write_to_buff(buff,m_size,b_size, header->subject()().summary());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->supported().ispresent()){   // Supported header
    write_to_buff(buff,m_size,b_size, h_mode?"k:":"Supported:");
    header_sep=!m_header?",":h_mode?"\r\nk:":"\r\nSupported:";
    if(header->supported()().optionsTags().ispresent()){
      const OptionTag__List *listptr= & header->supported()().optionsTags()();
      int paramnum=listptr->size_of();
      for(int a=0;a<paramnum;a++){
        if(a){write_to_buff(buff,m_size,b_size, header_sep);}
        write_to_buff(buff,m_size,b_size,(*listptr)[a]);
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->target__dialog().ispresent()){ // Target-Dialog header
    write_to_buff(buff,m_size,b_size, "Target-Dialog:");
    write_to_buff(buff,m_size,b_size, header->target__dialog()().callid());
    if(header->target__dialog()().td__params().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,&header->target__dialog()().td__params()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->timestamp().ispresent()){   // Timestamp header
    const Timestamp *tptr=&header->timestamp()();
    write_to_buff(buff,m_size,b_size, "Timestamp:");
    if(tptr->timeValue().ispresent()){
      const TimeValue *timeptr=& tptr->timeValue()();
      sprintf(tempsend, "%d" ,(int) timeptr->majorDigit() );
      write_to_buff(buff,m_size,b_size, tempsend);
      if(timeptr->minorDigit().ispresent()){
        write_to_buff(buff,m_size,b_size,".");
        sprintf(tempsend, "%d" ,(int) timeptr->minorDigit()() );
        write_to_buff(buff,m_size,b_size, tempsend);
      }
    }
    if(tptr->delay().ispresent()){
      const TimeValue *timeptr=& tptr->delay()();
      write_to_buff(buff,m_size,b_size," ");
      sprintf(tempsend, "%d" ,(int) timeptr->majorDigit() );
      write_to_buff(buff,m_size,b_size, tempsend);
      if(timeptr->minorDigit().ispresent()){
        write_to_buff(buff,m_size,b_size,".");
        sprintf(tempsend, "%d" ,(int) timeptr->minorDigit()() );
        write_to_buff(buff,m_size,b_size, tempsend);
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->toField().ispresent()){            // To header
    const To *toptr=&header->toField()();
    write_to_buff(buff,m_size,b_size, h_mode?"t:":"To:");
    print_addr_union(buff,m_size,b_size,& toptr->addressField(),ipv6enabled);
    if(toptr->toParams().ispresent()){
      const char *atm[]={";",";","","="};
      print_list(buff,m_size,b_size,& toptr->toParams()(), atm );
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->unsupported().ispresent()){   // UnSupported header
    const OptionTag__List *listptr=& header->unsupported()().optionsTags();
    int paramnum=listptr->size_of();
    header_sep=!m_header?",":"\r\nUnsupported:";
    write_to_buff(buff,m_size,b_size, "Unsupported:");
    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}
      write_to_buff(buff,m_size,b_size,(*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->userAgent().ispresent()){   // User-Agent header
    const ServerVal__List *listptr=& header->userAgent()().userAgentBody();
    int paramnum=listptr->size_of();
    write_to_buff(buff,m_size,b_size, "User-Agent:");
    for(int a=0;a<paramnum;a++){
      write_to_buff(buff,m_size,b_size, " ");
      write_to_buff(buff,m_size,b_size,(*listptr)[a]);
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->via().ispresent()){     // Via header
    const ViaBody__List *viabody = & header->via()().viaBody();
    int viabodymax = viabody->size_of();
    write_to_buff(buff,m_size,b_size, h_mode?"v:":"Via:");
    header_sep=!m_header?",":h_mode?"\r\nv:":"\r\nVia:";
      // space added as part of body below..
    for(int viabodycount=0;viabodycount<viabodymax;viabodycount++){
      const ViaBody *viabodyelement =  &(*viabody)[viabodycount];
      const SentProtocol *sentprotocol = & viabodyelement->sentProtocol();
      const HostPort *sentby = & viabodyelement->sentBy();
      if (viabodycount){write_to_buff(buff,m_size,b_size,header_sep);}
      write_to_buff(buff,m_size,b_size, sentprotocol->protocolName());
      write_to_buff(buff,m_size,b_size, "/");
      write_to_buff(buff,m_size,b_size, sentprotocol->protocolVersion());
      write_to_buff(buff,m_size,b_size, "/");
      write_to_buff(buff,m_size,b_size, sentprotocol->transport());
      write_to_buff(buff,m_size,b_size, " ");
      print_host(buff,m_size,b_size, sentby->host()() ,ipv6enabled);
      if (sentby->portField().ispresent()){
        write_to_buff(buff,m_size,b_size, ":");
        sprintf(tempsend,"%d",(int) sentby->portField()() );
        write_to_buff(buff,m_size,b_size, tempsend);
      }
      if (viabodyelement->viaParams().ispresent()){
        const char *atm[]={";",";","","="};
        print_list(buff,m_size,b_size,& viabodyelement->viaParams()(), atm );
      }
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->warning().ispresent()){    // Warning header
    const WarningValue__List *wptr=& header->warning()().warningValue();
    int paramnum=wptr->size_of();
    write_to_buff(buff,m_size,b_size, "Warning:");
    header_sep=!m_header?",":"\r\nWarning:";

    for(int a=0;a<paramnum;a++){
      if(a){write_to_buff(buff,m_size,b_size, header_sep);}

      sprintf(tempsend,"%d",(int)(*wptr)[a].warnCode());
      write_to_buff(buff,m_size,b_size, tempsend);
      write_to_buff(buff,m_size,b_size, " ");

      if((*wptr)[a].warnAgent().get_selection()==WarnAgent::ALT_hostPort){
        const HostPort *hptr=& (*wptr)[a].warnAgent().hostPort();
        if(hptr->host().ispresent()){
          print_host(buff,m_size,b_size,hptr->host()(),ipv6enabled);
        }
        if(hptr->portField().ispresent()){
          write_to_buff(buff,m_size,b_size, ":");
          sprintf(tempsend,"%d",(int) hptr->portField()() );
          write_to_buff(buff,m_size,b_size, tempsend);
        }
      }
      else{
        write_to_buff(buff,m_size,b_size, (*wptr)[a].warnAgent().pseudonym());
      }
      write_to_buff(buff,m_size,b_size, " \"");
      write_to_buff(buff,m_size,b_size,(*wptr)[a].WarnText());
      write_to_buff(buff,m_size,b_size, "\"");
    }
    write_to_buff(buff,m_size,b_size, "\r\n");
  }

  if (header->wwwAuthenticate().ispresent()){  // WWW-Authenticate header
    const GenericParam__List *listptr;
    const char *atm[]={"",", ","","="};
    int n_of_c=header->wwwAuthenticate()().challenge().size_of();
    for(int i=0;i<n_of_c;i++){
      const Challenge *ch=&header->wwwAuthenticate()().challenge()[i];
      write_to_buff(buff,m_size,b_size, "WWW-Authenticate:");
      if( ch->get_selection()==Challenge::ALT_digestCln){
        write_to_buff(buff,m_size,b_size, "Digest ");
        listptr = & ch->digestCln();
      }
      else{
        const OtherAuth *otherptr= & ch->otherChallenge();
        write_to_buff(buff,m_size,b_size, otherptr->authScheme());
        write_to_buff(buff,m_size,b_size, " ");
        listptr = & otherptr->authParams();
      }

      print_list(buff,m_size,b_size,listptr, atm );

      write_to_buff(buff,m_size,b_size, "\r\n");
    }
  }
  
  if (header->x__AUT().ispresent()){       // X_AUT header
    write_to_buff(buff,m_size,b_size, "X-AUT:");
    write_to_buff(buff,m_size,b_size,header->x__AUT()().x__AUT__Value());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }
  
  if (header->x__Carrier__Info().ispresent()){       // X_Carrier_Info header
    write_to_buff(buff,m_size,b_size, "X-Carrier-Info:");
    write_to_buff(buff,m_size,b_size,header->x__Carrier__Info()().x__Carrier__Info__Value());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }  

  if (header->x__CHGDelay().ispresent()){       // X_CHGDelay header
    write_to_buff(buff,m_size,b_size, "X-CHGDelay:");
    write_to_buff(buff,m_size,b_size,header->x__CHGDelay()().x__CHGDelay__Value());
    write_to_buff(buff,m_size,b_size, "\r\n");
  }  
  
  if (header->x__CHGInfo().ispresent()){     // X_CHGInfo header
    write_to_buff(buff,m_size,b_size, "X-CHGInfo:");
    write_to_buff(buff,m_size,b_size, header->x__CHGInfo()().x__ci__kind__data());
    if(header->x__CHGInfo()().cDR__Record().ispresent()){
      write_to_buff(buff,m_size,b_size, ";");
      write_to_buff(buff,m_size,b_size, header->x__CHGInfo()().cDR__Record()());
    }
    
    write_to_buff(buff,m_size,b_size, "\r\n");
  }
      
  if (header->undefinedHeader__List().ispresent()){  // Undefined headers
    const UndefinedHeader__List *listptr=& header->undefinedHeader__List()();
    int paramnum=listptr->size_of();
    for(int a=0;a<paramnum;a++){
      write_to_buff(buff,m_size,b_size, (*listptr)[a].headerName());
      write_to_buff(buff,m_size,b_size, ":");
      write_to_buff(buff,m_size,b_size, (*listptr)[a].headerValue());
      write_to_buff(buff,m_size,b_size, "\r\n");
    }
  }
}

void SIPmsg__PT::print_addr_union(char *& buff, int &m_size, int &b_size, const Addr__Union *addr, bool ipv6enabled){
//  prints out a Addr_Union. Parameters:
//  addr: contains the Addr_Union
//  buff: the buffer
  if(addr->get_selection()==Addr__Union::ALT_nameAddr){
    if(addr->nameAddr().displayName().ispresent()){
      write_to_buff(buff,m_size,b_size,addr->nameAddr().displayName()());
      write_to_buff(buff,m_size,b_size," ");
    }
    print_url(buff,m_size,b_size,& addr->nameAddr().addrSpec(), 2,ipv6enabled);
  }
  else{
    print_url(buff,m_size,b_size,& addr->addrSpecUnion(), 1,ipv6enabled);
  }
}


void SIPmsg__PT::print_list(char *& buff, int &m_size, int &b_size, const GenericParam__List *lista,const char* separator[]) {
//  prints out a parameter list. Parameters:
//  lista: contains the list of the parameters
//  separator: separators    separator[0] would be the leading
//                          separator ? or , or ; if needed
//        separator[1] is the separator between the paramerters  , or ; or &
//        separator[2] is "=" if the = is not optional otherwise ""
//                     see -> url headers
//        separator[3] conatins "=" if the = is optional
//  buff: the buffer
  int listamax = lista->size_of();

  if(!listamax){return;} //empty list

  write_to_buff(buff,m_size,b_size,separator[0]);  // prints leading separator if needed

  write_to_buff(buff,m_size,b_size,(*lista)[0].id());  // first parameter

  write_to_buff(buff,m_size,b_size,separator[2]);
  if ((*lista)[0].paramValue().ispresent())
    {
    write_to_buff(buff,m_size,b_size,separator[3]);
    write_to_buff(buff,m_size,b_size,(*lista)[0].paramValue()());
    }

  for(int headerscount=1;headerscount<listamax;headerscount++)
  // remaining parameters
    {
    write_to_buff(buff,m_size,b_size,separator[1]);
    write_to_buff(buff,m_size,b_size,(*lista)[headerscount].id());
    write_to_buff(buff,m_size,b_size,separator[2]);
    if ((*lista)[headerscount].paramValue().ispresent())
      {
      write_to_buff(buff,m_size,b_size,separator[3]);
      write_to_buff(buff,m_size,b_size,(*lista)[headerscount].paramValue()());
      }
    }
}

bool SIPmsg__PT::isIPv6address(const CHARSTRING &host){
#ifdef AF_INET6
  unsigned char buf[sizeof(struct in6_addr)];
  return (inet_pton(AF_INET6,(const char *)host,buf) ? true : false);
#else
  if (strchr((const char *)host,':')){
    for(int i=0;i<host.lengthof();i++){
      char ch=*((const char *)host+i);
      if(ch!=':' && (ch<'0' || ch>'9') && (ch<'a' || ch>'f') && (ch<'A' || ch>'F') ){
        return false;
      }
    }
  } else {return false;}
  
  return true;
//  return (strchr((const char *)host,':') ? true : false);
#endif


//  return (strchr((const char *)host,':') ? true : false);
}

void SIPmsg__PT::print_host(char *& buff, int &m_size, int &b_size, 
			    const CHARSTRING &host, bool ipv6enabled){
  if (ipv6enabled && isIPv6address(host)){
    write_to_buff(buff,m_size,b_size, "["+host+"]");
  } else {
    write_to_buff(buff,m_size,b_size, host);
  }
}

void SIPmsg__PT::print_url(char *& buff, int &m_size, int &b_size,
                                              const SipUrl *cim, int brallowed, bool ipv6enabled){
//  prints out a URL. Parameters:
//  cim: contains the URL
//  brallowed:   0 if angle brackets (< and >) is not allowed
//      1 if angle brackets (< and >) is allowed
//      2 if angle brackets (< and >) must be presented
//  buff: the buffer
  const HostPort *hostport;
  char tempsend[BUF_SIZE];
  char *closingbracket;

  if(brallowed == 2 || (brallowed!=0 &&
              (cim->urlParameters().ispresent()||cim->headers().ispresent()))){
    write_to_buff(buff,m_size,b_size, "<" );
    closingbracket=(char*)">";
    }
  else {closingbracket=(char*)"";}

  write_to_buff(buff,m_size,b_size,(const CHARSTRING&)cim->scheme() );
  write_to_buff(buff,m_size,b_size, ":" );
  hostport = &cim->hostPort();

  if ( cim->userInfo().ispresent() )    // User info
    {
    const UserInfo *userinfo = & cim->userInfo()();
    write_to_buff(buff,m_size,b_size, userinfo->userOrTelephoneSubscriber());
    if (userinfo->password().ispresent())    // password
      {
      write_to_buff(buff,m_size,b_size, ":" );
      write_to_buff(buff,m_size,b_size, userinfo->password()() );
      }
      if (hostport->host().ispresent()) write_to_buff(buff,m_size,b_size, "@" );
    }

  if (hostport->host().ispresent())
    print_host(buff,m_size,b_size, hostport->host()(),ipv6enabled);  // hostname
  else if(strcasecmp(cim->scheme(),"tel"))
    TTCN_warning("Host name is omitted in uri! The message will be invalid!");

  if (hostport->portField().ispresent())   // port number
    {
    write_to_buff(buff,m_size,b_size, ":");
    sprintf(tempsend,"%d",(int) hostport->portField()());
    write_to_buff(buff,m_size,b_size, tempsend);
    }

  if (cim->urlParameters().ispresent())  // URL parameters
    {
    const char *atm[]= {";",";","","="};

    print_list(buff,m_size,b_size,& cim->urlParameters()(), atm);
    }

  if (cim->headers().ispresent())  // URL headers
    {
    const char *atm[]= {"?","&","=",""};
    print_list(buff,m_size,b_size,& cim->headers()(), atm);
    }

  write_to_buff(buff,m_size,b_size,closingbracket);

}


void SIPmsg__PT::write_to_buff(char *& buff,int &s,int &maxsize ,const char *mit){
// write the *mit into the buffer.
// printf("%s\r\r",mit);
  int mh=strlen(mit);
  if(s+mh+1>=maxsize){
    maxsize=s+mh+1000;
    buff=(char*)Realloc(buff,maxsize*sizeof(char));
  }
  memcpy(buff+s,mit,mh);
//  strcat(buff,mit);
  s+=mh;
  buff[s]='\0';
}

void SIPmsg__PT::write_to_buff(char *& buff,int &s,int &maxsize, const CHARSTRING &mit){
  int mh=mit.lengthof();
  if(s+mh+1>=maxsize){
    maxsize=s+mh+1000;
    buff=(char*)Realloc(buff,maxsize*sizeof(char));
  }
  memcpy(buff+s,(const char*)mit,mh);
//  strcat(buff,mit);
  s+=mh;
  buff[s]='\0';
}

void SIPmsg__PT::write_to_buff(char *& buff,int &s,int &maxsize, const OCTETSTRING &mit){
  int mh=mit.lengthof();
  if(s+mh+1>=maxsize){
    maxsize=s+mh+1000;
    buff=(char*)Realloc(buff,maxsize*sizeof(char));
  }
  memcpy(buff+s,(const unsigned char*)mit,mh);
//  strcat(buff,mit);
  s+=mh;
  buff[s]='\0';
}


void SIPmsg__PT::set_addr_struct(struct sockaddr_in *addr_st,
                                                 int port_num, const char* host_name){
  memset(addr_st, 0, sizeof(*addr_st));
  addr_st->sin_family = AF_INET;
  addr_st->sin_port = htons(port_num);
  if(!host_name){    // own host
    addr_st->sin_addr.s_addr=htonl(INADDR_ANY);
    return;
  }
  in_addr_t addr = inet_addr(host_name);

  if (addr != (in_addr_t)-1){     // host name in XX:XX:XX:XX form
    memcpy(&addr_st->sin_addr.s_addr, &addr, sizeof(addr));
      }
      else{                               // host name in akarmi.hu form
    struct hostent  *hptr = gethostbyname(host_name);
    if ( hptr != NULL ){
        memcpy(&addr_st->sin_addr.s_addr, hptr->h_addr_list[0],hptr->h_length);}
    else TTCN_error("Getting of IP address of remote host failed.");
  }
}

int SIPmsg__PT::open_comm_socket(struct sockaddr_in addr_st){
  int reuse=1;
  int fp=socket(PF_INET, udportcp?SOCK_STREAM:SOCK_DGRAM, 0);
  if( fp == -1 ){ TTCN_error("Socket creation failed."); }
  if(udportcp)  if(setsockopt(fp, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse,
                           sizeof(reuse)) < 0) TTCN_error("setsockopt error");
  if(fcntl(fp, F_SETFD, O_NONBLOCK) == -1) TTCN_error("fcntl error");

  if(udportcp){        // TCP
    if(connect(fp, (struct sockaddr *)&addr_st, sizeof(addr_st))< 0)
                                                {TTCN_error("Connect failed.");}
  }
  else{                // UDP
    set_addr_struct(&listen_addr, local_port, local_addr);
    if (bind(fp,(struct sockaddr*)&listen_addr,sizeof(listen_addr)) < 0)
      {TTCN_error("Socket bind failed.");}
  }
  return fp;
}

int SIPmsg__PT::open_comm_socket(struct sockaddr_in addr_st, struct sockaddr_in listen_st, int uort){
  int reuse=1;
  int fp=socket(PF_INET, uort?SOCK_STREAM:SOCK_DGRAM, 0);
  if( fp == -1 ){
    if(report_error) {
      ADDRESS ad;
      ASP__SIP__error ec;
      ad.host()=OMIT_VALUE;
      ad.portField()=OMIT_VALUE;
      ad.tcporudp()=OMIT_VALUE;
      ec.error__code()=errno;
      ec.error__text()=strerror(errno);
      errno=0;
      ec.failed__operation()=SIP__operation::SIP__OP__CREATE__SOCKET;
      ec.addr()=OMIT_VALUE;
      ec.SIP__message()=OMIT_VALUE;
      incoming_message(ec,&ad);
      return -1;
    } else  TTCN_error("Socket creation failed.");
  }
  if(uort)  if(setsockopt(fp, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse,
			  sizeof(reuse)) < 0){
    if(report_error) {
      close(fp);
      ADDRESS ad;
      ASP__SIP__error ec;
      ad.host()=OMIT_VALUE;
      ad.portField()=OMIT_VALUE;
      ad.tcporudp()=OMIT_VALUE;
      ec.error__code()=errno;
      ec.error__text()=strerror(errno);
      errno=0;
      ec.failed__operation()=SIP__operation::SIP__OP__SET__SOCKET__OPT;
      ec.addr()=OMIT_VALUE;
      ec.SIP__message()=OMIT_VALUE;
      incoming_message(ec,&ad);
      return -1;
    } else TTCN_error("setsockopt error");
  }
  if(fcntl(fp, F_SETFD, O_NONBLOCK) == -1){
    if(report_error) {
      close(fp);
      ADDRESS ad;
      ASP__SIP__error ec;
      ad.host()=OMIT_VALUE;
      ad.portField()=OMIT_VALUE;
      ad.tcporudp()=OMIT_VALUE;
      ec.error__code()=errno;
      ec.error__text()=strerror(errno);
      errno=0;
      ec.failed__operation()=SIP__operation::SIP__OP__SOCKET__FCNTL;
      ec.addr()=OMIT_VALUE;
      ec.SIP__message()=OMIT_VALUE;
      incoming_message(ec,&ad);
      return -1;
    } else TTCN_error("fcntl error");
  }
  if(uort){        // TCP
    if(connect(fp, (struct sockaddr *)&addr_st, sizeof(addr_st))< 0){
      if(report_error) {
        close(fp);
        ADDRESS ad;
        ASP__SIP__error ec;
        ad.host()=OMIT_VALUE;
        ad.portField()=OMIT_VALUE;
        ad.tcporudp()=OMIT_VALUE;
        ec.error__code()=errno;
        ec.error__text()=strerror(errno);
        errno=0;
        ec.failed__operation()=SIP__operation::SIP__OP__SOCKET__CONNECT;
        ec.addr()=OMIT_VALUE;
        ec.SIP__message()=OMIT_VALUE;
        incoming_message(ec,&ad);
        return -1;
      } else TTCN_error("Connect failed.");
    }
  }
  else{                // UDP
    if (bind(fp,(struct sockaddr*)&listen_st,sizeof(listen_st)) < 0){
      if(report_error){
	close(fp);
	ADDRESS ad;
	ASP__SIP__error ec;
	ad.host()=OMIT_VALUE;
	ad.portField()=OMIT_VALUE;
	ad.tcporudp()=OMIT_VALUE;
	ec.error__code()=errno;
	ec.error__text()=strerror(errno);
	errno=0;
	ec.failed__operation()=SIP__operation::SIP__OP__SOCKET__CONNECT;
	ec.addr()=OMIT_VALUE;
	ec.SIP__message()=OMIT_VALUE;
	incoming_message(ec,&ad);
	return -1;
      } else TTCN_error("Socket bind failed.");
    }
  }
  return fp;
}

void SIPmsg__PT::send_msg(const char *buff){
  if(!udportcp && mtu_size && msgsize>mtu_size
#ifdef SIPPORT_WITH_SIGCOMP
    && !use_sigcomp
#endif
     ){
    if(report_error) {
      ADDRESS ad;
      ASP__SIP__error ec;
      ad.host()=OMIT_VALUE;
      ad.portField()=OMIT_VALUE;
      ad.tcporudp()=OMIT_VALUE;
      ec.error__code()=errno;
      ec.error__text()=strerror(errno);
      errno=0;
      ec.failed__operation()=SIP__operation::SIP__OP__LONG__MESSAGE;
      ec.addr()().remote__host()=conn_list[last_conn].addr;
      ec.addr()().remote__port()=conn_list[last_conn].port;
      ec.addr()().protocol()=udportcp?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
      if(local_addr)
	ec.addr()().local__host()()=local_addr;
      else ec.addr()().local__host()=OMIT_VALUE;
      ec.addr()().local__port()=local_port;
      ec.SIP__message()=CHARSTRING(msgsize,buff);
      incoming_message(ec,&ad);
      return;
    } else TTCN_error("Message too long for UDP.");
  }
  if (debug) {
    CHARSTRING ch(msgsize,buff);
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("SIP test port (%s) sent: ", get_name());
    ch.log();
    TTCN_Logger::end_event();
  }
  size_t local_msgsize=msgsize;
  while (local_msgsize > 0) {
    int sent_size;
    if (udportcp) {
      sent_size = ::send(comm_soc, buff, local_msgsize, 0);
    } else {
      int loc_soc;
      if (random_udp) {
        loc_soc = socket(PF_INET, SOCK_DGRAM, 0);
        if (loc_soc < 0){
          if(report_error) {
            ADDRESS ad;
            ASP__SIP__error ec;
            ad.host()=OMIT_VALUE;
            ad.portField()=OMIT_VALUE;
            ad.tcporudp()=OMIT_VALUE;
            ec.error__code()=errno;
            ec.error__text()=strerror(errno);
            errno=0;
            ec.failed__operation()=SIP__operation::SIP__OP__CREATE__SOCKET;
            ec.addr()().remote__host()=conn_list[last_conn].addr;
            ec.addr()().remote__port()=conn_list[last_conn].port;
            ec.addr()().protocol()=udportcp?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
            if(local_addr)
              ec.addr()().local__host()()=local_addr;
            else ec.addr()().local__host()=OMIT_VALUE;
            ec.addr()().local__port()=OMIT_VALUE;
            ec.SIP__message()=CHARSTRING(msgsize,buff);
            incoming_message(ec,&ad);
            return;
          } else  TTCN_error("Sending socket creation failed.");
	}
      } else loc_soc = comm_soc;
#ifdef SIPPORT_WITH_SIGCOMP
      if (use_sigcomp) {
        sent_size = sendSigcompMessage(loc_soc, buff, remote_addr);
      } else
#endif
      {
        sent_size = sendto(loc_soc, buff, local_msgsize, 0,
            (struct sockaddr*)&remote_addr, sizeof(remote_addr));
      }
      if (random_udp) close(loc_soc);
      if (close_soc) {close(comm_soc);comm_soc=-1;close_soc=false;}
    }
    if (sent_size < 0){
      if(report_error) {
        ADDRESS ad;
        ASP__SIP__error ec;
        ad.host()=OMIT_VALUE;
        ad.portField()=OMIT_VALUE;
        ad.tcporudp()=OMIT_VALUE;
        ec.error__code()=errno;
        ec.error__text()=strerror(errno);
        errno=0;
        ec.failed__operation()=SIP__operation::SIP__OP__SEND;
        ec.addr()().remote__host()=conn_list[last_conn].addr;
        ec.addr()().remote__port()=conn_list[last_conn].port;
        ec.addr()().protocol()=udportcp?SIP__com__prot::TCP__E:SIP__com__prot::UDP__E;
        if(local_addr)
          ec.addr()().local__host()()=local_addr;
        else ec.addr()().local__host()=OMIT_VALUE;
        ec.addr()().local__port()=local_port;
        ec.SIP__message()=CHARSTRING(msgsize,buff);
        incoming_message(ec,&ad);
        return;
      } else TTCN_error("Send failed.");
    }
    buff+=sent_size;
    local_msgsize-=sent_size;
  }
}

void SIPmsg__PT::check_address(const ADDRESS *destination_address){
  fd_set readfds;
  if(port_mode){
    if(destination_address){
      SIP__comm__adress dst;
       if(destination_address->host().ispresent())
        dst.remote__host()()=destination_address->host()();
      else dst.remote__host()=OMIT_VALUE;
      if(destination_address->portField().ispresent())
        dst.remote__port()()=destination_address->portField()();
      else dst.remote__host()=OMIT_VALUE;
      dst.local__port()()=local_port;
      dst.local__host()=OMIT_VALUE;
      if(destination_address->tcporudp().ispresent()) {
        if(destination_address->tcporudp()()==true){dst.protocol()()=SIP__com__prot::TCP__E;}
        else {dst.protocol()()=SIP__com__prot::UDP__E;}
      } else dst.protocol()=OMIT_VALUE;
      check_address(&dst);
    } else check_address((SIP__comm__adress*)NULL);
    return;
  }
  if(!destination_address || !destination_address->host().ispresent()){
    if(comm_soc==-1)
        {TTCN_error("There is no valid destination address available."
                    " Message can not be sent!");}
    return;
  }
  if(destination_address->tcporudp().ispresent() &&
                    (destination_address->tcporudp()()!=(udportcp==PROTO_TCP))){
    if( comm_soc != -1 ) {close(comm_soc);comm_soc=-1;}
    if( listen_soc != -1 ) {close(listen_soc);listen_soc=-1;}
    udportcp=(const BOOLEAN&)destination_address->tcporudp();
  }
  if(  !target_addr ||
      (target_addr && strcasecmp(target_addr,destination_address->host()())) ||
       (destination_address->portField().ispresent() &&
       (int)destination_address->portField() ()!=target_port)
       ||(comm_soc==-1)){
    target_addr=(char *)Realloc(target_addr,strlen(destination_address->host()())+1);
    strcpy(target_addr,destination_address->host()());
    if(destination_address->portField().ispresent())
        {target_port=(int) destination_address->portField()();}
    set_addr_struct(&remote_addr, target_port, target_addr);

    if(udportcp || (comm_soc == -1)){   // reopen the connection
//      if( listen_soc != -1 ) close(listen_soc);
      if( comm_soc != -1 ) close(comm_soc);
      comm_soc=open_comm_socket(remote_addr);
      FD_ZERO(&readfds);
      FD_SET(comm_soc, &readfds);
      status=CONNECTED;
      Uninstall_Handler();
      Install_Handler(&readfds,   NULL  ,   NULL  , 0.0);
      dest_addr.host()()=inet_ntoa(remote_addr.sin_addr);
      dest_addr.portField()()=ntohs(remote_addr.sin_port);
      dest_addr.tcporudp()()=udportcp;
    }
  }
#ifdef SIPPORT_WITH_SIGCOMP
  if (udportcp!=PROTO_UDP && use_sigcomp)
      TTCN_error("Sigcomp can only be used with UDP");
#endif
}

void SIPmsg__PT::check_address(const SIP__comm__adress *destination_address){
  fd_set readfds;
  bool new_connection=false;
  if(port_mode){
    comm_soc=get_conn_fp(destination_address);
#ifdef SIPPORT_WITH_SIGCOMP
      if (use_sigcomp)
        TTCN_error("Sigcomp can only be used with basic port type");
#endif
  } else {

    if(!destination_address || !destination_address->remote__host().ispresent()){
      if(comm_soc==-1)
          {TTCN_error("There is no valid destination address available."
                      " Message can not be sent!");}
      return;
    }
    if(destination_address->protocol().ispresent()){
      if(destination_address->protocol()()!=SIP__com__prot::TCP__E && udportcp==PROTO_TCP){
          new_connection=true;
          udportcp=PROTO_UDP;
          close(comm_soc);
          comm_soc=-1;
        }
      else if(destination_address->protocol()()!=SIP__com__prot::UDP__E && udportcp==PROTO_UDP){
          new_connection=true;
          udportcp=PROTO_TCP;
          close(comm_soc);
          comm_soc=-1;
        }
    }

    if(!target_addr || (target_addr && strcasecmp(target_addr,destination_address->remote__host()()))){
        new_connection=true;
        target_addr=(char *)Realloc(target_addr,strlen(destination_address->remote__host()())+1);
        strcpy(target_addr,destination_address->remote__host()());
      }

    if(destination_address->local__host().ispresent() && (!local_addr || (local_addr && strcasecmp(local_addr,destination_address->local__host()())))){
        new_connection=true;
        local_addr=(char *)Realloc(local_addr,strlen(destination_address->local__host()())+1);
        strcpy(local_addr,destination_address->local__host()());
      }

    if(destination_address->remote__port().ispresent() && target_port!=(int)destination_address->remote__port()()){
        new_connection=true;
        target_port=(int)destination_address->remote__port()();
      }

    if(destination_address->local__port().ispresent() && local_port!=(int)destination_address->local__port()()){
        new_connection=true;
        local_port=(int)destination_address->local__port()();
      }

    if(new_connection){
      set_addr_struct(&remote_addr, target_port, target_addr);
      if(udportcp || (comm_soc == -1)){   // reopen the connection
  //      if( listen_soc != -1 ) close(listen_soc);
        if( comm_soc != -1 ) close(comm_soc);
        comm_soc=open_comm_socket(remote_addr);
        FD_ZERO(&readfds);
        FD_SET(comm_soc, &readfds);
        status=CONNECTED;
        Uninstall_Handler();
        Install_Handler(&readfds,   NULL  ,   NULL  , 0.0);
        dest_addr.host()()=inet_ntoa(remote_addr.sin_addr);
        dest_addr.portField()()=ntohs(remote_addr.sin_port);
        dest_addr.tcporudp()()=udportcp;
      }
    }
#ifdef SIPPORT_WITH_SIGCOMP
    if (udportcp!=PROTO_UDP && use_sigcomp)
      TTCN_error("Sigcomp can only be used with UDP.");
#endif
  }
}

inline void SIPmsg__PT::reduce_buff(int lenght,char *buff,int bufflen){
  memmove((void*)buff,(void*)(buff+lenght),(bufflen-lenght)+1);
}

int SIPmsg__PT::get_conn_fp(const SIP__comm__adress *destination_address){
  const char *loc_comm_addr=local_addr;
  const char *dest_comm_addr=target_addr;
  int loc_com_port=local_port;
  int dest_com_port=target_port;
  int proto=udportcp;
  int fp;
  if(destination_address){
    if(destination_address->remote__host().ispresent())
      dest_comm_addr=destination_address->remote__host()();
    if(destination_address->local__host().ispresent())
      loc_comm_addr=destination_address->remote__host()();
    if(destination_address->remote__port().ispresent())
      dest_com_port=destination_address->remote__port()();
    if(destination_address->local__port().ispresent())
      loc_com_port=destination_address->local__port()();
    if(destination_address->protocol().ispresent()){
      if(destination_address->protocol()()==SIP__com__prot::TCP__E) proto =1;
      else proto=0;
    }
  } else if(last_conn!=-1){
    return conn_list[last_conn].fp;
  }

  if(!dest_comm_addr){
    if(last_conn!=-1){
      return conn_list[last_conn].fp;
    }
     TTCN_error("There is no valid destination address available."
                      " Message can not be sent!");
  }
  log("Connection requested to %s:%i proto:%s",dest_comm_addr,dest_com_port,proto?"TCP":"UDP");
  if(proto){
    int conn_id=get_conn_id(dest_comm_addr,dest_com_port);
    if(conn_id==-1) log("New connection");
    else log("Selected connection conn_id:%i %s:%i proto:%s",conn_id,conn_list[conn_id].addr,conn_list[conn_id].port,proto?"TCP":"UDP");
    if(conn_id==-1) conn_id=add_conn(dest_comm_addr,dest_com_port);
    if(conn_id==-1) return -1;
    fp=conn_list[conn_id].fp;
    last_conn=conn_id;
  } else {
    if(!conn_list[0].addr || strcasecmp(conn_list[0].addr,dest_comm_addr) || conn_list[0].port!=dest_com_port){
      conn_list[0].addr=(char *)Realloc(conn_list[0].addr,strlen(dest_comm_addr)+1);
       strcpy(conn_list[0].addr,dest_comm_addr);
      conn_list[0].port=dest_com_port;
      set_addr_struct(&remote_addr, dest_com_port, dest_comm_addr);
    }
    if(conn_list[0].fp==-1 && !random_udp){
      struct sockaddr_in listen_struct;
      set_addr_struct(&listen_struct, loc_com_port, loc_comm_addr);
      fp=open_comm_socket(remote_addr,listen_struct,0);
      if(fp==-1) return -1;
      close_soc=true;
    } else fp=conn_list[0].fp;
    last_conn=0;
  }
  udportcp=proto;
return fp;
}
int SIPmsg__PT::get_conn_id(const char* hostname,int port){
  for(int a=1;a<size_conn_list;a++){
    if(port==conn_list[a].port && !strcasecmp(conn_list[a].addr,hostname))
      return a;
  }
  return -1;
}

int SIPmsg__PT::get_conn_id(int fp){
  for(int a=1;a<size_conn_list;a++){
    if(fp==conn_list[a].fp)
      return a;
  }
  return -1;
}

int SIPmsg__PT::add_conn(const char* hostname,int port, int fp){
  int index=1;
  if(fp==-1){
    struct sockaddr_in addr_str;
    set_addr_struct(&addr_str, port, hostname);
    fp=open_comm_socket(addr_str,addr_str,1);
    if(fp==-1) return -1;
    FD_SET(fp, &connections_read_fds);
    Install_Handler(&connections_read_fds,   NULL  ,   NULL  , 0.0);
  }
  if(active_connections<(size_conn_list-1)){
    while(conn_list[index].port!=-1) index++;
  } else {
    conn_list=(connection_data *)Realloc(conn_list,(size_conn_list+10)*sizeof(connection_data));
    index=size_conn_list;
    size_conn_list+=10;
    for(int conn_id=index;conn_id<size_conn_list;conn_id++){
      conn_list[conn_id].fp=-1;
      conn_list[conn_id].port=-1;
      conn_list[conn_id].addr=NULL;
      conn_list[conn_id].buff=NULL;
      conn_list[conn_id].msgsize=0;
      conn_list[conn_id].buffsize=0;
      conn_list[conn_id].errorind=0;
      conn_list[conn_id].wait_msg_body=0;
      conn_list[conn_id].respmsg=NULL;
      conn_list[conn_id].reqmsg=NULL;
      conn_list[conn_id].raw_msg=NULL;
    }
  }
  conn_list[index].addr=(char *)Realloc(conn_list[index].addr,strlen(hostname)+1);
  strcpy(conn_list[index].addr,hostname);
  conn_list[index].fp=fp;
  conn_list[index].port=port;
  conn_list[index].raw_msg=new CHARSTRING;
  active_connections++;
  return index;
}

void SIPmsg__PT::remove_conn(int conn_id){
  Free(conn_list[conn_id].addr);
  conn_list[conn_id].fp=-1;
  conn_list[conn_id].port=-1;
  conn_list[conn_id].addr=NULL;
  Free(conn_list[conn_id].buff);
  conn_list[conn_id].buff=NULL;
  conn_list[conn_id].msgsize=0;
  conn_list[conn_id].buffsize=0;
  conn_list[conn_id].errorind=0;
  conn_list[conn_id].wait_msg_body=0;
  delete conn_list[conn_id].respmsg;
  delete conn_list[conn_id].reqmsg;
  delete conn_list[conn_id].raw_msg;
  conn_list[conn_id].respmsg=NULL;
  conn_list[conn_id].reqmsg=NULL;
  conn_list[conn_id].raw_msg=NULL;
  if(conn_id) active_connections--;
}

void SIPmsg__PT::close_conn(int conn_id){
  close(conn_list[conn_id].fp);
  FD_CLR(conn_list[conn_id].fp, &connections_read_fds);
  Install_Handler(&connections_read_fds,   NULL  ,   NULL  , 0.0);
  remove_conn(conn_id);
}

void SIPmsg__PT::log(const char *fmt, ...) {
  if (debug) {
    va_list ap;
    va_start(ap, fmt);
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("SIP Test Port (%s): ", get_name());
    TTCN_Logger::log_event_va_list(fmt, ap);
    TTCN_Logger::end_event();
    va_end(ap);
  }
}

#ifdef SIPPORT_WITH_SIGCOMP
void SIPmsg__PT::checkResult (const Result & inRes) {
    if (inRes.code() != Success) {
        log("Sigcomp error code: %d", inRes.code());
        log("Sigcomp error text: %s", inRes.text()==0?"":inRes.text());
        TTCN_error("SIGCOMP: Fatal error in SIGCOMP part of %s.", get_name());
    }
}

void SIPmsg__PT::openCompressor() {
    OctetString * usd = NULL;
    // ***** USD not supported by SigComp Core as of 05.06.03 ******
    //
    //    if(usd_file != 0) {
    //        FILE * f_usd = fopen(usd_file, "r");
    //        if (f_usd == NULL) {
    //            TTCN_error("SIGCOMP: Could not open USD file '%s'", usd_file);
    //        } else {
    //            if (fseek(f_usd, 0, SEEK_END) == 0) {
    //            unsigned int usd_len = ftell(f_usd);
    //            rewind(f_usd);
    //            char * usd_data = (char *) Malloc(usd_len);
    //            fread(usd_data, usd_len, 1, f_usd);
    //            fclose(f_usd);
    //            usd = new OctetString(usd_data, usd_len);
    //            Free(usd_data);
    //            } else
    //                TTCN_error("Error reading the usd file (code %i).", errno);
    //        }
    //    }
    //

    log("Sigcomp: Opening compartment \"%s\".",cid);

    ELib::OctetString tmp(cid, strlen(cid));
    res = cmgr->openCpmt(tmp, false, usd);

    log("Sigcomp: Compartment \"%s\" open.",cid);

    checkResult(res);

    Free(lastCpmtId);
    lastCpmtId = strdup(cid);
}

void SIPmsg__PT::closeCompressor() {
    if (lastCpmtId != 0) {
        ELib::OctetString tmp(lastCpmtId, strlen(lastCpmtId));
        res = cmgr->closeCpmt(tmp);
        checkResult(res);
        log("Sigcomp: Compartment \"%s\" closed.",lastCpmtId);
        Free(lastCpmtId);
        delete(dao); dao = NULL;
        delete(cmgr); cmgr = NULL;
    } else {
        log("Sigcomp: No compartment to close.");
    }
}

int SIPmsg__PT::sendSigcompMessage(int fd_srv, const char * msg, const sockaddr_in & remote) {
    bool saveDynamicState = true;
    bool saveSharedState = true;

    if (lastCpmtId == NULL)
    {
        TTCN_error("SIGCOMP: No compartment opened yet.");
    }


    OctetString um(msg, strlen(msg));
    OctetString m;

    OctetString tmp(lastCpmtId, strlen(lastCpmtId));
    Compressor comp(tmp, *dao);

    res = comp.compress(um, m,
                ApplicationParameters(Transport::MessageBased, saveDynamicState, saveSharedState));

    bool compressed = true;
    if (res.code() != Success) {
        log("SIGCOMP: error code: %d", res.code());
        log("SIGCOMP: error text: %s", res.text());
        log("SIGCOMP: message will be sent uncompressed");

        m = um;
        compressed = false;
    }

    if(mtu_size && ((int) m.getLength() > mtu_size))
        TTCN_error("Sigcomp: Message too long for UDP.");

    int send_res =
                sendto(fd_srv, m.getPtr(), m.getLength(), 0,
                        (struct sockaddr *)&remote,
                        sizeof(remote));

    if (send_res < 0) {
        TTCN_warning("SIGCOMP send error");
        return send_res;
    }


    log("SIGCOMP message sent, %d bytes", send_res);

    if (send_res<0)
        return send_res;
    else
        return strlen(msg);
}

int SIPmsg__PT::receiveSigcompMessage(int fd_srv, char * msg_in_buffer, int msg_in_len, const sockaddr_in & sender) {

#if defined LINUX || defined FREEBSD || defined SOLARIS8
    socklen_t
#else // SOLARIS or WIN32
    int
#endif
      sender_len = sizeof(sender);

    char ibuf[MAX_IN_BUFFER];
    int ibuf_len = recvfrom(fd_srv, ibuf,
                        sizeof(ibuf), 0,
                        (struct sockaddr *)&sender, &sender_len);

    if (ibuf_len == -1) {
        log("SIGCOMP: Receive error");
        return -1;
    }

    ELib::OctetString um;
    ELib::OctetString m(ibuf, ibuf_len);
    Decompressor decomp(*dao);

    bool compressed = true;
    if ((ibuf[0] & 0xf8) == 0xf8)
    {
        res = decomp.decompress(m, um, Transport::MessageBased);
        checkResult(res);
        res = decomp.accept(lastCpmtId);
        checkResult(res);
    }
    else
    {
        log("SIGCOMP: received uncompressed message");

        compressed = false;
        um = m;
    }

    if (um.getLength() >= (unsigned int)msg_in_len)
  TTCN_error("Sigcomp: uncompressed message "
      "is too long.");
    strcpy(msg_in_buffer, um.getBuffer());
    if(compressed)
        log("SIGCOMP message received, compressed %i bytes, decompressed %i bytes", ibuf_len, um.getLength());
    return um.getLength();
}

void SIPmsg__PT::loadConfig(char * configFile, char * usdFile) {
    log("Sigcomp config file load started");

    char * cfgFile = 0;
    if (configFile != 0)
        cfgFile = strdup(configFile);
    else
        cfgFile = strdup("sigcomp.cfg");

    log("SigComp config file set to \"%s\".",cfgFile);

    if (dao == 0) {
        dao = new BasicDataAccess(cfgFile);
        log("Sigcomp Data Access Object created");
    }
    else
        log ("Sigcomp Data Access Object already initialized");
    if (cmgr == 0) {
        cmgr = new CpmtMgr(*dao);
        log("Sigcomp Compartment Manager created");
    }
    else
        log("Sigcomp Compartment Manager already initialized");

    // read config and init state database
    State state;
    unsigned char * state_value;
    unsigned int state_length;
    unsigned int state_address;
    unsigned int state_instr;
    unsigned int min_acc_len;
    char state_file[PATH_MAX];

    FILE * sf ;

    ELib::OctetString_Vector SavedStateIds;
    char line[1024];
    log("Sigcomp config file open started");
    FILE * scfg = fopen(cfgFile, "r");
    if (scfg == NULL) {
        TTCN_error("SIGCOMP: Could not open configuration file");
    }
    // skip four lines in cfg file
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    log("Sigcomp bytecode load started");
    // load bytecode
    if (fscanf(scfg, "BytecodeState=%d,%d,%d,%s\n",
               &state_address, &state_instr, &min_acc_len, state_file) == 4) {
        sf = fopen((char *)state_file, "r");
        if (sf == NULL) {
            TTCN_error("SIGCOMP: Could not open bytecode state file");
        }

        fseek(sf, 0, SEEK_END);
        state_length = ftell(sf);
        rewind(sf);
        state_value = (unsigned char *)Malloc(state_length);
        fread(state_value, state_length, 1, sf);
        fclose(sf);

        state.set(state_length, state_address, state_instr,
                    min_acc_len, state_value);

        SavedStateIds.add(
                OctetString((const char *) state.id(), state.length()));

        Free(state_value);
        res = dao->createState(state);
        checkResult(res);
    } else {
        TTCN_error("SIGCOMP: Could not interpret bytecode entry from configuration file!");
    }

    log("Sigcomp static dict load started");

    // load static dictionary
    int x;
    if (fscanf(scfg, "StaticDictionary=%d,%d,%d,%d,%s\n",
               &x, &state_address, &state_instr, &min_acc_len, state_file) == 5) {
        //Open static dictionary file
        sf = fopen((char *)state_file, "r");
        if (sf == NULL) {
            TTCN_error("SIGCOMP: Could not open dictionary state file: %s",
                                                                state_file);
        }
        fseek(sf, 0, SEEK_END);
        state_length = ftell(sf);
        rewind(sf);
        state_value = (unsigned char *)Malloc(state_length);
        fread(state_value, state_length, 1, sf);
        fclose(sf);
        state.set(state_length, state_address, state_instr,
                    min_acc_len, state_value);
        Free(state_value);
        res = dao->createState(state);
     } else {
        TTCN_error("SIGCOMP: Could not interpret dictionary entry in configuration!");
    }
    // skip 9 lines
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    fgets(line, 1000, scfg);
    log("Sigcomp advertise states started");

    int fres = -1;

    while (fres != EOF && fres != 0) {
        fres = fscanf(scfg, "%d,%d,%d,%s\n", &state_address, &state_instr,
            &min_acc_len, state_file);
        if( fres == 4 ) {

            sf = fopen((char *)state_file, "r");
            if (sf == NULL)
            {
                TTCN_error("SIGCOMP: Could not open advertisement state file");
            }
            fseek(sf, 0, SEEK_END);
            state_length = ftell(sf);
            rewind(sf);
            state_value = (unsigned char *)Malloc(state_length);
            fread(state_value, state_length, 1, sf);
            fclose(sf);
            state.set(state_length, state_address, state_instr,
                min_acc_len, state_value);
            Free(state_value);
            res = dao->createState(state);
            if (res.code() != Success)
            {
                TTCN_error("SIGCOMP: Unable to create advertisement state:%s",
                    (const char *) hexdump(state.id(), SHA_DIGEST_LENGTH));
            }
        } else {
            TTCN_error("Could not read advertise states.");
        }
    }
    fclose(scfg);
    log("Sigcomp config file loaded");
    Free(cfgFile);
}

#endif
}
using namespace SIPmsg__PortType;
namespace SIPmsg__Types{

CHARSTRING f__SIP__encode__formatted(const PDU__SIP& pdu, const BOOLEAN &short__headers, const BOOLEAN& multiple__headers, const BOOLEAN& ipv6enabled){
  int msgsize=0;
  int msg_encode_buff_size=1500;
  char *msg_encode_buff=(char*)Malloc(msg_encode_buff_size*sizeof(char));
  int body_length=0;
  char tempsend[20];
  CHARSTRING ret_val;
  if(pdu.get_selection()==PDU__SIP::ALT_response){
    msg_encode_buff[0]='\0';
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.response().statusLine().sipVersion());
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );
    sprintf(tempsend, "%d" , (int)pdu.response().statusLine().statusCode());
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, tempsend);
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, pdu.response().statusLine().reasonPhrase() );
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );
    if (pdu.response().messageBody().ispresent()){
      body_length=pdu.response().messageBody()().lengthof();
    } else if (pdu.response().payload().ispresent()){
      body_length=pdu.response().payload()().payloadvalue().lengthof();
    }
    else body_length=-1;

    SIPmsg__PT::encode_headers(msg_encode_buff,msgsize,msg_encode_buff_size,body_length,short__headers,multiple__headers,ipv6enabled,&(pdu.response().msgHeader()));
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );  // LONE CRLF
    if (pdu.response().messageBody().ispresent()){
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.response().messageBody()());
    } else if (pdu.response().payload().ispresent()){
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.response().payload()().payloadvalue());
    }
    ret_val=CHARSTRING(msgsize,msg_encode_buff);
  } else if(pdu.get_selection()==PDU__SIP::ALT_request) {
    msgsize=strlen(Method::enum_to_str(pdu.request().requestLine().method()))-2;
    strncpy(msg_encode_buff, Method::enum_to_str(pdu.request().requestLine().method()),msgsize);
    msg_encode_buff[msgsize]='\0';  // Method
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );
    SIPmsg__PT::print_url(msg_encode_buff,msgsize,msg_encode_buff_size,&pdu.request().requestLine().requestUri(), 0,ipv6enabled ); // Request URI
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size," " );
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.request().requestLine().sipVersion()); // SIP version
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,"\r\n" );
    if (pdu.request().messageBody().ispresent()){
      body_length=pdu.request().messageBody()().lengthof();
    } else if (pdu.request().payload().ispresent()){
      body_length=pdu.request().payload()().payloadvalue().lengthof();
    }
    else body_length=-1;
    SIPmsg__PT::encode_headers(msg_encode_buff,msgsize,msg_encode_buff_size,body_length,short__headers,multiple__headers,ipv6enabled,&(pdu.request().msgHeader()));
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,"\r\n" );  // LONE CRLF
    if (pdu.request().messageBody().ispresent()){
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.request().messageBody()());
    } else if (pdu.request().payload().ispresent()){
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.request().payload()().payloadvalue());
    }
    ret_val=CHARSTRING(msgsize,msg_encode_buff);
  } else if(pdu.get_selection()==PDU__SIP::ALT_raw){
      ret_val=pdu.raw();
  }
  Free(msg_encode_buff);
  return ret_val;
}
CHARSTRING f__SIP__encode(const PDU__SIP& pdu){
  return f__SIP__encode__formatted(pdu,false,false,true);
}

OCTETSTRING f__SIP__encode__binary(const PDU__SIP& pdu){
  return f__SIP__encode__formatted__binary(pdu,false,false,true);
}
OCTETSTRING f__SIP__encode__formatted__binary(const PDU__SIP& pdu, const BOOLEAN &short__headers, const BOOLEAN& multiple__headers, const BOOLEAN& ipv6enabled){
  int msgsize=0;
  int msg_encode_buff_size=1500;
  char *msg_encode_buff=(char*)Malloc(msg_encode_buff_size*sizeof(char));
  int body_length=0;
  char tempsend[20];
  OCTETSTRING ret_val;
  if(pdu.get_selection()==PDU__SIP::ALT_response){
    msg_encode_buff[0]='\0';
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.response().statusLine().sipVersion());
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );
    sprintf(tempsend, "%d" , (int)pdu.response().statusLine().statusCode());
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, tempsend);
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, pdu.response().statusLine().reasonPhrase() );
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );
    if (pdu.response().messageBody().ispresent()){
      body_length=pdu.response().messageBody()().lengthof();
    } else if (pdu.response().payload().ispresent()){
      body_length=pdu.response().payload()().payloadvalue().lengthof();
    }
    else body_length=-1;

    SIPmsg__PT::encode_headers(msg_encode_buff,msgsize,msg_encode_buff_size,body_length,short__headers,multiple__headers,ipv6enabled,&(pdu.response().msgHeader()));
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );  // LONE CRLF
    if (pdu.response().messageBody().ispresent()){
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.response().messageBody()());
    } else if (pdu.response().payload().ispresent()){
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.response().payload()().payloadvalue());
    }
    ret_val=OCTETSTRING(msgsize,(unsigned char*)msg_encode_buff);
  } else if(pdu.get_selection()==PDU__SIP::ALT_request) {
    msgsize=strlen(Method::enum_to_str(pdu.request().requestLine().method()))-2;
    strncpy(msg_encode_buff, Method::enum_to_str(pdu.request().requestLine().method()),msgsize);
    msg_encode_buff[msgsize]='\0';  // Method
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );
    SIPmsg__PT::print_url(msg_encode_buff,msgsize,msg_encode_buff_size,&pdu.request().requestLine().requestUri(), 0,ipv6enabled ); // Request URI
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size," " );
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.request().requestLine().sipVersion()); // SIP version
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,"\r\n" );
    if (pdu.request().messageBody().ispresent()){
      body_length=pdu.request().messageBody()().lengthof();
    } else if (pdu.request().payload().ispresent()){
      body_length=pdu.request().payload()().payloadvalue().lengthof();
    }
    else body_length=-1;
    SIPmsg__PT::encode_headers(msg_encode_buff,msgsize,msg_encode_buff_size,body_length,short__headers,multiple__headers,ipv6enabled,&(pdu.request().msgHeader()));
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,"\r\n" );  // LONE CRLF
    if (pdu.request().messageBody().ispresent()){
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.request().messageBody()());
    } else if (pdu.request().payload().ispresent()){
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.request().payload()().payloadvalue());
    }
    ret_val=OCTETSTRING(msgsize,(unsigned char*)msg_encode_buff);
  } else if(pdu.get_selection()==PDU__SIP::ALT_raw){
      ret_val=OCTETSTRING(pdu.raw().lengthof(),(const unsigned char*)(const char*)pdu.raw());
  }
  Free(msg_encode_buff);
  return ret_val;
}


PDU__SIP f__SIP__decode(const CHARSTRING& pdu, const BOOLEAN& ipv6enabled, const BOOLEAN& wildcarded__uri){
  PDU__SIP ret_val;
  rqlineptr=NULL;
  stlineptr=NULL;
  headerptr= newMsgHdr();

  num_chars=0;
  erro_handling_sip_parser=1;
  errorind_loc=7;   // 111
  wildcarded_enabled_parser=wildcarded__uri;
  parsing((const char*)pdu,pdu.lengthof(),ipv6enabled);
//  SIP_parse_debug=0;
//  SIP_parse_parse();  // also sets appropriate fields of msg through pointers..
  if(errorind_loc){
    delete rqlineptr;
    delete stlineptr;
    ret_val.raw()=pdu;
  } else if(rqlineptr!=NULL){
    ret_val.request().requestLine()=*rqlineptr;
    ret_val.request().msgHeader()=*headerptr;
    if(pdu.lengthof()>num_chars) ret_val.request().messageBody()=CHARSTRING(pdu.lengthof()-num_chars, (const char*)pdu + num_chars);
    else ret_val.request().messageBody()=OMIT_VALUE;
    ret_val.request().payload()=OMIT_VALUE;
    delete rqlineptr;
  }
  else if(stlineptr!=NULL){
    ret_val.response().statusLine()=*stlineptr;
    ret_val.response().msgHeader()=*headerptr;
    if(pdu.lengthof()>num_chars) ret_val.response().messageBody()=CHARSTRING(pdu.lengthof()-num_chars, (const char*)pdu + num_chars);
    else ret_val.response().messageBody()=OMIT_VALUE;
    ret_val.response().payload()=OMIT_VALUE;
    delete stlineptr;
  }
  delete headerptr;
  return ret_val;
}


PDU__SIP f__SIP__decode__binary(const OCTETSTRING& pdu, const BOOLEAN& ipv6enabled, const BOOLEAN& wildcarded__uri, const SIPmsg__body__handling__modes& body__mode){
  PDU__SIP ret_val;
  rqlineptr=NULL;
  stlineptr=NULL;
  headerptr= newMsgHdr();

  num_chars=0;
  erro_handling_sip_parser=1;
  errorind_loc=7;   // 111
  wildcarded_enabled_parser=wildcarded__uri;
  parsing((const char*)(const unsigned char*)pdu,pdu.lengthof(),ipv6enabled);
//  SIP_parse_debug=0;
//  SIP_parse_parse();  // also sets appropriate fields of msg through pointers..
  if(errorind_loc){
    delete rqlineptr;
    delete stlineptr;
    ret_val.raw()=CHARSTRING(pdu.lengthof(),(const char*)(const unsigned char*)pdu);
  } else if(rqlineptr!=NULL){
    ret_val.request().requestLine()=*rqlineptr;
    ret_val.request().msgHeader()=*headerptr;
    if(pdu.lengthof()>num_chars){
      switch(body__mode){
        case 0:
          ret_val.request().messageBody()=CHARSTRING(pdu.lengthof()-num_chars, (const char*)(const unsigned char*)pdu + num_chars);
          ret_val.request().payload()=OMIT_VALUE;
          break;
        case 1:
          {
            bool binary_char=false;
            int a=0;
            int payload_len=pdu.lengthof()-num_chars;
            const unsigned char* pdu_ptr=(const unsigned char*)pdu + num_chars;
            while(a<payload_len && !binary_char){
              binary_char= !pdu_ptr[a] || (pdu_ptr[a] & (const unsigned char)0x80);
              a++;
            }
            if(binary_char){
              ret_val.request().messageBody()=OMIT_VALUE;
              ret_val.request().payload()().payloadvalue()=OCTETSTRING(pdu.lengthof()-num_chars, (const unsigned char*)pdu + num_chars);
            }
            else{
              ret_val.request().messageBody()=CHARSTRING(pdu.lengthof()-num_chars, (const char*)(const unsigned char*)pdu + num_chars);
              ret_val.request().payload()=OMIT_VALUE;
            }
          }
          break;
        case 2:
          ret_val.request().messageBody()=OMIT_VALUE;
          ret_val.request().payload()().payloadvalue()=OCTETSTRING(pdu.lengthof()-num_chars, (const unsigned char*)pdu + num_chars);
          break;
        case 3:
          ret_val.request().messageBody()=CHARSTRING(pdu.lengthof()-num_chars, (const char*)(const unsigned char*)pdu + num_chars);
          ret_val.request().payload()().payloadvalue()=OCTETSTRING(pdu.lengthof()-num_chars, (const unsigned char*)pdu + num_chars);
          break;
        default:
          break;
      }
      
    }
    else {
      ret_val.request().messageBody()=OMIT_VALUE;
      ret_val.request().payload()=OMIT_VALUE;
    }
    delete rqlineptr;
  }
  else if(stlineptr!=NULL){
    ret_val.response().statusLine()=*stlineptr;
    ret_val.response().msgHeader()=*headerptr;
    if(pdu.lengthof()>num_chars){
      switch(body__mode){
        case 0:
          ret_val.response().messageBody()=CHARSTRING(pdu.lengthof()-num_chars, (const char*)(const unsigned char*)pdu + num_chars);
          ret_val.response().payload()=OMIT_VALUE;
          break;
        case 1:
          {
            bool binary_char=false;
            int a=0;
            int payload_len=pdu.lengthof()-num_chars;
            const unsigned char* pdu_ptr=(const unsigned char*)pdu + num_chars;
            while(a<payload_len && !binary_char){
              binary_char= !pdu_ptr[a] || (pdu_ptr[a] & (const unsigned char)0x80);
              a++;
            }
            if(binary_char){
              ret_val.response().messageBody()=OMIT_VALUE;
              ret_val.response().payload()().payloadvalue()=OCTETSTRING(pdu.lengthof()-num_chars, (const unsigned char*)pdu + num_chars);
            }
            else{
              ret_val.response().messageBody()=CHARSTRING(pdu.lengthof()-num_chars, (const char*)(const unsigned char*)pdu + num_chars);
              ret_val.response().payload()=OMIT_VALUE;
            }
          }
          break;
        case 2:
          ret_val.response().messageBody()=OMIT_VALUE;
          ret_val.response().payload()().payloadvalue()=OCTETSTRING(pdu.lengthof()-num_chars, (const unsigned char*)pdu + num_chars);
          break;
        case 3:
          ret_val.response().messageBody()=CHARSTRING(pdu.lengthof()-num_chars, (const char*)(const unsigned char*)pdu + num_chars);
          ret_val.response().payload()().payloadvalue()=OCTETSTRING(pdu.lengthof()-num_chars, (const unsigned char*)pdu + num_chars);
          break;
        default:
          break;
      }
      
    }
    else {
      ret_val.response().messageBody()=OMIT_VALUE;
      ret_val.response().payload()=OMIT_VALUE;
    }
    delete stlineptr;
  }
  delete headerptr;
  return ret_val;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
CHARSTRING f__SIP__encode__fragment__formatted(const PDU__SIP__Fragment& pdu, const BOOLEAN &short__headers, const BOOLEAN& multiple__headers, const BOOLEAN& ipv6enabled){
  int msgsize=0;
  int msg_encode_buff_size=1500;
  char *msg_encode_buff=(char*)Malloc(msg_encode_buff_size*sizeof(char));
  int body_length=0;
  char tempsend[20];
  CHARSTRING ret_val;
 ///////// requestline is present in fragment:
  if(pdu.firstLine().ispresent()){
    if (pdu.firstLine()().get_selection()==FirstLine::ALT_requestLine) {
      msgsize=strlen(Method::enum_to_str(pdu.firstLine()().requestLine().method()))-2;
      strncpy(msg_encode_buff, Method::enum_to_str(pdu.firstLine()().requestLine().method()),msgsize);
      msg_encode_buff[msgsize]='\0';  // Method
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );
      SIPmsg__PT::print_url(msg_encode_buff,msgsize,msg_encode_buff_size,&pdu.firstLine()().requestLine().requestUri(), 0,ipv6enabled ); // Request URI
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size," " );
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.firstLine()().requestLine().sipVersion()); // SIP version
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,"\r\n" );
    } // end if requestline is chosen
    else if (pdu.firstLine()().get_selection()==FirstLine::ALT_statusLine) {
      msg_encode_buff[0]='\0';
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.firstLine()().statusLine().sipVersion());
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );
      sprintf(tempsend, "%d" , (int)pdu.firstLine()().statusLine().statusCode());
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, tempsend);
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, " " );
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, pdu.firstLine()().statusLine().reasonPhrase() );
      SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size, "\r\n" );
    } // end if statusline is chosen
  }
  if(pdu.msgHeader().ispresent()){
    if (pdu.messageBody().ispresent()){
      body_length=pdu.messageBody()().lengthof();
    } else body_length=-1;
    SIPmsg__PT::encode_headers(msg_encode_buff,msgsize,msg_encode_buff_size,body_length,short__headers,multiple__headers,ipv6enabled,&(pdu.msgHeader()()));
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,"\r\n" );  // LONE CRLF
  }
  if (pdu.messageBody().ispresent()){
    SIPmsg__PT::write_to_buff(msg_encode_buff,msgsize,msg_encode_buff_size,pdu.messageBody()());
  }
  ret_val=CHARSTRING(msgsize,msg_encode_buff);
  Free(msg_encode_buff);
  return ret_val;
}
///////////////////////////
CHARSTRING f__SIP__encode__fragment(const PDU__SIP__Fragment& pdu){
  return f__SIP__encode__fragment__formatted(pdu,false,false, true);
}
////////////////////////////////////////////////////////////////////////////////////////////////////

PDU__SIP__Fragment f__SIP__decode__fragment(const CHARSTRING& pdu, const BOOLEAN& ipv6enabled, const BOOLEAN& wildcarded__uri){
  PDU__SIP__Fragment ret_val;
  const char *data=(const char *)pdu;
  CHARSTRING pdu2="";
  while((*data)<' ' &&  (*data)>'\0') data++; // skip leading blank
  int spsp=strlen(data);
  int colonsp=spsp;
  const char* atm=strchr(data,' ');
  if(atm) spsp=atm-data;
  atm=strchr(data,':');
  if(atm) colonsp=atm-data;
  if(spsp>=colonsp) // No status or request line, add a fake
    pdu2="SIP/2.0 999 Filling\r\n";

  pdu2=pdu2+data;
  pdu2=pdu2+"\r\n"; // add alone CRLF

  // start decode
  rqlineptr=NULL;
  stlineptr=NULL;
  headerptr= newMsgHdr();

  num_chars=0;
  erro_handling_sip_parser=0;
  errorind_loc=7;   // 111
  wildcarded_enabled_parser=wildcarded__uri;
  parsing((const char*)pdu2,pdu2.lengthof(), ipv6enabled);
//  SIP_parse_debug=0;
//  SIP_parse_parse();  // also sets appropriate fields of msg through pointers..
  if(errorind_loc>7){
    delete rqlineptr;
    delete stlineptr;
    delete headerptr;
    TTCN_error("Parse error during decoding SIP fragment.");
  } else if(rqlineptr!=NULL){
    ret_val.firstLine()().requestLine()=*rqlineptr;
    delete rqlineptr;
  }
  else if(stlineptr!=NULL){
    if(spsp<colonsp)
      ret_val.firstLine()().statusLine()=*stlineptr;
    else ret_val.firstLine()=OMIT_VALUE;
    delete stlineptr;
  }
  ret_val.msgHeader()=*headerptr;
  if(pdu2.lengthof()>(num_chars+2))
    ret_val.messageBody()=CHARSTRING(pdu2.lengthof()-num_chars-2, (const char*)pdu2 + num_chars);
  else ret_val.messageBody()=OMIT_VALUE;
  ret_val.payload()=OMIT_VALUE;
  delete headerptr;
  return ret_val;
}
}
