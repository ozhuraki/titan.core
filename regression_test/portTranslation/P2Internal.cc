/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Szabo, Bence Janos
 *
 ******************************************************************************/

#include "P2Internal.hh"
#include "PortTranslationInternal.hh"

namespace PortTranslationInternal {

P2Internal_PROVIDER::P2Internal_PROVIDER(const char *par_port_name)
	: PORT(par_port_name)
{

}

P2Internal_PROVIDER::~P2Internal_PROVIDER()
{

}

void P2Internal_PROVIDER::set_parameter(const char * /*parameter_name*/,
	const char * /*parameter_value*/)
{

}

/*void P2Internal_PROVIDER::Handle_Fd_Event(int fd, boolean is_readable,
	boolean is_writable, boolean is_error) {}*/

void P2Internal_PROVIDER::Handle_Fd_Event_Error(int /*fd*/)
{

}

void P2Internal_PROVIDER::Handle_Fd_Event_Writable(int /*fd*/)
{

}

void P2Internal_PROVIDER::Handle_Fd_Event_Readable(int /*fd*/)
{

}

/*void P2Internal_PROVIDER::Handle_Timeout(double time_since_last_call) {}*/

void P2Internal_PROVIDER::user_map(const char * /*system_port*/)
{

}

void P2Internal_PROVIDER::user_unmap(const char * /*system_port*/)
{

}

void P2Internal_PROVIDER::user_start()
{

}

void P2Internal_PROVIDER::user_stop()
{

}

void P2Internal_PROVIDER::outgoing_send(const OCTETSTRING& send_par)
{
	INTEGER integer = send_par.lengthof();
	incoming_message(integer);
}

void P2Internal_PROVIDER::outgoing_send(const MyRec& send_par)
{
	OCTETSTRING os = send_par.val();
	incoming_message(os);
}

void P2Internal_PROVIDER::outgoing_send(const BITSTRING& send_par)
{
	incoming_message(send_par);
}

void P2Internal_PROVIDER::outgoing_send(const CHARSTRING& send_par)
{
	incoming_message(send_par);
}

void P2Internal_PROVIDER::outgoing_send(const INTEGER& send_par)
{
	incoming_message(send_par);
}

} /* end of namespace */

