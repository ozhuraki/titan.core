/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               PIPEasp_PT.hh
//  Description:        Header file of PIPE testport implementation
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 334
//  Updated:            2012-01-02
//  Contact:            http://ttcn.ericsson.se
//


#ifndef PIPEasp__PT_HH
#define PIPEasp__PT_HH

#include "PIPEasp_PortType.hh"

namespace PIPEasp__PortType {

class PIPEasp__PT : public PIPEasp__PT_BASE {
public:
	PIPEasp__PT(const char *par_port_name = NULL);
	~PIPEasp__PT();

	void set_parameter(const char *parameter_name,
		const char *parameter_value);

	void Event_Handler(const fd_set *read_fds,
		const fd_set *write_fds, const fd_set *error_fds,
		double time_since_last_call);

protected:
	void user_map(const char *system_port);
	void user_unmap(const char *system_port);

	void user_start();
	void user_stop();

	void outgoing_send(const PIPEasp__Types::ASP__PExecute& send_par);
	void outgoing_send(const PIPEasp__Types::ASP__PExecutePty& send_par);
	void outgoing_send(const PIPEasp__Types::ASP__PExecuteBinary& send_par);
	void outgoing_send(const PIPEasp__Types::ASP__PExecuteBinaryPty& send_par);
	void outgoing_send(const PIPEasp__Types::ASP__PExecuteBackground& send_par);
	void outgoing_send(const PIPEasp__Types::ASP__PExecuteBackgroundPty& send_par);
	void outgoing_send(const PIPEasp__Types::ASP__PStdin& send_par);
	void outgoing_send(const PIPEasp__Types::ASP__PStdinBinary& send_par);
	void outgoing_send(const PIPEasp__Types::ASP__PKill& send_par);
	void outgoing_send(const PIPEasp__Types::ASP__PLineMode& send_par);
	void outgoing_send(const PIPEasp__Types::ASP__PEndOfInput& send_par);
private:
        int execCommand(const char* command);
        void handle_childDeath();
        void sendStdout();
        void sendStderr();
        void sendExitCode();
        void sendResult();
        void sendError(const char* error_msg);
        void log(const char *fmt, ...);
        void log_buffer(const unsigned char * buf, size_t buflen);
        int get_len(const TTCN_Buffer& buff);
        int put_msg(TTCN_Buffer& buff, int msg_type, int len, const unsigned char* msg);
        // Msg types
        // 1 - kill server
        // 2 - execute
        // 3 - stdin or pty
        // 4 - stdout
        // 5 - stderr 
        // 6 - execute finished
        // 7 - kill command
        // 8 - end of input
        // 9 - unmap
        // 10 - execute in pty mode
        
        // Msg struct
        // 1. byte - type
        // 2-3 byte - len
        // 4- byte - data optional
        void processHandler();
        void handle_childDeath_inprocess();
        void newcmd(const char* command);
        void newcmd_pty(const char* command);

private:
  bool lineMode; // true if lineMode is enabled
  bool processExecuting; // true if process is executing: disable new processes
  bool binaryMode; // true if result should be returned in as binary data
  bool disableSend; // if true sendStdout/err is disabled

  fd_set readfds;     // fd set for event handler
  int processPid;     // pid of the process currently executing
  int processServerPid;
  int processServerUp;
  int processServerDown;
  TTCN_Buffer process_buffer;
  int processStdin;   // fd of stdin of the process
  int processStdout;  // fd of stdout of the process
  int processStderr;  // fd of stderr of the process

  bool ptyMode;       // pty mode
  int processPty;     // pty of the process (in pty mode)

  TTCN_Buffer stdout_buffer; // data sent to stdout 
  TTCN_Buffer stderr_buffer; // data sent to stderr
  int processExitCode;       // exit code of the process
  bool debugAllowed; 

};

}//namespace

#endif
