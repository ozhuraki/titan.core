/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
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

#include "LANL2asp_PT.hh"
#include "LANL2asp_Types.hh"
#include "LANL2asp_PortType.hh"
#include <memory.h>                   // Malloc, Free
#include <unistd.h>                   // close, ioctl
#include <netinet/in.h>               // htons
#include <errno.h>
#include <string.h>

#ifdef LINUX
    # include <sys/socket.h>          // PF_PACKET
    # include <netpacket/packet.h>    // PF_PACKET
    # include <sys/ioctl.h>           // ioctl
    # include <net/ethernet.h>        // ethhdr
    # include <net/if.h>              // ifreq, IFNAMSIZ
#endif

#if defined SOLARIS8 || defined SOLARIS
    # include <sys/bufmod.h>          // SBIOCSCHUNK
    # include <stropts.h>             // ioctl
    # include <fcntl.h>               // O_WRONLY
    # include <sys/dlpi.h>            // DLIOCRAW
    # include <sys/ethernet.h>        // ethernet
    # define IFNAMSIZ 16
    # define ETH_ALEN      ETHERADDRL // Octets in one ethernet addr
    # define ETH_HLEN      14         // Total octets in header.
    # define ETH_ZLEN      ETHERMIN   // Min. octets in frame sans FCS
    # define ETH_DATA_LEN  ETHERMTU   // Max. octets in payload
    # define ETH_FRAME_LEN ETHERMAX   // Max. octets in frame sans FCS
#endif

using namespace LANL2asp__Types;

namespace LANL2asp__PortType {

    LANL2__Parameter__result f__LANL2asp__set__param(LANL2asp__PT& portRef, const CHARSTRING& param__name, const CHARSTRING& param__value) {
        return portRef.own_set_parameter(param__name,param__value,FALSE);
    }

