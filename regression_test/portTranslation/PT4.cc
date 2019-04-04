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

#include "PT4.hh"

namespace PortTranslation {

PT4::PT4(const char *par_port_name)
	: PT4_BASE(par_port_name)
{

}

PT4::~PT4()
{

}

void PT4::set_parameter(const char * /*parameter_name*/,
	const char * /*parameter_value*/)
{

}

/*void PT4::Handle_Fd_Event(int fd, boolean is_readable,
	boolean is_writable, boolean is_error) {}*/

void PT4::Handle_Fd_Event_Error(int /*fd*/)
{

}

void PT4::Handle_Fd_Event_Writable(int /*fd*/)
{

}

void PT4::Handle_Fd_Event_Readable(int /*fd*/)
{

}

/*void PT4::Handle_Timeout(double time_since_last_call) {}*/

void PT4::user_map(const char * /*system_port*/)
{

}

void PT4::user_unmap(const char * /*system_port*/)
{

}

void PT4::user_start()
{

}

void PT4::user_stop()
{

}

void PT4::outgoing_send(const CHARSTRING& /*send_par*/)
{
  TTCN_Runtime::setverdict(FAIL, "The port mapping was not done in translation mode.");
}

} /* end of namespace */

