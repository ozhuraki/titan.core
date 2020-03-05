/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               TELNETasp_PT.hh
//  Description:        TELNET testport header file
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 320
//  Updated:            2012-08-08
//  Contact:            http://ttcn.ericsson.se
//

#ifndef TELNETasp__PT_HH
#define TELNETasp__PT_HH

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <fcntl.h>

#include <memory.h>

#include "TELNETasp_PortType.hh"

#define BUFFER_SIZE (100*1024)
#define READMODE_UNBUFFERED 0
#define READMODE_BUFFERED 1

#define WINDOW_SIZE_LENGTH	9 // IAC SB NAWS x x y y IAC SE
namespace TELNETasp__PortType {
//#define DEBUG_ProcessCmd 1

//extern INTEGER FindString(const CHARSTRING& msg_ch, const CHARSTRING& pattern_ch);


typedef enum {
    WSS_INIT,
    WSS_WILL_SENT,
    WSS_AGREED,
    WSS_REFUSED,
    WSS_NOT_SET
} WindowSize_state_t;

class Window_Size {
    WindowSize_state_t my_state;
    int width, height;
    char *port_name;
    bool ws_debugAllowed;

public:
    Window_Size(const char *p_port_name);
    ~Window_Size();
    inline void reset() { my_state = WSS_NOT_SET; width = -1; height = -1; ws_debugAllowed = false; }
    void set_state(WindowSize_state_t new_state);
    inline WindowSize_state_t get_state() const { return my_state; }
    void set_width(int w);
    inline int get_width() const { return width; }
    void set_height(int h);
    inline int get_height() const { return height; }
    const unsigned char *encode() const;
    inline bool is_set() const { return width > 0 && height > 0; }
    void log_debug(const char *fmt, ...);
    void set_debug(bool debug_enabled);
};

typedef struct {
    bool match;
    unsigned int start;
    unsigned int end;
} Regex_Prompt_MatchResult;

class Regex_Prompt {
    regex_t posix_regexp;
    bool rp_debugAllowed;
    
public:
    Regex_Prompt(const char *p_pattern, bool debug_enabled, bool raw_prompt);
    ~Regex_Prompt();
    Regex_Prompt_MatchResult match(const char *msg);
    void log_debug(const char *fmt, ...);
};

class Prompt_List {
    typedef struct {
	unsigned int id;
	bool is_regex;
	union {
	    char *prompt;
	    Regex_Prompt *regex_prompt;
	};
    } prompt_elem;
    prompt_elem **elems;
    size_t n_elems;
    bool id_to_index(unsigned int p_id, size_t &index) const;
    bool pl_debugAllowed;
public:
    Prompt_List() : elems(NULL), n_elems(0), pl_debugAllowed(false) { }
    ~Prompt_List();
    void set_prompt(unsigned int p_id, const char *p_prompt, bool p_is_regex, bool raw_prompt = false);
    void check(const char *warning_prefix);
    void clear();
    size_t nof_prompts() const { return n_elems; }
    int findPrompt(size_t &pos, const unsigned char *bufptr, size_t buflen) const;
    void log_debug(const char *fmt, ...);
    void set_debug(bool debug_enabled);
};


class TELNETasp__PT : public TELNETasp__PT_BASE {
public:
	TELNETasp__PT(const char *par_port_name = NULL);
	~TELNETasp__PT();

	void set_parameter(const char *parameter_name,
		const char *parameter_value);

  void Handle_Fd_Event_Readable(int fd);
/*	void Event_Handler(const fd_set *read_fds,
		const fd_set *write_fds, const fd_set *error_fds,
		double time_since_last_call);
*/


protected:
	void user_map(const char *system_port);
	void user_unmap(const char *system_port);

	void user_start();
	void user_stop();

	void outgoing_send(const CHARSTRING& send_par);
	void outgoing_send(const ASP__TelnetPortParameters& send_par);
	void outgoing_send(const ASP__TelnetDynamicConfig& send_par);
	void outgoing_send(const ASP__TelnetConnection& send_par);
        void outgoing_send(const ASP__TelnetClose& send_par);

	void	InitStrPar(char *&par, const char *name, const char *val);
	int 	ProcessCmd(unsigned char * buf, int buflen);
        int 	ProcessNoCmd(unsigned char * buf, int buflen);
	void	LoginCtrl( const char* buf, const char* nam, const char* pw );
	int 	isPrompt(size_t& pos); // returns the length of the found prompt
	int 	RecvClrMsg();
	void	log_buffer(const char * logmsg,
				const unsigned char * buf, size_t buflen);
        void    log_debug(const char *fmt, ...);
	bool	buf_strcmp(const char * s1, const unsigned char * s2,
				size_t s2_len, size_t& pos);

	// Class variables and constants
	int 	nvtsock;
	bool 	isConnected;
	bool 	isLoginSent;
	bool	isPasswordSent;
	bool	isDomainSent;
  bool  isRawRegexpPrompt;
	CHARSTRING	echobuf;
	TTCN_Buffer ttcn_buf;
	struct hostent	*he;
	bool	config_finished;
	
  int map_poll_timeout;
  
	// parameters
	char*	ctrl_hostname;
	int 	ctrl_portnum;
	char*	ctrl_username;
	char* 	ctrl_password;
	char*	ctrl_domain;
	int 	ctrl_readmode;
	bool	ctrl_login_skipped;
	bool	ctrl_detect_server_disconnected;
	bool	ctrl_detect_connection_establishment_result;
        bool    ctrl_client_cleanup_linefeed;
	char*	ctrl_terminal_type;
	bool	ctrl_echo;
	bool	ctrl_CRLF;
	Prompt_List prompt_list;
	Window_Size window_size;
        
        char*   ctrl_username_client;
        char*   ctrl_password_client;
        char*   ctrl_server_prompt;
        char*   ctrl_password_prompt;
        char*   ctrl_loginname_prompt;
        bool    server_mode_def;
        bool    ctrl_server_attach_prompt;
        bool    ctrl_detect_client_disconnected;
        bool    ctrl_server_failsafe_sending;
        bool    server_mode;
        
        bool    port_mapped;
        int     fd_server;
        bool    isClientLoggedIn;
        bool    pass_prompt_send;
        bool    login_incorrect;

        bool    debugAllowed;

        bool    emptyEcho;
        bool    suppressed;

        void recv_msg_from_client(int& fd);
        void close_connection(int& fd);
        bool check(const char * s1, const unsigned char * s2, int s2_len);
        
//        fd_set readfds;
        
        ASP__TelnetPortParameters *asp_params;
        void set_asp_params();
        void reset_configuration();
};
}
#endif