    LANL2asp__PT_PROVIDER::LANL2asp__PT_PROVIDER(const char *par_port_name) : PORT(par_port_name) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        eth_interface_name   = NULL;
        packet_filter        = NULL;
        src_mac_pdu          = NULL;
        dest_mac_pdu         = NULL;
        mac_address          = NULL;
        eth_proto            = 0;
        promisc_mode         = FALSE;
        dump_erroneous_frame = FALSE;
        if_index             = -1;
        error_mode           = 0;
        mapped               = FALSE;
        port_mode            = 0;
        interface_list       = NULL;
        interface_list_size  = 0;
    #endif
    }

    LANL2asp__PT_PROVIDER::~LANL2asp__PT_PROVIDER() {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        Free(eth_interface_name);
        Free(packet_filter);
        Free(src_mac_pdu);
        Free(dest_mac_pdu);
        Free(mac_address);
        for(int i=0; i<interface_list_size; i++) {
            delete interface_list[i];
        }
        Free(interface_list);
    #endif
    }

    void LANL2asp__PT_PROVIDER::user_start() {}
    void LANL2asp__PT_PROVIDER::user_stop()  {}

    void LANL2asp__PT_PROVIDER::set_parameter(const char *parameter_name, const char *parameter_value) {
        own_set_parameter(parameter_name,parameter_value,TRUE);
    }

    int LANL2asp__PT_PROVIDER::own_set_parameter(const char *parameter_name, const char *parameter_value, bool issue_error) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        if (strcmp(parameter_name, "eth_interface_name") == 0) {
            if (mapped) {
                TTCN_warning("LANL2asp_PT('%s'): eth_interface_name can not be changed if the port is mapped",port_name);
                return 3;
            }
            int len = strlen(parameter_value);
            if (len == 0) {
                if (issue_error) {
                    TTCN_error("LANL2asp_PT('%s'): Parameter '%s' is an empty string. It has to be set in config file.",port_name,parameter_name);
                } else {
                    TTCN_warning("LANL2asp_PT('%s'): Parameter '%s' is an empty string. It has to be set in config file.",port_name,parameter_name);
                    return 1;
                }
            } else if (len > IFNAMSIZ) {
                if (issue_error) {
                    TTCN_error("LANL2asp_PT('%s'): Parameter '%s' is too long (expected: at most %d, given: %d characters).",port_name,parameter_name, IFNAMSIZ, len);
                } else {
                    TTCN_warning("LANL2asp_PT('%s'): Parameter '%s' is too long (expected: at most %d, given: %d characters).",port_name,parameter_name, IFNAMSIZ, len);
                    return 1;
                }
            }
            Free(eth_interface_name);
            eth_interface_name = mcopystr(parameter_value);
        } else if (strcmp(parameter_name, "packet_filter") == 0) {
            int len = strlen(parameter_value);
            if (len == 0) {
                TTCN_warning("LANL2asp_PT('%s'): Parameter '%s' is an empty string. All type of packets is received.",port_name,parameter_name);
                return 1;
            } else {
                Free(packet_filter);
                packet_filter =  mcopystr(parameter_value);
            }
        } else if (strcmp(parameter_name, "eth_mac_source") == 0) {
            if (mapped) {
                TTCN_warning("LANL2asp_PT('%s'): eth_mac_source can not be changed if the port is mapped",port_name);
                return 3;
            }
            int len = strlen(parameter_value);
            if (len != ETH_ALEN*2) {
                if (issue_error) {
                    TTCN_error("LANL2asp_PT('%s'): Parameter '%s' is not of correct size (expected: %d, given: %d octets).",port_name,parameter_name, ETH_ALEN, len/2);
                } else {
                    TTCN_warning("LANL2asp_PT('%s'): Parameter '%s' is not of correct size (expected: %d, given: %d octets).",port_name,parameter_name, ETH_ALEN, len/2);
                    return 1;
                }
            }
            Free(src_mac_pdu);
            src_mac_pdu = (unsigned char *)Malloc(ETH_ALEN);
            memcpy(src_mac_pdu,(const unsigned char*)str2oct(parameter_value),ETH_ALEN);
        } else if (strcmp(parameter_name, "eth_mac_destination") == 0) {
            if (mapped) {
                TTCN_warning("LANL2asp_PT('%s'): eth_mac_destination can not be changed if the port is mapped",port_name);
                return 3;
            }
            int len = strlen(parameter_value);
            if (len != ETH_ALEN*2) {
                if (issue_error) {
                    TTCN_error("LANL2asp_PT('%s'): Parameter '%s' is not of correct size (expected: %d, given: %d octets).",port_name,parameter_name, ETH_ALEN, len/2);
                } else {
                    TTCN_warning("LANL2asp_PT('%s'): Parameter '%s' is not of correct size (expected: %d, given: %d octets).",port_name,parameter_name, ETH_ALEN, len/2);
                    return 1;
                }
            }
            Free(dest_mac_pdu);
            dest_mac_pdu = (unsigned char *)Malloc(ETH_ALEN);
            memcpy(dest_mac_pdu,(const unsigned char*)str2oct(parameter_value),ETH_ALEN);
        } else if (strcmp(parameter_name, "eth_proto") == 0) {
            if (mapped) {
                TTCN_warning("LANL2asp_PT('%s'): eth_proto can not be changed if the port is mapped",port_name);
                return 3;
            }
            int len = strlen(parameter_value);
            if (len != 4) {
                if (issue_error) {
                    TTCN_error("LANL2asp_PT('%s'): Parameter '%s' is not of correct size (expected: 2, given: %d octets).",port_name,parameter_name, len/2);
                } else {
                    TTCN_warning("LANL2asp_PT('%s'): Parameter '%s' is not of correct size (expected: 2, given: %d octets).",port_name,parameter_name, len/2);
                    return 1;
                }
            }
            eth_proto = oct2int(str2oct(parameter_value));
        } else if (strcmp(parameter_name, "promiscuous_mode") == 0) {
            if (str2int(parameter_value) == 1) promisc_mode = TRUE;
            else promisc_mode = FALSE;
        } else if (strcmp(parameter_name, "dump_erroneous_frame") == 0) {
            if (str2int(parameter_value) == 1) dump_erroneous_frame = TRUE;
            else dump_erroneous_frame = FALSE;
        } else if (strcasecmp(parameter_name, "error_mode") == 0) {
            if (strcasecmp(parameter_value, "error") == 0) {
                error_mode=0;
            } else if (strcasecmp(parameter_value, "warning") == 0) {
                error_mode=2;
            } else if (strcasecmp(parameter_value, "ignore") == 0) {
                error_mode=1;
            } else if (strcasecmp(parameter_value, "report") == 0) {
                error_mode=3;
            } else {
                error_mode=0;
            }
        } else if (strcasecmp(parameter_name, "port_mode") == 0) {
            if (mapped) {
                TTCN_warning("LANL2asp_PT('%s'): Port mode can not be changed if the port is mapped",port_name);
                return 3;
            }
            if (strcasecmp(parameter_value, "single_interface") == 0) {
                port_mode=0;
            } else if (strcasecmp(parameter_value, "multiple_interface") == 0) {
                port_mode=1;
            } else {
                TTCN_warning("LANL2asp_PT('%s'): Invalid port mode was specified: (%s)",port_name,parameter_value);
                port_mode=0;
                return 1;
            }
        } else {
            TTCN_warning("LANL2asp_PT('%s'): Unsupported Test Port parameter: '%s'.",port_name,parameter_name);
            return 2;
        }
        return 0;
    #else
        TTCN_warning("LANL2asp_PT('%s'): LANL2 Test Port is supported on LINUX and SOLARIS only.",port_name);
        return 3;
    #endif
    }

    int LANL2asp__PT_PROVIDER::get_index_by_fd(int fd) {
        for(int i=0;i<interface_list_size;i++) {
            if (interface_list[i]->socket_fd==fd) return i;
        }
        return -1;
    }

    void LANL2asp__PT_PROVIDER::Handle_Fd_Event_Readable(int fd) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        const unsigned char *packet;   // pcap, pointer to data
        struct pcap_pkthdr  header;    // pcap, packet header
        pcap_t              *p_handle;
        int                 index=0;

        if (port_mode) { index=get_index_by_fd(fd); }

        p_handle= interface_list[index]->p_handle;
        packet = pcap_next(p_handle,&header); // next packet

        if (packet == NULL) {
            // If filtering is not set in kernel, every packet is received, not only the filtered ones.
            // pcap_next() is not null only if the packet match the filter expression -> if null, return and don't give error
            return;
        } else if ((header.len >= ETH_HLEN) && (header.len <= interface_list[index]->max_octets)) {
            if (port_mode) {
                ASP__v2__LANL2 message;
                message.interface__id()  = index;
                message.eth__dst__addr() = OCTETSTRING(ETH_ALEN,packet);
                message.eth__src__addr() = OCTETSTRING(ETH_ALEN,packet+ETH_ALEN);
                message.type__field()    = OCTETSTRING(2,packet+ETH_ALEN+ETH_ALEN);
                message.payload()        = OCTETSTRING(header.len-ETH_HLEN,packet+ETH_HLEN);
                incoming_message(message);
            } else {
                ASP__LANL2 message;
                message.eth__dst__addr() = OCTETSTRING(ETH_ALEN,packet);
                message.eth__src__addr() = OCTETSTRING(ETH_ALEN,packet+ETH_ALEN);
                message.type__field()    = OCTETSTRING(2,packet+ETH_ALEN+ETH_ALEN);
                message.payload()        = OCTETSTRING(header.len-ETH_HLEN,packet+ETH_HLEN);
                incoming_message(message);
            }
            if (dump_erroneous_frame == TRUE) {
                if (header.len < ETH_ZLEN) { // packet size is smaller than correct value
                    TTCN_Logger::begin_event(TTCN_WARNING);
                    TTCN_Logger::log_event("LANL2asp_PT('%s'): Erroneous packet (size of captured packet is not correct: %d bytes)\n",port_name,header.len);
                    unsigned int i=0;
                    unsigned int j=0;
                    for(i=0;i<header.len;i++) {
                        TTCN_Logger::log_octet(packet[i]);
                        TTCN_Logger::log_char(' ');
                        if (j==7) TTCN_Logger::log_event_str("  ");
                        j++;
                        if (j>15) {
                            TTCN_Logger::log_char('\n');
                            j = 0;
                        }
                    }
                    TTCN_Logger::end_event();
                }
            }
        }
        else {
            TTCN_warning("LANL2asp_PT('%s'): Size of captured packet is not correct (expected: %d-%d byte, received: %d).",port_name,ETH_HLEN,interface_list[index]->max_octets,header.len);
        }
    #endif
    }

    void LANL2asp__PT_PROVIDER::user_map(const char *system_port) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        if (mapped) {
            TTCN_error("LANL2asp_PT('%s'): The port is already mapped.",port_name);
        }
        mapped = TRUE;
        if (!port_mode) {
            if (eth_interface_name == NULL) {
                TTCN_error("LANL2asp_PT('%s'): Parameter 'eth_interface_name' must be set in config file.",port_name);
            }
            interface_list = (LANL2asp_interface **)Malloc(sizeof(LANL2asp_interface*));
            interface_list[0] = new LANL2asp_interface;
            interface_list_size = 1;
            #ifdef LINUX
                open_interface(0,eth_interface_name,packet_filter,NULL,promisc_mode,TRUE);
            #elif defined SOLARIS8 || defined SOLARIS
                open_interface(0,eth_interface_name,packet_filter,src_mac_pdu,promisc_mode,TRUE);
            #endif
        }
    #else
        TTCN_warning("LANL2asp_PT('%s'): LANL2 Test Port is supported on LINUX and SOLARIS only.",port_name);
    #endif
    }

    unsigned LANL2asp__PT_PROVIDER::get_iface_mtu(const char *interface_name) {
        unsigned mtu = ETH_DATA_LEN;
        #ifdef LINUX
            int fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (fd < 0) {
                TTCN_Logger::log(TTCN_WARNING, "LANL2asp__PT_PROVIDER::get_iface_mtu(%s): socket() failed (%s), assuming MTU %d", interface_name, strerror(errno), mtu);
            } else {
                struct ifreq ifr;
                strncpy(ifr.ifr_name, interface_name, IFNAMSIZ-1);
                ifr.ifr_name[IFNAMSIZ-1]='\0';
                if (ioctl(fd, SIOCGIFMTU, &ifr) == 0) {
                    TTCN_Logger::log(TTCN_DEBUG, "LANL2asp__PT_PROVIDER::get_iface_mtu(%s): MTU is %d", interface_name, ifr.ifr_mtu);
                    mtu = ifr.ifr_mtu;
                } else {
                    TTCN_Logger::log(TTCN_WARNING, "LANL2asp__PT_PROVIDER::get_iface_mtu(%s): ioctl() failed (%s), assuming MTU %d", interface_name, strerror(errno), mtu);
                }
                close(fd);
            }
        #endif
        return mtu;
    }

    void LANL2asp__PT_PROVIDER::open_interface(const int id, const char* interface_name, const char* l_packet_filter, const unsigned char* def_src_addr, const bool l_promisc_mode, const bool issue_err) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        char errbuf[PCAP_ERRBUF_SIZE]; // pcap buffer for error
        struct bpf_program fp;         // pcap compiled filter
        if (interface_name == NULL) TTCN_error("LANL2asp_PT('%s'): Parameter 'eth_interface_name' must be set in config file.",port_name);
        interface_list[id]->mtu = get_iface_mtu(interface_name);
        interface_list[id]->max_octets = interface_list[id]->mtu + ETH_HLEN;
        interface_list[id]->eth_interface_name=mcopystr(interface_name);
        interface_list[id]->packet_filter=mcopystr(l_packet_filter);
        /* Socket for sending and reading (with pcap) */
        if ((interface_list[id]->p_handle = pcap_open_live(interface_name,interface_list[id]->max_octets,l_promisc_mode,0,errbuf)) == NULL) {
            if (issue_err) {
                TTCN_error("LANL2asp_PT('%s'): Cannot open socket on interface '%s' for reading with pcap.",port_name,eth_interface_name);
            } else {
                ASP__LANL2__open__result asp;
                asp.interface__id() = -1;
                asp.success() = FALSE;
                asp.interface__name() = interface_name;
                asp.default__src__addr() = OMIT_VALUE;
                asp.mtu() = OMIT_VALUE;
                asp.promisc__mode() = OMIT_VALUE;
                asp.packet__filter() = OMIT_VALUE;
                asp.os__error__code() = OMIT_VALUE;
                asp.os__error__text() = "Cannot open socket on the interface for reading with pcap.";
                incoming_message(asp);
            }
            return;
        }
        interface_list[id]->socket_fd = pcap_fileno(interface_list[id]->p_handle);
        if (interface_list[id]->socket_fd <0) {
            if (issue_err) {
                TTCN_error("LANL2asp_PT('%s'): Cannot open socket on interface '%s' with pcap.",port_name,interface_name);
            } else {
                ASP__LANL2__open__result asp;
                asp.interface__id() = -1;
                asp.success() = FALSE;
                asp.interface__name() = interface_name;
                asp.default__src__addr() = OMIT_VALUE;
                asp.mtu() = OMIT_VALUE;
                asp.promisc__mode() = OMIT_VALUE;
                asp.packet__filter() = OMIT_VALUE;
                asp.os__error__code() = OMIT_VALUE;
                asp.os__error__text() = "Cannot open socket on the interface with pcap.";
                incoming_message(asp);
            }
            return;
        }
        #ifdef LINUX
            /* Check interface status, set up if necessary */
            if (!get_ifstatus(interface_name,interface_list[id]->socket_fd)) {
                set_interface_flag(interface_name,interface_list[id]->socket_fd,IFF_UP,TRUE);
                TTCN_warning("LANL2asp_PT('%s'): Interface '%s' was down, now it is set up and running.",port_name,interface_name);
            }
            /* Set broadcasting option */
            int on = 1;
            if (setsockopt(interface_list[id]->socket_fd, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on)) < 0) {
                if (issue_err) {
                    TTCN_error("LANL2asp_PT('%s'): Cannot set broadcasting option for socket on interface '%s'.",port_name,interface_name);
                } else {
                    ASP__LANL2__open__result asp;
                    asp.interface__id() = -1;
                    asp.success() = FALSE;
                    asp.interface__name() = interface_name;
                    asp.default__src__addr() = OMIT_VALUE;
                    asp.mtu() = OMIT_VALUE;
                    asp.promisc__mode() = OMIT_VALUE;
                    asp.packet__filter() = OMIT_VALUE;
                    asp.os__error__code() = errno;
                    asp.os__error__text() = strerror(errno);
                    incoming_message(asp);
                    errno = 0;
                }
                return;
            }
            /* Set promiscuous mode if needed */
            if (l_promisc_mode == TRUE) set_interface_flag(interface_name,interface_list[id]->socket_fd,IFF_PROMISC,TRUE);
            /* Get index of the interface */
            interface_list[id]->if_index = get_ifindex(interface_name,interface_list[id]->socket_fd);
            /* Get MAC address of interface */
            if (def_src_addr==NULL) {
                interface_list[id]->src_mac_address=get_macaddress(interface_list[id]->eth_interface_name,interface_list[id]->socket_fd);
            } else {
                interface_list[id]->src_mac_address=(unsigned char*)Malloc(ETH_ALEN*sizeof(unsigned char));
                memcpy(interface_list[id]->src_mac_address,def_src_addr,ETH_ALEN);
            }
        #elif defined SOLARIS8 || defined SOLARIS
            /* Under Solaris, select() keeps waiting until the next packet, because it is buffered, we have to set timeout and chunk size to zero. */
            int size_zero = 0;
            struct timeval time_zero = {0, 0};
            if (ioctl(interface_list[id]->socket_fd, SBIOCSCHUNK, &size_zero) < 0) TTCN_warning("LANL2asp_PT('%s'): ioctl() with SBIOCSCHUNK returned an error.",port_name);
            if (ioctl(interface_list[id]->socket_fd, SBIOCSTIME,  &time_zero) < 0) TTCN_warning("LANL2asp_PT('%s'): ioctl() with SBIOCSTIME returned an error.",port_name);
            /* Set DLIOCRAW */
            struct strioctl si;
            si.ic_cmd = DLIOCRAW;
            si.ic_timout = -1;
            si.ic_len = 0;
            si.ic_dp = 0;
            if (ioctl(interface_list[id]->socket_fd, I_STR, &si) < 0) {
                if (issue_err) {
                    TTCN_error("LANL2asp_PT('%s'): ioctl() with DLIOCRAW returned an error.",port_name);
                } else {
                    ASP__LANL2__open__result asp;
                    asp.interface__id() = -1;
                    asp.success() = FALSE;
                    asp.interface__name() = interface_name;
                    asp.default__src__addr() = OMIT_VALUE;
                    asp.mtu() = OMIT_VALUE;
                    asp.promisc__mode() = OMIT_VALUE;
                    asp.packet__filter() = OMIT_VALUE;
                    asp.os__error__code() = errno;
                    asp.os__error__text() = strerror(errno);
                    incoming_message(asp);
                    errno=0;
                }
                return;
            }
            if (def_src_addr==NULL) {
                interface_list[id]->src_mac_address=NULL;
            } else {
                interface_list[id]->src_mac_address=(unsigned char*)Malloc(ETH_ALEN*sizeof(unsigned char));
                memcpy(interface_list[id]->src_mac_address,def_src_addr,ETH_ALEN);
            }
        #endif
        /* Check packet filter */
        if (l_packet_filter != NULL) {
            char* own_packet_filter=mcopystr(l_packet_filter);
            if (pcap_compile(interface_list[id]->p_handle,&fp,own_packet_filter,1,0) < 0) {
                if (issue_err) {
                    TTCN_error("LANL2asp_PT('%s'): Packet filter '%s' is not correct and cannot be compiled. Check configuration file.",port_name,l_packet_filter);
                } else {
                    ASP__LANL2__open__result asp;
                    asp.interface__id() = -1;
                    asp.success() = FALSE;
                    asp.interface__name() = interface_name;
                    asp.default__src__addr() = OMIT_VALUE;
                    asp.mtu() = OMIT_VALUE;
                    asp.promisc__mode() = OMIT_VALUE;
                    asp.packet__filter() = OMIT_VALUE;
                    asp.os__error__code() = OMIT_VALUE;
                    asp.os__error__text() = "Packet filter is not correct and cannot be compiled.";
                    incoming_message(asp);
                }
                Free(own_packet_filter);
                return;
            }
            Free(own_packet_filter);
            if (pcap_setfilter(interface_list[id]->p_handle,&fp) < 0) {
                if (issue_err) {
                    TTCN_error("LANL2asp_PT('%s'): Error occured while setting packet filter with pcap.",port_name);
                } else {
                    ASP__LANL2__open__result asp;
                    asp.interface__id() = -1;
                    asp.success() = FALSE;
                    asp.interface__name() = interface_name;
                    asp.default__src__addr() = OMIT_VALUE;
                    asp.mtu() = OMIT_VALUE;
                    asp.promisc__mode() = OMIT_VALUE;
                    asp.packet__filter() = OMIT_VALUE;
                    asp.os__error__code() = OMIT_VALUE;
                    asp.os__error__text() = "Error occured while setting packet filter with pcap.";
                    incoming_message(asp);
                }
                return;
            }
            pcap_freecode(&fp);
        }
        interface_list[id]->promisc_mode=l_promisc_mode;
        interface_list[id]->status=2;
        Handler_Add_Fd_Read(interface_list[id]->socket_fd);
    #endif
    }

    void LANL2asp__PT_PROVIDER::user_unmap(const char *system_port) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        mapped = FALSE;
        for(int i=0; i<interface_list_size; i++) {
            if (interface_list[i]->status==2) {
                if (interface_list[i]->socket_fd!=-1) {
                    interface_list[i]->set_idle();
                }
            }
        }
        Uninstall_Handler();
    #endif
    }

    void LANL2asp__PT_PROVIDER::outgoing_send(const ASP__LANL2& send_par) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        if (port_mode) {
            TTCN_error("LANL2asp_PT('%s'): The ASP_LANL2 can not be used in multi interface mode.",port_name);
        }
        /* Set destination MAC addresses from ASP_LANL2 */
        if (send_par.eth__dst__addr().lengthof() != ETH_ALEN) {
            TTCN_error("LANL2asp_PT('%s'): Length of 'eth_dst_addr' is not of correct size (expected: %d, given: %d octets).",port_name,ETH_ALEN,send_par.eth__dst__addr().lengthof());
        }
        /* Check protocol type field */
        if (send_par.type__field().lengthof() != (ETH_HLEN-ETH_ALEN-ETH_ALEN)) {
            TTCN_error("LANL2asp_PT('%s'): Length of 'type_field' is not of correct size (expected: %d, given: %d octets).",
            port_name,ETH_HLEN-ETH_ALEN-ETH_ALEN,send_par.type__field().lengthof());
        }
        /* Set source MAC addresses (from ASP_LANL2 or HW) */
        if (!send_par.eth__src__addr().ispresent()) {    //if not set in ASP, get MAC address from HW
        #ifdef LINUX
            send_packet(0,(const unsigned char*)send_par.eth__dst__addr(),interface_list[0]->src_mac_address,oct2int(send_par.type__field()),send_par.payload());
            return;
        #elif defined SOLARIS || defined SOLARIS8
            TTCN_error("LANL2asp_PT('%s'): Omit argument 'eth_src_addr'. Source MAC address has to be set in ASP_LANL2.",port_name);
        #endif
        }
        if (send_par.eth__src__addr()().lengthof() != ETH_ALEN) TTCN_error("LANL2asp_PT('%s'): Length of 'eth_src_addr' is not of correct size (expected: %d, given: %d octets).",port_name,ETH_ALEN,send_par.eth__src__addr()().lengthof());
        send_packet(0,(const unsigned char*)send_par.eth__dst__addr(),(const unsigned char*)send_par.eth__src__addr()(),oct2int(send_par.type__field()),send_par.payload());
    #endif
    }

    void LANL2asp__PT_PROVIDER::outgoing_send(const PDU__LANL2& send_par) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        if (port_mode) {
            TTCN_error("LANL2asp_PT('%s'): The PDU_LANL2 can not be used in multi interface mode.",port_name);
        }
        if (dest_mac_pdu == NULL) TTCN_error("LANL2asp_PT('%s'): Test port parameter 'eth_mac_destination' has to be set in config file.",port_name);
        if (eth_proto == 0) TTCN_error("LANL2asp_PT('%s'): Test port parameter 'eth_proto' has to be set in config file.",port_name);
        if (src_mac_pdu == NULL) {
        #ifdef LINUX
            /* Set source MAC if it is not set */
            TTCN_warning("LANL2asp_PT('%s'): Unbound argument 'eth_mac_source'. Source MAC was not set in config file. "
            "It is now set to '%.2x:%.2x:%.2x:%.2x:%.2x:%.2x' from HW.",port_name,interface_list[0]->src_mac_address[0],interface_list[0]->src_mac_address[1],
            interface_list[0]->src_mac_address[2],interface_list[0]->src_mac_address[3],interface_list[0]->src_mac_address[4],interface_list[0]->src_mac_address[5]);
            send_packet(0,dest_mac_pdu,interface_list[0]->src_mac_address,eth_proto,send_par);
        #elif defined SOLARIS8 || defined SOLARIS
            if (src_mac_pdu == NULL)
            TTCN_error("LANL2asp_PT('%s'): Test port parameter 'eth_mac_source' has to be set in config file.",port_name);
        #endif
        } else {
            send_packet(0,dest_mac_pdu,src_mac_pdu,eth_proto,send_par);
        }
    #endif
    }

    void LANL2asp__PT_PROVIDER::outgoing_send(const ASP__v2__LANL2& send_par) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        if (!port_mode) {
            TTCN_error("LANL2asp_PT('%s'): The ASP_v2_LANL2 can not be used in multi interface mode.",port_name);
        }
        int id=send_par.interface__id();
        if (id>=interface_list_size || interface_list[id]->status!=2) {
            switch(error_mode) {
                case 0: // 0 - error
                    TTCN_error("LANL2asp_PT('%s'): Invalid inteface id was specified '%d'.",port_name,id);
                    break;
                case 2: // 2 - warning
                    TTCN_warning("LANL2asp_PT('%s'): Invalid inteface id was specified '%d'.",port_name,id);
                    break;
                case 3: { // 3 - report
                        ASP__v2__LANL2__Error asp;
                        asp.interface__id()=id;
                        asp.error__type()=LANL2__Error__Types::INVALID__INTEFACE__ID;
                        asp.sent__asp()=send_par;
                        asp.sent__octet()=OMIT_VALUE;
                        asp.os__error__code()=OMIT_VALUE;
                        asp.os__error__text()=OMIT_VALUE;
                        incoming_message(asp);
                    }
                    break;
                case 1: // 1 - ignore
                default:
                ; // - nothing
            }
            return;
        }
        if (send_par.eth__dst__addr().lengthof() != ETH_ALEN) {
            TTCN_error("LANL2asp_PT('%s'): Length of 'eth_dst_addr' is not of correct size (expected: %d, given: %d octets).",port_name,ETH_ALEN,send_par.eth__dst__addr().lengthof());
        }
        /* Check protocol type field */
        if (send_par.type__field().lengthof() != (ETH_HLEN-ETH_ALEN-ETH_ALEN)) {
            TTCN_error("LANL2asp_PT('%s'): Length of 'type_field' is not of correct size (expected: %d, given: %d octets).",
            port_name,ETH_HLEN-ETH_ALEN-ETH_ALEN,send_par.type__field().lengthof());
        }
        /* Set source MAC addresses (from ASP_LANL2 or HW) */
        if (!send_par.eth__src__addr().ispresent()) {    //if not set in ASP, get MAC address from HW
        #ifdef LINUX
            send_packet(id,(const unsigned char*)send_par.eth__dst__addr(),interface_list[id]->src_mac_address,oct2int(send_par.type__field()),send_par.payload());
            return;
        #elif defined SOLARIS || defined SOLARIS8
            TTCN_error("LANL2asp_PT('%s'): Omit argument 'eth_src_addr'. Source MAC address has to be set in ASP_LANL2.",port_name);
        #endif
        }
        if (send_par.eth__src__addr()().lengthof() != ETH_ALEN) TTCN_error("LANL2asp_PT('%s'): Length of 'eth_src_addr' is not of correct size (expected: %d, given: %d octets).",port_name,ETH_ALEN,send_par.eth__src__addr()().lengthof());
        send_packet(id,(const unsigned char*)send_par.eth__dst__addr(),(const unsigned char*)send_par.eth__src__addr()(),oct2int(send_par.type__field()),send_par.payload());
    #endif
    }

    void LANL2asp__PT_PROVIDER::outgoing_send(const ASP__LANL2__open__interface& send_par) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        if (!port_mode) {
            TTCN_error("LANL2asp_PT('%s'): The ASP_LANL2_open_interface can not be used in multi interface mode.",port_name);
        }
        int id=0;
        int first_free=-1;
        for(id=0;id<interface_list_size;id++) {
            if (interface_list[id]->status==0 && first_free==-1) {
                first_free=id;
            } else if (send_par.interface__name()==interface_list[id]->eth_interface_name) {
                if (interface_list[id]->status==2) {
                    ASP__LANL2__open__result asp;
                    asp.interface__id() = id;
                    asp.success() = FALSE;
                    asp.interface__name() = send_par.interface__name();
                    if (interface_list[id]->src_mac_address) {
                        asp.default__src__addr() = OCTETSTRING(6,interface_list[id]->src_mac_address);
                    } else {
                        asp.default__src__addr() = OMIT_VALUE;
                    }
                    asp.mtu() = INTEGER(interface_list[id]->mtu);
                    asp.promisc__mode() = interface_list[id]->promisc_mode;
                    asp.packet__filter() = interface_list[id]->packet_filter;
                    asp.os__error__code() = OMIT_VALUE;
                    asp.os__error__text() = "Already opened";
                    incoming_message(asp);
                } else {
                    break;
                }
            }
        }
        if (id<interface_list_size) {
            Free(interface_list[id]->eth_interface_name);
        } else if (first_free!=-1) {
            id=first_free;
        } else {
            interface_list_size++;
            interface_list=(LANL2asp_interface**)Realloc(interface_list,interface_list_size*sizeof(LANL2asp_interface*));
            interface_list[id]=new LANL2asp_interface;
        }
        const char* loc_filter;
        if (send_par.packet__filter().ispresent()) {
            loc_filter=send_par.packet__filter()();
        } else {
            loc_filter=packet_filter;
        }
        const unsigned char* loc_src_addr;
        if (send_par.default__src__addr().ispresent()) {
            loc_src_addr=send_par.default__src__addr()();
        } else {
            loc_src_addr=src_mac_pdu;
        }
        bool loc_p_mode;
        if (send_par.promisc__mode().ispresent()) {
            loc_p_mode=send_par.promisc__mode()();
        } else {
            loc_p_mode=promisc_mode;
        }
        open_interface(id,send_par.interface__name(),loc_filter,loc_src_addr,loc_p_mode,FALSE);
        ASP__LANL2__open__result asp;
        asp.interface__id() = id;
        asp.success() = TRUE;
        asp.interface__name() = send_par.interface__name();
        if (interface_list[id]->src_mac_address) {
            asp.default__src__addr() = OCTETSTRING(6,interface_list[id]->src_mac_address);
        } else {
            asp.default__src__addr() = OMIT_VALUE;
        }
        asp.mtu() = INTEGER(interface_list[id]->mtu);
        asp.promisc__mode() = interface_list[id]->promisc_mode;
        asp.packet__filter() = interface_list[id]->packet_filter;
        asp.os__error__code() = OMIT_VALUE;
        asp.os__error__text() = OMIT_VALUE;
        incoming_message(asp);
    #endif
    }

    void LANL2asp__PT_PROVIDER::outgoing_send(const ASP__LANL2__close__interface& send_par) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        if (!port_mode) {
            TTCN_error("LANL2asp_PT('%s'): The ASP_LANL2_close_interface can not be used in multi interface mode.",port_name);
        }
        int id=send_par.interface__id();
        if (id>=interface_list_size || interface_list[id]->status!=2) {
            switch(error_mode) {
                case 0: // 0 - error
                    TTCN_error("LANL2asp_PT('%s'): Invalid inteface id was specified '%d'.",port_name,id);
                    break;
                case 2: // 2 - warning
                    TTCN_warning("LANL2asp_PT('%s'): Invalid inteface id was specified '%d'.",port_name,id);
                    break;
                case 3: { // 3 - report
                        ASP__v2__LANL2__Error asp;
                        asp.interface__id()=id;
                        asp.error__type()=LANL2__Error__Types::INVALID__INTEFACE__ID;
                        asp.sent__asp()=OMIT_VALUE;
                        asp.sent__octet()=OMIT_VALUE;
                        asp.os__error__code()=OMIT_VALUE;
                        asp.os__error__text()=OMIT_VALUE;
                        incoming_message(asp);
                    }
                    break;
                case 1: // 1 - ignore
                default:
                ; // - nothing
            }
            return;
        }
        if (interface_list[id]->socket_fd!=-1) {
            Handler_Remove_Fd_Read(interface_list[id]->socket_fd);
            interface_list[id]->socket_fd=-1;
        }
        interface_list[id]->set_idle();
    #endif
    }

    void LANL2asp__PT_PROVIDER::send_packet(const int id, const unsigned char* dst_addr, const unsigned char* src_addr, const int ether_type, const OCTETSTRING& data) {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        char                *buffer = NULL;        // buffer for ethernet frame
        unsigned int        padding = 0;           // octets of required padding
        #ifdef LINUX
            struct ethhdr       *eth_header;           // ethernet header
            struct sockaddr_ll  socket_address;        // target socket address
        #elif defined SOLARIS || defined SOLARIS8
            struct ether_header *eth_header;           // ethernet header
        #endif
        /* Check size of payload */
        int payload_len = data.lengthof();
        if (payload_len > ETH_DATA_LEN) TTCN_error("LANL2asp_PT('%s'): Length of 'payload' is not of correct size (expected: %d-%d, given: %d byte).", port_name,ETH_ZLEN-ETH_HLEN,ETH_DATA_LEN,payload_len);
        if (payload_len < (ETH_ZLEN-ETH_HLEN)) padding = ETH_ZLEN-ETH_HLEN-payload_len;
        /* Allocate required space, set ethernet header */
        int sizeofpacket = ETH_HLEN+payload_len+padding;
        buffer = (char *)Malloc(sizeofpacket);
        #ifdef LINUX
            eth_header  = (ethhdr *)buffer;
            memcpy(eth_header->h_dest,dst_addr,ETH_ALEN);
            memcpy(eth_header->h_source,src_addr,ETH_ALEN);
            eth_header->h_proto = htons(ether_type);
        #elif  defined SOLARIS || defined SOLARIS8
            eth_header  = (ether_header *)buffer;
            memcpy(eth_header->ether_dhost.ether_addr_octet,dst_addr,ETH_ALEN);
            memcpy(eth_header->ether_shost.ether_addr_octet,src_addr,ETH_ALEN);
            eth_header->ether_type = htons(ether_type);
        #endif
        /* Make the packet */
        memcpy((char*)(buffer+ETH_HLEN),data, payload_len);
        if (padding != 0) memset((char*)(buffer+ETH_HLEN+payload_len), 0, padding);
        /* Sending packet */
        #ifdef LINUX
            /* Set socket address */
            memset(&socket_address, 0, sizeof(struct sockaddr_ll));
            socket_address.sll_family   = PF_PACKET;   // RAW communication
            socket_address.sll_ifindex  = interface_list[id]->if_index;    // index of the network device
            socket_address.sll_halen    = ETH_ALEN;    // address length
            memcpy(socket_address.sll_addr,dst_addr,ETH_ALEN);
            int send_result = sendto(interface_list[id]->socket_fd, buffer, sizeofpacket, 0, (struct sockaddr*)&socket_address, sizeof(socket_address));
        #elif defined SOLARIS || defined SOLARIS8
            # if defined DLIOCRAW
                int send_result = write(interface_list[id]->socket_fd,buffer,sizeofpacket);
            # else
                TTCN_warning("LANL2asp_PT('%s'): This version of SOLARIS is not supported for sending.",port_name);
            # endif
        #endif
        if (send_result < 0) {
            switch(error_mode) {
                case 0: // 0 - error
                    TTCN_error("LANL2asp_PT('%s'): Cannot send packet on interface '%s'.",port_name,eth_interface_name);
                    break;
                case 2: // 2 - warning
                    TTCN_warning("LANL2asp_PT('%s'): Cannot send packet on interface '%s'.",port_name,eth_interface_name);
                    break;
                case 3: { // 3 - report
                        if (port_mode) {
                            ASP__v2__LANL2__Error asp;
                            asp.interface__id()=id;
                            asp.error__type()=LANL2__Error__Types::SEND__FAILED;
                            asp.sent__asp()().interface__id()=id;
                            asp.sent__asp()().eth__dst__addr()=OCTETSTRING(ETH_ALEN,dst_addr);
                            asp.sent__asp()().eth__src__addr()=OCTETSTRING(ETH_ALEN,src_addr);
                            asp.sent__asp()().type__field()=int2oct(ETH_ALEN,ether_type);
                            asp.sent__asp()().payload()=data;
                            asp.sent__octet()=0;
                            asp.os__error__code()=errno;
                            asp.os__error__text()=strerror(errno);
                            incoming_message(asp);
                        } else {
                            ASP__LANL2__Error asp;
                            asp.error__type()=LANL2__Error__Types::SEND__FAILED;
                            asp.sent__asp()().eth__dst__addr()=OCTETSTRING(ETH_ALEN,dst_addr);
                            asp.sent__asp()().eth__src__addr()=OCTETSTRING(ETH_ALEN,src_addr);
                            asp.sent__asp()().type__field()=int2oct(ETH_ALEN,ether_type);
                            asp.sent__asp()().payload()=data;
                            asp.sent__octet()=0;
                            asp.os__error__code()=errno;
                            asp.os__error__text()=strerror(errno);
                            incoming_message(asp);
                        }
                    }
                    break;
                case 1: // 1 - ignore
                default:
                ; // - nothing
            }
            errno=0;
        }
        else if (send_result != sizeofpacket) {
            switch(error_mode) {
                case 0: // 0 - error
                    TTCN_error("LANL2asp_PT('%s'): Sending error on interface '%s'. %d bytes was sent instead of %d.",port_name,eth_interface_name,send_result,sizeofpacket);
                    break;
                case 2: // 2 - warning
                    TTCN_warning("LANL2asp_PT('%s'): Sending error on interface '%s'. %d bytes was sent instead of %d.",port_name,eth_interface_name,send_result,sizeofpacket);
                    break;
                case 3: { // 3 - report
                        if (port_mode) {
                            ASP__v2__LANL2__Error asp;
                            asp.interface__id()=id;
                            asp.error__type()=LANL2__Error__Types::PARTIAL__SEND;
                            asp.sent__asp()().interface__id()=id;
                            asp.sent__asp()().eth__dst__addr()=OCTETSTRING(ETH_ALEN,dst_addr);
                            asp.sent__asp()().eth__src__addr()=OCTETSTRING(ETH_ALEN,src_addr);
                            asp.sent__asp()().type__field()=int2oct(ETH_ALEN,ether_type);
                            asp.sent__asp()().payload()=data;
                            asp.sent__octet()=send_result;
                            asp.os__error__code()=errno;
                            asp.os__error__text()=strerror(errno);
                            incoming_message(asp);
                        } else {
                            ASP__LANL2__Error asp;
                            asp.error__type()=LANL2__Error__Types::PARTIAL__SEND;
                            asp.sent__asp()().eth__dst__addr()=OCTETSTRING(ETH_ALEN,dst_addr);
                            asp.sent__asp()().eth__src__addr()=OCTETSTRING(ETH_ALEN,src_addr);
                            asp.sent__asp()().type__field()=int2oct(ETH_ALEN,ether_type);
                            asp.sent__asp()().payload()=data;
                            asp.sent__octet()=send_result;
                            asp.os__error__code()=errno;
                            asp.os__error__text()=strerror(errno);
                            incoming_message(asp);
                        }
                    }
                    break;
                case 1: // 1 - ignore
                default:
                ; // - nothing
            }
            errno=0;
        }
        Free(buffer);
    #endif
    }

    #ifdef LINUX
        /* Return the index of given interface */
        int LANL2asp__PT_PROVIDER::get_ifindex(const char *interface_name, int socket_fd) {
            if (socket_fd < 0)
            TTCN_error("LANL2asp_PT('%s'): Cannot open socket.",port_name);
            struct ifreq ifr;
            memset(&ifr, 0, sizeof(struct ifreq));
            strncpy(ifr.ifr_name, interface_name, sizeof(ifr.ifr_name)-1);
            if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0)
            TTCN_error("LANL2asp_PT('%s'): Cannot get index of interface '%s'.",port_name,interface_name);
            return ifr.ifr_ifindex;
        }

        /* Get the link status. Returns TRUE if the link is up and FALSE otherwise */
        boolean LANL2asp__PT_PROVIDER::get_ifstatus(const char *interface_name, int socket_fd) {
            if (socket_fd < 0) TTCN_error("LANL2asp_PT('%s'): Cannot open socket.",port_name);
            struct ifreq ifr;
            memset(&ifr, 0, sizeof(struct ifreq));
            strncpy(ifr.ifr_name, interface_name, sizeof(ifr.ifr_name)-1);
            if (ioctl(socket_fd, SIOCGIFFLAGS, &ifr) < 0) TTCN_error("LANL2asp_PT('%s'): Cannot get flags of interface '%s'.",port_name,interface_name);
            return ifr.ifr_flags & IFF_RUNNING ? TRUE : FALSE;
        }

        /* Get MAC address from HW */
        // Caller is responsible for deallocating the return value
        unsigned char* LANL2asp__PT_PROVIDER::get_macaddress(const char* interface_name, int socket_fd) {
            if (socket_fd < 0) {
                socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
                if (socket_fd < 0) TTCN_error("LANL2asp_PT('%s'): Cannot open socket.",port_name);
            }
            struct ifreq ifr;
            memset(&ifr, 0, sizeof(struct ifreq));
            strncpy(ifr.ifr_name, interface_name, sizeof(ifr.ifr_name)-1);
            if (ioctl(socket_fd, SIOCGIFHWADDR, &ifr) < 0) TTCN_error("LANL2asp_PT('%s'): Cannot get MAC address of interface '%s'.",port_name,interface_name);
            unsigned char *retval = (unsigned char *)Malloc(sizeof(ifr.ifr_hwaddr.sa_data));
            memcpy(retval,ifr.ifr_hwaddr.sa_data,sizeof(ifr.ifr_hwaddr.sa_data));
            return retval;
        }

        void LANL2asp__PT_PROVIDER::set_interface_flag(const char* interface_name, int socket_fd, int if_flag, boolean flag_value) {
            if (socket_fd < 0) TTCN_error("LANL2asp_PT: Cannot open socket.");
            struct ifreq ifr;
            memset(&ifr, 0, sizeof(struct ifreq));
            strncpy(ifr.ifr_name, interface_name, sizeof(ifr.ifr_name)-1);
            if (ioctl(socket_fd, SIOCGIFFLAGS, &ifr) < 0) TTCN_error("LANL2asp_PT: Cannot get flags of interface '%s'.",interface_name);
            if (flag_value == TRUE) ifr.ifr_flags |= if_flag;
            else ifr.ifr_flags &= ~if_flag;
            if (ioctl(socket_fd, SIOCSIFFLAGS, &ifr) < 0) TTCN_error("LANL2asp_PT: Cannot set flags of interface '%s'.",interface_name);
        }
    #endif

    LANL2asp_interface::LANL2asp_interface() {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        status=0;
        socket_fd=-1;
        if_index=-1;
        src_mac_address=NULL;
        eth_interface_name=NULL;
        p_handle=NULL;
        packet_filter=NULL;
        promisc_mode = FALSE;
        mtu = ETH_DATA_LEN;
        max_octets = ETH_FRAME_LEN;
    #endif
    }

    LANL2asp_interface::~LANL2asp_interface() {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        set_idle();
        Free(eth_interface_name);
    #endif
    }

    void LANL2asp_interface::set_idle() {
    #if defined LINUX || defined SOLARIS8 || defined SOLARIS
        #ifdef LINUX
            if (status==2) {
                if (promisc_mode == TRUE) LANL2asp__PT_PROVIDER::set_interface_flag(eth_interface_name,socket_fd,IFF_PROMISC,FALSE);
            }
        #endif
            if (p_handle!=NULL) { pcap_close(p_handle); p_handle=NULL; }
            Free(src_mac_address);
            src_mac_address=NULL;
            Free(packet_filter);
            packet_filter=NULL;
            socket_fd=-1;
            if_index=-1;
            status=1;
            promisc_mode = FALSE;
    #endif
    }
}
