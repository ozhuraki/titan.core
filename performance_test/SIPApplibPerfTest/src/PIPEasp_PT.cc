/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************///
//  File:               PIPEasp_PT.cc
//  Description:        Source code of PIPE testport implementation
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 334
//  Updated:            2012-01-02
//  Contact:            http://ttcn.ericsson.se
//


#include "PIPEasp_PT.hh"
#include <signal.h> //kill
#include <unistd.h> //pipe
#include <errno.h>  //errno
#include <ctype.h>  //isspace
#include <sys/select.h>  //FD_ZERO
#include <stdio.h>      // sys_errlist
#include <sys/types.h>  //wait
#include <sys/socket.h>  //wait
#include <sys/wait.h>   //wait
#include <termios.h>

#ifndef PIPE_BUF_SIZE
#define PIPE_BUF_SIZE 65534
#endif

#if defined LINUX || defined FREEBSD || defined WIN32
#include <pty.h>
#endif

#if defined SOLARIS || defined SOLARIS8
#include <strings.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stream.h>
#include <sys/stropts.h>
/////////////////////////////////////

int forkpty_solaris (int *amaster, char *name, struct termios
         *termp, struct winsize *winp)
{
    int fdm, fds;
    char *slavename;
    pid_t pid;
    fdm = open("/dev/ptmx", O_RDWR);  /* open master */
    grantpt(fdm);                     /* change permission of slave */
    unlockpt(fdm);                    /* unlock slave */
    slavename = ptsname(fdm);         /* get name of slave */
    if (name) strcpy(name, slavename);
    *amaster = fdm;
    if ((pid = fork()) < 0) {
    return pid;         /* error */
    }
    else if (pid != 0) {        /* parent */
    return pid;
    }
    else {            /* child */
    pid_t pgid;
    /* create a new session */
    pgid = setsid();
    if (pgid == -1) {
        perror("forkpty_solaris() setsid failed");
        return -1;
    }
    fds = open(slavename, O_RDWR);    /* open slave */
    ioctl(fds, I_PUSH, "ptem");       /* push ptem */
    ioctl(fds, I_PUSH, "ldterm");    /* push ldterm */
    dup2(fds, 0);
    dup2(fds, 1);
    dup2(fds, 2);
    ioctl(fds, TIOCSPGRP, &pgid);
    /* magic */
    if (termp)
        ioctl(fds, TCSETS, termp);
    if (winp)
        ioctl(fds, TIOCSWINSZ, winp);
    return pid;
    }
}

/////////////////////////////////////
#define forkpty forkpty_solaris
#endif

