/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Szabo, Bence Janos
 *
 ******************************************************************************/

#include "P1.hh"
#include "PortTranslation.hh"

namespace PortTranslation {

P1_PROVIDER::P1_PROVIDER(const char *par_port_name)
	: PORT(par_port_name)
{
	user_start_ran = FALSE;
	user_map_ran = 0;
	port_param_set = FALSE;
}

P1_PROVIDER::~P1_PROVIDER()
{

}

void P1_PROVIDER::set_parameter(const char * parameter_name,
	const char * parameter_value)
{
	if (strcmp(parameter_name, "test_param") == 0) {
		port_param_set = TRUE;
	}
}

/*void P1_PROVIDER::Handle_Fd_Event(int fd, boolean is_readable,
	boolean is_writable, boolean is_error) {}*/

void P1_PROVIDER::Handle_Fd_Event_Error(int /*fd*/)
{

}

void P1_PROVIDER::Handle_Fd_Event_Writable(int /*fd*/)
{

}

void P1_PROVIDER::Handle_Fd_Event_Readable(int /*fd*/)
{

}

/*void P1_PROVIDER::Handle_Timeout(double time_since_last_call) {}*/

void P1_PROVIDER::user_map(const char * /*system_port*/)
{
	if (user_start_ran == FALSE) {
		TTCN_error("user_start did not ran in P1");
	}
	if (port_param_set == FALSE) {
		TTCN_error("Port parameter is not set in P1");
	}
	user_map_ran++;
}

void P1_PROVIDER::user_unmap(const char * /*system_port*/)
{
	if (user_start_ran == FALSE) {
		TTCN_error("user_start did not ran in P1");
	}
	user_map_ran--;
	port_param_set = FALSE;
}

void P1_PROVIDER::user_start()
{
	if (user_start_ran == TRUE) {
		TTCN_error("user_start already ran in P1");
	}
	user_start_ran = TRUE;
}

void P1_PROVIDER::user_stop()
{
	if (user_start_ran == FALSE) {
		TTCN_error("user_start did not ran in P1");
	}
	user_start_ran = FALSE;
}

void P1_PROVIDER::outgoing_send(const MyRec& send_par)
{
	if (user_start_ran == FALSE) {
		TTCN_error("user_start did not ran in P1");
	}
	OCTETSTRING os = send_par.val();
	incoming_message(os);
}

void P1_PROVIDER::outgoing_send(const OCTETSTRING& send_par)
{
	if (user_start_ran == FALSE) {
		TTCN_error("user_start did not ran in P1");
	}
	if (user_map_ran <= 0) {
		TTCN_error("user_map did not ran in P1");
	}
	INTEGER integer = send_par.lengthof();
	incoming_message(integer);
}

void P1_PROVIDER::outgoing_send(const BITSTRING& send_par)
{
	if (user_start_ran == FALSE) {
		TTCN_error("user_start did not ran in P1");
	}
	if (user_map_ran <= 0) {
		TTCN_error("user_map did not ran in P1");
	}
	// Test that the receive mapping handles fragmented case
	if (send_par.lengthof() == 48) {
		for (int i = 0; i < 48; i++) {
			BITSTRING bs = send_par[i];
			incoming_message(bs);
		}
	} else {
		incoming_message(send_par);
	}
}

void P1_PROVIDER::outgoing_send(const CHARSTRING& send_par)
{
	if (user_start_ran == FALSE) {
		TTCN_error("user_start did not ran in P1");
	}
	if (user_map_ran <= 0) {
		TTCN_error("user_map did not ran in P1");
	}
	incoming_message(send_par);
}

void P1_PROVIDER::outgoing_send(const INTEGER& send_par)
{
	if (user_start_ran == FALSE) {
		TTCN_error("user_start did not ran in P1");
	}
	if (user_map_ran <= 0) {
		TTCN_error("user_map did not ran in P1");
	}
	incoming_message(send_par);
}

void P1_PROVIDER::outgoing_send(const HEXSTRING& send_par)
{
	if (user_start_ran == FALSE) {
		TTCN_error("user_start did not ran in P1");
	}
	if (user_map_ran <= 0) {
		TTCN_error("user_map did not ran in P1");
	}
	incoming_message(send_par);
}

} /* end of namespace */

