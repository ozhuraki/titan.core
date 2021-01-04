/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Botond, Baranyi
 *
 ******************************************************************************/

#include "PT3.hh"

namespace PortTranslation {

PT3::PT3(const char *par_port_name)
	: PT3_BASE(par_port_name)
{

}

PT3::~PT3()
{

}

void PT3::set_parameter(const char * /*parameter_name*/,
	const char * /*parameter_value*/)
{

}

/*void PT3::Handle_Fd_Event(int fd, boolean is_readable,
	boolean is_writable, boolean is_error) {}*/

void PT3::Handle_Fd_Event_Error(int /*fd*/)
{

}

void PT3::Handle_Fd_Event_Writable(int /*fd*/)
{

}

void PT3::Handle_Fd_Event_Readable(int /*fd*/)
{

}

/*void PT3::Handle_Timeout(double time_since_last_call) {}*/

void PT3::user_map(const char * /*system_port*/)
{

}

void PT3::user_unmap(const char * /*system_port*/)
{

}

void PT3::user_start()
{

}

void PT3::user_stop()
{

}

void PT3::outgoing_send(const INTEGER& /*send_par*/)
{

}

void PT3::outgoing_send(const CHARSTRING& /*send_par*/)
{

}

void PT3::outgoing_send(const BOOLEAN& /*send_par*/)
{

}

} /* end of namespace */