namespace PIPEasp__PortType {

PIPEasp__PT::PIPEasp__PT(const char *par_port_name)
	: PIPEasp__PT_BASE(par_port_name)
        , lineMode(true)
        , processExecuting(false)
        , binaryMode(false)
        , disableSend(false)
        , processPid(-1)     // pid of the process currently executing
        , processStdin(-1)   // fd of stdin of the process
        , processStdout(-1)  // fd of stdout of the process
        , processStderr(-1)  // fd of stderr of the process
        , ptyMode(false)     // pty mode (= opposite of stdio mode)
        , processPty(-1)     // fd of process pty (in pty mode)
        , processExitCode(0) // exit code of the process
        , debugAllowed(false) // debug disabled by default
{
  FD_ZERO(&readfds);
  stdout_buffer.clear();
  stderr_buffer.clear();
  process_buffer.clear();
  processServerPid=-1;
  

}

PIPEasp__PT::~PIPEasp__PT()
{
//printf("processServerPid %d\r\n",processServerPid);
  if(processServerPid>0){
    char kill_msg[3];
    kill_msg[0]=1;
    kill_msg[1]=0;
    kill_msg[2]=0;
    
//    write(processServerDown,kill_msg,3);
    close(processServerDown);
    processExitCode = 0;  // reset the exitcode
    wait(&processExitCode);
  }
}

void PIPEasp__PT::set_parameter(const char *parameter_name,
	const char *parameter_value)
{
  if (!strcmp(parameter_name, "debug")) {
    if (!strcasecmp(parameter_value,"YES")) {
      debugAllowed = true;
    }
  }
}

void PIPEasp__PT::Event_Handler(const fd_set *read_fds,
	const fd_set *write_fds, const fd_set *error_fds,
	double time_since_last_call)
{
  log("PIPEasp__PT::Event_Handler called");
  
  long nBytes;
  int r;

  nBytes = PIPE_BUF_SIZE;
  unsigned char* buffer;
  size_t end_len = nBytes;
  process_buffer.get_end(buffer, end_len);
  r = read(processServerUp,buffer,(int)nBytes);
  log("read returned %d",r);
  if (r <= 0) {
      log("ttcn_pipe_port: read problem from server process. %d, %d, %s\n", r, errno, strerror(errno));
      close(processServerUp);
      close(processServerDown);
      Uninstall_Handler();
      int status=0;
      wait(&status);
      processServerPid=-1;
  }
  else {
    process_buffer.increase_length(r);
    int msg_len;
    int buff_len=process_buffer.get_len();
    const unsigned char* buff_data=process_buffer.get_data();

//     log("process buffer length %d",buff_len);
//      if(buff_len>=3) {
//    	  const unsigned char* data=process_buffer.get_data();
//    	  log("buffer data %02x %02x %02x", data[0],data[1],data[2]);
//      }
    
    log_buffer(buff_data,buff_len);

    while((msg_len=get_len(process_buffer))!=-1){
      const unsigned char* msg=process_buffer.get_data();

      switch(msg[0]){
        case 4:
          if (!processExecuting) {
            TTCN_warning("Unexpected message from stdout, no command is executing");
          } else {
            log("Incoming stdout message received from process: %d",msg_len);
          }
          stdout_buffer.put_s(msg_len,msg+3);
          sendStdout();
          break;
        case 5:
          if (!processExecuting) {
            TTCN_warning("Unexpected message from stderr, no command is executing");
          } else {
            log("Incoming stderr message received from process");
          }
          stderr_buffer.put_s(msg_len,msg+3);
          sendStderr();
          break;
        case 6:
          processExitCode=((int)msg[3]<<24) + ((int)msg[4]<<16) + ((int)msg[5]<<8) + (int)msg[6];
          handle_childDeath();
          break;
        default:
          break;
      }
      process_buffer.set_pos(3+msg_len);
      process_buffer.cut();
      buff_len=process_buffer.get_len();
      buff_data=process_buffer.get_data();

//     log("process buffer length %d",buff_len);
//      if(buff_len>=3) {
//    	  const unsigned char* data=process_buffer.get_data();
//    	  log("buffer data %02x %02x %02x", data[0],data[1],data[2]);
//      }

      log_buffer(buff_data,buff_len);

		}
	  }
	}

void PIPEasp__PT::user_map(const char *system_port)
{
    FD_ZERO(&readfds);
    FD_SET(processServerUp, &readfds);
    Install_Handler(&readfds, NULL, NULL, 0.0);
}

void PIPEasp__PT::user_unmap(const char *system_port)
{
    log("user_unmap started");

    FD_ZERO(&readfds);
    Uninstall_Handler();
    log("Sending unmap to processServer");
    TTCN_Buffer out_buff;
    out_buff.clear();
    stdout_buffer.clear(); 
    stderr_buffer.clear(); 
    process_buffer.clear(); 

    int write_len=put_msg(out_buff,9,0,NULL); // Sending unmap to processServer
    write(processServerDown,out_buff.get_data(),write_len);

    processExecuting = false;
    disableSend = false;

    log("user_unmap finsihed");

}

void PIPEasp__PT::user_start()
{
	  if(processServerPid != -1) return;
	  int pipefd[2];
	  int pipefd2[2];
	  if (pipe(pipefd) != 0) {
	      return;
	  }
	  if (pipe(pipefd2) != 0) {
	      return;
	  }
	  processServerPid=fork();
	  if (processServerPid < 0) {
	    //
	    // Error
	    //

	    // close the pipes
	    close(pipefd[0]);
	    close(pipefd[1]);
	    close(pipefd2[0]);
	    close(pipefd2[1]);
	    processServerUp=-1;
	    processServerDown=-1;
	  }
	  else if (processServerPid == 0) {

	      //
	      // Child process
	      //

	      // close the parent end of the pipes
	      processServerUp=pipefd2[1];
	      processServerDown=pipefd[0];
	      close(pipefd[1]);
	      close(pipefd2[0]);
	      processHandler();
	//printf("exit\r\n");
	//throw TC_Error();
	      _exit(0); // end of child process
	  }
	  else {

	    //
	    // Parent process
	    //

	    //printf("Process started with pid: %d\r\n", processServerPid);
	    // close child end of the pipes
	      processServerUp=pipefd2[0];
	      processServerDown=pipefd[1];
	    close(pipefd[0]);
	    close(pipefd2[1]);
	 }


}

void PIPEasp__PT::user_stop()
{
}

/*************************************
*  Specific outgoing_send functions
*************************************/
void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PExecute& send_par) {
  log("PIPEasp__PT::outgoing_send_PExecute called");
  // disable sendStdout, sendStderr until process exits
  if (processExecuting) {
    sendError("Pipe Test Port: Command already executing");
    if(debugAllowed)
    {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("PIPE test port (%s): Command already executing. Following ASP is ignored: ", get_name());
    send_par.log();
    TTCN_Logger::end_event();
    }
    return;
  }
  PIPEasp__Types::ASP__PExecuteBackground message_PExecuteBackground;
  // starting command
  message_PExecuteBackground.command() = send_par.command();
  outgoing_send(message_PExecuteBackground);
  // sending input
  PIPEasp__Types::ASP__PStdin message_PStdin;
  message_PStdin.stdin_() = send_par.stdin_();
  outgoing_send(message_PStdin);
  disableSend = true;

  // closing stdin pipe:
  outgoing_send(PIPEasp__Types::ASP__PEndOfInput());
  
  log("PIPEasp__PT::outgoing_send_PExecute exited");
}

