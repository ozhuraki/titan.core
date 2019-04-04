/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Botond, Baranyi
 *
 ******************************************************************************/

#include "P4.hh"
#include "PortTranslation.hh"

namespace PortTranslation {

P4_PROVIDER::P4_PROVIDER(const char *par_port_name)
	: PORT(par_port_name)
{

}

P4_PROVIDER::~P4_PROVIDER()
{

}

void P4_PROVIDER::set_parameter(const char * /*parameter_name*/,
	const char * /*parameter_value*/)
{

}

/*void P4_PROVIDER::Handle_Fd_Event(int fd, boolean is_readable,
	boolean is_writable, boolean is_error) {}*/

void P4_PROVIDER::Handle_Fd_Event_Error(int /*fd*/)
{

}

void P4_PROVIDER::Handle_Fd_Event_Writable(int /*fd*/)
{

}

void P4_PROVIDER::Handle_Fd_Event_Readable(int /*fd*/)
{

}

/*void P4_PROVIDER::Handle_Timeout(double time_since_last_call) {}*/

void P4_PROVIDER::user_map(const char * /*system_port*/)
{

}

void P4_PROVIDER::user_unmap(const char * /*system_port*/)
{

}

void P4_PROVIDER::user_start()
{

}

void P4_PROVIDER::user_stop()
{

}

void P4_PROVIDER::outgoing_send(const INTEGER& send_par)
{
  // redirect the message back to the port, to test port translation for 'receive' operations
  // (these messages should be discarded by the translation function)
  incoming_message(send_par);
}

void P4_PROVIDER::outgoing_send(const CHARSTRING& send_par)
{
  // redirect the message back to the port, to test port translation for 'receive' operations
  // (these messages should be correctly translated and received)
  incoming_message(send_par);
}

void P4_PROVIDER::outgoing_send(const BOOLEAN& send_par)
{
  // this should've been discarded by the translation function
  TTCN_Runtime::setverdict(FAIL, "Sending a boolean value.");
}

} /* end of namespace */

