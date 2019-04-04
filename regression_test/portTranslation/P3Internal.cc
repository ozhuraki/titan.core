/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Szabo, Bence Janos
 *
 ******************************************************************************/

#include "P3Internal.hh"

namespace PortTranslationInternal {

P3Internal::P3Internal(const char *par_port_name)
	: P3Internal_BASE(par_port_name)
{

}

P3Internal::~P3Internal()
{

}

void P3Internal::set_parameter(const char * /*parameter_name*/,
	const char * /*parameter_value*/)
{

}

/*void P3Internal::Handle_Fd_Event(int fd, boolean is_readable,
	boolean is_writable, boolean is_error) {}*/

void P3Internal::Handle_Fd_Event_Error(int /*fd*/)
{

}

void P3Internal::Handle_Fd_Event_Writable(int /*fd*/)
{

}

void P3Internal::Handle_Fd_Event_Readable(int /*fd*/)
{

}

/*void P3Internal::Handle_Timeout(double time_since_last_call) {}*/

void P3Internal::user_map(const char * /*system_port*/)
{

}

void P3Internal::user_unmap(const char * /*system_port*/)
{

}

void P3Internal::user_start()
{

}

void P3Internal::user_stop()
{

}

void P3Internal::outgoing_send(const MyRec& send_par)
{
	OCTETSTRING os = send_par.val();
	incoming_message(os);
}

void P3Internal::outgoing_send(const BITSTRING& /*send_par*/)
{

}

void P3Internal::outgoing_send(const OCTETSTRING& /*send_par*/)
{

}

void P3Internal::outgoing_send(const HEXSTRING& /*send_par*/)
{

}

void P3Internal::outgoing_send(const INTEGER& /*send_par*/)
{

}

} /* end of namespace */