void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PExecutePty& send_par) {
  log("PIPEasp__PT::outgoing_send_PExecutePty called");
  // disable sendStdout, sendStderr until process exits
  if (processExecuting) {
    sendError("Pipe Test Port: Command already executing");
    if(debugAllowed)
    {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("PIPE test port (%s): Command already executing. Following ASP is ignored: ", get_name());
    send_par.log();
    TTCN_Logger::end_event();
    }
    return;
  }
  PIPEasp__Types::ASP__PExecuteBackgroundPty message_PExecuteBackgroundPty;
  // starting command
  message_PExecuteBackgroundPty.command() = send_par.command();
  outgoing_send(message_PExecuteBackgroundPty);
  // sending input
  PIPEasp__Types::ASP__PStdin message_PStdin;
  message_PStdin.stdin_() = send_par.stdin_();
  outgoing_send(message_PStdin);
  disableSend = true;

  // closing stdin pipe:
  outgoing_send(PIPEasp__Types::ASP__PEndOfInput());
  log("PIPEasp__PT::outgoing_send_PExecutePty exited");
}

void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PExecuteBinary& send_par) {
  log("PIPEasp__PT::outgoing_send_PExecuteBinary called");
  // disable sendStdout, sendStderr until process exits
  if (processExecuting) {
    sendError("Pipe Test Port: Command already executing");
    if(debugAllowed)
    {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("PIPE test port (%s): Command already executing. Following ASP is ignored: ", get_name());
    send_par.log();
    TTCN_Logger::end_event();
    }
    return;
  }
  PIPEasp__Types::ASP__PExecuteBackground message_PExecuteBackground;
  // starting command
  message_PExecuteBackground.command() = send_par.command();
  outgoing_send(message_PExecuteBackground);
  // sending input
  PIPEasp__Types::ASP__PStdinBinary message_PStdinBinary;
  message_PStdinBinary.stdin_() = send_par.stdin_();
  outgoing_send(message_PStdinBinary);
  disableSend = true;
  
  // closing stdin pipe:
  outgoing_send(PIPEasp__Types::ASP__PEndOfInput());
  
  log("PIPEasp__PT::outgoing_send_PExecuteBinary exited");
}

void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PExecuteBinaryPty& send_par) {
  log("PIPEasp__PT::outgoing_send_PExecuteBinaryPty called");
  // disable sendStdout, sendStderr until process exits
  if (processExecuting) {
    sendError("Pipe Test Port: Command already executing");
    if(debugAllowed)
    {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("PIPE test port (%s): Command already executing. Following ASP is ignored: ", get_name());
    send_par.log();
    TTCN_Logger::end_event();
    }
    return;
  }
  PIPEasp__Types::ASP__PExecuteBackgroundPty message_PExecuteBackgroundPty;
  // starting command
  message_PExecuteBackgroundPty.command() = send_par.command();
  outgoing_send(message_PExecuteBackgroundPty);
  // sending input
  PIPEasp__Types::ASP__PStdinBinary message_PStdinBinary;
  message_PStdinBinary.stdin_() = send_par.stdin_();
  outgoing_send(message_PStdinBinary);
  disableSend = true;

  // closing stdin pipe:
  outgoing_send(PIPEasp__Types::ASP__PEndOfInput());

  log("PIPEasp__PT::outgoing_send_PExecuteBinaryPty exited");
}

void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PExecuteBackground& send_par) {
  log("PIPEasp__PT::outgoing_send_PExecuteBackground called");
  
  if (processExecuting) {
    log("Process already executing. Cannot start new process.");
    sendError("Pipe Test Port: Command already executing");
    if(debugAllowed)
    {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("PIPE test port (%s): Command already executing. Following ASP is ignored: ", get_name());
    send_par.log();
    TTCN_Logger::end_event();
    log("PIPEasp__PT::outgoing_send_PExecuteBackground exited");
    }
    return;
  }

  TTCN_Buffer out_buff;
  out_buff.clear();
  int write_len=put_msg(out_buff,2,send_par.command().lengthof(),(const unsigned char*)(const char*)send_par.command());
  write(processServerDown,out_buff.get_data(),write_len);
  processExecuting=true;
  log("PIPEasp__PT::outgoing_send_PExecuteBackground exited");
}

