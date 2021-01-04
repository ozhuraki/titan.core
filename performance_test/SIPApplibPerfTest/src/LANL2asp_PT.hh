/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               LANL2asp_PT.cc
//  Description:        LANL2 testport source
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 519
//  Updated:            2012-02-15
//  Contact:            http://ttcn.ericsson.se
//

// LANL2 Test port is supported on LINUX and SOLARIS only

#ifndef LANL2asp__PT_PROVIDER_HH
#define LANL2asp__PT_PROVIDER_HH

    #include "LANL2asp_Types.hh"

    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
    #include <pcap.h> //libpcap
    #include <TTCN3.hh>
    #endif

    namespace LANL2asp__PortType {

        class LANL2asp_interface {
            public:
            LANL2asp_interface();
            ~LANL2asp_interface();
            void set_idle();
            int status;   //0 - empty, 1 - idle, 2 - used
            int if_index;
            int socket_fd;
            unsigned char *src_mac_address;
            char          *eth_interface_name;
            char          *packet_filter;
            boolean       promisc_mode;
            unsigned      mtu;
            unsigned      max_octets;
            #if defined LINUX || defined SOLARIS8 || defined SOLARIS
                pcap_t    *p_handle; //pcap descriptor
            #endif
        };

        class LANL2asp__PT_PROVIDER : public PORT {
          private:
            LANL2asp_interface **interface_list;
            int                interface_list_size;
            boolean            mapped;               // Map flag
            int                port_mode;            // port mode 0-old 1-multi interface
            boolean            promisc_mode;         // flag for promiscuous mode
            boolean            dump_erroneous_frame; // flag for dumping erroneous frames
            int                if_index;             // index of interface
            unsigned char      *mac_address;         // MAC address of interface
            char               *eth_interface_name;  // test port parameter, name of the interface
            char               *packet_filter;       // test port parameter, expression for filtering packets
            unsigned char      *src_mac_pdu;         // test port parameter, source MAC address of PDU
            unsigned char      *dest_mac_pdu;        // test port parameter, destination MAC address of PDU
            unsigned int       eth_proto;            // test port parameter, ethernet type field
            int                error_mode;           // 0 - error, 1 - ignore, 2 - warning, 3 - report

          public:
            LANL2asp__PT_PROVIDER(const char *par_port_name = NULL);
            ~LANL2asp__PT_PROVIDER();
            void set_parameter(const char *parameter_name, const char *parameter_value);
            void Handle_Fd_Event_Readable(int fd);
            int own_set_parameter(const char *parameter_name, const char *parameter_value, bool issue_error);
            #ifdef LINUX
                static void set_interface_flag(const char* interface_name, int socket_fd, int if_flag, boolean flag_value);  // Set flag of given interface
            #endif

          protected:
            void user_map(const char *system_port);
            void user_unmap(const char *system_port);
            void user_start();
            void user_stop();
            void outgoing_send(const LANL2asp__Types::PDU__LANL2& send_par);
            void outgoing_send(const LANL2asp__Types::ASP__LANL2& send_par);
            void outgoing_send(const LANL2asp__Types::ASP__v2__LANL2& send_par);
            void outgoing_send(const LANL2asp__Types::ASP__LANL2__open__interface& send_par);
            void outgoing_send(const LANL2asp__Types::ASP__LANL2__close__interface& send_par);
            virtual void incoming_message (const LANL2asp__Types::ASP__LANL2& incoming_par) = 0;
            virtual void incoming_message (const LANL2asp__Types::ASP__LANL2__open__result& incoming_par) = 0;
            virtual void incoming_message (const LANL2asp__Types::ASP__LANL2__Error& incoming_par) = 0;
            virtual void incoming_message (const LANL2asp__Types::ASP__v2__LANL2& incoming_par) = 0;
            virtual void incoming_message (const LANL2asp__Types::ASP__v2__LANL2__Error& incoming_par) = 0;
            unsigned get_iface_mtu(const char *interface_name);
            void open_interface(const int id, const char* interface_name, const char* l_packet_filter, const unsigned char* def_src_addr, const bool l_promisc_mode, const bool issue_err);
            void send_packet(const int id, const unsigned char* dst_addr, const unsigned char* src_addr, const int ether_type, const OCTETSTRING& data);
            #ifdef LINUX
                int get_ifindex(const char *interface_name, int socket_fd);               // Return the index of given interface
                boolean get_ifstatus(const char *interface_name, int socket_fd);          // Get link status of interface
                unsigned char* get_macaddress(const char* interface_name, int socket_fd); // Return MAC address of interface
            #endif
            int get_index_by_fd(int fd);
        };
    }
#endif
