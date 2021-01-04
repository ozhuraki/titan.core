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

#ifndef P1_HH
#define P1_HH

#include <TTCN3.hh>

// Note: Header file PortTranslation.hh must not be included into this file!
// (because it includes this file)
// Please add the declarations of message types manually.

namespace PortTranslation {

class MyRec;

class P1_PROVIDER : public PORT {
public:
	P1_PROVIDER(const char *par_port_name);
	~P1_PROVIDER();

	void set_parameter(const char *parameter_name,
		const char *parameter_value);

private:
	/* void Handle_Fd_Event(int fd, boolean is_readable,
		boolean is_writable, boolean is_error); */
	void Handle_Fd_Event_Error(int fd);
	void Handle_Fd_Event_Writable(int fd);
	void Handle_Fd_Event_Readable(int fd);
	/* void Handle_Timeout(double time_since_last_call); */

	int user_map_ran;
	boolean user_start_ran;
	boolean port_param_set;
protected:
	void user_map(const char *system_port);
	void user_unmap(const char *system_port);

	void user_start();
	void user_stop();

	void outgoing_send(const MyRec& send_par);
	void outgoing_send(const OCTETSTRING& send_par);
	void outgoing_send(const BITSTRING& send_par);
	void outgoing_send(const CHARSTRING& send_par);
	void outgoing_send(const INTEGER& send_par);
	void outgoing_send(const HEXSTRING& send_par);
	virtual void incoming_message(const INTEGER& incoming_par) = 0;
	virtual void incoming_message(const BITSTRING& incoming_par) = 0;
	virtual void incoming_message(const OCTETSTRING& incoming_par) = 0;
	virtual void incoming_message(const CHARSTRING& incoming_par) = 0;
	virtual void incoming_message(const HEXSTRING& incoming_par) = 0;
};

} /* end of namespace */

#endif