void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PExecuteBackgroundPty& send_par) {
  log("PIPEasp__PT::outgoing_send_PExecuteBackgroundPty called");
  
  if (processExecuting) {
    log("Process already executing. Cannot start new process.");
    sendError("Pipe Test Port: Command already executing");
    if(debugAllowed)
    {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("PIPE test port (%s): Command already executing. Following ASP is ignored: ", get_name());
    send_par.log();
    TTCN_Logger::end_event();
    log("PIPEasp__PT::outgoing_send_PExecuteBackgroundPty exited");
    }
    return;
  }

  TTCN_Buffer out_buff;
  out_buff.clear();
  int write_len=put_msg(out_buff,10,send_par.command().lengthof(),(const unsigned char*)(const char*)send_par.command());
  write(processServerDown,out_buff.get_data(),write_len);
  processExecuting=true;
  log("PIPEasp__PT::outgoing_send_PExecuteBackgroundPty exited");
}

void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PStdin& send_par) {
  log("PIPEasp__PT::outgoing_send_PStdin called");
  binaryMode = false;
  if (!processExecuting) {  
    sendError("Pipe Test Port: No command executing");
    return;
  }
  if (disableSend) {  // process was started with PExecute(Binary)
    sendError("Pipe Test Port: PStdin is not sent: current process is not started with PExecuteBackground!");
    return;
  }

  log("will now write to stdin: '%s'",
		 (const char*)(send_par.stdin_()+((lineMode)?"\n":"")));

  TTCN_Buffer out_buff;
  out_buff.clear();
  int write_len=put_msg(out_buff,3,send_par.stdin_().lengthof()+((lineMode)?1:0),
    (const unsigned char*)(const char*)(send_par.stdin_()+((lineMode)?"\n":"")));
  write(processServerDown,out_buff.get_data(),write_len);
  
  log("PIPEasp__PT::outgoing_send_PStdin exited");
}

void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PStdinBinary& send_par) {
  log("PIPEasp__PT::outgoing_send_PStdinBinary called");
  binaryMode = true;
  if (!processExecuting) {  
    sendError("Pipe Test Port: No command executing");
    return;
  }
  if (disableSend) {  // process was started with PExecute(Binary)
    sendError("Pipe Test Port: PStdinBinary is not sent: current process is not started with PExecuteBackground!");
    return;
  }

  if(debugAllowed)
  {
  TTCN_Logger::begin_event(TTCN_DEBUG);
  TTCN_Logger::log_event("PIPE test port (%s): will now write binary data to stdin: ", get_name());
  send_par.stdin_().log();
  TTCN_Logger::end_event();
  }

  TTCN_Buffer out_buff;
  out_buff.clear();
  int write_len=put_msg(out_buff,3,send_par.stdin_().lengthof(),
    (const unsigned char*)(send_par.stdin_()));
  write(processServerDown,out_buff.get_data(),write_len);

  log("PIPEasp__PT::outgoing_send_PStdinBinary exited");
}

void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PKill& send_par) {
  log("PIPEasp__PT::outgoing_send_PKill called");
  if (!processExecuting) {
    // no process is running
    log("No process executing.");
    sendError("Pipe Test Port: No command executing");
    log("PIPEasp__PT::outgoing_send_PKill exited");
    return;
  }
  
  int signo = (int)send_par.signal();
  if (signo<1 || signo>31) {
    // signo out of range;
    log("Signo out of range.");
    sendError(
      "Pipe Test port: Signal number should be "
      "between 1 and 31");
    log("PIPEasp__PT::outgoing_send_PKill exited");
    return;
  }
  // killing process
  log("Killing process with signo: %d", signo);
  unsigned char msg[4];
  msg[0]=7;
  msg[1]=0;
  msg[2]=1;
  msg[3]=signo;
  write(processServerDown,msg,4);
  log("PIPEasp__PT::outgoing_send_PKill exited");
}

void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PLineMode& send_par) {
  log("PIPEasp__PT::outgoing_send_PLineMode called");
  lineMode = (bool)send_par.lineMode();
  log("LineMode is set to %s", (lineMode)?"TRUE":"FALSE");
  log("PIPEasp__PT::outgoing_send_PLineMode exited");
}

