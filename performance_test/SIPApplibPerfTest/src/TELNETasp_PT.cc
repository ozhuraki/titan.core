/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               TELNETasp_PT.cc
//  Description:        TELNET testport source file
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 320
//  Updated:            2012-08-08
//  Contact:            http://ttcn.ericsson.se
//

#include "TELNETasp_PT.hh"

#define TELCMDS 1
#define TELOPTS 1
#include <arpa/telnet.h>

#include <poll.h>

#if ( defined TTCN3_VERSION_MONOTONE ) && ! ( TTCN3_VERSION_MONOTONE <= 1100099)
#include "pattern.hh"
char* TTCN_pattern_to_regexp_telnet(const char* patt){
  return TTCN_pattern_to_regexp(patt);
}
#else
#ifndef TTCN_pattern_to_regexp
extern char* TTCN_pattern_to_regexp(const char*,int);
char* TTCN_pattern_to_regexp_telnet(const char* patt){
  return TTCN_pattern_to_regexp(patt,1);
}
#endif
#endif


namespace TELNETasp__PortType {
TELNETasp__PT::TELNETasp__PT(const char *par_port_name)
	: TELNETasp__PT_BASE(par_port_name), window_size(par_port_name)
{
    ctrl_hostname = NULL;
    ctrl_portnum = 0;
    ctrl_username = NULL;
    ctrl_password = NULL;
    ctrl_domain = NULL;
    ctrl_readmode = READMODE_BUFFERED;
    ctrl_login_skipped = false;
    ctrl_detect_server_disconnected = false;
    ctrl_detect_connection_establishment_result = false;
    ctrl_client_cleanup_linefeed = true;
    ctrl_terminal_type = NULL;
    ctrl_echo = false;
    ctrl_CRLF = false;
    echobuf = "";
    config_finished = false;
    asp_params = NULL;
    map_poll_timeout=10000; // 10 sec = 10000 millisec
    ctrl_username_client = NULL;
    ctrl_password_client = NULL;
    ctrl_server_prompt = NULL;
    server_mode_def = false;
    ctrl_server_attach_prompt = true;
    ctrl_detect_client_disconnected = false;
    ctrl_server_failsafe_sending = false;
    port_mapped = false;
    fd_server = -1;
    isClientLoggedIn = false;
    isRawRegexpPrompt = false;
    pass_prompt_send = false;
    emptyEcho = false;
    suppressed = false;
    login_incorrect = false;
    ctrl_password_prompt = NULL;
    ctrl_loginname_prompt = NULL;

    debugAllowed = false;
}

TELNETasp__PT::~TELNETasp__PT()
{
    reset_configuration();
}

void TELNETasp__PT::set_parameter(const char *parameter_name,
	const char *parameter_value)
{
    if (config_finished) {
      reset_configuration();
      config_finished = false;
    }

    if(strcmp( "MAP_RECV_TIMEOUT", parameter_name) == 0) {
	    map_poll_timeout = atoi ( parameter_value );
     	log_debug(
	      "%s: Reading testport parameter: MAP_RECV_TIMEOUT = %d",
	      port_name, map_poll_timeout );
    }


    else if(strcmp( "CTRL_HOSTNAME", parameter_name) == 0)
	InitStrPar(ctrl_hostname, parameter_name, parameter_value);
    else if(strcmp( "CTRL_PORTNUM", parameter_name) == 0) {
	ctrl_portnum = atoi ( parameter_value );
	log_debug(
	    "%s: Reading testport parameter: CTRL_PORTNUM = %d",
	    port_name, ctrl_portnum );
    }

    else if(strcmp("CTRL_USERNAME", parameter_name) == 0)
	InitStrPar(ctrl_username, parameter_name, parameter_value);

    else if(strcmp("CTRL_PASSWORD", parameter_name) == 0) {
	//InitStrPar(ctrl_password, parameter_name, parameter_value);
	InitStrPar(ctrl_password, NULL, parameter_value);
	log_debug(
	    "%s: Reading testport parameter: CTRL_PASSWORD", port_name);
    }

    else if(strcmp("CTRL_DOMAIN", parameter_name) == 0)
	InitStrPar(ctrl_domain, parameter_name, parameter_value);

    else if(strcmp("CTRL_READMODE", parameter_name) == 0) {
	log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcmp(parameter_value, "buffered") == 0) 
	    ctrl_readmode = READMODE_BUFFERED;
	else if(strcmp(parameter_value, "unbuffered") == 0) 
	    ctrl_readmode = READMODE_UNBUFFERED;
	else TTCN_error("Invalid value for: CTRL_READMODE");
    }

    else if(strncmp("PROMPT", parameter_name, 6) == 0) {
	if(strlen(parameter_name) < 7) TTCN_error("%s: PROMPT "
	    "parameters should be given as PROMPT<number> := "
	    "\"value\".", port_name);
	errno = 0;
	size_t prompt_id = atoi(parameter_name + 6);
	if(errno) TTCN_error("%s: error converting string \"%s\" in parameter "
	    "name \"%s\" to number.", port_name, parameter_name + 6,
	    parameter_name);
	if(strlen(parameter_value)!=0) {
	    log_debug( "%s: Reading testport parameter: "
		"%s = %s", port_name, parameter_name, parameter_value );
	    prompt_list.set_prompt(prompt_id, parameter_value, false);
	} else
	    TTCN_error("PROMPT parameter must contain at least one character");
    }

    else if(strncmp("REGEX_PROMPT", parameter_name, 12) == 0) {
	if(strlen(parameter_name) < 13) TTCN_error("%s: REGEX_PROMPT "
	    "parameters should be given as REGEX_PROMPT<number> := "
	    "\"value\".", port_name);
	errno = 0;
	size_t prompt_id = atoi(parameter_name + 12);
	if(errno) TTCN_error("%s: error converting string \"%s\" in parameter "
	    "name \"%s\" to number.", port_name, parameter_name + 12,
	    parameter_name);
	if(strlen(parameter_value)!=0) {
	    log_debug( "%s: Reading testport parameter: "
		"%s = %s", port_name, parameter_name, parameter_value );
	    prompt_list.set_prompt(prompt_id, parameter_value, true);
	} else
	    TTCN_error("REGEX_PROMPT parameter must contain at least one character");
    }

    else if(strncmp("RAW_REGEX_PROMPT", parameter_name, 16) == 0) {
	if(strlen(parameter_name) < 16) TTCN_error("%s: REGEX_PROMPT "
	    "parameters should be given as RAW_REGEX_PROMPT<number> := "
	    "\"value\".", port_name);
	errno = 0;
	size_t prompt_id = atoi(parameter_name + 16);
	if(errno) TTCN_error("%s: error converting string \"%s\" in parameter "
	    "name \"%s\" to number.", port_name, parameter_name + 12,
	    parameter_name);
	if(strlen(parameter_value)!=0) {
	    log_debug( "%s: Reading testport parameter: "
		"%s = %s", port_name, parameter_name, parameter_value );
	    prompt_list.set_prompt(prompt_id, parameter_value, true,true);
	} else
	    TTCN_error("REGEX_PROMPT parameter must contain at least one character");
    }

