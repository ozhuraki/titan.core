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

#include "P5.hh"
#include "PortTranslation.hh"

namespace PortTranslation {

P5_PROVIDER::P5_PROVIDER(const char *par_port_name)
	: PORT(par_port_name)
{

}

P5_PROVIDER::~P5_PROVIDER()
{

}

void P5_PROVIDER::set_parameter(const char * /*parameter_name*/,
	const char * /*parameter_value*/)
{

}

/*void P5_PROVIDER::Handle_Fd_Event(int fd, boolean is_readable,
	boolean is_writable, boolean is_error) {}*/

void P5_PROVIDER::Handle_Fd_Event_Error(int /*fd*/)
{

}

void P5_PROVIDER::Handle_Fd_Event_Writable(int /*fd*/)
{

}

void P5_PROVIDER::Handle_Fd_Event_Readable(int /*fd*/)
{

}

/*void P5_PROVIDER::Handle_Timeout(double time_since_last_call) {}*/

void P5_PROVIDER::user_map(const char * /*system_port*/)
{

}

void P5_PROVIDER::user_unmap(const char * /*system_port*/)
{

}

void P5_PROVIDER::user_start()
{

}

void P5_PROVIDER::user_stop()
{

}

void P5_PROVIDER::outgoing_send(const CHARSTRING& /*send_par*/)
{
  
}

} /* end of namespace */