void PIPEasp__PT::outgoing_send(const PIPEasp__Types::ASP__PEndOfInput& send_par) {
  log("PIPEasp__PT::outgoing_send_PEndOfInput called");
  // close stdin pipe
  unsigned char msg[3];
  msg[0]=8;
  msg[1]=0;
  msg[2]=0;
  write(processServerDown,msg,3);
  log("stdin closed");
  log("PIPEasp__PT::outgoing_send_PEndOfInput exited");
}

/********************************
* Execute the given command
* returns the exitcode of the process
*********************************/
int PIPEasp__PT::execCommand(const char* command) {
//  log("PIPEasp__PT::execCommand called");
//  log("Executing command: %s", command);

  // with this it is not possible to access the pid of the process
  //return system(command);

  int argc = 0;
  char* argv[1024];

  CHARSTRING temp = "";
  for (int i = 0; command[i] != 0; i++) {
      if (isspace(command[i])) {
	  argv[argc++] = strdup(temp);
//          log("command argument added: %s", (const char*)temp);
	  while (command[i] != '0' && isspace(command[i])) i++;
	  i--;
	  temp = "";
      } else {
	  temp = temp + CHARSTRING(1, command+i);
      }
  }

  if (temp != "") {
      argv[argc++] = strdup(temp);
//      log("command argument added: %s", (const char*)temp);
  }

  argv[argc++] = (char*)NULL;

//  log("execCommand(%s,%d)\n", argv[0], argc);
  execvp(argv[0],argv);

//  f//printf(stderr,"Error executing command %s (%d): %s\n",
//    argv[0], errno, strerror(errno));
//  fflush(stderr);
//  exit(errno);
  return errno;
}

/***********************************
* if the the child process died, gets
* the exit code and sends it to TTCN
* should be called when stdout/err are closed
************************************/
void PIPEasp__PT::handle_childDeath() {
  log("Child process exit status is: %d", processExitCode);
  // send code to TTCN:
  sendExitCode();
  // send result to TTCN
  sendResult();

  processExecuting = false;
  disableSend = false;
}

void PIPEasp__PT::handle_childDeath_inprocess() {

  processExitCode = 0;  // reset the exitcode

  int pid = wait(&processExitCode);
  //waitpid(processPid,&processExitCode, 0);
  //printf("processExitCode %d\n",processExitCode);
  if (pid!=processPid) {
    return;
  }

  if (!ptyMode) {
      if(processStdin!=-1){
          close(processStdin);
          processStdin=-1;
      }
  } else {
      if(processPty!=-1){
          close(processPty);
          processPty=-1;
      }
  }
  // send code to TTCN:
  unsigned char msg[7];
  msg[0]=6;
  msg[1]=0;
  msg[2]=4;
  msg[3]=(processExitCode>>24)&0x0F;
  msg[4]=(processExitCode>>16)&0x0F;
  msg[5]=(processExitCode>>8)&0x0F;
  msg[6]=processExitCode&0x0F;
  
  write(processServerUp,msg,7);
//printf("write\n");

  processExecuting = false;
  disableSend = false;
}


/***************************
* Send stdout msg to TTCN
***************************/
void PIPEasp__PT::sendStdout() {
  if (disableSend) return;
  

  PIPEasp__Types::ASP__PStdout message_PStdout;
  PIPEasp__Types::ASP__PStdoutBinary message_PStdoutBinary;
  if (lineMode && !binaryMode) {
    // send complete lines from buffer
    const unsigned char* pos = stdout_buffer.get_read_data();
    for(unsigned int i=0; i<stdout_buffer.get_read_len(); i++) {
      // not end of line:
      if (pos[i] != '\n') {
        continue;
      }
      
      // at end of line
      // length of data is i (+1 is for \n and is not sent)
      message_PStdout.stdout_() = CHARSTRING(i, (const char*)pos);
      
      // send message
      incoming_message(message_PStdout);
      
      // remove the complete line from buffer,
      // also set i and pos to the beginning of buffer
      stdout_buffer.set_pos(i+1);
      stdout_buffer.cut();
      i = 0;
      pos = stdout_buffer.get_read_data();
    }
  } else {
    // lineMode false or binaryMode true
    if (binaryMode) {
      message_PStdoutBinary.stdout_() =
        OCTETSTRING(stdout_buffer.get_read_len(), stdout_buffer.get_read_data());
      stdout_buffer.clear();
      incoming_message(message_PStdoutBinary);
    }
    else {
      message_PStdout.stdout_() = 
        CHARSTRING(stdout_buffer.get_read_len(), (const char*)stdout_buffer.get_read_data());
      stdout_buffer.clear();
      incoming_message(message_PStdout);
    }
//    incoming_message(message);
  }
}