    else if(strcmp("API_REGEXP_PROMPT_MODE", parameter_name) == 0) {
	log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "raw") == 0)
          isRawRegexpPrompt = true;
	else if(strcasecmp(parameter_value, "normal") == 0)
          isRawRegexpPrompt = false;
	else TTCN_error( "Invalid value for: API_REGEXP_PROMPT_MODE" );
    }

    else if(strcmp("CTRL_LOGIN_SKIPPED", parameter_name) == 0) {
	log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "yes") == 0)
          ctrl_login_skipped = true;
	else if(strcasecmp(parameter_value, "no") == 0)
          ctrl_login_skipped = false;
	else TTCN_error( "Invalid value for: CTRL_LOGIN_SKIPPED" );
    }

    else if(strcmp("CTRL_DETECT_SERVER_DISCONNECTED", parameter_name) == 0) {
	log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "yes") == 0)
	    ctrl_detect_server_disconnected = true;
	else if(strcasecmp(parameter_value, "no") == 0)
	    ctrl_detect_server_disconnected = false;
	else TTCN_error("Invalid value for: CTRL_DETECT_SERVER_DISCONNECTED");
    }

    else if(strcmp("CTRL_DETECT_CONNECTION_ESTABLISHMENT_RESULT", parameter_name) == 0) {
	log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "yes") == 0)
	    ctrl_detect_connection_establishment_result = true;
	else if(strcasecmp(parameter_value, "no") == 0)
	    ctrl_detect_connection_establishment_result = false;
	else TTCN_error("Invalid value for: CTRL_DETECT_CONNECTION_ESTABLISHMENT_RESULT");
    }


    else if(strcmp("CTRL_TERMINAL_TYPE", parameter_name) == 0)
	InitStrPar( ctrl_terminal_type, parameter_name, parameter_value );

    else if(strcmp("CTRL_ECHO", parameter_name) == 0) {
        log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "yes") == 0)
          ctrl_echo = true;
	else if(strcasecmp(parameter_value, "no") == 0) 
          ctrl_echo = false;
	else TTCN_error( "Invalid value for: CTRL_ECHO" );
    }

    else if(strcmp("CTRL_CRLF", parameter_name) == 0) {
        log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "yes") == 0)
          ctrl_CRLF = true;
	else if(strcasecmp(parameter_value, "no") == 0)
          ctrl_CRLF = false;
	else TTCN_error( "Invalid value for: CTRL_CRLF" );
    }

    else if(strcmp("CTRL_WINDOW_WIDTH", parameter_name) == 0) {
	window_size.set_width(atoi(parameter_value));
	log_debug(
	    "%s: Reading testport parameter: CTRL_WINDOW_WIDTH = %d",
	    port_name, window_size.get_width() );
    }

    else if(strcmp("CTRL_WINDOW_HEIGHT", parameter_name) == 0) {
	window_size.set_height(atoi(parameter_value));
	log_debug(
	    "%s: Reading testport parameter: CTRL_WINDOW_HEIGHT = %d",
	    port_name, window_size.get_height() );
    }
    else if (strcmp("CTRL_USERNAME_CLIENT", parameter_name) == 0)
        InitStrPar(ctrl_username_client, parameter_name, parameter_value);
    else if (strcmp("CTRL_PASSWORD_CLIENT", parameter_name) == 0)
        InitStrPar(ctrl_password_client, parameter_name, parameter_value);
    else if ( strcmp(parameter_name, "CTRL_MODE") == 0){
        log_debug("%s: Reading testport parameter: %s = %s",
                         port_name, parameter_name, parameter_value );
        server_mode_def = true;
        if (strcmp(parameter_value, "server") == 0)
            server_mode = true;
        else if (strcmp(parameter_value, "client") == 0)
            server_mode = false;
        else TTCN_error("Invalid value for: CTRL_MODE");
    }
    else if (strcmp(parameter_name, "CTRL_SERVER_PROMPT") == 0)
        InitStrPar(ctrl_server_prompt, parameter_name, parameter_value);
    else if (strcmp(parameter_name, "CTRL_PASSWORD_PROMPT") == 0)
        InitStrPar(ctrl_password_prompt, parameter_name, parameter_value);
    else if (strcmp(parameter_name, "CTRL_LOGINNAME_PROMPT") == 0)
        InitStrPar(ctrl_loginname_prompt, parameter_name, parameter_value);

    
    else if (strcmp(parameter_name, "CTRL_CLIENT_CLEANUP_LINEFEED") == 0){
      log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "yes") == 0)
          ctrl_client_cleanup_linefeed = true;
	else if(strcasecmp(parameter_value, "no") == 0)
          ctrl_client_cleanup_linefeed = false;
	else TTCN_error( "Invalid value for: CTRL_CLIENT_CLEANUP_LINEFEED" );
    }
    
    else if(strcmp(parameter_name, "CTRL_SERVER_ATTACH_PROMPT") == 0){
            log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "yes") == 0)
          ctrl_server_attach_prompt = true;
	else if(strcasecmp(parameter_value, "no") == 0)
          ctrl_server_attach_prompt = false;
	else TTCN_error( "Invalid value for: CTRL_SERVER_ATTACH_PROMPT" );
    }
    
    else if(strcmp(parameter_name, "CTRL_DETECT_CLIENT_DISCONNECTED") == 0){
            log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "yes") == 0)
          ctrl_detect_client_disconnected = true;
	else if(strcasecmp(parameter_value, "no") == 0)
          ctrl_detect_client_disconnected = false;
	else TTCN_error( "Invalid value for: CTRL_DETECT_CLIENT_DISCONNECTED" );
   }
   
   else if(strcmp(parameter_name, "CTRL_SERVER_FAILSAFE_SENDING") == 0){
            log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "yes") == 0)
          ctrl_server_failsafe_sending = true;
	else if(strcasecmp(parameter_value, "no") == 0)
          ctrl_server_failsafe_sending = false;
	else TTCN_error( "Invalid value for: CTRL_SERVER_FAILSAFE_SENDING" );
   }


  else if (!strcmp(parameter_name, "DEBUG")) {
            log_debug( "%s: Reading testport parameter: %s = %s",
	    port_name, parameter_name, parameter_value );
	if(strcasecmp(parameter_value, "yes") == 0) {
          debugAllowed = true;
          window_size.set_debug(true);
          prompt_list.set_debug(true);
          }
	else if(strcasecmp(parameter_value, "no") == 0) {
          debugAllowed = false;
          window_size.set_debug(false);
          prompt_list.set_debug(false);
          }
	else TTCN_error( "Invalid value for: DEBUG" );
  }

  else if (strcmp(parameter_name, "empty_echo") == 0 ) {
    log_debug("%s: Reading testport parameter: %s = %s", port_name, parameter_name, parameter_value);
    if (strcmp(parameter_value, "yes") == 0) {
      emptyEcho = true;
    } else {
      log_debug("%s wasn't yes -> empty echo disabled.", parameter_value);
    }
  }

  else TTCN_warning("%s: unknown parameter: %s", port_name, parameter_name);
}

/*void TELNETasp__PT::Event_Handler(const fd_set *read_fds,
	const fd_set *write_fds, const fd_set *error_fds,
	double time_since_last_call)
*/
void TELNETasp__PT::Handle_Fd_Event_Readable(int fd)
{
log_debug("Handle_Fd_Event_Readable(int fd=%d)",fd);
if (server_mode){
    if( fd==fd_server){ //incoming connection arrived
        char addr[INET_ADDRSTRLEN];
        struct sockaddr_in client;
#if defined SOLARIS8 || LINUX || FREEBSD
    	socklen_t
#else /* SOLARIS or WIN32 */
    	int
#endif
        addr_len = sizeof(client);
            
        if ( (nvtsock = accept(fd_server, (struct sockaddr*)&client, &addr_len)) < 0 )
            TTCN_error("Error accepting connection");
            
        inet_ntop(AF_INET, &(client.sin_addr), addr, sizeof(addr));
        log_debug("Accepting connection from: %s on socket %d", addr , nvtsock);
        Handler_Remove_Fd_Read(fd_server);
        Handler_Add_Fd_Read(nvtsock);
//        FD_CLR(fd_server, &readfds);
//        FD_SET(nvtsock, &readfds);
//        Install_Handler(&readfds, NULL, NULL, 0.0);

        if (ctrl_login_skipped){
           if(ctrl_server_attach_prompt){
             if ( ::send(nvtsock, "\r\n", 2 , 0) < 0)
                 TTCN_error("TCP send failed");
             if ( ::send(nvtsock, ctrl_server_prompt, strlen(ctrl_server_prompt), 0) < 0)
                 TTCN_error("TCP send failed");
            }
            incoming_message(INTEGER(1));
            isClientLoggedIn = true;
            return;
        }
        //starting login procedure
        if ( ::send(nvtsock,"\r\n", 2,0 ) < 0)
            TTCN_error("TCP send failed");

        if ( ::send(nvtsock,ctrl_loginname_prompt, strlen(ctrl_loginname_prompt),0 ) < 0)
            TTCN_error("TCP send failed");
    } else if (fd==nvtsock){
        recv_msg_from_client(nvtsock);
    } else {TTCN_error("Invalid file descriptor to read %d",fd);}
} else {

    if (RecvClrMsg() < 0) {
	if(ctrl_detect_server_disconnected) return;
	else TTCN_error ("*** Socket error or the server closed the connection "
	    "(in Event_Handler).");
    }

    while(true) {
	size_t echo_pos;
	if(ctrl_echo==false &&
	    buf_strcmp((const char*)echobuf, ttcn_buf.get_data(),
	    ttcn_buf.get_len(), echo_pos) && echo_pos == 0) {
	    /*log_buffer("Before echo proc.:", ttcn_buf.get_data(),
		ttcn_buf.get_len());*/

	    ttcn_buf.set_pos(strlen((const char*)echobuf));
	    ttcn_buf.cut();
	    echobuf = "";

            suppressed = true;

	    /*log_buffer("After echo proc.:", ttcn_buf.get_data(),
		ttcn_buf.get_len());*/

	    if(!ttcn_buf.get_len()) return;
	}

	const unsigned char * bufptr = ttcn_buf.get_data();
	int prompt_len;
	bool nl_found = false, prompt_found = false;
	size_t prompt_start_pos_prompt, prompt_start_pos_nl;

	if (ctrl_readmode == READMODE_UNBUFFERED) {
	    nl_found = buf_strcmp("\n", bufptr, ttcn_buf.get_len(),
		prompt_start_pos_nl);
	    if ((prompt_len = isPrompt(prompt_start_pos_prompt))>=0)
		prompt_found = true;

	    /*If there is neither new line nor prompt in the buffer then simply
	    return and wait for more data*/
	    if(!nl_found && !prompt_found)
              return;

	    else if((nl_found && !prompt_found) ||
		(nl_found && prompt_found && prompt_start_pos_nl <
		prompt_start_pos_prompt)) {
		if(prompt_start_pos_nl){
                  suppressed = false;
                  incoming_message(CHARSTRING(prompt_start_pos_nl,
			(const char*)bufptr));
                }
			ttcn_buf.set_pos(prompt_start_pos_nl+1);
			ttcn_buf.cut();
	    } else {
		//First send the data previous to the prompt ...
		if(prompt_start_pos_prompt) {
                    suppressed = false;
		    incoming_message(CHARSTRING(prompt_start_pos_prompt,
			(const char*)bufptr));
		    ttcn_buf.set_pos(prompt_start_pos_prompt);
		    ttcn_buf.cut();
		}
		//... and then send the prompt itself
		bufptr = ttcn_buf.get_data();
                suppressed = false;
		incoming_message(CHARSTRING(prompt_len,
		    (const char*)bufptr));
		ttcn_buf.set_pos(prompt_len);
		ttcn_buf.cut();
	    }
	    nl_found = false;
	    prompt_found = false;
            
	    if(!ttcn_buf.get_len()) return;
	} else { // READMODE_BUFFERED
	    /* only send if the last line is prompt */
	    if ((prompt_len = isPrompt(prompt_start_pos_prompt))>=0) {
		// promptlen==0 is not checked here since it must contain
		// at least 1 character, that is checked in set_parameter()
		//First send the data previous to the prompt ...
                log_debug("Prompt start pos: %d", prompt_start_pos_prompt);
              	if(prompt_start_pos_prompt) {
		    int msg_end_pos = prompt_start_pos_prompt;
                    if(ctrl_client_cleanup_linefeed){
		     while(msg_end_pos>0) {
		         if(bufptr[msg_end_pos-1]!='\n') break;
		         else msg_end_pos--;
		     }
                   }
		    if(msg_end_pos){  
                      suppressed = false;
                      incoming_message(CHARSTRING(msg_end_pos,
			(const char*)bufptr));
                    }
		    ttcn_buf.set_pos(prompt_start_pos_prompt);
		    ttcn_buf.cut();
		} 
                
                if (emptyEcho && suppressed){
                  suppressed = false;
                  incoming_message("");
                }

		//... and then send the prompt itself
		bufptr = ttcn_buf.get_data();
                suppressed = false;
		incoming_message(CHARSTRING(prompt_len, (const char*)bufptr));
		ttcn_buf.set_pos(prompt_len);
		ttcn_buf.cut();
		if(!ttcn_buf.get_len()) return;
	    }
	    return;
	}
    } // while
  } // server-mode else (client mode)
} // TELNETasp__PT::Handle_Fd_Event_Readable

