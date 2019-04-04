/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               XTDPasp_PT.hh
//  Description:        XTDP test port header
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 494
//  Updated:            2009.03.28
//  Contact:            http://ttcn.ericsson.se
//


#ifndef XTDPasp__PT_HH
#define XTDPasp__PT_HH

#include "XTDPasp_Types.hh"
#include "XTDPasp_PortType.hh"
#include "Abstract_Socket.hh"
namespace XTDPasp__PortType {
#ifdef AS_USE_SSL
class XTDPasp__PT : public SSL_Socket, public XTDPasp__PT_BASE {
#else
  class XTDPasp__PT : public Abstract_Socket, public XTDPasp__PT_BASE {
#endif
  public:
    XTDPasp__PT(const char *par_port_name=NULL);
    ~XTDPasp__PT();

    void set_parameter(const char *parameter_name,
                       const char *parameter_value);

  protected:
    void user_map(const char *system_port);
    void user_unmap(const char *system_port);

    void user_start();
    void user_stop();

    const char* local_port_name();
    const char* remote_address_name();
    const char* local_address_name();
    const char* remote_port_name();
    const char* halt_on_connection_reset_name();
    // const char* server_mode_name();
    const char* socket_debugging_name();
    const char* nagling_name();
    //const char* server_backlog_name();
    const char* ssl_use_ssl_name();
    const char* ssl_use_session_resumption_name();
    const char* ssl_private_key_file_name();
    const char* ssl_trustedCAlist_file_name();
    const char* ssl_certificate_file_name();
    const char* ssl_password_name();
    const char* ssl_cipher_list_name();
    const char* ssl_verifycertificate_name();

    void outgoing_send(const XTDPasp__Types::ASP__XTDP& send_par);
    void outgoing_send(const XTDPasp__Types::ASP__XTDP__Bye& send_par);
    void outgoing_send(const XTDPasp__Types::ASP__XTDP__Connect& send_par);
    void outgoing_send(const XTDPasp__Types::ASP__XTDP__Close& send_par);
    void outgoing_send(const XTDPasp__Types::ASP__XTDP__Listen& send_par);
    void outgoing_send(const XTDPasp__Types::ASP__XTDP__Shutdown& send_par);

    void message_incoming(const unsigned char* msg, int length, int client_id = -1);
    void listen_port_opened(int port_number);
    void client_connection_opened(int client_id);
    void peer_connected(int client_id, sockaddr_in& addr);
    void peer_disconnected(int client_id);
    void Add_Fd_Read_Handler(int fd) { Handler_Add_Fd_Read(fd); }
    void Add_Fd_Write_Handler(int fd) { Handler_Add_Fd_Write(fd); }
    void Remove_Fd_Read_Handler(int fd) { Handler_Remove_Fd_Read(fd); }
    void Remove_Fd_Write_Handler(int fd) { Handler_Remove_Fd_Write(fd); }
    void Remove_Fd_All_Handlers(int fd) { Handler_Remove_Fd(fd); }
    void Handler_Uninstall() { Uninstall_Handler(); }
    void Timer_Set_Handler(double call_interval, boolean is_timeout = TRUE,
      boolean call_anyway = TRUE, boolean is_periodic = TRUE) {
      Handler_Set_Timer(call_interval, is_timeout, call_anyway, is_periodic);
    }
    void report_error(int client_id, int msg_length, int sent_length, const unsigned char* msg, const char* error_text);

    const PacketHeaderDescr* Get_Header_Descriptor() const
		{return header_descr;}
		
  private:
    void Handle_Fd_Event(int fd, boolean is_readable, boolean is_writable, boolean is_error);
    void Handle_Timeout(double time_since_last_call);

  	PacketHeaderDescr *header_descr;
	bool is_packet_hdr_length_offset, is_packet_hdr_nr_bytes_in_length,
		is_packet_hdr_byte_order;
	int packet_hdr_length_offset, packet_hdr_nr_bytes_in_length;
	 PacketHeaderDescr::HeaderByteOrder packet_hdr_byte_order;
  };
}
#endif