/***************************
* Send stderr msg to TTCN
***************************/
void PIPEasp__PT::sendStderr() {
  if (disableSend) return;

  PIPEasp__Types::ASP__PStderr message_PStderr;
  PIPEasp__Types::ASP__PStderrBinary message_PStderrBinary;

  if (lineMode && !binaryMode) {
    // send complete lines from buffer
    const unsigned char* pos = stderr_buffer.get_read_data();
    for(unsigned int i=0; i<stderr_buffer.get_read_len(); i++) {
      // not end of line:
      if (pos[i] != '\n') {
        continue;
      }
      
      // at end of line
      // length of data is i (+1 is for \n and is not sent)
      message_PStderr.stderr_() = CHARSTRING(i, (const char*)pos);
      
      // send message
      incoming_message(message_PStderr);
      // remove the complete line from buffer,
      // also set i and pos to the beginning of buffer
      stderr_buffer.set_pos(i+1);
      stderr_buffer.cut();
      i = 0;
      pos = stderr_buffer.get_read_data();
    }
  } else {
    // lineMode false or binaryMode true
    if (binaryMode) {
      message_PStderrBinary.stderr_() =
        OCTETSTRING(stderr_buffer.get_read_len(), stderr_buffer.get_read_data());
      stderr_buffer.clear();
      incoming_message(message_PStderrBinary);
    }
    else {
      message_PStderr.stderr_() = 
        CHARSTRING(stderr_buffer.get_read_len(), (const char*)stderr_buffer.get_read_data());
      stderr_buffer.clear();
      incoming_message(message_PStderr);
    }
//    incoming_message(message);
  }
}


/***************************
* Send exitcode msg to TTCN
***************************/
void PIPEasp__PT::sendExitCode() {
  if (disableSend) return;

  log("Sending ExitCode to TTCN");
  PIPEasp__Types::ASP__PExit message_PExit;
  message_PExit.code() = processExitCode;
  incoming_message(message_PExit);
}


/***************************
* Send error msg to TTCN
***************************/
void PIPEasp__PT::sendError(const char* error_msg) {
  PIPEasp__Types::ASP__PError message_PError;
  message_PError.errorMessage() = error_msg;
  incoming_message(message_PError);
}


/***************************
* Send Result msg to TTCN
***************************/
void PIPEasp__PT::sendResult() {
  if (!disableSend) return; // do not send result if process was started by PExecuteBackground
  
  log("Sending result to TTCN...");
  PIPEasp__Types::ASP__PResult message_PResult;
  PIPEasp__Types::ASP__PResultBinary message_PResultBinary;
  if (binaryMode) {
    message_PResultBinary.stdout_() =
     OCTETSTRING(stdout_buffer.get_read_len(), stdout_buffer.get_read_data());
    message_PResultBinary.stderr_() =
     OCTETSTRING(stderr_buffer.get_read_len(), stderr_buffer.get_read_data());
    message_PResultBinary.code() = processExitCode;
    incoming_message(message_PResultBinary);
  } else {
    int messageLen = stdout_buffer.get_read_len();
    const char* messageData = (const char*)stdout_buffer.get_read_data();
    
    if (messageLen>0 && lineMode && messageData[messageLen-1]=='\n') {
      messageLen--; // remove newline from the end
    }
    
    message_PResult.stdout_() = CHARSTRING(messageLen, messageData);

    messageLen = stderr_buffer.get_read_len();
    messageData = (const char*)stderr_buffer.get_read_data();
    
    if (messageLen>0 && lineMode && messageData[messageLen-1]=='\n') {
      messageLen--; // remove newline from the end
    }
    
    message_PResult.stderr_() = CHARSTRING(messageLen, messageData);
    message_PResult.code() = processExitCode;
    incoming_message(message_PResult);
  }

  // clearing the buffers
  stdout_buffer.clear();
  stderr_buffer.clear();
  //incoming_message(message);
}


////////////////
// Log function
////////////////
void PIPEasp__PT::log(const char *fmt, ...)
{ 
  if(debugAllowed)
  {
  TTCN_Logger::begin_event(TTCN_DEBUG);
  TTCN_Logger::log_event("PIPE test port (%s): ", get_name());
  va_list ap;
  va_start(ap, fmt);
  TTCN_Logger::log_event_va_list(fmt, ap);
  va_end(ap);
  TTCN_Logger::end_event();
  }
}


void PIPEasp__PT::log_buffer(const unsigned char * buf, size_t buflen)
{
 if(debugAllowed)
 {
    TTCN_logger.log(TTCN_DEBUG,"------------------------ PIPE test port (%s) Logging process buffer -----------------------------\n", get_name());

    TTCN_logger.log(TTCN_DEBUG,"Process buffer in hexstring format:\n");
    for(size_t j=0;j<buflen;) {
	TTCN_Logger::begin_event(TTCN_DEBUG);
	for(size_t i=0;j<buflen && i<16;i++) {
	    if(i == 8) TTCN_Logger::log_event("  ");
	    TTCN_Logger::log_event("%02x ", buf[j++]);
	}
	TTCN_Logger::end_event();
    }
    
    CHARSTRING cc = CHARSTRING((int)buflen, (const char*)buf);
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("Process buffer in charstring format:\n");
    cc.log();
    TTCN_Logger::end_event();

    TTCN_logger.log(TTCN_DEBUG,"------------------------ End logging -----------------------------\n");
 }
}



