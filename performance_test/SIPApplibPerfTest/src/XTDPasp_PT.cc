/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               XTDPasp_PT.cc
//  Description:        XTDP test port source
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 494
//  Updated:            2009.03.28
//  Contact:            http://ttcn.ericsson.se
//


#include "XTDPasp_PT.hh"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define  XTDPLENGTHFIELDLENGTH  4
namespace XTDPasp__PortType {

static const CHARSTRING cs_nl('\n');
static const OCTETSTRING os_bye(char2oct(CHARSTRING("bye\n")));

XTDPasp__PT::XTDPasp__PT(const char *par_port_name)
#ifdef AS_USE_SSL
  : SSL_Socket("XTDP", par_port_name)
#else
    : Abstract_Socket("XTDP", par_port_name)
#endif        
    , XTDPasp__PT_BASE(par_port_name), header_descr(0),
    is_packet_hdr_length_offset(false), is_packet_hdr_nr_bytes_in_length(false),
    is_packet_hdr_byte_order(false)
{
}

XTDPasp__PT::~XTDPasp__PT()
{
	delete header_descr;
	header_descr = 0;
}

void XTDPasp__PT::set_parameter(const char *parameter_name,
                               const char *parameter_value)
{
  log_debug("entering XTDPasp__PT::set_parameter(%s, %s)", parameter_name, parameter_value);
  if(strcmp(parameter_name, "packet_hdr_length_offset") == 0) {
  	is_packet_hdr_length_offset = true;
	if((packet_hdr_length_offset = atoi(parameter_value))<0)
		log_error("The value of parameter 'packet_hdr_length_offset'"
		" must be a non-negative integer");
  }
  else if(strcmp(parameter_name, "packet_hdr_nr_bytes_in_length") == 0) {
  	is_packet_hdr_nr_bytes_in_length = true;
	if((packet_hdr_nr_bytes_in_length = atoi(parameter_value))<0)
		log_error("The value of parameter 'packet_hdr_nr_bytes_in_length'"
		" must be a non-negative integer");
  }
  else if(strcmp(parameter_name, "packet_hdr_byte_order") == 0) {
  	is_packet_hdr_byte_order = true;
	if(strcmp(parameter_value, "MSB") == 0) {
		packet_hdr_byte_order = PacketHeaderDescr::Header_MSB;
	}
	else if (strcmp(parameter_value, "LSB") == 0){
		packet_hdr_byte_order = PacketHeaderDescr::Header_LSB;}
	else log_error("Parameter value '%s' not recognized for parameter '%s'", parameter_value, "packet_hdr_byte_order");
  }
  else {
	if(!parameter_set(parameter_name ,parameter_value)) {
      TTCN_warning("XTDPasp__PT::set_parameter(): Unsupported Test Port parameter: %s", parameter_name);
	}
  }
  log_debug("leaving XTDPasp__PT::set_parameter(%s, %s)", parameter_name, parameter_value);
}

void XTDPasp__PT::Handle_Fd_Event(int fd,
  boolean is_readable, boolean is_writable, boolean is_error)
{
  log_debug("entering XTDPasp__PT::Handle_Fd_Event()");
  Handle_Socket_Event(fd, is_readable, is_writable, is_error);
  log_debug("leaving XTDPasp__PT::Handle_Fd_Event()");
}

void XTDPasp__PT::Handle_Timeout(double time_since_last_call)
{
  log_debug("entering XTDPasp__PT::Handle_Timeout()");
  Handle_Timeout_Event(time_since_last_call);
  log_debug("leaving XTDPasp__PT::Handle_Timeout()");
}

void XTDPasp__PT::message_incoming(const unsigned char* msg, int messageLength, int client_id) {
  log_debug("entering XTDPasp__PT::message_incoming()");
  XTDPasp__Types::ASP__XTDP parameters;
  //parameters.data() = OCTETSTRING(messageLength, msg);
  parameters.data() = XTDPasp__Types::dec__XTDP__Message(CHARSTRING(messageLength - XTDPLENGTHFIELDLENGTH , (const char*)msg + XTDPLENGTHFIELDLENGTH));
  if(client_id != -1) {
    parameters.client__id() = client_id;
  } else {
    parameters.client__id() = OMIT_VALUE;
  }

  incoming_message(parameters); 
  log_debug("leaving XTDPasp__PT::message_incoming()");
}

void XTDPasp__PT::peer_disconnected(int client_id) {
  log_debug("entering XTDPasp__PT::peer_disconnected()");
  if(get_use_connection_ASPs())
  {
    XTDPasp__Types::ASP__XTDP__Close asp;
    asp.client__id() = client_id;
    incoming_message(asp);
  }
  else Abstract_Socket::peer_disconnected(client_id);
  log_debug("leaving XTDPasp__PT::peer_disconnected()");
}

void XTDPasp__PT::user_map(const char *system_port)
{
  log_debug("entering XTDPasp__PT::user_map()");
  if(is_packet_hdr_length_offset && is_packet_hdr_nr_bytes_in_length &&
  	is_packet_hdr_byte_order)
  	header_descr = new PacketHeaderDescr(packet_hdr_length_offset,
	  packet_hdr_nr_bytes_in_length, packet_hdr_byte_order);
  if(!get_use_connection_ASPs())
    map_user();
  log_debug("leaving XTDPasp__PT::user_map()");
}

void XTDPasp__PT::user_unmap(const char *system_port)
{
  log_debug("entering XTDPasp__PT::user_unmap()");

  //FIXME: send "bye" to each GUI implicitly

  //calling unmap_user from  AbstractSocket
  unmap_user();
  log_debug("leaving XTDPasp__PT::user_unmap()");
}

void XTDPasp__PT::user_start()
{
  log_debug("entering XTDPasp__PT::user_start()");
  log_debug("XTDPasp_PT version: R4A02");
  log_debug("leaving XTDPasp__PT::user_start()");
}

void XTDPasp__PT::user_stop()
{
  log_debug("entering XTDPasp__PT::user_stop()");
  log_debug("leaving XTDPasp__PT::user_stop()");
}

void XTDPasp__PT::outgoing_send(const XTDPasp__Types::ASP__XTDP& send_par)
{
  log_debug("entering XTDPasp__PT::outgoing_send(ASP__XTDP)");

  //FIXME: use malloc/free instead of obect-oriented
  //FIXME: use direct C-memory buffer instead of object passing
  //appending linefeed, encoding to charstring then transforming to octetstring
  //and storing in temp variable
  OCTETSTRING encoded = char2oct(XTDPasp__Types::enct__XTDP__Message(send_par.data()));
  
  //preceding 32bit length info
  encoded = int2oct(encoded.lengthof() + XTDPLENGTHFIELDLENGTH, XTDPLENGTHFIELDLENGTH) + encoded;

  if(send_par.client__id().is_bound() && send_par.client__id().ispresent()) {
    send_outgoing((const unsigned char*)encoded,
                  encoded.lengthof(), send_par.client__id()());
  } else {
    send_outgoing((const unsigned char*)encoded,
                  encoded.lengthof());
  }
  log_debug("leaving XTDPasp__PT::outgoing_send(ASP__XTDP)");
}

void XTDPasp__PT::outgoing_send(const XTDPasp__Types::ASP__XTDP__Bye& send_par)
{
  log_debug("entering XTDPasp__PT::outgoing_send(ASP__XTDP__Bye)");

  if(send_par.client__id().is_bound() && send_par.client__id().ispresent()) {
    send_outgoing((const unsigned char*)os_bye,
                  os_bye.lengthof(), send_par.client__id()());
  } else {
    send_outgoing((const unsigned char*)os_bye,
                  os_bye.lengthof());
  }
  log_debug("leaving XTDPasp__PT::outgoing_send(ASP__XTDP__Bye)");
}


void XTDPasp__PT::outgoing_send(const XTDPasp__Types::ASP__XTDP__Connect& send_par)
{
  log_debug("entering XTDPasp__PT::outgoing_send(ASP__XTDP__Connect)");
  
  sockaddr_in localAddr, remoteAddr;
  get_host_id(send_par.hostname(), &remoteAddr);
  remoteAddr.sin_port = htons((unsigned int)(INTEGER)send_par.portnumber());
  if(send_par.local__hostname().is_bound() && send_par.local__hostname().ispresent())
    get_host_id(send_par.local__hostname()(), &localAddr);
  else
    get_host_id("localhost", &localAddr);
  if(send_par.local__portnumber().is_bound() && send_par.local__portnumber().ispresent())
    localAddr.sin_port = htons(send_par.local__portnumber()());
  else
    localAddr.sin_port = htons(0);
  
  open_client_connection(remoteAddr, localAddr);

  log_debug("leaving XTDPasp__PT::outgoing_send(ASP__XTDP__Connect)");
}

void XTDPasp__PT::report_error(int client_id, int msg_length, int sent_length, const unsigned char* msg, const char* error_text)
{
  log_debug("entering XTDPasp__PT::report_error");
  if(get_use_connection_ASPs()){
    XTDPasp__Types::ASP__XTDP__Send__error asp;
    if(client_id != -1)
      asp.client__id() = client_id;
    else
      asp.client__id() = OMIT_VALUE;
    asp.data() = OCTETSTRING(17, (const unsigned char*)"last message sent");
    asp.os__error__code() = errno;
    if(sent_length == -2){
      asp.sent__octets() = 0;
      asp.error__type() = XTDPasp__Types::XTDP__Send__error__types::XTDP__SEND__INVALID__CLIENT__ID;
    }
    else if(sent_length == -1) {
      asp.sent__octets() = 0;
      asp.error__type() = XTDPasp__Types::XTDP__Send__error__types::XTDP__SEND__ERROR;
    }
    else {
      asp.sent__octets() = sent_length;
      asp.error__type() = XTDPasp__Types::XTDP__Send__error__types::XTDP__SEND__MESSAGE__NOT__COMPLETE;
    }
    char *emsg = strerror(errno);
    if(emsg)
      asp.os__error__text() = emsg;
    else
      asp.os__error__text()="";
    asp.ttcn__error__text() = error_text;
    incoming_message(asp);
    errno=0;
  } else
    log_error(error_text);
  log_debug("leaving XTDPasp__PT::report_error");
}

void XTDPasp__PT::client_connection_opened(int client_id)
{
  log_debug("entering XTDPasp__PT::client_connection_opened(%d)", client_id);
  
  if(get_use_connection_ASPs())
  {
    XTDPasp__Types::ASP__XTDP__Connect__result asp;

    asp.client__id() = client_id;

    incoming_message(asp);
  }
  else Abstract_Socket::client_connection_opened(client_id);

  log_debug("leaving XTDPasp__PT::client_connection_opened()");
}

void XTDPasp__PT::peer_connected(int client_id, sockaddr_in& addr)
{
  log_debug("entering XTDPasp__PT::peer_connected(%d)", client_id);
  
  if(get_use_connection_ASPs())
  {
    XTDPasp__Types::ASP__XTDP__Connected asp;

    asp.hostname() = inet_ntoa(addr.sin_addr);
    asp.portnumber() = ntohs(addr.sin_port);
    asp.client__id() = client_id;

    incoming_message(asp);
  }
  else Abstract_Socket::peer_connected(client_id, addr);
  
  log_debug("leaving XTDPasp__PT::peer_connected()");
}

void XTDPasp__PT::outgoing_send(const XTDPasp__Types::ASP__XTDP__Close& send_par)
{
  log_debug("entering XTDPasp__PT::outgoing_send(ASP__XTDP__Close)");

  if(send_par.client__id().is_bound() && send_par.client__id().ispresent())
    remove_client((int)send_par.client__id()());
  else
    remove_all_clients();
  
  log_debug("leaving XTDPasp__PT::outgoing_send(ASP__XTDP__Close)");
}

void XTDPasp__PT::outgoing_send(const XTDPasp__Types::ASP__XTDP__Listen& send_par)
{
  log_debug("entering XTDPasp__PT::outgoing_send(ASP__XTDP__Listen)");

  sockaddr_in addr;
  if(send_par.local__hostname().ispresent())
    get_host_id(send_par.local__hostname()(), &addr);
  else if(get_local_host_name())
  {
    log_debug("using local host name configured in %s: %s", local_address_name(), get_local_host_name());
    get_host_id(get_local_host_name(), &addr);
  }
  else
  {
    log_debug("using 'localhost' as local host name");
    get_host_id("localhost", &addr);
  }
    
  if(send_par.portnumber().ispresent())
    addr.sin_port = htons((unsigned int)send_par.portnumber()());
  else if(get_local_port_number() != 0)
  {
    log_debug("using local port number configured in %s: %d", local_port_name(), get_local_port_number());
    addr.sin_port = htons(get_local_port_number());
  }
  else
  {
    log_debug("using ephemeral local port number");
    addr.sin_port = htons(0);
  }
  
  open_listen_port(addr);
  
  log_debug("leaving XTDPasp__PT::outgoing_send(ASP__XTDP__Listen)");
}

void XTDPasp__PT::listen_port_opened(int port_number)
{
  log_debug("entering XTDPasp__PT::listen_port_opened(%d)", port_number);
  
  if(get_use_connection_ASPs())
  {
    XTDPasp__Types::ASP__XTDP__Listen__result asp;
    asp.portnumber() = port_number;
    incoming_message(asp);
  }
  else Abstract_Socket::listen_port_opened(port_number);
  
  log_debug("leaving XTDPasp__PT::listen_port_opened()");
}

void XTDPasp__PT::outgoing_send(const XTDPasp__Types::ASP__XTDP__Shutdown& send_par)
{
  log_debug("entering XTDPasp__PT::outgoing_send(ASP__XTDP__Shutdown)");

  close_listen_port();

  log_debug("leaving XTDPasp__PT::outgoing_send(ASP__XTDP__Shutdown)");
}

const char* XTDPasp__PT::local_port_name()              { return "serverPort";}
const char* XTDPasp__PT::remote_address_name()          { return "destIPAddr";}
const char* XTDPasp__PT::local_address_name()           { return "serverIPAddr";}
const char* XTDPasp__PT::remote_port_name()             { return "destPort";}
const char* XTDPasp__PT::halt_on_connection_reset_name(){ return "halt_on_connection_reset";}
//const char* XTDPasp__PT::server_mode_name()             { return "server_mode";}
const char* XTDPasp__PT::socket_debugging_name()        { return "socket_debugging";}
const char* XTDPasp__PT::nagling_name()                 { return "nagling";}
//const char* XTDPasp__PT::server_backlog_name()          { return "server_backlog";}
const char* XTDPasp__PT::ssl_use_ssl_name()                { return "ssl_use_ssl";}
const char* XTDPasp__PT::ssl_use_session_resumption_name() { return "ssl_use_session_resumption";}
const char* XTDPasp__PT::ssl_private_key_file_name()       { return "ssl_private_key_file";}
const char* XTDPasp__PT::ssl_trustedCAlist_file_name()     { return "ssl_trustedCAlist_file";}
const char* XTDPasp__PT::ssl_certificate_file_name()       { return "ssl_certificate_chain_file";}
const char* XTDPasp__PT::ssl_password_name()               { return "ssl_private_key_password";}
const char* XTDPasp__PT::ssl_cipher_list_name()            { return "ssl_allowed_ciphers_list";}
const char* XTDPasp__PT::ssl_verifycertificate_name()      { return "ssl_verify_certificate";}
}