void TELNETasp__PT::user_map(const char *system_port)
{

set_asp_params();
config_finished = true;
    suppressed = false;

if(ctrl_portnum == 0)
    TTCN_error("Missing mandatory parameter: CTRL_PORTNUM");
if (!server_mode_def){
    TTCN_warning("Mode is not defined. Test Port will operate in client mode operation.");
    server_mode = false;
}

struct sockaddr_in address;
int enabled = 1;

if (server_mode){
    nvtsock=-1;
    if (port_mapped){
        TTCN_warning("%s: user_map() was called to a mapped port", port_name);
        return;
    }

    if(ctrl_server_prompt == NULL)
        TTCN_error("Missing mandatory parameter: CTRL_SERVER_PROMPT");
        
    if (ctrl_username_client == NULL && !ctrl_login_skipped)
        TTCN_error("Missing mandatory parameter: CTRL_USERNAME_CLIENT");
    if (ctrl_password_client == NULL && !ctrl_login_skipped)
        TTCN_error("Missing mandatory parameter: CTRL_PASSWORD_CLIENT");

    if (ctrl_loginname_prompt == NULL){
        if ((ctrl_loginname_prompt = (char*)malloc(8*sizeof(char))) == NULL )
            TTCN_error("Not enough memory");
        strcpy(ctrl_loginname_prompt, "login: ");
    }
    
    if (ctrl_password_prompt == NULL){
        if ((ctrl_password_prompt = (char*)malloc(11*sizeof(char))) == NULL )
            TTCN_error("Not enough memory");
        strcpy(ctrl_password_prompt, "password: ");
    }
    
    if ((fd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        TTCN_error("Socket creation failed");   
    
    log_debug( "%s: Server socket created: %d", port_name, fd_server);
    if ( setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0)
        TTCN_error("setsockopt(SO_REUSEADDR) failed");
     
    address.sin_family = AF_INET;
    address.sin_port = htons(ctrl_portnum);
    address.sin_addr.s_addr = htons(INADDR_ANY);
    memset(&(address.sin_zero), '\0', 8);
     
    if ( bind(fd_server, (struct sockaddr*)&address, sizeof(address)) < 0 )
        TTCN_error("Error bindig socket to port %d", ctrl_portnum);
    if (listen(fd_server,1) < 0 )
        TTCN_error("Error listening on port %d", ctrl_portnum);
      
//    FD_ZERO(&readfds);
//    FD_SET(fd_server, &readfds);
//    Install_Handler(&readfds, NULL, NULL, 0.0);
    Handler_Add_Fd_Read(fd_server);
     
    log_debug( "%s: Listening on port %d for incoming connection", port_name, ctrl_portnum);
    port_mapped = true;
}else{
    //char* nvtmsg;
    size_t prompt_start_pos = 0;

    if(ctrl_hostname == NULL)
	TTCN_error("Missing mandatory parameter: CTRL_HOSTNAME");
    if(ctrl_username == NULL && !ctrl_login_skipped)
	TTCN_error("Missing mandatory parameter: CTRL_USERNAME");
    if(ctrl_password == NULL && !ctrl_login_skipped)
	TTCN_error("Missing mandatory parameter: CTRL_PASSWORD");
    if(prompt_list.nof_prompts() == 0)
	TTCN_error("Missing mandatory parameter: at least one PROMPT or "
	    "REGEX_PROMPT parameter must be provided");
    prompt_list.check(port_name);

    nvtsock = socket( AF_INET, SOCK_STREAM, 0 );
    if (nvtsock < 0) TTCN_error("Socket creation failed");
    log_debug( "%s: Client socket created: %d", port_name, nvtsock);

    if((he = gethostbyname(ctrl_hostname))==NULL)
	TTCN_error("Unable to resolve hostname: %s", ctrl_hostname);

    if(setsockopt(nvtsock, SOL_SOCKET, SO_REUSEADDR, (const char *)&enabled,
	sizeof(int)) == -1)
	TTCN_error("setsockopt(SO_REUSEADDR) failed");

    memcpy(&address.sin_addr, he->h_addr_list[0], he->h_length);
    address.sin_family = AF_INET;
    address.sin_port = htons(ctrl_portnum);

    // workaround for WinSock bug (For Cygwin)
    int retries = 0;
    while (connect(nvtsock, (struct sockaddr*)&address, sizeof(address))<0){
      
      	if (errno != EADDRINUSE) {
          if(ctrl_detect_connection_establishment_result) {
            break;
          } else {
            TTCN_error( "Error connecting %s", ctrl_hostname );
	    break;
          }
	} 
	if (++retries > 18) {
          if(ctrl_detect_connection_establishment_result) {
            break;
          } else {
            TTCN_error( "Error connecting %s. Address already in use.", ctrl_hostname );
	    break;
          }
	}
        log_debug( "%s: WinSock EADDRINUSE bug"
          " workaround: %i. retry", port_name, retries);
        sleep(10); 
    }
    

    log_debug(
	"%s: Successfully connected to %s on TELNET port", port_name,
	ctrl_hostname );

    isLoginSent = false;
    isPasswordSent = false;
    isDomainSent = false;
    isConnected = (ctrl_portnum==7)?true:false;
    log_debug( "%s: ----- TELNET Login Started -----",
	port_name);

    if(window_size.is_set()) {
	const unsigned char IAC_WILL_NAWS[] = {IAC, WILL, TELOPT_NAWS};
	::send(nvtsock, IAC_WILL_NAWS, sizeof(IAC_WILL_NAWS), 0);
	window_size.set_state(WSS_WILL_SENT);
	log_debug("%s: IAC WILL TELOPT_NAWS sent.",
	    port_name);
    }

#ifdef TELNET_ECHO_OPTION
    if(ctrl_echo) {
	const unsigned char IAC_DO_ECHO[] = {IAC, DO, TELOPT_ECHO};
	::send(nvtsock, IAC_DO_ECHO, sizeof(IAC_DO_ECHO), 0);
	log_debug("%s: IAC DO ECHO sent.", port_name);
    } else {
	const unsigned char IAC_DONT_ECHO[] = {IAC, DONT, TELOPT_ECHO};
	::send(nvtsock, IAC_DONT_ECHO, sizeof(IAC_DONT_ECHO), 0);
	log_debug("%s: IAC DONT ECHO sent.", port_name);
    }
#endif
struct pollfd poll_fds;
  poll_fds.fd=nvtsock;
  poll_fds.events= POLLIN | POLLPRI;
  while(!isConnected) {
	    int poll_res=poll(&poll_fds,1,map_poll_timeout);
      if(poll_res==1){
        if(RecvClrMsg() < 0) {
	          if(ctrl_detect_server_disconnected) return;
	          else TTCN_error("*** Socket error or the server closed"
		      " the connection.");
	      }
      } else {
	      if(ctrl_detect_server_disconnected) {
	          log_debug( "%s: Connection timeout during map."
	          " Unmap the port and map it again.", port_name);
	          if(ttcn_buf.get_len()!=0) {
		      ttcn_buf.clear();
	          }
	          Uninstall_Handler();
	          close( nvtsock );
	          nvtsock = -1;
	          isConnected = false;
	          incoming_message(INTEGER(0));
            return;
	      } else {
          TTCN_error("*** Socket timeot during map operation");
        }
      }

	if(!ctrl_login_skipped) { //perform login procedure
	    const unsigned char * ttcn_buf_data = ttcn_buf.get_data();
	    size_t buflen = ttcn_buf.get_len(), pos = 0;

	    if((!isLoginSent) && 
		(buf_strcmp("sername", ttcn_buf_data, buflen, pos) ||
		buf_strcmp("ogin", ttcn_buf_data, buflen, pos)))
	    {
		//usleep(100000UL);         // 100 ms	
		::send(nvtsock, ctrl_username, strlen(ctrl_username), 0);
		if(ctrl_CRLF) ::send( nvtsock, "\r\n", 2, 0 );
		else ::send( nvtsock, "\n", 1, 0 );
		isLoginSent = true;
		ttcn_buf.clear();
		log_debug("%s: user name sent.",port_name);
	    }
	    else if((!isPasswordSent) && 
		buf_strcmp("assword", ttcn_buf_data, buflen, pos))
	    {
		//usleep(100000UL);         // 100 ms
		::send( nvtsock, ctrl_password, strlen(ctrl_password), 0);
		if(ctrl_CRLF) ::send( nvtsock, "\r\n", 2, 0 );
		else ::send( nvtsock, "\n", 1, 0 );
		isLoginSent = true;
		isPasswordSent = true;
		ttcn_buf.clear();
		log_debug("%s: password sent.",port_name);
	    }
	    else if((!isDomainSent) && 
		buf_strcmp("omain", ttcn_buf_data, buflen, pos))
	    {
		//usleep(100000UL);         // 100 ms
		if(ctrl_domain == NULL) TTCN_error("%s: missing parameter "
		    "CTRL_DOMAIN.", port_name);
		if(strlen(ctrl_domain))::send( nvtsock, ctrl_domain, strlen(ctrl_domain), 0);
		if(ctrl_CRLF) ::send( nvtsock, "\r\n", 2, 0 );
		else ::send( nvtsock, "\n", 1, 0 );
		isDomainSent = true;
		ttcn_buf.clear();
		log_debug("%s: domain sent.",port_name);
	    }
	    else if(isPasswordSent && (isPrompt(prompt_start_pos) >= 0))
	    {
		isConnected = true;
	    }

	    else if((buf_strcmp("ogin incorrect", ttcn_buf_data, buflen,
		pos) || buf_strcmp("ogin failed", ttcn_buf_data, buflen,
		pos)) && (isPasswordSent))
	    {
		TTCN_error("Unable to connect to '%s' as '%s'",
		    ctrl_hostname, ctrl_username);
	    }
	}
	else { //login procedure is skipped
	    if(isPrompt(prompt_start_pos)>=0) isConnected = true;
	}
    }

    ttcn_buf.clear();
    
    if(ctrl_detect_connection_establishment_result) {
      incoming_message(INTEGER(2)); // connection establishment succeeded
    }

    log_debug( "%s: ----- TELNET %sLogin Finished -----\n",
	    port_name, ctrl_portnum==7?"ECHO":"" );
//    FD_ZERO( &readfds );
//    FD_SET( nvtsock, &readfds );
//    Install_Handler( &readfds, NULL, NULL, 0.0 );
    Handler_Add_Fd_Read(nvtsock);

}
}

void TELNETasp__PT::user_unmap(const char *system_port)
{

if(ttcn_buf.get_len()!=0) {
    TTCN_warning("%s: Dropping partial message", port_name);
    log_buffer("The content of the buffer:", ttcn_buf.get_data(), ttcn_buf.get_len());
    ttcn_buf.clear();
}

if (server_mode){
    port_mapped = false;

    if (nvtsock != -1){
        close_connection(nvtsock);
    }
    
    if (fd_server != -1) close(fd_server);
    fd_server = -1;
//    FD_ZERO(&readfds);
}else{
    if (nvtsock != -1) {
      close(nvtsock);
      nvtsock = -1;
    }
}
Uninstall_Handler();
}

void TELNETasp__PT::user_start()
{

}

void TELNETasp__PT::user_stop()
{

}

void TELNETasp__PT::outgoing_send(const ASP__TelnetClose& send_par){

    close_connection(nvtsock);
//    FD_SET(fd_server, &readfds);
//    Install_Handler(&readfds, NULL, NULL, 0.0);
    Handler_Add_Fd_Read(fd_server);

}

void TELNETasp__PT::outgoing_send(const CHARSTRING& send_par)
{
  if (server_mode) {
  
    if (!port_mapped){
        TTCN_Logger::log(TTCN_WARNING, "%s: Send operation failed: the port"
	    " is disconnected.", port_name);
	return;
    }
    if (!isClientLoggedIn){
        TTCN_warning("%s: Send operation failed: the client has not logged in", port_name);  
        return;  
    }
    if(ctrl_server_attach_prompt){
     if  (::send(nvtsock, (const char*) (send_par + "\r\n" + ctrl_server_prompt), send_par.lengthof() + 2 + strlen(ctrl_server_prompt), 0 ) < 0){
         if(ctrl_server_failsafe_sending){
           TTCN_warning("TCP send failed");
         }
         else{
           TTCN_error("TCP send failed");
         }
       }
    } else {
     if  (::send(nvtsock, (const char*) (send_par), send_par.lengthof(), 0 ) < 0) {
         if(ctrl_server_failsafe_sending){
           TTCN_warning("TCP send failed");
         }
         else{
           TTCN_error("TCP send failed");
         }
       }
    }

    if(debugAllowed) {
        TTCN_Logger::begin_event(TTCN_DEBUG);
        TTCN_Logger::log_event("%s: Message sent: ", port_name);
        send_par.log();
        TTCN_Logger::end_event();
    }
  } else {
    
    if(!isConnected) {
	log_debug( "%s: Send operation failed: the port"
	    " is disconnected.", port_name);
	return;
    }
    if(ctrl_CRLF) ::send( nvtsock, ( const char* ) (send_par+"\r\n"),
	send_par.lengthof()+2, 0 );
    else ::send( nvtsock, ( const char* ) (send_par+"\n"),
	send_par.lengthof()+1, 0 );
    if(ctrl_echo==false) echobuf = send_par+"\n";
    if(debugAllowed){
        TTCN_Logger::begin_event(TTCN_DEBUG);
        TTCN_Logger::log_event("%s: Message sent: ", port_name);
        send_par.log();
        TTCN_Logger::end_event();
    }
  }
}

void TELNETasp__PT::outgoing_send(const ASP__TelnetPortParameters& send_par)
{
    if(asp_params) delete asp_params;
    asp_params = new ASP__TelnetPortParameters(send_par);
}

void TELNETasp__PT::outgoing_send(const ASP__TelnetDynamicConfig& send_par)
{
    switch(send_par.get_selection()) {
    case ASP__TelnetDynamicConfig::ALT_readmode:
	if(send_par.readmode() == TelnetReadMode::UNBUFFERED)
	    ctrl_readmode = READMODE_UNBUFFERED;
	else
	    ctrl_readmode = READMODE_BUFFERED;
	break;
    case ASP__TelnetDynamicConfig::ALT_window__size: {
	const unsigned char IAC_WILL_NAWS[] = {
	    IAC,
	    WILL,
	    TELOPT_NAWS
	};
	window_size.set_width(send_par.window__size().width());
	window_size.set_height(send_par.window__size().height());
	if(nvtsock != -1) {
	    switch(window_size.get_state()) {
	    case WSS_INIT:
		::send(nvtsock, IAC_WILL_NAWS, sizeof(IAC_WILL_NAWS), 0);
		window_size.set_state(WSS_WILL_SENT);
		break; // wait for server to send IAC DO TELOPT_NAWS
	    case WSS_WILL_SENT:
		break; // wait for server to send IAC DO TELOPT_NAWS
	    case WSS_AGREED:
		::send(nvtsock, window_size.encode(), WINDOW_SIZE_LENGTH, 0);
		break;
	    //case WSS_NOT_SET: - we did set window size so this never happens
	    case WSS_REFUSED:
	    default:
		TTCN_warning("%s: server refused window size negotiation.",
		    port_name);
		break;
	    }
	} else {
	    TTCN_error("%s: error sending window size: not connected.",
		port_name);
	}
	break; }
    case ASP__TelnetDynamicConfig::ALT_echo: {
#ifdef TELNET_ECHO_OPTION
	unsigned char cmd[3]={ IAC, 0, TELOPT_ECHO };
	if(send_par.echo()==TRUE) {
	    ctrl_echo = true;
	    cmd[1] = DO;
	    log_debug("%s: IAC DO ECHO sent.", port_name);
	} else {
	    ctrl_echo = false;
	    cmd[1] = DONT;
	    log_debug("%s: IAC DONT ECHO sent.", port_name);
	}
	::send(nvtsock, cmd, sizeof(cmd), 0);
#else
        if(send_par.echo()==TRUE) {
            ctrl_echo = true;
        } else {
            ctrl_echo = false;
        }
#endif
	break; }
    case ASP__TelnetDynamicConfig::ALT_prompt:
	prompt_list.set_prompt(send_par.prompt().id(),
	    send_par.prompt().prompt(),
	    send_par.prompt().has__wildcards()==TRUE, isRawRegexpPrompt);
	break;
    default:
	TTCN_error("%s: ASP_TelnetDynamicConfig has invalid selection.",
	    port_name);
	break;
    }
}

void TELNETasp__PT::outgoing_send(const ASP__TelnetConnection& send_par)
{
    user_unmap(NULL);
    user_map(NULL);
}

void TELNETasp__PT::InitStrPar(char *&par, const char* name, const char* val )
{
    if(name)log_debug( "%s: Reading testport parameter: "
	"%s = %s", port_name, name, val);

    if(par) free(par);
    par = (char*) malloc(strlen(val)+1);
    if(par == NULL) TTCN_error("Not enough memory.");
    strcpy(par, val);
}


int TELNETasp__PT::ProcessCmd(unsigned char * buf, int buflen)
{
    TTCN_Buffer CMD;
    unsigned char *ptr = buf;
    int remaining_bytes = buflen;

    // option negotiation part
    while(remaining_bytes>2) { // min. length of a command is 3
#ifdef DEBUG_ProcessCmd
	unsigned char *cmd_start=ptr;
#endif
	if(*ptr++ == IAC) {
	    remaining_bytes -= 2;
	    switch(*ptr++) {
	    case WONT:
	    case DONT:
		switch(*ptr) {
		case TELOPT_NAWS:
		    window_size.set_state(WSS_REFUSED);
		    break;
/*		case TELOPT_ECHO:
		    CMD.put_c(IAC);
		    CMD.put_c(WILL);
		    CMD.put_c(TELOPT_ECHO);
		    break;
*/		default:
		    break;
		}
		ptr++;
		remaining_bytes--;
		break;
	    case DO:

		switch(*ptr) {
		case TELOPT_TTYPE:
		    CMD.put_c(IAC);
		    ptr++;
		    remaining_bytes--;
		    if(ctrl_terminal_type!=NULL)CMD.put_c(WILL);
		    else CMD.put_c(WONT);
		    CMD.put_c(TELOPT_TTYPE);
		    break;
		case TELOPT_NAWS:
		    ptr++;
		    remaining_bytes--;

		    switch(window_size.get_state()) {
		    case WSS_INIT:
			CMD.put_c(IAC);
			CMD.put_c(WILL);
			CMD.put_c(TELOPT_NAWS);
			// no break!
		    case WSS_WILL_SENT:
			window_size.set_state(WSS_AGREED);
			// no break!
		    case WSS_AGREED:
			CMD.put_s(WINDOW_SIZE_LENGTH, window_size.encode());
			break;
		    default:
			CMD.put_c(IAC);
			CMD.put_c(WONT);
			CMD.put_c(TELOPT_NAWS);
			break;
		    }

		    break;
		case TELOPT_ECHO:
/*		    if(!ctrl_echo) {
			CMD.put_c(IAC);
			CMD.put_c(WILL);
			CMD.put_c(TELOPT_ECHO);
			ptr++;
			remaining_bytes--;
			break;
		    }*/
		default:
		    CMD.put_c(IAC);
		    CMD.put_c(WONT);
		    CMD.put_c(*ptr++);
		    remaining_bytes--;
		    break;
		}

		break;
	    case WILL:

		switch(*ptr) {
		case TELOPT_NAWS:
		    CMD.put_c(IAC);

		    switch(window_size.get_state()) {
		    case WSS_INIT:
		    case WSS_WILL_SENT:
		    case WSS_AGREED:
			CMD.put_c(DO);
			break;
		    default:
			CMD.put_c(DONT);
			break;
		    }

		    CMD.put_c(TELOPT_NAWS);
		    break;
#ifdef TELNET_ECHO_OPTION
		case TELOPT_ECHO:
		    if(ctrl_echo) {
			CMD.put_c(IAC);
			CMD.put_c(DO);
			CMD.put_c(TELOPT_ECHO);
		    } else {
			CMD.put_c(IAC);
			CMD.put_c(DONT);
			CMD.put_c(TELOPT_ECHO);
		    }
		    break;
#endif
		default:
		    CMD.put_c(IAC);
		    CMD.put_c(DONT);
		    CMD.put_c(*ptr);
		    break;
		}

		ptr++;
		remaining_bytes--;
		break;
	    case SB: // Subnegotiation Begin
		remaining_bytes--;

		switch(*ptr++) {
		case TELOPT_TTYPE:
		    if(remaining_bytes<1) {
			TTCN_warning("%s: Unable to decode Terminal Type "
			    "sub-negotiation.", port_name);
			break;
		    }
		    if(*ptr==TELQUAL_IS) {
			ptr++;
			remaining_bytes--;
			while(*ptr!=IAC) { // skip terminal type string
			    if(remaining_bytes>0) {
				remaining_bytes--;
				ptr++;
			    } else break;
			}
		    } else if(*ptr==TELQUAL_SEND) {
			ptr++;
			remaining_bytes--;
			CMD.put_c(IAC);
			CMD.put_c(SB);
			CMD.put_c(TELOPT_TTYPE);
			CMD.put_c(TELQUAL_IS);
			CMD.put_s(strlen(ctrl_terminal_type),
			    (const unsigned char *)ctrl_terminal_type);
			CMD.put_c(IAC);
			CMD.put_c(SE);
		    } else {
			ptr++;
			remaining_bytes--;
			log_debug( "%s: octet after IAC SB "
			    "TERMINAL-TYPE is 0x%02x in option negotiation.",
			    port_name, *ptr);
		    }
		    break;
		case TELOPT_NAWS: // Window Size - in case server sends this
		    ptr += 4;
		    remaining_bytes -= 4;
		    break;
		default:
		    log_debug("%s: Telnet option not "
			"handled: 0x%02x.", port_name, *ptr);
		    ptr++;
		    remaining_bytes--;
		    while(*ptr!=IAC) { // skip until IAC
			if(remaining_bytes>0) {
			    remaining_bytes--;
			    ptr++;
			} else break;
		    }
		    break;
		}

		ptr += 2; // skip IAC SE
		remaining_bytes -= 2;
		break;
	    default:
		log_debug(" %s: Telnet Command not handled: "
		    "0x%02x", port_name, *ptr);
		break;
	    }
	} else { ptr--; break;}
#ifdef DEBUG_ProcessCmd
        if(debugAllowed) {
  	    TTCN_Logger::begin_event(TTCN_DEBUG);
	    TTCN_Logger::log_event("%s: processed command: ", port_name);
	    bool ttype=false;
	    while(cmd_start<ptr) {
	        if(*cmd_start==IAC)ttype=false;
	        else if(*cmd_start==TELOPT_TTYPE)ttype=true;
	        if(*cmd_start >= TELCMD_FIRST)
		    TTCN_Logger::log_event("%s ",TELCMD(*cmd_start));
	        else if(*cmd_start <= TELOPT_LAST) {
		    if(ttype && (*cmd_start)<2)
		        TTCN_Logger::log_event_str((*cmd_start)?"SEND ":"IS ");
		    else
		        TTCN_Logger::log_event("%s ", TELOPT(*cmd_start));
	        }
	        else TTCN_Logger::log_event("%d ", *cmd_start);
	        cmd_start++;
	    }
	TTCN_Logger::end_event();
       }
#endif
    }

    if(CMD.get_len() > 0) {
	CMD.rewind();
	::send( nvtsock, CMD.get_read_data(), CMD.get_read_len(), 0 );
	if(debugAllowed) {
            
#ifdef DEBUG_ProcessCmd          
            if(TTCN_Logger::log_this_event(TTCN_DEBUG)) {  
	        TTCN_Logger::begin_event(TTCN_DEBUG);
	        TTCN_Logger::log_event("%s: command sent:", port_name);
	        bool ttype=false, subcmd=false, naws=false;
	        const unsigned char *cmd_ptr = CMD.get_read_data();
	        unsigned int cmd_len=CMD.get_read_len();
	        for(unsigned int i=0;i<cmd_len;i++) {
		    if(*cmd_ptr==SB)subcmd=true;
		    else if(*cmd_ptr==SE)subcmd=false;
		    if(*cmd_ptr==IAC) {
		        if(!subcmd)TTCN_Logger::log_event("\n        ");
		        ttype=false;
		        naws=false;
		    }
		    if(*cmd_ptr >= TELCMD_FIRST)
		        TTCN_Logger::log_event("%s ",TELCMD(*cmd_ptr));
		    else if(ttype) {
		        switch(*cmd_ptr) {
		        case 0: TTCN_Logger::log_event_str("IS "); break;
		        case 1: TTCN_Logger::log_event_str("SEND "); break;
		        default:
			    if(*cmd_ptr>' ' && *cmd_ptr<128)
			        TTCN_Logger::log_event("\"%c\" ",*cmd_ptr);
			    else TTCN_Logger::log_event("%d ",*cmd_ptr);
		        }
		    } else if(naws) TTCN_Logger::log_event("%d ", *cmd_ptr);
		    else if(*cmd_ptr <= TELOPT_LAST)
		        TTCN_Logger::log_event("%s ", TELOPT(*cmd_ptr));
		    else TTCN_Logger::log_event("%d ", *cmd_ptr);
		    if(*cmd_ptr==TELOPT_NAWS)naws=true;
		    else if(*cmd_ptr==TELOPT_TTYPE)ttype=true;
		    cmd_ptr++;
	        }
	        TTCN_Logger::end_event();
            }
#else
	    log_buffer("ProcessCmd: command sent:", CMD.get_read_data(),
		CMD.get_read_len());
#endif
	}
    }

    if(remaining_bytes != buflen) {
	ttcn_buf.set_pos(buflen-remaining_bytes);
	ttcn_buf.cut();
	memmove(buf, ptr, remaining_bytes);
	buflen = remaining_bytes;
    }

    return buflen;
}

int TELNETasp__PT::isPrompt(size_t &pos)
{
    return prompt_list.findPrompt(pos, ttcn_buf.get_data(), ttcn_buf.get_len());
}

int TELNETasp__PT::RecvClrMsg()
{
    unsigned char inbuf[BUFFER_SIZE];
    errno=0;
    int end_len = BUFFER_SIZE, len = recv(nvtsock, inbuf , end_len, 0);
    
    if(len>0){
      log_debug( "%s: ********************** NEW MESSAGE"
	    " RECEIVED **********************", port_name);
      log_buffer("RecvClrMsg received message:", inbuf, len);
    } else if (len==0){
      log_debug("%s: ******** Connection is closed "
      "by peer.",port_name);
    } else {
      log_debug("%s: ******** Error occured during reading "
      "socket. errno value: %d",port_name,errno);
    }

    if (len < 1) {
	if(ctrl_detect_server_disconnected) {
	    log_debug( "%s: Connection is broken."
	    " Unmap the port and map it again.", port_name);
	    if(ttcn_buf.get_len()!=0) {
		ttcn_buf.clear();
	    }
	    Uninstall_Handler();
	    close( nvtsock );
	    nvtsock = -1;
	    isConnected = false;
	    incoming_message(INTEGER(0));
	}
	return -1;
    }

    len = ProcessCmd(inbuf, len);

    for(int i=0;i<len;i++) {
	if(inbuf[i]!='\r' && inbuf[i]!='\0') ttcn_buf.put_c(inbuf[i]);
    }

    log_buffer("RecvClrMsg ttcn_buf message:", ttcn_buf.get_data(),
	ttcn_buf.get_len());
    CHARSTRING tmpchr(ttcn_buf.get_len(), (const char*)ttcn_buf.get_data());
    if(debugAllowed) {
        TTCN_Logger::begin_event(TTCN_DEBUG);
        tmpchr.log();
        TTCN_Logger::end_event();
    }

    return ttcn_buf.get_len();
}

/* buf_strcmp compares the NULL-terminated string s1 to s2 of length s2_len
(s2 is not necessarily NULL-terminated). If s2 containes the substring s1,
then the function returns true and pos is set to the starting position of s1
within s2, otherwise the return value is false and pos remains unchanged.*/

bool TELNETasp__PT::buf_strcmp(const char * s1, const unsigned char * s2,
	size_t s2_len, size_t& pos)
{
    size_t s1_len = strlen(s1);
    if(s1_len == 0) return false;
    if(s1_len>s2_len) { return false; }
    for(int i=0;i<(int)(s2_len-s1_len+1); ) {
	size_t j = 0;
	while((unsigned char)s1[j] == s2[i+j]) { j++; }
	if(j==s1_len) { pos = i; return true; }
	i+=j>0?j:1;
    }
    return false;
}

void TELNETasp__PT::log_debug(const char *fmt, ...)
{ 
  if(debugAllowed)  {
      TTCN_Logger::begin_event(TTCN_DEBUG);
      TTCN_Logger::log_event("TELNET test port (%s): ", get_name());
      va_list ap;
      va_start(ap, fmt);
      TTCN_Logger::log_event_va_list(fmt, ap);
      va_end(ap);
      TTCN_Logger::end_event();
  }
}

void TELNETasp__PT::log_buffer(const char * logmsg, const unsigned char * buf,
	size_t buflen)
{
  if(debugAllowed) {
    log_debug( "%s: %s: ", port_name, logmsg);
    for(size_t j=0;j<buflen;) {
	TTCN_Logger::begin_event(TTCN_DEBUG);
	for(size_t i=0;j<buflen && i<16;i++) {
	    if(i == 8) TTCN_Logger::log_event("  ");
	    TTCN_Logger::log_event("%02x ", buf[j++]);
	}
	TTCN_Logger::end_event();
    }
  }
}

void TELNETasp__PT::set_asp_params()
{
    if(asp_params == NULL) return;
    TTCN_Logger::begin_event(TTCN_PORTEVENT);
    TTCN_Logger::log_event("%s: setting parameters from ASP: ", port_name);
    asp_params->log();
    TTCN_Logger::end_event();

    if(asp_params->ctrl__hostname().ispresent()==TRUE)
	InitStrPar(ctrl_hostname, NULL,
	    (CHARSTRING)asp_params->ctrl__hostname());

    if(asp_params->ctrl__portnum().ispresent()==TRUE)
	ctrl_portnum = (INTEGER)asp_params->ctrl__portnum();

    if(asp_params->ctrl__username().ispresent()==TRUE)
	InitStrPar(ctrl_username, NULL,
	    (CHARSTRING)asp_params->ctrl__username());

    if(asp_params->ctrl__password().ispresent()==TRUE)
	InitStrPar(ctrl_password, NULL,
	    (CHARSTRING)asp_params->ctrl__password());

    if(asp_params->ctrl__domain().ispresent()==TRUE)
	InitStrPar(ctrl_domain, NULL,
	    (CHARSTRING)asp_params->ctrl__domain());

    if(asp_params->ctrl__readmode().ispresent()==TRUE) {
	if(((TelnetReadMode)asp_params->ctrl__readmode())==
	    TelnetReadMode::UNBUFFERED) {
	    ctrl_readmode = READMODE_UNBUFFERED;
	} else {
	    ctrl_readmode = READMODE_BUFFERED;
	}
    }

    if(asp_params->ctrl__login__skipped().ispresent()==TRUE) {
	if((BOOLEAN)asp_params->ctrl__login__skipped() == TRUE)
	    ctrl_login_skipped = true;
	else ctrl_login_skipped = false;
    }

    if(asp_params->ctrl__detect__server__disconnected().ispresent()==TRUE) {
	if((BOOLEAN)asp_params->ctrl__detect__server__disconnected() == TRUE)
	    ctrl_detect_server_disconnected = true;
	else ctrl_detect_server_disconnected = false;
    }

    if(asp_params->prompts().ispresent()==TRUE &&
	((TelnetPrompts)asp_params->prompts()).size_of() > 0) {
	prompt_list.clear();
	const TelnetPrompts& asp_prompts =
	    (TelnetPrompts)asp_params->prompts();
	unsigned int nof_prompts = asp_prompts.size_of();
	for(unsigned int i=0;i<nof_prompts;i++) {
	    unsigned int id = asp_prompts[i].id();
	    const char *p = asp_prompts[i].prompt();
	    bool is_regex = asp_prompts[i].has__wildcards();
	    if(strlen(p)==0) TTCN_error("%s: prompt parameter must contain at "
		"least one character", port_name);
/*	    if(p[0]=='*')
		TTCN_error("%s: prompt parameter with wildcards shouldn't "
		    "start with \"*\": \"%s\".", port_name, p);
	    if(p[strlen(p)-1]=='*')
		TTCN_error("%s: prompt parameter with wildcards shouldn't "
		    "end with \"*\": \"%s\".", port_name, p);*/
	    prompt_list.set_prompt(id, p, is_regex, isRawRegexpPrompt);
	}
    }

    if(asp_params->ctrl__terminal__type().ispresent()==TRUE)
	InitStrPar(ctrl_terminal_type, NULL,
	    (CHARSTRING)asp_params->ctrl__terminal__type());

    if(asp_params->ctrl__echo().ispresent()==TRUE) {
	if((BOOLEAN)asp_params->ctrl__echo() == TRUE) ctrl_echo = true;
	else ctrl_echo = false;
    }

    if(asp_params->ctrl__CRLF().ispresent()==TRUE) {
	if((BOOLEAN)asp_params->ctrl__CRLF() == TRUE) ctrl_CRLF = true;
	else ctrl_CRLF = false;
    }

    if(asp_params->ctrl__window__size().ispresent()==TRUE) {
	window_size.set_width(((TelnetWindowSize)
	    asp_params->ctrl__window__size()).width());
	window_size.set_height(((TelnetWindowSize)
	    asp_params->ctrl__window__size()).height());
    }

    delete asp_params;
    asp_params = NULL;
}

void TELNETasp__PT::reset_configuration()
{
    free(ctrl_hostname);
    ctrl_hostname = NULL;
    ctrl_portnum = 0;
    free(ctrl_username);
    ctrl_username = NULL;
    free(ctrl_password);
    ctrl_password = NULL;
    free(ctrl_domain);
    ctrl_domain = NULL;
    ctrl_readmode = READMODE_BUFFERED;
    prompt_list.clear();
    ctrl_login_skipped = false;
    ctrl_detect_server_disconnected = false;
    free(ctrl_terminal_type);
    ctrl_terminal_type = NULL;
    ctrl_echo = false;
    ctrl_CRLF = false;
    window_size.reset();
    delete asp_params;
    asp_params = NULL;
    
    free(ctrl_username_client);
    ctrl_username_client = NULL;
    free(ctrl_password_client);
    ctrl_password_client = NULL;
    free(ctrl_server_prompt);
    ctrl_server_prompt = NULL;
    server_mode_def = false;
    isClientLoggedIn = false;
    pass_prompt_send = false;
    login_incorrect = false;
    free(ctrl_password_prompt);
    ctrl_password_prompt = NULL;
    free(ctrl_loginname_prompt);
    ctrl_loginname_prompt = NULL;
    
    debugAllowed = false;
}

void TELNETasp__PT::recv_msg_from_client(int &fd){
  unsigned char inbuf[BUFFER_SIZE];
  unsigned int length;
  
  log_debug("%s: Receive message from client", port_name);
  length = recv(fd, inbuf, BUFFER_SIZE, O_NONBLOCK);
  
  switch (length){
  case -1:{ 
          log_debug("%s: Error reading from file descriptor %d. Errno = %d", port_name, fd, errno);
          errno = 0;
          close_connection(fd);
//          nvtsock=-1;
//          FD_SET(fd_server, &readfds);
//          Install_Handler(&readfds, NULL, NULL, 0.0);
          Handler_Add_Fd_Read(fd_server);

          if(ctrl_detect_client_disconnected){ //notify the server that the client disonnected.
            incoming_message(INTEGER(3));
          }
          break;
  }
  case 0:{
          log_debug("Client closed connection (filedescriptor: %d)", fd);
          close_connection(fd);
//          FD_SET(fd_server, &readfds);
//          Install_Handler(&readfds, NULL, NULL, 0.0);
//          nvtsock=-1;
          Handler_Add_Fd_Read(fd_server);

          if(ctrl_detect_client_disconnected){ //notify the server that the client disonnected.
            incoming_message(INTEGER(3));
          }
          break;
  }
  default:{
            int new_length;
            bool delimiter_came = false;
            if ( (new_length = ProcessNoCmd(inbuf, length)) == 0) return;

            if(debugAllowed) {
                TTCN_logger.log(TTCN_DEBUG,"%s: ------------------------ Logging incoming messages -----------------------------", port_name);
                TTCN_logger.log(TTCN_DEBUG,"             (characters after the first \\n or \\r character will be discarded)");
                log_buffer("Message received from client: (hexstring fromat)", inbuf, length);
                CHARSTRING cc = CHARSTRING((int)length, (const char*)inbuf);
                TTCN_Logger::begin_event(TTCN_DEBUG);
                TTCN_Logger::log_event("%s: Message received from client: (charstring format)\n", port_name);
                cc.log();
                TTCN_Logger::end_event();
                TTCN_logger.log(TTCN_DEBUG,"%s ------------------------ End logging -----------------------------\n", port_name);
            }

            if (!isClientLoggedIn){
              
              int pos;
              for (int i = 0; i < new_length; i++){
                if ( (inbuf[i] == '\n') || (inbuf[i] == '\r')){ delimiter_came = true; pos = i; break; }
              }              

              if (delimiter_came) ttcn_buf.put_s(pos+1, inbuf);
              else ttcn_buf.put_s(new_length, inbuf);

              if(debugAllowed) {
                  TTCN_logger.log(TTCN_DEBUG,"%s: ------------------------ Logging the incoming buffer -----------------------------", port_name);
                  log_buffer("The content of the incoming buffer: (hexstring fromat)", inbuf, length);
                  CHARSTRING cc = CHARSTRING((int)ttcn_buf.get_len(), (const char*)ttcn_buf.get_data());
                  TTCN_Logger::begin_event(TTCN_DEBUG);
                  TTCN_Logger::log_event("%s: The content of the incoming buffert: (charstring format)\n", port_name);
                  cc.log();
                  TTCN_Logger::end_event();
                  TTCN_logger.log(TTCN_DEBUG,"%s ------------------------ End logging -----------------------------\n", port_name);
              }

              if (delimiter_came && !pass_prompt_send){                
                if (!check((const char *)ctrl_username_client, ttcn_buf.get_data(), ttcn_buf.get_len() ) ){
                    login_incorrect = true;
                }
                ttcn_buf.set_pos(ttcn_buf.get_len());
                
                if (::send(fd, "\r\n", 2, 0) < 0 )
                  TTCN_error("TCP send failed");
                
                if (::send(fd, ctrl_password_prompt, strlen(ctrl_password_prompt), 0) < 0 )
                  TTCN_error("TCP send failed");

                pass_prompt_send = true;
              } else if (delimiter_came && pass_prompt_send){
                  int pos = ttcn_buf.get_pos();
                  if (login_incorrect || !check((const char *)ctrl_password_client, ttcn_buf.get_data()+pos, ttcn_buf.get_len()-pos) ) {                    
                    int len = ttcn_buf.get_len();
                    const char *data = (const char*)ttcn_buf.get_data();
                    CHARSTRING cc1( pos - 1, data);
                    CHARSTRING cc2(len - pos - 1, data + pos);
                    TTCN_Logger::begin_event(TTCN_ERROR);
                    TTCN_Logger::log_event("%s: ---------------------------Login incorrect------------------------\n", port_name);
                    TTCN_Logger::log_event("Username: ");
                    cc1.log();
                    TTCN_Logger::log_event("\nPassword: ");
                    cc2.log();
                    if (::send(nvtsock, "Login incorrect\r\n", 17, 0) < 0)
                      TTCN_error("TCP send failed");
                    TTCN_Logger::end_event();
                    TTCN_error("Login incorrect!");
                  }
                  isClientLoggedIn = true;
                  
                  incoming_message(INTEGER(1));
                  ttcn_buf.clear();

                  const char *welcome = "Welcome\r\n";
                  if (::send(fd, welcome, strlen(welcome), 0) < 0)
                    TTCN_error("TCP send failed");
                  if (::send(fd, ctrl_server_prompt, strlen(ctrl_server_prompt),0) < 0)
                    TTCN_error("TCP send failed");
              }              
              
              break;
            }
            
            
            ttcn_buf.put_s(new_length,inbuf);
            
            if(debugAllowed) {
                TTCN_logger.log(TTCN_DEBUG,"%s: ------------------------ Logging the incoming buffer -----------------------------", port_name);
                log_buffer("The content of the incoming buffer: (hexstring fromat)", inbuf, length);
                CHARSTRING cc = CHARSTRING((int)ttcn_buf.get_len(), (const char*)ttcn_buf.get_data());
                TTCN_Logger::begin_event(TTCN_DEBUG);
                TTCN_Logger::log_event("%s: The content of the incoming buffert: (charstring format)\n", port_name);
                cc.log();
                TTCN_Logger::end_event();
                TTCN_logger.log(TTCN_DEBUG,"%s ------------------------ End logging -----------------------------\n", port_name);
            }
           
            int i,len;
            const unsigned char *data;
            for(;;){
                data = ttcn_buf.get_data();
                len = ttcn_buf.get_len();
                for (i = 0; i < len ; i++){
                    if ( (data[i] == '\n' ) || (data[i] == '\r') ){
                        incoming_message(CHARSTRING(i, (const char*)data ));
                        break;
                    }
                }
                if (i == len) break;
                if (i < len - 1 && data[i] == '\r' && ( data[i+1] == '\0' || data[i+1] == '\n') ) i++;
                ttcn_buf.set_pos(i+1);
                ttcn_buf.cut();
                if (i == len) break;
            };
  }
  }

}

int TELNETasp__PT::ProcessNoCmd(unsigned char * buf, int buflen)
{
	unsigned char CMD[1024];
	int cmd_len = 0, i;
	
	log_debug("Processing commands");
        // option negotiation part
	for ( i = 0; i+2 < buflen; i += 3 ) {
	    if(buf[i] == IAC ) {
			switch(buf[i+1] ) {
				case WONT:
				case DONT:
					break;
				case DO:
					CMD[cmd_len++] = IAC;
					CMD[cmd_len++] = WONT;
					CMD[cmd_len++] = buf[i+2];
					break;
				case WILL:
					CMD[cmd_len++] = IAC;
					CMD[cmd_len++] = DONT;
					CMD[cmd_len++] = buf[i+2];
					break;
				default:
					log_debug("%s: Telnet Command not handled: 0x%02x", port_name, buf[i+1]);
			}
		}
		else break;
	}
	if(i!=0) {
		log_debug("Sending commands");
                ::send( nvtsock, (const char*)CMD, cmd_len, 0 );
		ttcn_buf.set_pos(i);
		ttcn_buf.cut();
		log_buffer("ProcessCmd: command sent:", CMD, cmd_len);
		memmove(buf, &buf[i], buflen-i);
		buflen -= i;
	}
	return buflen;
}

bool TELNETasp__PT::check(const char * s1, const unsigned char * s2, int s2_len){
  int s1_len = strlen(s1);
  if (s1_len > s2_len){return false;}
  for (int i = 0; i< s1_len; i++){
    if (s1[i] != s2[i] ) return false;
  }
  return s2[s1_len] == '\n' || s2[s1_len] == '\r' || s2[s1_len] == '\0';
}

INTEGER FindString(const CHARSTRING& msg_ch, const CHARSTRING& pattern_ch)
{
	const char* p;
	int n = 0, x = 0;
	const char* msg = ( const char* ) msg_ch;
	const char* pattern = ( const char* ) pattern_ch;
	int plen = strlen( pattern );
	int mlen = strlen( msg );
	while( x < mlen + plen - 1 )
	{
		if( ( p = strstr( &msg[x], pattern ) ) == NULL ) break;
		else
		{
			x = ( int ) ( p - msg ) + plen;
			n++;
		}
	}
	return n;
}

Window_Size::Window_Size(const char *p_port_name)
	: my_state(WSS_NOT_SET), width(-1), height(-1), ws_debugAllowed(false)
{
    if(p_port_name==NULL) p_port_name = "<telnet>";
    port_name = new char[strlen(p_port_name) + 1];
    strcpy(port_name, p_port_name);
}

Window_Size::~Window_Size()
{
    delete []port_name;
}

void Window_Size::log_debug(const char *fmt, ...)
{ 
  if(ws_debugAllowed)  {
      TTCN_Logger::begin_event(TTCN_DEBUG);
      va_list ap;
      va_start(ap, fmt);
      TTCN_Logger::log_event_va_list(fmt, ap);
      va_end(ap);
      TTCN_Logger::end_event();
  }
}

void Window_Size::set_debug(bool debug_enabled)
{
  ws_debugAllowed = debug_enabled;
}

 
void Window_Size::set_state(WindowSize_state_t new_state)
{
    if(!is_set()) my_state = WSS_NOT_SET;
    else switch(new_state) {
    case WSS_INIT: 
            log_debug("Window_Size::set_state(): can't set "
	    "state to WSS_INIT.");
	break;
    case WSS_WILL_SENT:
	if(my_state == WSS_INIT) my_state = WSS_WILL_SENT;
	else log_debug("Window_Size::set_state(): can't set "
	    "state to WSS_WILL_SENT.");
	break;
    case WSS_AGREED:

	switch(my_state) {
	case WSS_INIT:
	case WSS_WILL_SENT:
	case WSS_AGREED:
	    my_state = WSS_AGREED;
	    break;
	default:
	    log_debug("Window_Size::set_state(): can't set "
		"state to WSS_AGREED.");
	    break;
	}

	break;
    case WSS_REFUSED:
	if(my_state != WSS_REFUSED && is_set()) TTCN_warning("%s: server "
	    "refused window size negotiation.", port_name);
	my_state = WSS_REFUSED;
	break;
    case WSS_NOT_SET:
	TTCN_error("Window_Size::set_state(): state shouldn't be set to "
	    "WSS_NOT_SET explicitly.");
	break;
    default:
	TTCN_error("Window_Size::set_state(): unknown state.");
	break;
    }
}

void Window_Size::set_width(int w)
{
    if(w<=0||w>65535) TTCN_error("%s: width of window size should be greater "
	"than 0 and less than 65536.", port_name);
    width = w;
    if(height > 0 && my_state == WSS_NOT_SET) my_state = WSS_INIT;
}

void Window_Size::set_height(int h)
{
    if(h<=0||h>65535) TTCN_error("%s: height of window size should be greater "
	"than 0 and less than 65536.", port_name);
    height = h;
    if(width > 0 && my_state == WSS_NOT_SET) my_state = WSS_INIT;
}

const unsigned char *Window_Size::encode() const
{
    static unsigned char pdu[] = {
	IAC,
	SB,
	TELOPT_NAWS,
	0, 80,
	0, 24,
	IAC,
	SE
    };
    if(!is_set()) TTCN_error("%s: sending window size option without window "
	"size being set.", port_name);
    pdu[3] = width >> 8;
    pdu[4] = width;
    pdu[5] = height >> 8;
    pdu[6] = height;
    return pdu;
}


Regex_Prompt::Regex_Prompt(const char *p_pattern, bool debug_enabled, bool raw_prompt)
{
    rp_debugAllowed = debug_enabled;
    char *posix_str=NULL;
    if(raw_prompt){
      posix_str = mcopystr(p_pattern);
    }
    else {
      CHARSTRING cstr("(*)("); // string before prompt
      cstr = cstr + p_pattern; // prompt
      cstr = cstr + ")(*)";    // string after prompt
      posix_str = TTCN_pattern_to_regexp_telnet(cstr);
    }
    if(posix_str == NULL) TTCN_error("Cannot convert pattern \"%s\" to "
	"POSIX-equivalent.", p_pattern);
//    posix_str[strlen(posix_str)-1] = '\0'; // remove trailing "$"
    int ret_val=regcomp(&posix_regexp, posix_str, REG_EXTENDED); // +1 -> no ^
    log_debug("Translated pattern (%zd subexpressions): %s",posix_regexp.re_nsub, posix_str);
    Free(posix_str);
    if(ret_val!=0) {
	char err[512];
	regerror(ret_val, &posix_regexp, err, sizeof(err));
	regfree(&posix_regexp);
	TTCN_error("Function regcomp() failed while setting regexp pattern "
	    "\"%s\" as prompt: %s", p_pattern, err);
    }
}

Regex_Prompt::~Regex_Prompt()
{
    regfree(&posix_regexp);
}

void Regex_Prompt::log_debug(const char *fmt, ...)
{
    if(rp_debugAllowed)
    {
      TTCN_Logger::begin_event(TTCN_DEBUG);
      va_list ap;
      va_start(ap, fmt);
      TTCN_Logger::log_event_va_list(fmt, ap);
      va_end(ap);
      TTCN_Logger::end_event();
    }
}

Regex_Prompt_MatchResult Regex_Prompt::match(const char *msg)
{
    Regex_Prompt_MatchResult ret_val;
    regmatch_t pmatch[4];
    int result = regexec(&posix_regexp, msg, 4, pmatch, 0);
/*    for (int i=0;i<4;i++) log_debug(
	"result: %d, start: %u, end: %u", result, pmatch[i].rm_so,
	pmatch[i].rm_eo);*/
    if(result == 0) {
	ret_val.match = true;
	ret_val.start = pmatch[2].rm_so;
	ret_val.end = pmatch[2].rm_eo;
    } else if(result == REG_NOMATCH) {
	ret_val.match = false;
	ret_val.start = 0;
	ret_val.end = 0;
    } else {
	char err[512];
	regerror(result, &posix_regexp, err, sizeof(err));
	TTCN_error("Error matching regexp prompt: %s", err);
    }
    return ret_val;
}


Prompt_List::~Prompt_List()
{
    clear();
}
void Prompt_List::log_debug(const char *fmt, ...)
{
    if(pl_debugAllowed)
    {
      TTCN_Logger::begin_event(TTCN_DEBUG);
      va_list ap;
      va_start(ap, fmt);
      TTCN_Logger::log_event_va_list(fmt, ap);
      va_end(ap);
      TTCN_Logger::end_event();
    }
}

void Prompt_List::set_debug(bool debug_enabled)
{
    pl_debugAllowed = debug_enabled;
}

void Prompt_List::set_prompt(unsigned int p_id, const char *p_prompt,
	bool p_is_regex, bool raw_prompt)
{
    size_t index;
    if( !id_to_index(p_id, index) ) {
	index = n_elems++;
	if(n_elems==1) elems = (prompt_elem**)malloc(sizeof(prompt_elem*));
	else elems = (prompt_elem**)realloc(elems, n_elems
	    *sizeof(prompt_elem*));
	if(!elems) TTCN_error("Not enough memory.");
    } else {
	if(elems[index]->is_regex) delete elems[index]->regex_prompt;
	else delete [](elems[index]->prompt);
	delete elems[index];
    }
    elems[index] = new prompt_elem;
    elems[index]->id = p_id;
    elems[index]->is_regex = p_is_regex;
    if(p_is_regex) {
	elems[index]->regex_prompt = new Regex_Prompt(p_prompt,pl_debugAllowed,raw_prompt);
    } else {
	elems[index]->prompt = new char[strlen(p_prompt)+1];
	strcpy(elems[index]->prompt, p_prompt);
    }
}

void Prompt_List::check(const char *warning_prefix)
{
    if(n_elems) {
	for(unsigned int i=0;i<n_elems-1;i++) {
	    if(elems[i]->is_regex) continue;
	    const char *pi = elems[i]->prompt;
	    for(unsigned int j=i+1;j<n_elems;j++) {
		if(elems[j]->is_regex) continue;
		const char *pj = elems[j]->prompt;
		if(!strcmp(pi, pj)) {
		    TTCN_warning("Duplicated prompt string '%s'", pi);
		    break;
		} else if(strstr(pi, pj)) {
		    TTCN_warning("Using prompt '%s' that is a substring of"
			" prompt '%s' might cause problems", pj, pi);
		    break;
		} else if(strstr(pj, pi)) {
		    TTCN_warning("Using prompt '%s' that is a substring of"
			" prompt '%s' might cause problems", pi, pj);
		    break;
		}
	    }
	}
    }
}

void Prompt_List::clear()
{
    if(elems) {
	for(size_t i=0;i<n_elems;i++) {
	    if(elems[i]->is_regex) delete elems[i]->regex_prompt;
	    else delete [](elems[i]->prompt);
	    delete elems[i];
	}
	free(elems);
	elems = NULL;
    }
    n_elems = 0;
}

int Prompt_List::findPrompt(size_t &pos, const unsigned char *bufptr,
	size_t buflen) const
{
    char *buf_asciiz = NULL;
    for(size_t i=0;i<n_elems;i++) {
	if(elems[i]->is_regex == false) {
	    size_t prompt_len = strlen(elems[i]->prompt);
	    if(prompt_len<=buflen) {
		if(strncmp(elems[i]->prompt,
		    (const char *) (bufptr+buflen-prompt_len), prompt_len)==0) {
		    pos = buflen-prompt_len;
		    if(buf_asciiz) delete []buf_asciiz;
		    return prompt_len;
		}
	    }
	} else {
	    if(!buf_asciiz) {
		buf_asciiz = new char[buflen+1];
		memcpy(buf_asciiz, bufptr, buflen);
		buf_asciiz[buflen] = '\0';
	    }
	    Regex_Prompt_MatchResult result =
		elems[i]->regex_prompt->match(buf_asciiz);
	    if(result.match) {
		pos = result.start;
		delete []buf_asciiz;
		return result.end - result.start;
	    }
	}
    }
    if(buf_asciiz) delete []buf_asciiz;
    return -1;
}

bool Prompt_List::id_to_index(unsigned int p_id, size_t &index) const
{
    for(index=0; index<n_elems; index++)
	if(elems[index]->id == p_id) return true;
    return false;
}

void TELNETasp__PT::close_connection(int& fd){

    if (fd < 0){
        log_debug("close_connection(): file descriptor is negative (%d)", fd);
        return;
    }
//    FD_CLR(fd, &readfds);

//    Install_Handler(&readfds, NULL, NULL, 0.0);
    Handler_Remove_Fd_Read(fd);

    close(fd);
    isClientLoggedIn = false;  
    pass_prompt_send = false;
    login_incorrect = false;
    fd = -1;
}
}