int PIPEasp__PT::get_len(const TTCN_Buffer& buff){
  int buff_len=buff.get_len();
  if(buff_len<3) return -1; // not enough data in buffer
  const unsigned char* data=buff.get_data();
  int msg_len=(data[1]<<8)+data[2];
//printf("get_len,%d, %d, %d,%d,%d\n",buff_len, data[0], msg_len, data[1], data[2]);
  if(buff_len<(3+msg_len)) return -1; // partial message
  return msg_len;
}

int PIPEasp__PT::put_msg(TTCN_Buffer& buff, int msg_type, int len, const unsigned char* msg){
  buff.put_c(msg_type);
  int whole_len=len+3;
  buff.put_c((len>>8)&0xFF);
  buff.put_c(len&0xFF);
  buff.put_s(len,msg);
  return whole_len;
}

void PIPEasp__PT::processHandler(){
  
  bool executing=true;
  int nBytes;
  int r;
//printf("PIPEasp__PT::processHandler() started\r\n");
  while(executing){
//printf("while(executing)\r\n");
    int max_fd;
    FD_ZERO(&readfds);
    FD_SET(processServerDown, &readfds);
    max_fd=processServerDown;
    if(processStdout!=-1){
      FD_SET(processStdout, &readfds);
      max_fd=max_fd>processStdout?max_fd:processStdout;
    }
    if(processStderr!=-1){
      FD_SET(processStderr, &readfds);
      max_fd=max_fd>processStderr?max_fd:processStderr;
    }
    if(processPty!=-1){
      FD_SET(processPty, &readfds);
      max_fd=max_fd>processPty?max_fd:processPty;
    }
//printf("call select, %d\r\n",r);
    int result = select(max_fd+1,&readfds,NULL,NULL,NULL);
//printf("result = select(max_fd+1,&readfds,NULL,NULL,NULL), %d,%d,%s\r\n",result,errno, strerror(errno));
    if(result<0){

      if(errno != EINTR) {
        close(processServerDown);
        close(processServerUp);
        return;
      }
    } else {
      if(FD_ISSET(processServerDown,&readfds)){
        nBytes = PIPE_BUF_SIZE;
        unsigned char* buffer;
        size_t end_len = nBytes;
        process_buffer.get_end(buffer, end_len);
        r = read(processServerDown,buffer,(int)nBytes);
//printf("messages read from ttcn, %d\r\n",r);
        if(r>0){
          process_buffer.increase_length(r);
          int msg_len;
          int sig_no;
          int a;
          while((msg_len=get_len(process_buffer))!=-1){
            const unsigned char* msg=process_buffer.get_data();
 //printf("messages from ttcn, %d, %d,%d,%d\n", msg[0], msg_len, msg[1], msg[2]);
            switch(msg[0]){
              case 1: // kill the server
                close(processServerDown);
                close(processServerUp);
                return;
                break;
              case 2: // execute command
                  ptyMode = false;
                 newcmd(CHARSTRING(msg_len,(const char*)msg+3));
                 break;
              case 10: // execute command in pty mode
                  ptyMode = true;
                 newcmd_pty(CHARSTRING(msg_len,(const char*)msg+3));
                 break;
              case 3: // stdin or pty
                  if (!ptyMode) {
                      if(processStdin!=-1){
                          write(processStdin,msg+3,msg_len);
                      }
                  } else {
                      if(processPty!=-1){
                          write(processPty,msg+3,msg_len);
                      }
                  }
                break;
              case 8: // end of input
                  if (!ptyMode) {
                      close(processStdin);
                      processStdin=-1;
                  } else {
                      shutdown(processPty,SHUT_WR);
                  }
                break;
              case 7: // kill command
                sig_no=msg[3];
                a=kill(processPid,sig_no);
//printf("kill(%d,%d) returned %d\r\n", processPid,sig_no,a);
                break;
              case 9: // unmap command
                close(processStdin);
                processStdin=-1;
                close(processStdout);
                processStdout=-1;
                close(processStderr);
                processStderr=-1;
                close(processPty);
                processPty=-1;
                break;

              default:
                break;
            }
            process_buffer.set_pos(3+msg_len);
            process_buffer.cut();
          }
//printf("messages processing end, %d\r\n",msg_len);
        } else {
          close(processServerDown);
          close(processServerUp);
          return;
        }
      }
      if(processStdout!=-1 && FD_ISSET(processStdout,&readfds)){
        nBytes = PIPE_BUF_SIZE;
        unsigned char buffer[PIPE_BUF_SIZE];
        r = read(processStdout,buffer,(int)nBytes);
//printf("processStdout end, %d\r\n",r);
        if(r>0){
          TTCN_Buffer msg;
          int msg_len=put_msg(msg,4,r,buffer);
          write(processServerUp,msg.get_data(),msg_len);
        } else {
          close(processStdout);
          processStdout=-1;
          if(processStderr==-1) handle_childDeath_inprocess();
//printf("handle_childDeath_inprocess end, %d \r\n", executing);
        }
      }
      if(processStderr!=-1 && FD_ISSET(processStderr,&readfds)){
        nBytes = PIPE_BUF_SIZE;
        unsigned char buffer[PIPE_BUF_SIZE];
        r = read(processStderr,buffer,(int)nBytes);
//printf("processStderr end, %d\r\n",r);
        if(r>0){
          TTCN_Buffer msg;
          int msg_len=put_msg(msg,5,r,buffer);
          write(processServerUp,msg.get_data(),msg_len);
        } else {
          close(processStderr);
          processStderr=-1;
          if(processStdout==-1) handle_childDeath_inprocess();
//printf("handle_childDeath_inprocess end, \r\n");
        }
      }
      if(processPty!=-1 && FD_ISSET(processPty,&readfds)){
        nBytes = PIPE_BUF_SIZE;
        unsigned char buffer[PIPE_BUF_SIZE];
        r = read(processPty,buffer,(int)nBytes);
        //printf("processPty end, %d\r\n",r);
        if(r>0){
          TTCN_Buffer msg;
          // sent as STDIN
          int msg_len=put_msg(msg,4,r,buffer);
          write(processServerUp,msg.get_data(),msg_len);
        } else {
          close(processPty);
          processPty=-1;
          handle_childDeath_inprocess();
//printf("handle_childDeath_inprocess end, %d \r\n", executing);
        }
      }
    }
//printf("while end, %d\r\n",executing);

  }
}

void PIPEasp__PT::newcmd(const char* command){
  // creating pipes for process
  int pipesStdin[2];
  int pipesStdout[2];
  int pipesStderr[2];
				    
  if (pipe(pipesStdin) != 0) {
      return;
  }
  if (pipe(pipesStdout) != 0) {
      return;
  }
  if (pipe(pipesStderr) != 0) {
      return;
  }
  
  processStdin = pipesStdin[1];
  processStdout = pipesStdout[0];
  processStderr = pipesStderr[0];

  processPid = fork();
  if (processPid < 0) {
    //
    // Error
    //

    // close the pipes
    close(pipesStdin[0]);
    close(pipesStdout[1]);
    close(pipesStderr[1]);

    close(processStdin);
    close(processStdout);
    close(processStderr);

  }
  else if (processPid == 0) {

      //
      // Child process
      //

      // close the parent end of the pipes
      close(processStdin);
      close(processStdout);
      close(processStderr);

      int r;
      // redirect pipeStdin to stdin
      r = dup2(pipesStdin[0], 0);
      if (r<0) {
        exit(errno);
      }

      // redirect pipeStdout to stdout
      r = dup2(pipesStdout[1], 1);
      if (r<0) {
        exit(errno);
      }

      // redirect pipeStderr to stderr
      r = dup2(pipesStderr[1], 2);
      if (r<0) {
        exit(errno);
      }

      processExitCode = execCommand(command);
      
      // There is a problem executing the command
      // Exiting...
      
      fflush(stdout);
      fflush(stderr);
      
      //closing pipes:
      close(pipesStdin[0]);
      close(pipesStdout[1]);
      close(pipesStderr[1]);
      
//      exit(processExitCode); // end of child process
  }
  else {
					
    //
    // Parent process
    //
//printf("PIPEasp__PT::newcmd(%s) started, %d\r\n",command,processPid);

    // close child end of the pipes
    close(pipesStdin[0]);
    close(pipesStdout[1]);
    close(pipesStderr[1]);
    
    
    processExecuting = true;

 }

}

void PIPEasp__PT::newcmd_pty(const char* command){


  processPid = forkpty(&processPty,NULL,NULL,NULL);
  if (processPid < 0) {
    //
    // Error
    //
    processPty=-1;

  }
  else if (processPid == 0) {

      //
      // Child process
      //


      processExitCode = execCommand(command);

  }
  else {

    //
    // Parent process
    //


    processExecuting = true;

 }

}



}//namespace
