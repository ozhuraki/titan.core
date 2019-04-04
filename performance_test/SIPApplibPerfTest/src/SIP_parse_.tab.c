/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         SIP_parse_parse
#define yylex           SIP_parse_lex
#define yyerror         SIP_parse_error
#define yylval          SIP_parse_lval
#define yychar          SIP_parse_char
#define yydebug         SIP_parse_debug
#define yynerrs         SIP_parse_nerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "SIP_parse.y"


/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               SIP_parse.y
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 319
//  Updated:            2012-02-19
//  Contact:            http://ttcn.ericsson.se
//  Reference:          RFC3261, RFC2806, RFC2976, RFC3262, RFC3311, RFC3323, 
//                      RFC3325, RFC3326, RFC3265, RFC3455, RFC4244, RFC4538,
//                      RFC6442, RFC6086, RFC6050
//                      IETF Draft draft-ietf-dip-session-timer-15.txt,
//                      IETF Draft draft-levy-sip-diversion-08.txt, RFC5009
//                      IETF draft-ott-sip-serv-indication-notification-00
//                      IETF draft-holmberg-sipcore-proxy-feature-04,
//                      531/0363-FCP 101 5091

/* C declarations */
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "memory.h"

#include "SIPmsg_Types.hh"

#define YYDEBUG 1
using namespace SIPmsg__Types;
extern const char *myinputptr;
extern const char *myinputlim;
  extern void init_SIP_parse_lex();
  struct yy_buffer_state;
  extern yy_buffer_state *SIP_parse__scan_bytes(const char*, int);
  extern void SIP_parse__delete_buffer(yy_buffer_state*);
extern int SIP_parse_error(const char *);
extern int SIP_parse_lex();
extern int SIP_parse_parse();
extern void initcounters();
extern int SIP_parse_lex_destroy();
//Response *msgptr;

RequestLine *rqlineptr;
StatusLine *stlineptr;
MessageHeader *headerptr;
int wildcarded_enabled_parser;

// header part pointers
extern SipUrl *uriptr;
extern GenericParam__List *paramptr;
extern OptionTag__List *optptr;
extern RouteBody__List *routeptr;


// parameter counters
extern int paramcount;
extern int urlparamcount;
extern int headercount;
extern int optioncount;
extern int rcount;

// header counters
extern int acceptcount;
extern int aceptenccount;
extern int acceptlangcount;
extern int alertinfcount;
extern int allowcount;
extern int callinfocount;
extern int contactcount;
extern int contentenccount;
extern int contentlangcount;
extern int errorinfocount;
extern int featureCapscount;
extern int inreplytocount;
extern int passertedidcount;
extern int ppreferredidcount;
extern int privacycount;
extern int proxyreqcount;
extern int reasoncount;
extern int recroutecount;
extern int recvinfocount;
extern int routecount;
extern int reqcount;
extern int suppcount;
extern int unsuppcount;
extern int servercount;
extern int useragentcount;
extern int viacount;
extern int warncount;
extern int undefcount;
extern int aos_count;

char comment_buf[1500];
int errorind_loc;

char *trim(char *string);

bool ipv6enabled;
char *trimOnIPv6(char *string);
void resetptr();

extern char * stream_buffer; // EPTEBAL


/* Line 268 of yacc.c  */
#line 194 "SIP_parse_.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     _SP = 258,
     LONE_CRLF = 259,
     EXTENSION_CODE = 260,
     REASON_PHRASE = 261,
     SIP_VERSION = 262,
     SIP_SPHT_VERSION = 263,
     _METHOD = 264,
     CONTENT_WITHOUTENDINGCRLF = 265,
     _CALLID = 266,
     WWW_AUTHENTICATELWSCOLON = 267,
     _CRLF = 268,
     PROXY_AUTHENTICATELWSCOLON = 269,
     TOLWSCOLON = 270,
     CONTACTLWSCOLON = 271,
     ACCEPTRESPRIOLWSCOLON = 272,
     RESPRIOLWSCOLON = 273,
     FROMLWSCOLON = 274,
     _TYPEID = 275,
     TOKENS = 276,
     QUOTED_STRING = 277,
     EPARENT = 278,
     SPARENT = 279,
     OTHERLWSCOLON = 280,
     CALL_IDLWSCOLON = 281,
     SOMELWS = 282,
     _LAES_CONTENT = 283,
     _TOKEN = 284,
     _STOKEN = 285,
     _DIGEST = 286,
     _COMMENT = 287,
     USERINFO_AT = 288,
     _STAR = 289,
     GEOLOCERRORLWSCOLON = 290,
     GEOLOCATIONLWSCOLON = 291,
     SUPPORTEDLWSCOLON = 292,
     VIALWSCOLON = 293,
     SUBJECTLWSCOLON = 294,
     CONTENT_ENCODINGLWSCOLON = 295,
     CONTENT_LENGTHLWSCOLON = 296,
     SOMEDIGITS = 297,
     CONTENT_TYPELWSCOLON = 298,
     ACCEPTLWSCOLON = 299,
     ACCEPT_ENCODINGLWSCOLON = 300,
     ACCEPT_LANGUAGELWSCOLON = 301,
     AUTHINFOLWSCOLON = 302,
     CALL_INFOLWSCOLON = 303,
     MINEXPIRESLWSCOLON = 304,
     CSEQLWSCOLON = 305,
     EVENTLWSCOLON = 306,
     ALLOWEVENTLWSCOLON = 307,
     SUBSTATEWSCOLON = 308,
     DATELWSCOLON = 309,
     MIME_VERSIONLWSCOLON = 310,
     ORGANIZATIONLWSCOLON = 311,
     RECORD_ROUTELWSCOLON = 312,
     PCALLEPPTYLWSCOLON = 313,
     PVISITEDNETLWSCOLON = 314,
     REQUIRELWSCOLON = 315,
     HISTORYLWSCOLON = 316,
     TIMESTAMPLWSCOLON = 317,
     USER_AGENTLWSCOLON = 318,
     PASSOCURILWSCOLON = 319,
     DIVERSIONWSCOLON = 320,
     ERROR_INFOLWSCOLON = 321,
     RETRY_AFTERLWSCOLON = 322,
     SERVERLWSCOLON = 323,
     UNSUPPORTEDLWSCOLON = 324,
     WARNINGLWSCOLON = 325,
     PASSERTEDLWSCOLON = 326,
     REASONLWSCOLON = 327,
     PPREFERREDLWSCOLON = 328,
     PRIVACYLWSCOLON = 329,
     RACKLWSCOLON = 330,
     RSEQLWSCOLON = 331,
     ALERT_INFOLWSCOLON = 332,
     AUTHORIZATIONLWSCOLON = 333,
     IN_REPLY_TOLWSCOLON = 334,
     REPLY_TOLWSCOLON = 335,
     MAX_FORWARDSLWSCOLON = 336,
     REFER_TOLWSCOLON = 337,
     PRIORITYLWSCOLON = 338,
     PROXY_AUTHORIZATIONLWSCOLON = 339,
     PROXY_REQUIRELWSCOLON = 340,
     ROUTELWSCOLON = 341,
     ALLOWLWSCOLON = 342,
     PEARLYMEDIALWSCOLON = 343,
     CONTENT_DISPOSITIONLWSCOLON = 344,
     CONTENT_LANGUAGELWSCOLON = 345,
     EXPIRESLWSCOLON = 346,
     SESSIONEXPWSCOLON = 347,
     SESSIONIDLWSCOLON = 348,
     MINSELWSCOLON = 349,
     SOMELWSCOMMA = 350,
     PACCESSNETLWSCOLON = 351,
     REQDISPLWSCOLON = 352,
     PCHARGEADDRLWSCOLON = 353,
     PCHARGEVECTORLWSCOLON = 354,
     ANSWERMODELWSCOLON = 355,
     PRIVANSWERMODELWSCOLON = 356,
     ALERTMODELWSCOLON = 357,
     REFERSUBLWSCOLON = 358,
     PALERTINGMODELWSCOLON = 359,
     PANSWERSTATELWSCOLON = 360,
     ACCEPTCONTACTLWSCOLON = 361,
     REJECTCONTACTLWSCOLON = 362,
     PMEDIAAUTHLWSCOLON = 363,
     PATHLWSCOLON = 364,
     SECCLIENTLWSCOLON = 365,
     SECSERVERLWSCOLON = 366,
     SECVERIFYLWSCOLON = 367,
     PTARCEPTYIDLWSCOLON = 368,
     POSPSLWSCOLON = 369,
     PBILLINGINFOLWSCOLON = 370,
     PLAESLWSCOLON = 371,
     PDCSREDIRLWSCOLON = 372,
     PUSERDBASELWSCOLON = 373,
     SERVICEROUTELWSCOLON = 374,
     REFERREDBYTOLWSCOLON = 375,
     REPLACESLWSCOLON = 376,
     IFMATCHLWSCOLON = 377,
     ETAGLWSCOLON = 378,
     JOINLWSCOLON = 379,
     PSERVEDUSERLWSCOLON = 380,
     PPROFILEKEYLWSCOLON = 381,
     PSERVICEINDICATIONLWSCOLON = 382,
     PSERVICENOTIFICATIONLWSCOLON = 383,
     GEOROUTINGLWSCOLON = 384,
     TARGETDIALOGLWSCOLON = 385,
     INFOPACKAGELWSCOLON = 386,
     FEATURECAPSLWSCOLON = 387,
     RECVINFOLWSCOLON = 388,
     _TOKEN_NO_DOT = 389,
     _HEXTOKEN = 390,
     _DOT = 391,
     _SLASH = 392,
     SEMICOLON = 393,
     EQUALSIGN = 394,
     _ABO = 395,
     _ABC = 396,
     _COLON = 397,
     QUESTMARK = 398,
     AMPERSANT = 399,
     SOMELWS_SEMICOLON = 400,
     _AT = 401,
     _HOST = 402,
     SCHEME = 403,
     _HNAME = 404,
     _HVALUE = 405,
     _PORT = 406,
     SOMELWS_COMMA_SOMELWS = 407,
     SOMELWS_SLASH_SOMELWS = 408,
     PROTOCOL_NAME = 409,
     PROTOCOL_VERSION = 410,
     TRANSPORT = 411,
     _URLTOKEN = 412,
     PASSERTEDSERVICELWSCOLON = 413,
     PPREFERREDSERVICELWSCOLON = 414,
     XAUTLWSCOLON = 415,
     XCARRIERINFOLWSCOLON = 416,
     XCHGDELAYLWSCOLON = 417,
     PAREAINFOLWSCOLON = 418,
     XCHGINFOLWSCOLON = 419
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 119 "SIP_parse.y"

  unsigned int iv;
  char cv  ;
  char *sv ;
  SIPmsg__Types::Event__type *e_type;
  SIPmsg__Types::Event__template__list *e_template_list;
  SIPmsg__Types::Event__type__list *e_list;
  SIPmsg__Types::NameAddr *naddr;
  SIPmsg__Types::P__Assoc__uri__spec__list *u_spec_list;
  SIPmsg__Types::P__Assoc__uri__spec *u_spec;
  SIPmsg__Types::Network__spec *n_spec;
  SIPmsg__Types::Network__spec__list *n_spec_list;
  SIPmsg__Types::Access__net__spec *an_spec;
  SIPmsg__Types::Access__net__spec__list *an_spec_list;
  SIPmsg__Types::GenericParam__List *p_list;
  SIPmsg__Types::Contact__list *c_list;
  SIPmsg__Types::Request__disp__directive__list *d_list;
  SIPmsg__Types::Media__auth__token__list *a_t_list;
  SIPmsg__Types::RouteBody *route_val;
  SIPmsg__Types::RouteBody__List *route_list;
  SIPmsg__Types::Security__mechanism *sec_mech;
  SIPmsg__Types::Security__mechanism__list *sec_mech_list;
  SIPmsg__Types::HostPort *host_p;
  SIPmsg__Types::Diversion__params *divparam;
  SIPmsg__Types::Diversion__params__list *divparam_list;
  SIPmsg__Types::Hi__Entry *history_entry;
  SIPmsg__Types::Hi__Entry__list *history_entry_list;
  SIPmsg__Types::Em__param__List *em_bdy_list;
  SIPmsg__Types::Service__ID__List *service_1toN;
  SIPmsg__Types::Rvalue__List *r_val_list;
  SIPmsg__Types::Rvalue *rval;
  SIPmsg__Types::Location__value__list *g_val_list;
  SIPmsg__Types::Location__value *g_val;



/* Line 293 of yacc.c  */
#line 431 "SIP_parse_.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 443 "SIP_parse_.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1691

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  165
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  239
/* YYNRULES -- Number of rules.  */
#define YYNRULES  558
/* YYNRULES -- Number of states.  */
#define YYNSTATES  1108

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   419

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,    11,    18,    21,    22,    24,
      26,    29,    31,    33,    35,    37,    40,    43,    46,    49,
      52,    55,    58,    61,    63,    66,    69,    74,    78,    83,
      86,    90,    93,    96,   101,   104,   107,   112,   116,   120,
     124,   128,   132,   136,   140,   146,   149,   152,   155,   158,
     161,   164,   167,   170,   173,   176,   179,   182,   185,   188,
     191,   199,   206,   211,   218,   224,   228,   236,   243,   248,
     255,   261,   265,   273,   280,   285,   292,   298,   302,   306,
     310,   318,   325,   330,   337,   343,   347,   350,   352,   356,
     361,   367,   370,   374,   378,   382,   384,   389,   393,   396,
     399,   403,   407,   410,   413,   418,   421,   423,   427,   434,
     440,   446,   451,   454,   458,   465,   469,   470,   472,   474,
     477,   479,   481,   483,   485,   488,   491,   494,   497,   500,
     503,   506,   509,   512,   515,   518,   521,   524,   529,   531,
     535,   538,   540,   544,   549,   555,   559,   567,   576,   581,
     583,   586,   590,   592,   596,   600,   602,   605,   609,   613,
     615,   620,   625,   631,   635,   637,   641,   643,   646,   649,
     652,   655,   658,   661,   664,   667,   670,   673,   676,   679,
     682,   685,   688,   691,   694,   697,   700,   703,   706,   709,
     712,   715,   718,   721,   724,   727,   730,   733,   736,   739,
     742,   745,   748,   751,   754,   757,   760,   763,   766,   769,
     772,   775,   778,   781,   784,   787,   790,   793,   796,   799,
     802,   805,   808,   811,   814,   817,   820,   823,   826,   829,
     832,   835,   838,   841,   844,   847,   850,   853,   856,   859,
     862,   865,   868,   871,   874,   877,   880,   883,   886,   889,
     892,   894,   897,   899,   903,   906,   910,   914,   919,   923,
     928,   931,   933,   937,   940,   944,   948,   953,   957,   963,
     966,   968,   972,   977,   983,   986,   989,   992,   997,  1001,
    1003,  1006,  1009,  1014,  1018,  1023,  1027,  1031,  1035,  1040,
    1043,  1049,  1056,  1060,  1065,  1069,  1075,  1077,  1081,  1083,
    1085,  1090,  1094,  1096,  1104,  1113,  1117,  1120,  1123,  1126,
    1129,  1131,  1135,  1138,  1142,  1145,  1147,  1151,  1158,  1164,
    1170,  1175,  1178,  1181,  1186,  1189,  1192,  1197,  1200,  1203,
    1205,  1209,  1212,  1216,  1220,  1225,  1229,  1234,  1238,  1243,
    1246,  1248,  1252,  1254,  1257,  1260,  1263,  1265,  1269,  1272,
    1274,  1278,  1281,  1285,  1289,  1294,  1298,  1303,  1307,  1312,
    1316,  1321,  1325,  1330,  1334,  1339,  1342,  1345,  1347,  1351,
    1354,  1358,  1361,  1365,  1368,  1370,  1374,  1376,  1379,  1382,
    1384,  1388,  1390,  1393,  1399,  1404,  1408,  1413,  1416,  1419,
    1424,  1428,  1433,  1435,  1439,  1442,  1447,  1450,  1452,  1456,
    1459,  1463,  1466,  1468,  1472,  1473,  1479,  1484,  1487,  1490,
    1492,  1496,  1497,  1503,  1508,  1511,  1514,  1517,  1522,  1524,
    1532,  1536,  1539,  1546,  1552,  1556,  1562,  1567,  1570,  1573,
    1576,  1580,  1583,  1586,  1590,  1595,  1599,  1605,  1611,  1616,
    1621,  1625,  1627,  1630,  1634,  1638,  1644,  1648,  1650,  1654,
    1658,  1664,  1670,  1678,  1686,  1696,  1698,  1701,  1704,  1707,
    1710,  1714,  1720,  1723,  1725,  1729,  1734,  1740,  1744,  1745,
    1747,  1749,  1752,  1754,  1758,  1763,  1769,  1771,  1774,  1777,
    1782,  1784,  1787,  1789,  1793,  1796,  1800,  1803,  1807,  1811,
    1814,  1818,  1822,  1830,  1837,  1842,  1849,  1855,  1859,  1862,
    1866,  1870,  1873,  1875,  1879,  1886,  1892,  1896,  1902,  1907,
    1910,  1918,  1925,  1930,  1937,  1943,  1947,  1950,  1954,  1959,
    1963,  1966,  1968,  1971,  1973,  1977,  1980,  1982,  1984,  1986,
    1989,  1991,  1994,  1996,  2000,  2003,  2006,  2009,  2011,  2017,
    2022,  2027,  2031,  2032,  2034,  2037,  2038,  2041,  2043,  2046,
    2049,  2052,  2054,  2058,  2059,  2061,  2063,  2066,  2069
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     166,     0,    -1,   167,    -1,   212,    -1,   168,   169,     4,
      -1,     9,     3,   392,     3,     7,    13,    -1,     1,    13,
      -1,    -1,   170,    -1,   171,    -1,   170,   171,    -1,   236,
      -1,   188,    -1,   217,    -1,   172,    -1,     1,    13,    -1,
     173,    13,    -1,   175,    13,    -1,   177,    13,    -1,   179,
      13,    -1,   181,    13,    -1,   182,    13,    -1,   183,    13,
      -1,    87,    -1,    87,   174,    -1,   362,    29,    -1,   174,
      95,   362,    29,    -1,    89,   362,    29,    -1,    89,   362,
      29,   176,    -1,   145,   382,    -1,   176,   145,   382,    -1,
      40,   178,    -1,   362,    30,    -1,   178,    95,   362,    30,
      -1,    90,   180,    -1,   362,    30,    -1,   180,    95,   362,
      30,    -1,    41,   362,    42,    -1,    43,   362,    20,    -1,
      91,   362,    29,    -1,   160,   362,    29,    -1,   161,   362,
      29,    -1,   162,   362,    29,    -1,   164,   362,    29,    -1,
     164,   362,    29,   145,    29,    -1,   195,    13,    -1,   198,
      13,    -1,   200,    13,    -1,   202,    13,    -1,   203,    13,
      -1,   204,    13,    -1,   206,    13,    -1,   208,    13,    -1,
     211,    13,    -1,   194,    13,    -1,   191,    13,    -1,   190,
      13,    -1,   189,    13,    -1,   219,    13,    -1,   220,    13,
      -1,   120,   362,   390,   140,   391,   141,   381,    -1,   120,
     362,   140,   391,   141,   381,    -1,   120,   362,   388,   381,
      -1,   120,   362,   390,   140,   391,   141,    -1,   120,   362,
     140,   391,   141,    -1,   120,   362,   388,    -1,   126,   362,
     390,   140,   391,   141,   381,    -1,   126,   362,   140,   391,
     141,   381,    -1,   126,   362,   388,   381,    -1,   126,   362,
     390,   140,   391,   141,    -1,   126,   362,   140,   391,   141,
      -1,   126,   362,   388,    -1,   125,   362,   390,   140,   391,
     141,   381,    -1,   125,   362,   140,   391,   141,   381,    -1,
     125,   362,   388,   381,    -1,   125,   362,   390,   140,   391,
     141,    -1,   125,   362,   140,   391,   141,    -1,   125,   362,
     388,    -1,   127,   362,    29,    -1,   128,   362,    29,    -1,
      82,   362,   390,   140,   391,   141,   381,    -1,    82,   362,
     140,   391,   141,   381,    -1,    82,   362,   388,   381,    -1,
      82,   362,   390,   140,   391,   141,    -1,    82,   362,   140,
     391,   141,    -1,    82,   362,   388,    -1,    77,   196,    -1,
     197,    -1,   197,    95,   196,    -1,   362,   140,   157,   141,
      -1,   362,   140,   157,   141,   381,    -1,    78,   199,    -1,
     362,    31,   222,    -1,   362,    29,   222,    -1,    79,   362,
     201,    -1,    11,    -1,   201,    95,   362,    11,    -1,    81,
     362,    42,    -1,    83,    10,    -1,    84,   205,    -1,   362,
      31,   222,    -1,   362,    29,   222,    -1,    85,   207,    -1,
     362,    29,    -1,   207,    95,   362,    29,    -1,    86,   209,
      -1,   210,    -1,   209,    95,   210,    -1,   362,   390,   140,
     391,   141,   376,    -1,   362,   140,   391,   141,   376,    -1,
     362,   390,   140,   391,   141,    -1,   362,   140,   391,   141,
      -1,    39,    10,    -1,   213,   214,     4,    -1,     8,     3,
       5,     3,     6,    13,    -1,     1,     6,    13,    -1,    -1,
     215,    -1,   216,    -1,   215,   216,    -1,   236,    -1,   188,
      -1,   217,    -1,   172,    -1,     1,    13,    -1,   223,    13,
      -1,   373,    13,    -1,   226,    13,    -1,   228,    13,    -1,
     231,    13,    -1,   218,    13,    -1,   232,    13,    -1,   371,
      13,    -1,    47,   222,    -1,   158,   221,    -1,   159,   221,
      -1,   362,    29,    -1,   221,    95,   362,    29,    -1,   382,
      -1,   382,    95,   222,    -1,    66,   224,    -1,   225,    -1,
     225,    95,   224,    -1,   362,   140,   157,   141,    -1,   362,
     140,   157,   141,   381,    -1,    67,   362,    29,    -1,    67,
     362,    29,   362,    24,   227,    23,    -1,    67,   362,    29,
     362,    24,   227,    23,   381,    -1,    67,   362,    29,   381,
      -1,    32,    -1,   227,    32,    -1,    68,   362,   229,    -1,
     230,    -1,   229,    27,   230,    -1,    24,   227,    23,    -1,
      29,    -1,    69,   207,    -1,    70,   362,   233,    -1,    70,
       3,   233,    -1,   234,    -1,   233,    95,     3,   234,    -1,
     233,    95,   362,   234,    -1,   151,     3,   235,     3,    22,
      -1,    29,   142,   151,    -1,    29,    -1,   147,   142,   151,
      -1,   147,    -1,   363,    13,    -1,   366,    13,    -1,   368,
      13,    -1,   361,    13,    -1,   358,    13,    -1,   377,    13,
      -1,   357,    13,    -1,   356,    13,    -1,   347,    13,    -1,
     320,    13,    -1,   318,    13,    -1,   250,    13,    -1,   251,
      13,    -1,   380,    13,    -1,   290,    13,    -1,   348,    13,
      -1,   355,    13,    -1,   332,    13,    -1,   310,    13,    -1,
     313,    13,    -1,   309,    13,    -1,   326,    13,    -1,   306,
      13,    -1,   329,    13,    -1,   296,    13,    -1,   294,    13,
      -1,   293,    13,    -1,   335,    13,    -1,   323,    13,    -1,
     336,    13,    -1,   354,    13,    -1,   337,    13,    -1,   353,
      13,    -1,   352,    13,    -1,   289,    13,    -1,   287,    13,
      -1,   288,    13,    -1,   317,    13,    -1,   351,    13,    -1,
     375,    13,    -1,   349,    13,    -1,   340,    13,    -1,   284,
      13,    -1,   283,    13,    -1,   281,    13,    -1,   279,    13,
      -1,   276,    13,    -1,   271,    13,    -1,   272,    13,    -1,
     273,    13,    -1,   270,    13,    -1,   269,    13,    -1,   254,
      13,    -1,   268,    13,    -1,   263,    13,    -1,   262,    13,
      -1,   261,    13,    -1,   260,    13,    -1,   259,    13,    -1,
     257,    13,    -1,   258,    13,    -1,   256,    13,    -1,   192,
      13,    -1,   193,    13,    -1,   299,    13,    -1,   300,    13,
      -1,   301,    13,    -1,   302,    13,    -1,   303,    13,    -1,
     304,    13,    -1,   247,    13,    -1,   246,    13,    -1,   245,
      13,    -1,   241,    13,    -1,   242,    13,    -1,   240,    13,
      -1,   237,    13,    -1,   184,    13,    -1,   185,    13,    -1,
     186,    13,    -1,   305,    13,    -1,   187,    13,    -1,   339,
      13,    -1,   133,    -1,   133,   238,    -1,   239,    -1,   239,
      95,   238,    -1,   362,    29,    -1,   362,    29,   381,    -1,
     131,   362,    29,    -1,   131,   362,    29,   381,    -1,   130,
     362,    11,    -1,   130,   362,    11,   381,    -1,   132,   243,
      -1,   244,    -1,   244,    95,   243,    -1,   362,    29,    -1,
     362,    29,   381,    -1,    35,   362,    42,    -1,    35,   362,
      42,   381,    -1,   129,   362,    29,    -1,   129,   362,    29,
     362,   384,    -1,    36,   248,    -1,   249,    -1,   248,    95,
     249,    -1,   362,   140,   391,   141,    -1,   362,   140,   391,
     141,   381,    -1,    17,   252,    -1,    18,   252,    -1,   362,
     253,    -1,   252,    95,   362,   253,    -1,   134,   136,   134,
      -1,    88,    -1,    88,   255,    -1,   362,    29,    -1,   255,
      95,   362,    29,    -1,   124,   362,    11,    -1,   124,   362,
      11,   381,    -1,   123,   362,    29,    -1,   122,   362,    29,
      -1,   121,   362,    11,    -1,   121,   362,    11,   381,    -1,
     119,   277,    -1,   118,   362,   140,   391,   141,    -1,   118,
     362,   140,   391,   141,   381,    -1,   117,   391,   141,    -1,
     117,   391,   141,   381,    -1,   116,   362,   267,    -1,   116,
     362,   267,   145,   264,    -1,   265,    -1,   265,   145,   264,
      -1,   266,    -1,   382,    -1,   362,    28,   362,   267,    -1,
     147,   142,   151,    -1,   147,    -1,   115,   362,   135,   153,
     135,   146,   147,    -1,   115,   362,   135,   153,   135,   146,
     147,   381,    -1,   114,   362,    29,    -1,   113,   316,    -1,
     110,   274,    -1,   111,   274,    -1,   112,   274,    -1,   275,
      -1,   274,    95,   275,    -1,   362,    29,    -1,   362,    29,
     381,    -1,   109,   277,    -1,   278,    -1,   277,    95,   278,
      -1,   362,   390,   140,   391,   141,   376,    -1,   362,   140,
     391,   141,   376,    -1,   362,   390,   140,   391,   141,    -1,
     362,   140,   391,   141,    -1,   108,   280,    -1,   362,    29,
      -1,   280,    95,   362,    29,    -1,    97,   282,    -1,   362,
      29,    -1,   282,    95,   362,    29,    -1,   107,   285,    -1,
     106,   285,    -1,   286,    -1,   285,    95,   286,    -1,   362,
      34,    -1,   362,    34,   381,    -1,    92,   362,    29,    -1,
      92,   362,    29,   381,    -1,    93,   362,    29,    -1,    93,
     362,    29,   381,    -1,    94,   362,    29,    -1,    94,   362,
      29,   381,    -1,    61,   291,    -1,   292,    -1,   291,    95,
     292,    -1,   316,    -1,   316,   381,    -1,    99,   295,    -1,
      98,   295,    -1,   382,    -1,   295,   145,   382,    -1,    96,
     297,    -1,   298,    -1,   297,    95,   298,    -1,   362,   390,
      -1,   362,   390,   381,    -1,   100,   362,    29,    -1,   100,
     362,    29,   381,    -1,   101,   362,    29,    -1,   101,   362,
      29,   381,    -1,   102,   362,    29,    -1,   102,   362,    29,
     381,    -1,   103,   362,    29,    -1,   103,   362,    29,   381,
      -1,   104,   362,    29,    -1,   104,   362,    29,   381,    -1,
     105,   362,    29,    -1,   105,   362,    29,   381,    -1,   163,
     295,    -1,    59,   307,    -1,   308,    -1,   307,    95,   308,
      -1,   362,   390,    -1,   362,   390,   381,    -1,    58,   316,
      -1,    58,   316,   381,    -1,    64,   311,    -1,   312,    -1,
     311,    95,   312,    -1,   316,    -1,   316,   381,    -1,    65,
     314,    -1,   315,    -1,   314,    95,   315,    -1,   316,    -1,
     316,   381,    -1,   362,   390,   140,   391,   141,    -1,   362,
     140,   391,   141,    -1,    53,   362,    29,    -1,    53,   362,
      29,   381,    -1,    52,   319,    -1,   362,   321,    -1,   319,
      95,   362,   321,    -1,    51,   362,   321,    -1,    51,   362,
     321,   381,    -1,   134,    -1,   134,   136,   322,    -1,   362,
     134,    -1,   322,   136,   362,   134,    -1,    72,   324,    -1,
     325,    -1,   324,    95,   325,    -1,   362,    29,    -1,   362,
      29,   176,    -1,    71,   327,    -1,   328,    -1,   327,    95,
     328,    -1,    -1,   362,   390,   140,   391,   141,    -1,   362,
     140,   391,   141,    -1,   362,   391,    -1,    73,   330,    -1,
     331,    -1,   330,    95,   331,    -1,    -1,   362,   390,   140,
     391,   141,    -1,   362,   140,   391,   141,    -1,   362,   388,
      -1,    74,   333,    -1,   362,   334,    -1,   333,   145,   362,
     334,    -1,    29,    -1,    75,   362,    42,    27,    42,    27,
      29,    -1,    76,   362,    42,    -1,    80,   338,    -1,   362,
     390,   140,   391,   141,   376,    -1,   362,   140,   391,   141,
     376,    -1,   362,   388,   376,    -1,   362,   390,   140,   391,
     141,    -1,   362,   140,   391,   141,    -1,   362,   388,    -1,
      25,    10,    -1,    38,   341,    -1,   362,   343,   342,    -1,
     362,   343,    -1,   152,   343,    -1,   342,   152,   343,    -1,
     344,   362,   345,   346,    -1,   344,   362,   345,    -1,   154,
     153,   155,   153,   156,    -1,   147,    27,   142,    27,   151,
      -1,   147,    27,   142,   151,    -1,   147,   142,    27,   151,
      -1,   147,   142,   151,    -1,   147,    -1,   145,   382,    -1,
     346,   145,   382,    -1,    49,   362,    29,    -1,    55,   362,
      42,   136,    42,    -1,    63,   362,   350,    -1,   230,    -1,
     350,    27,   230,    -1,    62,   362,    42,    -1,    62,   362,
      42,   136,    42,    -1,    62,   362,    42,    27,    42,    -1,
      62,   362,    42,   136,    42,    27,    42,    -1,    62,   362,
      42,    27,    42,   136,    42,    -1,    62,   362,    42,   136,
      42,    27,    42,   136,    42,    -1,    37,    -1,    37,   207,
      -1,    60,   207,    -1,    57,   209,    -1,    56,    10,    -1,
      54,   362,    20,    -1,    50,   362,    42,   362,     9,    -1,
      48,   359,    -1,   360,    -1,   360,    95,   359,    -1,   362,
     140,   157,   141,    -1,   362,   140,   157,   141,   381,    -1,
      26,   362,    11,    -1,    -1,    27,    -1,    44,    -1,    44,
     364,    -1,   365,    -1,   365,    95,   364,    -1,   362,    29,
     137,    29,    -1,   362,    29,   137,    29,   381,    -1,    45,
      -1,    45,   367,    -1,   362,    30,    -1,   367,    95,   362,
      30,    -1,    46,    -1,    46,   369,    -1,   370,    -1,   370,
      95,   369,    -1,   362,    29,    -1,   362,    29,   381,    -1,
      12,   372,    -1,   362,    31,   222,    -1,   362,    29,   222,
      -1,    14,   374,    -1,   362,    31,   222,    -1,   362,    29,
     222,    -1,    15,   362,   390,   140,   391,   141,   381,    -1,
      15,   362,   140,   391,   141,   381,    -1,    15,   362,   388,
     381,    -1,    15,   362,   390,   140,   391,   141,    -1,    15,
     362,   140,   391,   141,    -1,    15,   362,   388,    -1,   145,
     382,    -1,   376,   145,   382,    -1,    16,   362,    34,    -1,
      16,   378,    -1,   379,    -1,   378,    95,   379,    -1,   362,
     390,   140,   391,   141,   376,    -1,   362,   140,   391,   141,
     376,    -1,   362,   388,   376,    -1,   362,   390,   140,   391,
     141,    -1,   362,   140,   391,   141,    -1,   362,   388,    -1,
      19,   362,   390,   140,   391,   141,   381,    -1,    19,   362,
     140,   391,   141,   381,    -1,    19,   362,   388,   381,    -1,
      19,   362,   390,   140,   391,   141,    -1,    19,   362,   140,
     391,   141,    -1,    19,   362,   388,    -1,   145,   382,    -1,
     381,   145,   382,    -1,    27,    29,    27,   384,    -1,    29,
      27,   384,    -1,    27,   383,    -1,   383,    -1,    29,   384,
      -1,    29,    -1,   139,    27,   385,    -1,   139,   385,    -1,
      29,    -1,   147,    -1,    22,    -1,    29,   387,    -1,    29,
      -1,   139,   385,    -1,   389,    -1,   148,   393,   394,    -1,
      21,   362,    -1,    29,   362,    -1,    22,   362,    -1,   392,
      -1,   148,   393,   394,   396,   399,    -1,   148,   393,   394,
     396,    -1,   148,   393,   394,   399,    -1,   148,   393,   394,
      -1,    -1,    33,    -1,   147,   395,    -1,    -1,   142,   151,
      -1,   397,    -1,   396,   397,    -1,   138,   398,    -1,   145,
     398,    -1,   386,    -1,   143,   403,   400,    -1,    -1,   401,
      -1,   402,    -1,   401,   402,    -1,   144,   403,    -1,   149,
     139,   150,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   349,   349,   356,   365,   371,   382,   385,   386,   388,
     389,   391,   392,   393,   394,   395,   399,   400,   402,   404,
     406,   408,   410,   413,   414,   416,   421,   429,   434,   444,
     445,   448,   451,   456,   463,   466,   471,   478,   482,   488,
     494,   499,   504,   510,   514,   522,   523,   525,   526,   528,
     529,   531,   533,   534,   535,   536,   537,   538,   539,   540,
     542,   557,   569,   579,   588,   597,   607,   622,   634,   644,
     653,   662,   671,   686,   698,   708,   717,   726,   735,   741,
     747,   762,   774,   784,   793,   802,   811,   814,   815,   818,
     825,   837,   840,   846,   857,   860,   865,   872,   876,   882,
     885,   892,   904,   913,   918,   925,   934,   935,   938,   952,
     965,   976,   988,   996,  1000,  1014,  1017,  1018,  1020,  1021,
    1023,  1024,  1025,  1026,  1027,  1030,  1031,  1033,  1034,  1035,
    1036,  1038,  1039,  1043,  1051,  1066,  1081,  1086,  1094,  1095,
    1098,  1101,  1102,  1105,  1112,  1124,  1130,  1137,  1148,  1159,
    1163,  1169,  1171,  1176,  1183,  1193,  1200,  1209,  1210,  1213,
    1214,  1215,  1218,  1228,  1235,  1248,  1255,  1272,  1273,  1275,
    1277,  1278,  1279,  1280,  1281,  1282,  1283,  1284,  1285,  1286,
    1287,  1288,  1290,  1292,  1294,  1295,  1297,  1299,  1301,  1303,
    1305,  1307,  1309,  1311,  1313,  1314,  1315,  1316,  1318,  1319,
    1320,  1321,  1322,  1323,  1324,  1325,  1326,  1327,  1328,  1329,
    1330,  1331,  1332,  1333,  1334,  1335,  1336,  1337,  1338,  1339,
    1340,  1341,  1342,  1343,  1344,  1345,  1346,  1347,  1348,  1349,
    1350,  1351,  1353,  1355,  1357,  1359,  1361,  1363,  1364,  1365,
    1366,  1367,  1368,  1369,  1370,  1371,  1372,  1373,  1374,  1375,
    1380,  1383,  1386,  1387,  1390,  1399,  1412,  1417,  1427,  1432,
    1444,  1447,  1448,  1451,  1458,  1470,  1474,  1483,  1487,  1494,
    1509,  1515,  1523,  1532,  1548,  1563,  1578,  1583,  1591,  1601,
    1606,  1622,  1627,  1635,  1640,  1650,  1656,  1662,  1667,  1677,
    1692,  1700,  1712,  1720,  1732,  1737,  1747,  1748,  1752,  1753,
    1757,  1769,  1775,  1783,  1793,  1807,  1813,  1820,  1835,  1850,
    1865,  1870,  1878,  1884,  1894,  1909,  1914,  1922,  1936,  1949,
    1960,  1973,  1988,  1993,  2001,  2016,  2021,  2029,  2044,  2059,
    2064,  2072,  2076,  2084,  2089,  2099,  2104,  2114,  2119,  2129,
    2144,  2149,  2157,  2163,  2174,  2197,  2206,  2207,  2211,  2226,
    2231,  2239,  2245,  2257,  2262,  2272,  2277,  2287,  2292,  2302,
    2307,  2317,  2322,  2332,  2337,  2347,  2355,  2370,  2375,  2382,
    2388,  2400,  2405,  2415,  2430,  2435,  2443,  2449,  2460,  2475,
    2480,  2488,  2494,  2505,  2514,  2525,  2530,  2541,  2556,  2561,
    2570,  2575,  2585,  2591,  2600,  2605,  2613,  2616,  2617,  2620,
    2626,  2637,  2640,  2641,  2643,  2644,  2655,  2665,  2675,  2678,
    2679,  2681,  2682,  2693,  2703,  2713,  2716,  2717,  2720,  2727,
    2735,  2740,  2743,  2757,  2769,  2779,  2788,  2797,  2806,  2814,
    2817,  2818,  2821,  2822,  2826,  2833,  2839,  2848,  2853,  2858,
    2863,  2868,  2875,  2876,  2879,  2885,  2891,  2894,  2899,  2906,
    2911,  2916,  2922,  2928,  2934,  2943,  2946,  2955,  2964,  2973,
    2979,  2985,  2992,  2995,  2996,  2999,  3006,  3018,  3024,  3025,
    3028,  3031,  3034,  3035,  3038,  3058,  3084,  3088,  3091,  3096,
    3103,  3106,  3109,  3110,  3113,  3121,  3136,  3139,  3149,  3165,
    3168,  3178,  3195,  3208,  3219,  3229,  3238,  3246,  3255,  3256,
    3259,  3263,  3266,  3267,  3270,  3286,  3301,  3314,  3327,  3339,
    3351,  3366,  3378,  3388,  3397,  3406,  3415,  3416,  3419,  3425,
    3431,  3432,  3437,  3444,  3452,  3453,  3456,  3457,  3458,  3461,
    3467,  3475,  3478,  3481,  3496,  3497,  3498,  3501,  3503,  3514,
    3526,  3538,  3554,  3555,  3571,  3577,  3578,  3581,  3582,  3585,
    3586,  3589,  3592,  3594,  3596,  3599,  3600,  3603,  3606
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "_SP", "LONE_CRLF", "EXTENSION_CODE",
  "REASON_PHRASE", "SIP_VERSION", "SIP_SPHT_VERSION", "_METHOD",
  "CONTENT_WITHOUTENDINGCRLF", "_CALLID", "WWW_AUTHENTICATELWSCOLON",
  "_CRLF", "PROXY_AUTHENTICATELWSCOLON", "TOLWSCOLON", "CONTACTLWSCOLON",
  "ACCEPTRESPRIOLWSCOLON", "RESPRIOLWSCOLON", "FROMLWSCOLON", "_TYPEID",
  "TOKENS", "QUOTED_STRING", "EPARENT", "SPARENT", "OTHERLWSCOLON",
  "CALL_IDLWSCOLON", "SOMELWS", "_LAES_CONTENT", "_TOKEN", "_STOKEN",
  "_DIGEST", "_COMMENT", "USERINFO_AT", "_STAR", "GEOLOCERRORLWSCOLON",
  "GEOLOCATIONLWSCOLON", "SUPPORTEDLWSCOLON", "VIALWSCOLON",
  "SUBJECTLWSCOLON", "CONTENT_ENCODINGLWSCOLON", "CONTENT_LENGTHLWSCOLON",
  "SOMEDIGITS", "CONTENT_TYPELWSCOLON", "ACCEPTLWSCOLON",
  "ACCEPT_ENCODINGLWSCOLON", "ACCEPT_LANGUAGELWSCOLON", "AUTHINFOLWSCOLON",
  "CALL_INFOLWSCOLON", "MINEXPIRESLWSCOLON", "CSEQLWSCOLON",
  "EVENTLWSCOLON", "ALLOWEVENTLWSCOLON", "SUBSTATEWSCOLON", "DATELWSCOLON",
  "MIME_VERSIONLWSCOLON", "ORGANIZATIONLWSCOLON", "RECORD_ROUTELWSCOLON",
  "PCALLEPPTYLWSCOLON", "PVISITEDNETLWSCOLON", "REQUIRELWSCOLON",
  "HISTORYLWSCOLON", "TIMESTAMPLWSCOLON", "USER_AGENTLWSCOLON",
  "PASSOCURILWSCOLON", "DIVERSIONWSCOLON", "ERROR_INFOLWSCOLON",
  "RETRY_AFTERLWSCOLON", "SERVERLWSCOLON", "UNSUPPORTEDLWSCOLON",
  "WARNINGLWSCOLON", "PASSERTEDLWSCOLON", "REASONLWSCOLON",
  "PPREFERREDLWSCOLON", "PRIVACYLWSCOLON", "RACKLWSCOLON", "RSEQLWSCOLON",
  "ALERT_INFOLWSCOLON", "AUTHORIZATIONLWSCOLON", "IN_REPLY_TOLWSCOLON",
  "REPLY_TOLWSCOLON", "MAX_FORWARDSLWSCOLON", "REFER_TOLWSCOLON",
  "PRIORITYLWSCOLON", "PROXY_AUTHORIZATIONLWSCOLON",
  "PROXY_REQUIRELWSCOLON", "ROUTELWSCOLON", "ALLOWLWSCOLON",
  "PEARLYMEDIALWSCOLON", "CONTENT_DISPOSITIONLWSCOLON",
  "CONTENT_LANGUAGELWSCOLON", "EXPIRESLWSCOLON", "SESSIONEXPWSCOLON",
  "SESSIONIDLWSCOLON", "MINSELWSCOLON", "SOMELWSCOMMA",
  "PACCESSNETLWSCOLON", "REQDISPLWSCOLON", "PCHARGEADDRLWSCOLON",
  "PCHARGEVECTORLWSCOLON", "ANSWERMODELWSCOLON", "PRIVANSWERMODELWSCOLON",
  "ALERTMODELWSCOLON", "REFERSUBLWSCOLON", "PALERTINGMODELWSCOLON",
  "PANSWERSTATELWSCOLON", "ACCEPTCONTACTLWSCOLON", "REJECTCONTACTLWSCOLON",
  "PMEDIAAUTHLWSCOLON", "PATHLWSCOLON", "SECCLIENTLWSCOLON",
  "SECSERVERLWSCOLON", "SECVERIFYLWSCOLON", "PTARCEPTYIDLWSCOLON",
  "POSPSLWSCOLON", "PBILLINGINFOLWSCOLON", "PLAESLWSCOLON",
  "PDCSREDIRLWSCOLON", "PUSERDBASELWSCOLON", "SERVICEROUTELWSCOLON",
  "REFERREDBYTOLWSCOLON", "REPLACESLWSCOLON", "IFMATCHLWSCOLON",
  "ETAGLWSCOLON", "JOINLWSCOLON", "PSERVEDUSERLWSCOLON",
  "PPROFILEKEYLWSCOLON", "PSERVICEINDICATIONLWSCOLON",
  "PSERVICENOTIFICATIONLWSCOLON", "GEOROUTINGLWSCOLON",
  "TARGETDIALOGLWSCOLON", "INFOPACKAGELWSCOLON", "FEATURECAPSLWSCOLON",
  "RECVINFOLWSCOLON", "_TOKEN_NO_DOT", "_HEXTOKEN", "_DOT", "_SLASH",
  "SEMICOLON", "EQUALSIGN", "_ABO", "_ABC", "_COLON", "QUESTMARK",
  "AMPERSANT", "SOMELWS_SEMICOLON", "_AT", "_HOST", "SCHEME", "_HNAME",
  "_HVALUE", "_PORT", "SOMELWS_COMMA_SOMELWS", "SOMELWS_SLASH_SOMELWS",
  "PROTOCOL_NAME", "PROTOCOL_VERSION", "TRANSPORT", "_URLTOKEN",
  "PASSERTEDSERVICELWSCOLON", "PPREFERREDSERVICELWSCOLON", "XAUTLWSCOLON",
  "XCARRIERINFOLWSCOLON", "XCHGDELAYLWSCOLON", "PAREAINFOLWSCOLON",
  "XCHGINFOLWSCOLON", "$accept", "SIP_Message", "Request", "Request_Line",
  "gen_req_ent_header_0toN", "gen_req_ent_header_1toN",
  "gen_req_ent_header", "entity_header", "Allow", "allowcontent_1toN",
  "Content_Disposition", "semicolon_dispparam_1toN", "Content_Encoding",
  "conctencoding1_N", "Content_Language", "clangencoding1_N",
  "Content_Length", "Content_Type", "Expires", "X_AUT", "X_Carrier_Info",
  "X_CHGDelay", "X_CHGInfo", "request_header", "Referedby", "PprofileKey",
  "PservedUser", "P_Service_Indication", "P_Service_Notification",
  "ReferTo", "Alert_Info", "alert_info_body1toN", "alert_info_body",
  "Authorization", "authbody", "In_Reply_To", "callid1toN", "Max_Forwards",
  "Priority", "Proxy_Authorization", "pauthbody", "Proxy_Require",
  "optioncontent_1toN", "Route", "routebody1toN", "routeadress", "Subject",
  "Response", "Status_Line", "gen_res_ent_header_0toN",
  "gen_res_ent_header_1toN", "gen_res_ent_header", "response_header",
  "Auth_info", "P_Asserted_Service", "P_Preferred_Service", "service1toN",
  "coma_authparam1_N", "Error_Info", "error_info_body1toN",
  "error_info_body", "Retry_After", "comment", "Server",
  "product_comment_s1toN", "product_comment", "Unsupported", "Warning",
  "warncontent_1toN", "warncontent", "host_port", "general_header",
  "recv_info_header", "info_package_range1toN", "info_package_type",
  "info_package_header", "target_dialogheader", "featureCapsHeader",
  "fc_values1toN", "fc_value", "geolocerr_header", "georouting_header",
  "geolocation_header", "geoloaction_list1toN", "geoloaction_value",
  "AcceptResPrioheader", "ResPrioheader", "rvalue_list1toN",
  "rvalue_value", "P_early_media", "embody1toN", "joinheader", "SIPetag",
  "SIPifmatch", "replacesheader", "service_route_header", "P_userdbase",
  "P_dcsredir", "P_laes", "laes_params", "laes_param", "laes_content",
  "host_and_port", "P_billing_info", "P_osps", "P_trace_pty", "Sec_client",
  "Sec_server", "Sec_verify", "Secmechanism_1toN", "Secmechanism",
  "Pathheader", "routebdy1toN", "routeadr", "P_media_auth",
  "auth_token_1toN", "Req_disp_cont", "directive_1toN", "Reject_cont",
  "Accept_cont", "conatact_1toN", "conatact_value", "session_exp_header",
  "session_id_header", "min_se_header", "HistoryInfo_header",
  "h_urispec_1toN", "h_urispec", "p_charge_vector", "p_charge_addr",
  "semicolon_param_1toN", "p_access_net_header", "anetspec_1toN",
  "anetspec", "answer_mode_header", "priv_answer_mode_header",
  "alert_mode_header", "refer_sub_header", "p_alerting_mode_header",
  "p_answer_sate_header", "p_area_info", "p_visited_net_id_header",
  "vnetspec_1toN", "vnetspec", "P_called_party_id_header",
  "P_assoc_uri_header", "p_urispec_1toN", "p_urispec",
  "P_diversion_header", "p_divspec_1toN", "p_divspec", "p_nameaddr",
  "Subscription_stateheader", "AllowEventheader", "event_list1toN",
  "Eventheader", "event_event", "event_event_template1toN", "Reasonheader",
  "reason_1toN", "reason_", "P_Asserted_header", "asserted_id_1toN",
  "asserted_id", "P_Preferred_header", "preferred_id_1toN", "preferred_id",
  "Privacyheader", "privacy_value1ton", "privacy_value", "Rackheader",
  "Rseqheader", "ReplyTo", "replytobody", "Other", "Via",
  "sntp_sntb_scviap0N_cmt01_1cN", "LWS0N_c_LWS0N_spbscvpc_1toN",
  "sntp_sntb_scviap0N_cmt01", "sent_protocol", "sent_by",
  "semicolon_viaparams_1toN", "MinExpires", "MIME_Version", "User_Agent",
  "product_comment_1toN", "Timestamp", "Supported", "Require",
  "Record_Route", "Organization", "Date", "Cseq", "Call_Info",
  "call_info_body1toN", "call_info_body", "Call_ID", "LWS_0toN", "Accept",
  "acceptrange1toN", "acceptrange", "Accept_Encoding", "encoding1_N",
  "Accept_Language", "accept_lang_range1toN", "accept_lang_range",
  "WWW_Authenticate", "wwwauthbody", "Proxy_Authenticate", "proxyauthbody",
  "Toheader", "semicolon_toparam_1toN", "Contact", "contactbody1toN",
  "contactadress", "From", "semicolon_fromparam_1toN", "from_param",
  "from_param_withoutlws", "equals_token_host_qtdstr",
  "token_or_host_or_quotedstring", "generic_param_withoutlws",
  "equals_token_host_qtdstr_withoutlws", "addr_spec_withnoparam",
  "SIP_URL_withnoparam", "display_name", "addr_spec", "SIP_URL",
  "userinfo_at_0to1", "hostport", "colon_port_0to1",
  "semicolon_urlparam_1toN", "semicolon_urlparam", "url_parameter",
  "headers", "amp_header_0toN", "amp_header_1toN", "amp_header", "header", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   165,   166,   166,   167,   168,   168,   169,   169,   170,
     170,   171,   171,   171,   171,   171,   172,   172,   172,   172,
     172,   172,   172,   173,   173,   174,   174,   175,   175,   176,
     176,   177,   178,   178,   179,   180,   180,   181,   182,   183,
     184,   185,   186,   187,   187,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     189,   189,   189,   189,   189,   189,   190,   190,   190,   190,
     190,   190,   191,   191,   191,   191,   191,   191,   192,   193,
     194,   194,   194,   194,   194,   194,   195,   196,   196,   197,
     197,   198,   199,   199,   200,   201,   201,   202,   203,   204,
     205,   205,   206,   207,   207,   208,   209,   209,   210,   210,
     210,   210,   211,   212,   213,   213,   214,   214,   215,   215,
     216,   216,   216,   216,   216,   217,   217,   217,   217,   217,
     217,   217,   217,   218,   219,   220,   221,   221,   222,   222,
     223,   224,   224,   225,   225,   226,   226,   226,   226,   227,
     227,   228,   229,   229,   230,   230,   231,   232,   232,   233,
     233,   233,   234,   235,   235,   235,   235,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     237,   237,   238,   238,   239,   239,   240,   240,   241,   241,
     242,   243,   243,   244,   244,   245,   245,   246,   246,   247,
     248,   248,   249,   249,   250,   251,   252,   252,   253,   254,
     254,   255,   255,   256,   256,   257,   258,   259,   259,   260,
     261,   261,   262,   262,   263,   263,   264,   264,   265,   265,
     266,   267,   267,   268,   268,   269,   270,   271,   272,   273,
     274,   274,   275,   275,   276,   277,   277,   278,   278,   278,
     278,   279,   280,   280,   281,   282,   282,   283,   284,   285,
     285,   286,   286,   287,   287,   288,   288,   289,   289,   290,
     291,   291,   292,   292,   293,   294,   295,   295,   296,   297,
     297,   298,   298,   299,   299,   300,   300,   301,   301,   302,
     302,   303,   303,   304,   304,   305,   306,   307,   307,   308,
     308,   309,   309,   310,   311,   311,   312,   312,   313,   314,
     314,   315,   315,   316,   316,   317,   317,   318,   319,   319,
     320,   320,   321,   321,   322,   322,   323,   324,   324,   325,
     325,   326,   327,   327,   328,   328,   328,   328,   329,   330,
     330,   331,   331,   331,   331,   332,   333,   333,   334,   335,
     336,   337,   338,   338,   338,   338,   338,   338,   339,   340,
     341,   341,   342,   342,   343,   343,   344,   345,   345,   345,
     345,   345,   346,   346,   347,   348,   349,   350,   350,   351,
     351,   351,   351,   351,   351,   352,   352,   353,   354,   355,
     356,   357,   358,   359,   359,   360,   360,   361,   362,   362,
     363,   363,   364,   364,   365,   365,   366,   366,   367,   367,
     368,   368,   369,   369,   370,   370,   371,   372,   372,   373,
     374,   374,   375,   375,   375,   375,   375,   375,   376,   376,
     377,   377,   378,   378,   379,   379,   379,   379,   379,   379,
     380,   380,   380,   380,   380,   380,   381,   381,   382,   382,
     382,   382,   383,   383,   384,   384,   385,   385,   385,   386,
     386,   387,   388,   389,   390,   390,   390,   391,   392,   392,
     392,   392,   393,   393,   394,   395,   395,   396,   396,   397,
     397,   398,   399,   400,   400,   401,   401,   402,   403
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     3,     6,     2,     0,     1,     1,
       2,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     2,     2,     4,     3,     4,     2,
       3,     2,     2,     4,     2,     2,     4,     3,     3,     3,
       3,     3,     3,     3,     5,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       7,     6,     4,     6,     5,     3,     7,     6,     4,     6,
       5,     3,     7,     6,     4,     6,     5,     3,     3,     3,
       7,     6,     4,     6,     5,     3,     2,     1,     3,     4,
       5,     2,     3,     3,     3,     1,     4,     3,     2,     2,
       3,     3,     2,     2,     4,     2,     1,     3,     6,     5,
       5,     4,     2,     3,     6,     3,     0,     1,     1,     2,
       1,     1,     1,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     4,     1,     3,
       2,     1,     3,     4,     5,     3,     7,     8,     4,     1,
       2,     3,     1,     3,     3,     1,     2,     3,     3,     1,
       4,     4,     5,     3,     1,     3,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     2,     1,     3,     2,     3,     3,     4,     3,     4,
       2,     1,     3,     2,     3,     3,     4,     3,     5,     2,
       1,     3,     4,     5,     2,     2,     2,     4,     3,     1,
       2,     2,     4,     3,     4,     3,     3,     3,     4,     2,
       5,     6,     3,     4,     3,     5,     1,     3,     1,     1,
       4,     3,     1,     7,     8,     3,     2,     2,     2,     2,
       1,     3,     2,     3,     2,     1,     3,     6,     5,     5,
       4,     2,     2,     4,     2,     2,     4,     2,     2,     1,
       3,     2,     3,     3,     4,     3,     4,     3,     4,     2,
       1,     3,     1,     2,     2,     2,     1,     3,     2,     1,
       3,     2,     3,     3,     4,     3,     4,     3,     4,     3,
       4,     3,     4,     3,     4,     2,     2,     1,     3,     2,
       3,     2,     3,     2,     1,     3,     1,     2,     2,     1,
       3,     1,     2,     5,     4,     3,     4,     2,     2,     4,
       3,     4,     1,     3,     2,     4,     2,     1,     3,     2,
       3,     2,     1,     3,     0,     5,     4,     2,     2,     1,
       3,     0,     5,     4,     2,     2,     2,     4,     1,     7,
       3,     2,     6,     5,     3,     5,     4,     2,     2,     2,
       3,     2,     2,     3,     4,     3,     5,     5,     4,     4,
       3,     1,     2,     3,     3,     5,     3,     1,     3,     3,
       5,     5,     7,     7,     9,     1,     2,     2,     2,     2,
       3,     5,     2,     1,     3,     4,     5,     3,     0,     1,
       1,     2,     1,     3,     4,     5,     1,     2,     2,     4,
       1,     2,     1,     3,     2,     3,     2,     3,     3,     2,
       3,     3,     7,     6,     4,     6,     5,     3,     2,     3,
       3,     2,     1,     3,     6,     5,     3,     5,     4,     2,
       7,     6,     4,     6,     5,     3,     2,     3,     4,     3,
       2,     1,     2,     1,     3,     2,     1,     1,     1,     2,
       1,     2,     1,     3,     2,     2,     2,     1,     5,     4,
       4,     3,     0,     1,     2,     0,     2,     1,     2,     2,
       2,     1,     3,     0,     1,     1,     2,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,     0,     0,     2,     0,     3,     0,     0,
       6,     0,     0,     1,     0,   468,   468,   468,   468,   468,
     468,   468,     0,   468,   468,   468,   455,   468,     0,   468,
     468,   468,   468,   468,   468,     0,   468,   468,   468,   468,
     468,   468,   468,   468,     0,   468,   468,   468,   468,   468,
     468,   468,   468,   468,   468,   468,   468,   468,   468,   468,
     468,   468,   468,   468,   468,   468,   468,   468,   468,   468,
     468,     0,   468,   468,   468,    23,   279,   468,   468,   468,
     468,   468,   468,   468,   468,     0,     0,   468,   468,   468,
     468,   468,   468,   468,   468,   468,   468,   468,   468,   468,
     468,   468,   468,   468,     0,   468,   468,   468,   468,   468,
     468,   468,   468,   468,   468,   468,   468,   468,   468,   468,
     250,   468,   468,   468,   468,   468,     0,   468,     0,     0,
       9,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    12,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    13,
       0,     0,     0,     0,     0,     0,     0,     0,    11,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   123,
     121,     0,     0,   118,   122,   120,   115,     0,   542,     0,
      15,   469,     0,   486,     0,   489,     0,     0,   501,   502,
     274,     0,   275,     0,   428,     0,     0,   269,   270,     0,
     456,     0,   429,     0,   112,    31,     0,     0,     0,     0,
     471,   472,     0,   477,     0,   481,   482,     0,   523,   133,
     138,   521,   462,   463,     0,     0,     0,     0,   387,     0,
       0,     0,     0,   459,   458,   106,     0,   371,     0,   366,
     367,     0,   457,   339,   340,   342,     0,     0,   373,   374,
     376,   378,   379,   381,   140,   141,     0,     0,     0,   156,
       0,     0,   401,   402,     0,   396,   397,     0,   408,   409,
       0,   415,     0,     0,     0,    86,    87,     0,    91,     0,
       0,   421,     0,     0,     0,    98,    99,     0,   102,   105,
      24,     0,   280,     0,     0,    34,     0,     0,     0,     0,
       0,   348,   349,     0,   324,     0,   345,   346,   344,     0,
       0,     0,     0,     0,     0,   328,   329,     0,   327,   321,
       0,   314,   315,     0,   307,   310,     0,   308,   309,   306,
       0,     0,     0,     0,   537,     0,   289,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   260,
     261,     0,   251,   252,     0,   134,     0,   135,     0,     0,
       0,   365,     0,     4,    10,    16,    17,    18,    19,    20,
      21,    22,   244,   245,   246,   248,    57,    56,    55,   229,
     230,    54,    45,    46,    47,    48,    49,    50,    51,    52,
      53,   130,    58,    59,   125,   127,   128,   129,   131,   243,
     242,   240,   241,   239,   238,   237,   178,   179,   219,   228,
     226,   227,   225,   224,   223,   222,   221,   220,   218,   217,
     214,   215,   216,   213,   212,   211,   210,   209,   202,   203,
     201,   181,   193,   192,   191,   231,   232,   233,   234,   235,
     236,   247,   189,   187,   185,   186,   204,   177,   176,   195,
     188,   190,   184,   194,   196,   198,   249,   208,   175,   182,
     207,   205,   200,   199,   197,   183,   174,   173,   171,   170,
     167,   168,   169,   132,   126,   206,   172,   180,   124,   113,
     119,     0,   543,     0,     0,     0,     0,     0,     0,   468,
     468,   468,     0,   542,   497,   532,     0,   500,     0,   509,
       0,   468,   468,     0,   276,     0,   515,     0,   467,   265,
     468,     0,   468,   103,     0,   431,   468,   468,    32,    37,
      38,     0,   468,   478,   468,   484,   468,   523,   520,     0,
       0,   522,     0,   468,     0,   444,   468,   392,   390,   468,
     388,   385,   460,     0,   468,     0,     0,     0,   372,     0,
       0,   468,   369,   468,   343,   449,     0,   155,   447,   446,
     468,   377,   468,   382,   468,     0,   145,   151,   152,     0,
     158,   159,   157,   468,     0,     0,   407,   468,   399,   468,
       0,   414,     0,   468,   418,   416,     0,   420,   468,     0,
       0,     0,    95,    94,     0,   427,     0,    97,     0,    85,
       0,     0,     0,   468,    25,   468,   281,    27,   468,    35,
      39,   333,   335,   337,   468,   351,   468,   325,     0,   353,
     355,   357,   359,   361,   363,   468,   331,   468,   322,   468,
       0,     0,   468,   312,   305,     0,   302,   294,   292,     0,
       0,    65,     0,   287,   286,   285,   283,     0,    77,     0,
       0,    71,     0,    78,    79,   267,   258,   256,   468,   263,
     468,   254,   468,   136,    40,    41,    42,    43,     0,   545,
     541,     0,   488,   487,   491,   490,   534,   536,   535,     0,
       0,   494,     0,     0,     0,   506,     0,     0,   503,     0,
       0,     0,   512,     0,   266,   271,     0,     0,     0,     0,
     430,     0,     0,     0,   473,     0,   485,   483,     0,   519,
     528,     0,   526,   527,   525,   139,   464,     0,     0,   468,
     391,     0,   386,     0,   107,     0,     0,   516,     0,     0,
       0,   368,   370,   341,     0,     0,   149,     0,     0,   375,
     380,   142,     0,     0,   148,     0,     0,   468,   403,     0,
       0,   398,     0,   400,   410,     0,     0,     0,     0,    88,
       0,    93,    92,   468,     0,   424,     0,     0,    82,     0,
     101,   100,     0,     0,    28,     0,   334,   336,   338,   350,
     352,     0,   347,   354,   356,   358,   360,   362,   364,   330,
     332,     0,   316,     0,     0,   311,   313,     0,     0,   468,
     293,     0,     0,    62,     0,   288,   284,     0,    74,     0,
       0,    68,     0,     0,   259,   257,   262,   264,   253,   255,
       0,     0,   114,     0,   544,     0,     0,     0,   539,   547,
     540,     5,   496,   533,     0,   508,   498,     0,     0,   277,
     278,   514,     0,   272,   104,     0,   432,     0,   441,   435,
      33,   474,   479,   518,   524,   465,   461,   393,     0,   389,
     445,   111,     0,   517,   384,     0,   451,   450,   154,   150,
     448,   143,     0,   153,   164,   166,     0,     0,     0,   406,
       0,    29,     0,   413,     0,   417,     0,    89,     0,   426,
       0,    84,     0,    26,   282,    36,   326,   323,   320,     0,
       0,   301,   469,   295,   296,   298,     0,   299,   290,    64,
       0,    76,     0,    70,     0,   268,   137,    44,   546,   530,
     551,   549,     0,   553,   550,   548,   538,   493,   495,   505,
     499,   507,   511,   513,   273,     0,   433,     0,     0,     0,
     434,   475,   466,   468,   394,   109,   110,   383,     0,     0,
     144,     0,     0,     0,     0,   160,   161,   405,    30,   412,
       0,    90,    96,   423,   425,    81,    83,   318,   319,     0,
     468,   468,   291,    61,    63,    73,    75,    67,    69,     0,
     529,     0,     0,   552,   554,   555,   492,   504,   510,   436,
       0,     0,   440,   442,     0,     0,   108,   453,   452,   146,
     163,   165,   162,   419,   422,    80,   317,   303,   297,     0,
      60,    72,    66,   531,   558,   557,   556,     0,   438,   439,
     443,   395,     0,   147,   304,   300,   437,   454
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     6,   128,   129,   130,   131,   132,   370,
     133,   833,   134,   285,   135,   375,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   355,   356,   151,   358,   152,   673,   153,   154,   155,
     366,   156,   280,   157,   314,   315,   158,     7,     8,   251,
     252,   253,   159,   160,   161,   162,   435,   299,   163,   334,
     335,   164,   817,   165,   647,   638,   166,   167,   650,   651,
     956,   168,   169,   432,   433,   170,   171,   172,   429,   430,
     173,   174,   175,   277,   278,   176,   177,   270,   584,   178,
     372,   179,   180,   181,   182,   183,   184,   185,   186,   983,
     984,   985,   717,   187,   188,   189,   190,   191,   192,   404,
     405,   193,   401,   402,   194,   399,   195,   384,   196,   197,
     395,   396,   198,   199,   200,   201,   323,   324,   202,   203,
     386,   204,   381,   382,   205,   206,   207,   208,   209,   210,
     211,   212,   319,   320,   213,   214,   328,   329,   215,   331,
     332,   325,   216,   217,   308,   218,   618,   937,   219,   345,
     346,   220,   342,   343,   221,   348,   349,   222,   351,   665,
     223,   224,   225,   361,   226,   227,   282,   780,   595,   596,
     929,  1020,   228,   229,   230,   639,   231,   232,   233,   234,
     235,   236,   237,   238,   302,   303,   239,   318,   240,   290,
     291,   241,   293,   242,   295,   296,   243,   263,   244,   265,
     245,   765,   246,   268,   269,   247,   628,   300,   301,   611,
     794,  1000,  1060,   579,   575,   580,   413,   414,   563,   750,
     904,   908,   909,  1001,   910,  1063,  1064,  1065,  1003
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -758
static const yytype_int16 yypact[] =
{
     227,   236,   141,   159,   185,  -758,  1068,  -758,  1221,   218,
    -758,   256,   132,  -758,   264,   262,   262,   262,   262,   262,
     262,   262,   298,   262,   262,   262,   168,   262,   301,   262,
     262,   262,   137,   156,   199,   212,   262,   262,   262,   262,
     262,   262,   262,   262,   310,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   262,   262,   154,   188,
     262,   190,   262,   262,   262,   262,   262,   262,   262,   262,
     262,   311,   262,   262,   262,   168,   168,   262,   262,   262,
     262,   262,   262,   262,   262,   212,   212,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   132,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   262,   262,   262,   262,
     168,   262,   262,   262,   262,   262,   212,   262,   318,  1374,
    -758,  -758,   314,   315,   316,   317,   319,   320,   321,   322,
     325,   326,   327,  -758,   328,   329,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,  -758,
     344,   346,   348,   349,   350,   351,   352,   353,  -758,   354,
     355,   356,   357,   358,   360,   361,   364,   365,   366,   368,
     369,   370,   371,   372,   373,   374,   375,   376,   377,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,   389,
     390,   392,   393,   395,   396,   397,   398,   399,   400,   401,
     402,   403,   404,   406,   407,   408,   409,   410,   411,   412,
     413,   414,   415,   416,   417,   418,   420,   421,   422,   423,
     424,   426,   427,   428,   429,   430,   432,   433,   434,   435,
     436,   437,   439,   440,   441,   442,   443,   445,   446,  -758,
    -758,   456,  1527,  -758,  -758,  -758,  -758,   458,   290,   459,
    -758,  -758,   255,  -758,   259,  -758,   108,   105,   229,  -758,
     378,   330,   378,   111,  -758,   452,   289,   405,  -758,   362,
     438,   447,  -758,   312,  -758,   444,   450,   425,   448,   453,
    -758,   451,   454,   455,   464,  -758,   457,   465,   104,  -758,
     462,  -758,  -758,   463,   363,   467,   466,   367,   468,   367,
     469,   449,   471,  -758,   470,  -758,   124,   391,   153,   473,
    -758,   208,   438,   474,  -758,   391,   477,   226,   475,  -758,
     391,   476,  -758,   391,  -758,   478,   394,   481,   226,   438,
     323,   323,   479,  -758,   114,   483,  -758,   486,   484,  -758,
     120,   419,   487,   482,   488,  -758,   485,   460,  -758,   263,
     461,  -758,   130,   489,   139,  -758,  -758,   267,   438,   470,
     493,   492,   495,   499,   500,   497,   502,   506,   508,   509,
     512,   504,  -758,   208,   507,   513,   472,  -758,   472,   514,
     516,   518,   519,   520,   522,   510,  -758,   525,   510,   511,
     524,   515,  -758,   169,   517,  -758,   531,   517,   517,  -758,
     532,   480,   324,   431,  -758,   490,   515,   151,   551,   554,
     555,   583,   155,   157,   566,   568,   574,   596,   579,  -758,
     523,   582,  -758,   526,   584,   527,   585,   527,   590,   591,
     594,   472,   595,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,   491,  -758,   359,   618,   212,   212,   212,   212,   262,
     262,   262,   132,   290,   391,  -758,   494,  -758,   132,   496,
     503,   262,   262,   347,  -758,   132,   391,   505,  -758,   391,
     262,   132,   262,  -758,   498,   528,   262,   262,  -758,  -758,
    -758,   521,   262,  -758,   262,   391,   262,   177,  -758,   501,
     165,  -758,   212,   262,   313,  -758,   262,   529,   391,   262,
    -758,   391,  -758,   530,   262,   132,   534,   212,   533,   132,
     535,   262,   391,   262,   533,   129,   601,  -758,  -758,   599,
     262,   533,   262,   533,   262,   536,   131,   600,  -758,   632,
     541,  -758,   541,   188,   132,   537,  -758,   262,   538,   190,
     132,  -758,   542,   262,  -758,  -758,   610,  -758,   262,   543,
     212,   212,  -758,   552,   132,   496,   544,  -758,   132,   391,
     545,   212,   212,   262,  -758,   262,  -758,   538,   262,  -758,
    -758,   391,   391,   391,   262,   391,   262,  -758,   212,   391,
     391,   391,   391,   391,   391,   262,   391,   262,  -758,   262,
     132,   546,   262,   391,  -758,   548,   547,   550,   391,   132,
     132,   391,   559,   391,  -758,  -758,   391,   132,   391,   563,
     132,   391,   564,  -758,  -758,   178,   391,   391,   262,   391,
     262,   391,   262,  -758,  -758,  -758,  -758,   561,   633,   549,
      95,   636,  -758,  -758,  -758,  -758,  -758,  -758,  -758,   567,
     359,   533,   132,   569,   212,   571,   132,   167,  -758,   330,
     553,   573,   533,   132,   533,  -758,   577,   621,   575,   312,
     570,   572,   622,   625,  -758,   626,   533,  -758,   501,  -758,
    -758,    99,  -758,  -758,  -758,  -758,  -758,   587,   648,   262,
     533,   367,   533,   617,  -758,   588,   132,  -758,   212,   592,
     132,  -758,   533,  -758,   619,   620,  -758,   195,   226,  -758,
    -758,  -758,   593,   639,   533,   226,    60,   197,  -758,   597,
     132,  -758,   212,   586,  -758,   598,   132,   487,   627,  -758,
     602,  -758,  -758,   262,   603,   571,   132,   604,   533,   132,
    -758,  -758,   631,   635,   586,   637,   533,   533,   533,  -758,
     533,   642,  -758,   533,   533,   533,   533,   533,   533,  -758,
     533,   659,  -758,   605,   132,  -758,   533,   589,   581,   277,
     533,   606,   607,   533,   132,   533,   533,   608,   533,   132,
     611,   533,   132,   501,   533,   533,  -758,   533,  -758,   533,
     706,   707,  -758,   609,  -758,   712,   612,   712,    95,  -758,
    -758,  -758,   391,  -758,   613,   496,  -758,   212,   615,  -758,
    -758,   391,   623,   391,  -758,   614,  -758,   312,   121,   624,
    -758,   391,  -758,  -758,  -758,   391,  -758,   629,   628,  -758,
    -758,   496,   630,  -758,  -758,   640,   634,   715,  -758,  -758,
    -758,   391,   601,  -758,   638,   641,   748,   323,   323,  -758,
     644,  -758,   212,  -758,   645,  -758,   732,   391,   752,   496,
     646,   391,   647,  -758,  -758,  -758,  -758,  -758,   496,   649,
     643,  -758,   465,  -758,   650,  -758,   738,  -758,   391,   391,
     652,   391,   655,   391,   656,  -758,  -758,  -758,  -758,   661,
    -758,  -758,   662,   654,  -758,  -758,  -758,   533,   391,   571,
    -758,   496,   533,   391,   533,   616,  -758,   660,    58,   212,
     658,   533,   533,   262,  -758,   571,   496,  -758,   726,   731,
     533,   202,   653,   657,   754,  -758,  -758,  -758,  -758,  -758,
     749,   533,  -758,   571,   496,   533,   391,   571,   496,   663,
     277,   262,   533,   533,   391,   533,   391,   533,   391,    99,
    -758,   664,   612,  -758,   654,  -758,   533,   571,   533,  -758,
      93,   665,  -758,  -758,   212,   671,   571,  -758,   670,   391,
    -758,  -758,  -758,  -758,   571,   533,   571,   391,  -758,   324,
     533,   533,   533,  -758,  -758,  -758,  -758,   666,  -758,  -758,
    -758,  -758,   740,   533,   533,  -758,  -758,  -758
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -758,  -758,  -758,  -758,  -758,  -758,   678,    29,  -758,  -758,
    -758,  -212,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,    30,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,   123,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,    90,  -758,   735,   187,  -758,  -758,  -758,  -758,
    -758,   578,    48,  -758,  -758,  -758,   691,  -345,  -758,   171,
    -758,  -758,  -133,  -758,  -758,  -307,  -758,  -758,   539,  -700,
    -758,    62,  -758,    78,  -758,  -758,  -758,  -758,    82,  -758,
    -758,  -758,  -758,  -758,   231,  -758,  -758,   802,    54,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -226,
    -758,  -758,  -264,  -758,  -758,  -758,  -758,  -758,  -758,   220,
     115,  -758,   720,   122,  -758,  -758,  -758,  -758,  -758,  -758,
     739,   133,  -758,  -758,  -758,  -758,  -758,   196,  -758,  -758,
      80,  -758,  -758,   140,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,   204,  -758,  -758,  -758,   201,  -758,  -758,
     203,    71,  -758,  -758,  -758,  -758,  -296,  -758,  -758,  -758,
     180,  -758,  -758,   186,  -758,  -758,   184,  -758,  -758,     9,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -708,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,   234,  -758,  -758,   -15,  -758,   246,
    -758,  -758,  -758,  -758,   243,  -758,  -758,  -758,  -758,  -758,
    -758,  -668,  -758,  -758,   269,  -758,  -214,    28,  -277,  -580,
    -757,  -758,  -758,  -148,  -758,  -151,  -134,   839,   279,    94,
    -758,  -758,   -55,   -52,   -51,  -758,  -758,  -208,  -204
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -481
static const yytype_int16 yytable[] =
{
     262,   264,   266,   267,   271,   271,   273,   845,   275,   276,
     279,   281,   283,   620,   286,   287,   288,   289,   292,   294,
     608,   304,   305,   306,   307,   309,   310,   311,   312,   789,
     316,   648,   321,   281,   934,   326,   327,   249,   250,   336,
     337,   338,   281,   341,   344,   347,   350,   352,   353,   354,
     357,   359,   360,   362,   363,   364,   254,   367,   281,   316,
     371,   373,   374,   376,   377,   378,   379,   380,   383,   385,
     255,   926,   389,   390,   391,   392,   393,   394,   397,   397,
     400,   403,   406,   406,   406,  1071,   410,   411,   412,   954,
     415,   403,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   431,   434,   436,   436,   438,   439,
     440,   634,   442,   387,   387,   576,   641,   317,   574,   643,
    1097,   790,   587,   330,   333,   586,   569,   570,   792,   569,
     570,   609,   569,   570,   571,   569,   570,   571,   322,   577,
     571,   569,   570,   571,    11,   569,   570,   339,  1017,   571,
    -470,   569,   570,   571,   387,  -468,   814,   340,   261,   571,
     569,   570,    12,   368,   261,   626,   388,   630,   571,  -476,
     632,   409,   569,   570,   569,   570,   569,   570,   569,   570,
     571,   261,   571,   261,   571,    13,   571,   790,   569,   570,
     569,   570,   791,   655,   792,   261,   571,  -468,   571,   662,
     957,  -404,   661,  -411,   788,   261,   441,   955,   933,  1072,
     656,   676,  -480,   680,   675,   261,   679,   261,   948,  1016,
     752,   753,   754,   755,   261,  1079,   261,   949,     1,   569,
     570,   256,   695,   905,   949,     2,     3,   571,   906,   297,
     907,   298,     9,   610,  1098,   578,   793,  1009,   572,    10,
     636,   585,   711,   573,   654,   637,   573,  1035,  1036,   573,
     660,   257,   258,  1018,   625,   815,   722,   795,   573,   721,
     674,   729,   732,  1025,   728,   731,   627,   260,   573,   678,
     258,   249,   250,  -404,   565,  -411,   566,   573,   567,   261,
     568,   720,   670,   629,   671,   727,   681,   730,   682,   573,
     254,  1043,  1093,   573,   982,   573,   298,   578,   274,   710,
    1047,   284,   793,   995,   255,   573,   610,  -468,   407,   408,
     313,   365,   443,   562,   581,   841,   842,   445,   446,   447,
     448,   589,   449,   450,   451,   452,   850,   851,   453,   454,
     455,   456,   457,  1067,   458,   459,   460,   461,   462,   463,
     464,   465,   466,   467,   468,   469,   470,   471,  1076,   472,
     761,   473,   474,   475,   476,   477,   478,   479,   480,   481,
     482,   483,   772,   484,   485,   774,  1084,   486,   487,   488,
    1086,   489,   490,   491,   492,   493,   494,   495,   496,   497,
     498,   786,   499,   500,   501,   502,   503,   504,   505,   506,
     507,   508,   509,   510,   800,   511,   512,   802,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   812,   523,
     524,   525,   526,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   824,   536,   537,   538,   539,   540,   759,   541,
     542,   543,   544,   545,   763,   546,   547,   548,   549,   550,
     551,   771,   552,   553,   554,   555,   556,   776,   557,   558,
     559,   561,   564,   588,   583,   848,   594,   599,   600,   622,
     797,   716,   672,   582,   649,   854,   593,   856,   857,   858,
     598,   860,   601,   770,   603,   863,   864,   865,   866,   867,
     868,   805,   870,   605,   607,   809,   615,   748,   621,   876,
     590,   617,   591,   614,   880,   939,   749,   883,   616,   885,
     646,   950,   886,   623,   888,   658,   664,   891,   953,   635,
     829,   684,   894,   895,   666,   897,   835,   899,   686,   687,
     667,   677,   689,   592,   645,   690,   627,   691,   692,   597,
     844,   693,   697,   699,   847,   700,   602,   701,   702,   703,
     604,   704,   606,   708,   756,   757,   758,   612,   613,   706,
     713,   714,   723,   619,   663,   624,   767,   769,   631,   633,
     640,   642,   718,   644,   653,   279,   873,   777,   657,   659,
     668,   781,   782,   724,   725,   881,   882,   289,   683,   785,
     685,   294,   688,   887,   726,   733,   890,   734,   304,   694,
     669,   798,   696,   735,   801,   705,   707,   736,   737,   316,
     709,   739,   712,   741,   743,   715,   321,   698,   738,   744,
     745,   740,   742,   746,   747,   751,   818,   825,   914,   336,
     719,   823,   918,   816,   762,   826,   827,   838,   344,   922,
     610,   764,   347,   766,   350,   773,   902,   843,   837,   911,
     924,   778,   930,   357,   931,   807,   932,   936,   783,   940,
     973,   946,   947,   952,   974,   799,   803,   975,   852,   966,
     853,   976,   942,   855,   806,   810,   945,   830,   808,   383,
     779,   861,   836,   832,   846,   849,   874,   920,   977,   878,
     397,   903,   871,   822,   403,   879,   960,   406,  1007,   884,
     840,   877,   964,   889,   892,   608,   901,  1012,   912,  1014,
     915,   330,   970,   333,   921,   972,   917,  1021,   923,   928,
     893,  1022,   927,   431,   980,   434,   862,   900,   935,   941,
     925,   962,   981,   944,   951,   996,   997,  1030,   959,   963,
     979,   999,  1029,   967,   969,   971,   978,   988,   989,   991,
     990,  1034,   993,  1041,  1008,   992,  1011,  1045,   994,  1040,
     998,  1002,  1024,  1042,  1013,  1023,  1051,  1015,  1077,  1019,
    1028,  1026,  1069,  1078,  1052,  1053,  1082,  1055,  1083,  1057,
    1032,  1027,  1107,  1033,   938,  1037,  1039,  1044,  1046,  1049,
    1048,   839,   916,  1054,  1066,  1050,  1056,  1058,  1062,  1068,
    1059,  1061,  1070,  1074,  1080,  1101,  1102,   444,  1081,   369,
    1087,   804,   958,   437,  1094,   821,  1099,  1106,   898,  1031,
     896,   775,   272,   919,  1088,  1105,   416,   875,   968,   813,
     560,   872,  1085,   398,   859,   811,   943,   831,   869,   828,
    1090,   819,  1091,   834,  1092,   820,   965,   796,   784,   787,
     768,   259,   760,  1005,   913,  1004,  1096,  1006,  1095,     0,
     961,     0,     0,     0,   986,  1103,     0,     0,     0,     0,
       0,     0,     0,  1104,     0,     0,     0,     0,     0,     0,
     652,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   987,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1010,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1038,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1075,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   986,  1089,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1073,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,     0,    -7,     0,     0,     0,     0,     0,   987,     0,
      15,     0,    16,    17,    18,    19,    20,    21,     0,     0,
       0,     0,     0,    22,    23,     0,     0,     0,     0,     0,
       0,     0,  1100,    24,    25,    26,    27,    28,    29,    30,
       0,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,     0,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   248,     0,     0,  -116,   121,   122,   123,   124,
     125,   126,   127,    15,     0,    16,    17,    18,    19,    20,
      21,     0,     0,     0,     0,     0,    22,    23,     0,     0,
       0,     0,     0,     0,     0,     0,    24,    25,    26,    27,
      28,    29,    30,     0,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,     0,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,     0,    -8,   121,
     122,   123,   124,   125,   126,   127,    15,     0,    16,    17,
      18,    19,    20,    21,     0,     0,     0,     0,     0,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,    24,
      25,    26,    27,    28,    29,    30,     0,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,     0,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   248,     0,
       0,  -117,   121,   122,   123,   124,   125,   126,   127,    15,
       0,    16,    17,    18,    19,    20,    21,     0,     0,     0,
       0,     0,    22,    23,     0,     0,     0,     0,     0,     0,
       0,     0,    24,    25,    26,    27,    28,    29,    30,     0,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,     0,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   121,   122,   123,   124,   125,
     126,   127
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-758))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      15,    16,    17,    18,    19,    20,    21,   675,    23,    24,
      25,    26,    27,   309,    29,    30,    31,    32,    33,    34,
     297,    36,    37,    38,    39,    40,    41,    42,    43,   609,
      45,   338,    47,    48,   791,    50,    51,     8,     8,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,     8,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
       8,   779,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,    27,   101,   102,   103,    29,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   325,   127,    85,    86,   266,   330,    46,   266,   333,
      27,    22,   273,    52,    53,   273,    21,    22,    29,    21,
      22,    27,    21,    22,    29,    21,    22,    29,    48,    34,
      29,    21,    22,    29,     3,    21,    22,    57,    27,    29,
      13,    21,    22,    29,   126,    24,    27,     3,    27,    29,
      21,    22,     3,    73,    27,   316,    86,   318,    29,    13,
     321,   100,    21,    22,    21,    22,    21,    22,    21,    22,
      29,    27,    29,    27,    29,     0,    29,    22,    21,    22,
      21,    22,    27,   344,    29,    27,    29,    29,    29,   350,
       3,    13,   350,    13,    27,    27,   126,   147,   788,   151,
     344,   362,    13,   364,   362,    27,   364,    27,    23,   927,
     565,   566,   567,   568,    27,    23,    27,    32,     1,    21,
      22,    13,   383,   138,    32,     8,     9,    29,   143,    27,
     145,    29,     6,   139,   151,   140,   147,   915,   140,    13,
      24,   140,   403,   148,   140,    29,   148,   957,   958,   148,
     140,     5,   148,   142,   140,   136,   417,   612,   148,   417,
     140,   422,   423,   941,   422,   423,   145,    13,   148,   140,
     148,   252,   252,    95,    29,    95,    31,   148,    29,    27,
      31,   140,    29,   140,    31,   140,    29,   140,    31,   148,
     252,   969,  1059,   148,    27,   148,    29,   140,    10,   140,
     978,    10,   147,   893,   252,   148,   139,   139,    98,    99,
      10,    10,     4,    33,    95,   670,   671,    13,    13,    13,
      13,    42,    13,    13,    13,    13,   681,   682,    13,    13,
      13,    13,    13,  1011,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,  1026,    13,
     574,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,   586,    13,    13,   589,  1044,    13,    13,    13,
    1048,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,   605,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,   618,    13,    13,   621,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,   632,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,   646,    13,    13,    13,    13,    13,   572,    13,
      13,    13,    13,    13,   578,    13,    13,    13,    13,    13,
      13,   585,    13,    13,    13,    13,    13,   591,    13,    13,
       4,     3,     3,    11,   134,   679,   154,    42,    20,    20,
     157,   147,    11,    95,   151,   687,    29,   691,   692,   693,
      30,   695,    29,   136,    30,   699,   700,   701,   702,   703,
     704,   625,   706,    29,    29,   629,    29,     6,    29,   713,
      95,   134,   140,   140,   718,   801,   147,   721,    42,   723,
      29,   818,   726,    42,   728,    29,    29,   731,   825,    42,
     654,    29,   736,   737,    42,   739,   660,   741,    29,    29,
      42,    42,    30,    95,   140,    29,   145,    29,    29,    95,
     674,    29,    29,    29,   678,    29,    95,    29,    29,    29,
      95,    29,    95,    29,   569,   570,   571,    95,    95,    34,
      29,    29,    11,    95,   145,    95,   581,   582,    95,    95,
      95,    95,   141,    95,    95,   590,   710,   592,    95,    95,
      95,   596,   597,    29,    29,   719,   720,   602,    95,   604,
      95,   606,    95,   727,    11,    29,   730,    29,   613,    95,
     140,   616,    95,    29,   619,    95,    95,    11,    29,   624,
      95,    29,    95,    29,    29,   135,   631,   145,    95,    29,
      29,    95,    95,    29,    29,     7,    27,    27,   762,   644,
     140,   646,   766,    32,   140,     3,    95,    27,   653,   773,
     139,   145,   657,   140,   659,   140,    13,    95,   663,    13,
      29,   153,    30,   668,    29,   627,    30,     9,   137,    42,
      29,    42,    42,    24,    29,   136,   136,    30,   683,    42,
     685,    29,   806,   688,   140,   140,   810,   140,   145,   694,
     152,   696,   140,   145,   140,   140,   140,   134,    29,   142,
     705,   142,   707,   157,   709,   145,   830,   712,   912,   140,
     157,   153,   836,   140,   140,   982,   145,   921,   141,   923,
     141,   640,   846,   642,   141,   849,   145,   931,   141,   147,
     735,   935,   152,   738,   135,   740,   698,   742,   141,   141,
     155,   145,   151,   141,   141,    29,    29,   951,   141,   141,
     874,    29,    27,   141,   141,   141,   141,   141,   141,   141,
     884,     3,   141,   967,   141,   889,   141,   971,   892,    27,
     151,   149,   134,    11,   141,   136,    28,   153,    42,   145,
     136,   141,   156,    42,   988,   989,    22,   991,    29,   993,
     142,   141,    42,   142,   799,   141,   141,   141,   141,   146,
     141,   668,   764,   141,  1008,   145,   141,   141,   144,  1013,
     139,   139,   142,   145,   151,   134,   136,   129,   151,    74,
     147,   624,   827,   122,   150,   644,   151,   151,   740,   952,
     738,   590,    20,   769,  1050,  1089,   106,   712,   843,   633,
     252,   709,  1046,    94,   694,   631,   808,   657,   705,   653,
    1054,   640,  1056,   659,  1058,   642,   837,   613,   602,   606,
     581,    12,   573,   908,   760,   907,  1064,   908,  1062,    -1,
     832,    -1,    -1,    -1,   879,  1079,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1087,    -1,    -1,    -1,    -1,    -1,    -1,
     341,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   879,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   917,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     962,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1023,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1050,  1051,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1019,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,
      -1,    -1,     4,    -1,    -1,    -1,    -1,    -1,  1050,    -1,
      12,    -1,    14,    15,    16,    17,    18,    19,    -1,    -1,
      -1,    -1,    -1,    25,    26,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1074,    35,    36,    37,    38,    39,    40,    41,
      -1,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    -1,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    -1,    -1,     4,   158,   159,   160,   161,
     162,   163,   164,    12,    -1,    14,    15,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,    25,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    37,    38,
      39,    40,    41,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    -1,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,     4,   158,
     159,   160,   161,   162,   163,   164,    12,    -1,    14,    15,
      16,    17,    18,    19,    -1,    -1,    -1,    -1,    -1,    25,
      26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
      36,    37,    38,    39,    40,    41,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    -1,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,
      -1,     4,   158,   159,   160,   161,   162,   163,   164,    12,
      -1,    14,    15,    16,    17,    18,    19,    -1,    -1,    -1,
      -1,    -1,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    35,    36,    37,    38,    39,    40,    41,    -1,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    -1,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   158,   159,   160,   161,   162,
     163,   164
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     1,     8,     9,   166,   167,   168,   212,   213,     6,
      13,     3,     3,     0,     1,    12,    14,    15,    16,    17,
      18,    19,    25,    26,    35,    36,    37,    38,    39,    40,
      41,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   158,   159,   160,   161,   162,   163,   164,   169,   170,
     171,   172,   173,   175,   177,   179,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   198,   200,   202,   203,   204,   206,   208,   211,   217,
     218,   219,   220,   223,   226,   228,   231,   232,   236,   237,
     240,   241,   242,   245,   246,   247,   250,   251,   254,   256,
     257,   258,   259,   260,   261,   262,   263,   268,   269,   270,
     271,   272,   273,   276,   279,   281,   283,   284,   287,   288,
     289,   290,   293,   294,   296,   299,   300,   301,   302,   303,
     304,   305,   306,   309,   310,   313,   317,   318,   320,   323,
     326,   329,   332,   335,   336,   337,   339,   340,   347,   348,
     349,   351,   352,   353,   354,   355,   356,   357,   358,   361,
     363,   366,   368,   371,   373,   375,   377,   380,     1,   172,
     188,   214,   215,   216,   217,   236,    13,     5,   148,   392,
      13,    27,   362,   372,   362,   374,   362,   362,   378,   379,
     252,   362,   252,   362,    10,   362,   362,   248,   249,   362,
     207,   362,   341,   362,    10,   178,   362,   362,   362,   362,
     364,   365,   362,   367,   362,   369,   370,    27,    29,   222,
     382,   383,   359,   360,   362,   362,   362,   362,   319,   362,
     362,   362,   362,    10,   209,   210,   362,   316,   362,   307,
     308,   362,   207,   291,   292,   316,   362,   362,   311,   312,
     316,   314,   315,   316,   224,   225,   362,   362,   362,   207,
       3,   362,   327,   328,   362,   324,   325,   362,   330,   331,
     362,   333,   362,   362,   362,   196,   197,   362,   199,   362,
     362,   338,   362,   362,   362,    10,   205,   362,   207,   209,
     174,   362,   255,   362,   362,   180,   362,   362,   362,   362,
     362,   297,   298,   362,   282,   362,   295,   382,   295,   362,
     362,   362,   362,   362,   362,   285,   286,   362,   285,   280,
     362,   277,   278,   362,   274,   275,   362,   274,   274,   316,
     362,   362,   362,   391,   392,   362,   277,   362,   362,   362,
     362,   362,   362,   362,   362,   362,   362,   362,   362,   243,
     244,   362,   238,   239,   362,   221,   362,   221,   362,   362,
     362,   295,   362,     4,   171,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,     4,
     216,     3,    33,   393,     3,    29,    31,    29,    31,    21,
      22,    29,   140,   148,   388,   389,   390,    34,   140,   388,
     390,    95,    95,   134,   253,   140,   388,   390,    11,    42,
      95,   140,    95,    29,   154,   343,   344,    95,    30,    42,
      20,    29,    95,    30,    95,    29,    95,    29,   383,    27,
     139,   384,    95,    95,   140,    29,    42,   134,   321,    95,
     321,    29,    20,    42,    95,   140,   390,   145,   381,   140,
     390,    95,   390,    95,   381,    42,    24,    29,   230,   350,
      95,   381,    95,   381,    95,   140,    29,   229,   230,   151,
     233,   234,   233,    95,   140,   390,   391,    95,    29,    95,
     140,   388,   390,   145,    29,   334,    42,    42,    95,   140,
      29,    31,    11,   201,   140,   388,   390,    42,   140,   388,
     390,    29,    31,    95,    29,    95,    29,    29,    95,    30,
      29,    29,    29,    29,    95,   390,    95,    29,   145,    29,
      29,    29,    29,    29,    29,    95,    34,    95,    29,    95,
     140,   390,    95,    29,    29,   135,   147,   267,   141,   140,
     140,   388,   390,    11,    29,    29,    11,   140,   388,   390,
     140,   388,   390,    29,    29,    29,    11,    29,    95,    29,
      95,    29,    95,    29,    29,    29,    29,    29,     6,   147,
     394,     7,   222,   222,   222,   222,   362,   362,   362,   391,
     393,   381,   140,   391,   145,   376,   140,   362,   379,   362,
     136,   391,   381,   140,   381,   249,   391,   362,   153,   152,
     342,   362,   362,   137,   364,   362,   381,   369,    27,   384,
      22,    27,    29,   147,   385,   222,   359,   157,   362,   136,
     381,   362,   381,   136,   210,   391,   140,   382,   145,   391,
     140,   308,   381,   292,    27,   136,    32,   227,    27,   312,
     315,   224,   157,   362,   381,    27,     3,    95,   328,   391,
     140,   325,   145,   176,   331,   391,   140,   362,    27,   196,
     157,   222,   222,    95,   391,   376,   140,   391,   381,   140,
     222,   222,   362,   362,   176,   362,   381,   381,   381,   298,
     381,   362,   382,   381,   381,   381,   381,   381,   381,   286,
     381,   362,   278,   391,   140,   275,   381,   153,   142,   145,
     381,   391,   391,   381,   140,   381,   381,   391,   381,   140,
     391,   381,   140,   362,   381,   381,   243,   381,   238,   381,
     362,   145,    13,   142,   395,   138,   143,   145,   396,   397,
     399,    13,   141,   394,   391,   141,   382,   145,   391,   253,
     134,   141,   391,   141,    29,   155,   343,   152,   147,   345,
      30,    29,    30,   384,   385,   141,     9,   322,   362,   321,
      42,   141,   391,   382,   141,   391,    42,    42,    23,    32,
     230,   141,    24,   230,    29,   147,   235,     3,   362,   141,
     391,   382,   145,   141,   391,   334,    42,   141,   362,   141,
     391,   141,   391,    29,    29,    30,    29,    29,   141,   391,
     135,   151,    27,   264,   265,   266,   362,   382,   141,   141,
     391,   141,   391,   141,   391,   384,    29,    29,   151,    29,
     386,   398,   149,   403,   398,   397,   399,   381,   141,   376,
     382,   141,   381,   141,   381,   153,   343,    27,   142,   145,
     346,   381,   381,   136,   134,   376,   141,   141,   136,    27,
     381,   227,   142,   142,     3,   234,   234,   141,   382,   141,
      27,   381,    11,   376,   141,   381,   141,   376,   141,   146,
     145,    28,   381,   381,   141,   381,   141,   381,   141,   139,
     387,   139,   144,   400,   401,   402,   381,   376,   381,   156,
     142,    27,   151,   382,   145,   362,   376,    42,    42,    23,
     151,   151,    22,    29,   376,   381,   376,   147,   264,   362,
     381,   381,   381,   385,   150,   403,   402,    27,   151,   151,
     382,   134,   136,   381,   381,   267,   151,    42
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 267: /* "host_and_port" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->host_p);};

/* Line 1391 of yacc.c  */
#line 2642 "SIP_parse_.tab.c"
	break;
      case 274: /* "Secmechanism_1toN" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->sec_mech_list);};

/* Line 1391 of yacc.c  */
#line 2651 "SIP_parse_.tab.c"
	break;
      case 275: /* "Secmechanism" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->sec_mech);};

/* Line 1391 of yacc.c  */
#line 2660 "SIP_parse_.tab.c"
	break;
      case 277: /* "routebdy1toN" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->route_list);};

/* Line 1391 of yacc.c  */
#line 2669 "SIP_parse_.tab.c"
	break;
      case 278: /* "routeadr" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->route_val);};

/* Line 1391 of yacc.c  */
#line 2678 "SIP_parse_.tab.c"
	break;
      case 285: /* "conatact_1toN" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->c_list);};

/* Line 1391 of yacc.c  */
#line 2687 "SIP_parse_.tab.c"
	break;
      case 286: /* "conatact_value" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->p_list);};

/* Line 1391 of yacc.c  */
#line 2696 "SIP_parse_.tab.c"
	break;
      case 291: /* "h_urispec_1toN" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->history_entry_list);};

/* Line 1391 of yacc.c  */
#line 2705 "SIP_parse_.tab.c"
	break;
      case 292: /* "h_urispec" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->history_entry);};

/* Line 1391 of yacc.c  */
#line 2714 "SIP_parse_.tab.c"
	break;
      case 297: /* "anetspec_1toN" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->an_spec_list);};

/* Line 1391 of yacc.c  */
#line 2723 "SIP_parse_.tab.c"
	break;
      case 298: /* "anetspec" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->an_spec);};

/* Line 1391 of yacc.c  */
#line 2732 "SIP_parse_.tab.c"
	break;
      case 307: /* "vnetspec_1toN" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->n_spec_list);};

/* Line 1391 of yacc.c  */
#line 2741 "SIP_parse_.tab.c"
	break;
      case 308: /* "vnetspec" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->n_spec);};

/* Line 1391 of yacc.c  */
#line 2750 "SIP_parse_.tab.c"
	break;
      case 311: /* "p_urispec_1toN" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->u_spec_list);};

/* Line 1391 of yacc.c  */
#line 2759 "SIP_parse_.tab.c"
	break;
      case 312: /* "p_urispec" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->u_spec);};

/* Line 1391 of yacc.c  */
#line 2768 "SIP_parse_.tab.c"
	break;
      case 316: /* "p_nameaddr" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->naddr);};

/* Line 1391 of yacc.c  */
#line 2777 "SIP_parse_.tab.c"
	break;
      case 319: /* "event_list1toN" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->e_list);};

/* Line 1391 of yacc.c  */
#line 2786 "SIP_parse_.tab.c"
	break;
      case 321: /* "event_event" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->e_type);};

/* Line 1391 of yacc.c  */
#line 2795 "SIP_parse_.tab.c"
	break;
      case 322: /* "event_event_template1toN" */

/* Line 1391 of yacc.c  */
#line 326 "SIP_parse.y"
	{delete (yyvaluep->e_template_list);};

/* Line 1391 of yacc.c  */
#line 2804 "SIP_parse_.tab.c"
	break;

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1806 of yacc.c  */
#line 349 "SIP_parse.y"
    { 
        delete uriptr;
        uriptr=NULL;
        delete paramptr;
        paramptr=NULL;
        YYACCEPT;
      }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 356 "SIP_parse.y"
    {
        delete uriptr;
        uriptr=NULL;
        delete paramptr;
        paramptr=NULL;
         YYACCEPT;
      }
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 371 "SIP_parse.y"
    {
        // rqlineptr->method()=Method::str_to_enum($<char1024>1); EPTEBAL
        rqlineptr->method()=Method::str_to_enum((yyvsp[(1) - (6)].sv));
        rqlineptr->sipVersion()=(yyvsp[(5) - (6)].sv);
        rqlineptr->requestUri()= *uriptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        /*Free($1);*/
        /*Free($5);*/
      }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 382 "SIP_parse.y"
    {yyerrok; errorind_loc=255;}
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 385 "SIP_parse.y"
    {}
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 386 "SIP_parse.y"
    {}
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 391 "SIP_parse.y"
    {}
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 392 "SIP_parse.y"
    {}
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 393 "SIP_parse.y"
    {}
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 394 "SIP_parse.y"
    {}
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 395 "SIP_parse.y"
    { resetptr();  errorind_loc=255; yyerrok;}
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 399 "SIP_parse.y"
    { headerptr->allow()().fieldName()=FieldName::ALLOW__E;}
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 401 "SIP_parse.y"
    { headerptr->contentDisposition()().fieldName()=FieldName::CONTENT__DISPOSITION__E;}
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 403 "SIP_parse.y"
    { headerptr->contentEncoding()().fieldName()=FieldName::CONTENT__ENCODING__E;}
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 405 "SIP_parse.y"
    { headerptr->contentLanguage()().fieldName()=FieldName::CONTENT__LANGUAGE__E;}
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 407 "SIP_parse.y"
    { headerptr->contentLength()().fieldName()=FieldName::CONTENT__LENGTH__E;}
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 409 "SIP_parse.y"
    { headerptr->contentType()().fieldName()=FieldName::CONTENT__TYPE__E;}
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 410 "SIP_parse.y"
    { headerptr->expires()().fieldName()=FieldName::EXPIRES__E;}
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 413 "SIP_parse.y"
    { headerptr->allow()().methods()=OMIT_VALUE;}
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 414 "SIP_parse.y"
    {}
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 416 "SIP_parse.y"
    {
        headerptr->allow()().methods()()[allowcount] = (yyvsp[(2) - (2)].sv);
        allowcount++;
        /*Free($2);*/
      }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 421 "SIP_parse.y"
    {
        headerptr->allow()().methods()()[allowcount] = (yyvsp[(4) - (4)].sv);
        allowcount++;
        /*Free($4);*/
      }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 429 "SIP_parse.y"
    {
        headerptr->contentDisposition()().dispositionType()= (yyvsp[(3) - (3)].sv);
        headerptr->contentDisposition()().dispositionParams()=OMIT_VALUE;
        /*Free($3);*/
      }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 434 "SIP_parse.y"
    {
        headerptr->contentDisposition()().dispositionType()= (yyvsp[(3) - (4)].sv);
        headerptr->contentDisposition()().dispositionParams()()=*paramptr;
        delete paramptr;
        paramptr=new GenericParam__List;
        paramcount=0;
//        Free($3);
      }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 444 "SIP_parse.y"
    {}
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 445 "SIP_parse.y"
    {}
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 448 "SIP_parse.y"
    {}
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 451 "SIP_parse.y"
    {
       headerptr->contentEncoding()().contentCoding()[contentenccount]=trim((yyvsp[(2) - (2)].sv));
       contentenccount++;
//       Free($2);
     }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 456 "SIP_parse.y"
    {
       headerptr->contentEncoding()().contentCoding()[contentenccount]=trim((yyvsp[(4) - (4)].sv));
       contentenccount++;
//       Free($4);
     }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 463 "SIP_parse.y"
    {}
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 466 "SIP_parse.y"
    {
        headerptr->contentLanguage()().languageTag()[contentlangcount]=trim((yyvsp[(2) - (2)].sv));
        contentlangcount++;
//        Free($2);
      }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 471 "SIP_parse.y"
    {
        headerptr->contentLanguage()().languageTag()[contentlangcount]=trim((yyvsp[(4) - (4)].sv));
        contentlangcount++;
//        Free($4);
      }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 479 "SIP_parse.y"
    { headerptr->contentLength()().len() = str2int((yyvsp[(3) - (3)].sv)); /*Free($3)*/}
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 482 "SIP_parse.y"
    {
        headerptr->contentType()().mediaType() = trim((yyvsp[(3) - (3)].sv));
//        Free($3);
      }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 488 "SIP_parse.y"
    {
        headerptr->expires()().deltaSec() = (yyvsp[(3) - (3)].sv);
//        Free($3);
      }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 494 "SIP_parse.y"
    {
        headerptr->x__AUT()().x__AUT__Value() = (yyvsp[(3) - (3)].sv);
      }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 499 "SIP_parse.y"
    {
        headerptr->x__Carrier__Info()().x__Carrier__Info__Value() = (yyvsp[(3) - (3)].sv);
      }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 504 "SIP_parse.y"
    {
        headerptr->x__CHGDelay()().x__CHGDelay__Value() = (yyvsp[(3) - (3)].sv);
      }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 510 "SIP_parse.y"
    {    
        headerptr->x__CHGInfo()().x__ci__kind__data()= (yyvsp[(3) - (3)].sv); 
        headerptr->x__CHGInfo()().cDR__Record()=OMIT_VALUE; 
      }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 514 "SIP_parse.y"
    {
        headerptr->x__CHGInfo()().x__ci__kind__data()= (yyvsp[(3) - (5)].sv); 
        headerptr->x__CHGInfo()().cDR__Record()()=(yyvsp[(5) - (5)].sv);        
      }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 522 "SIP_parse.y"
    {headerptr->alertInfo()().fieldName()=FieldName::ALERT__INFO__E;}
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 524 "SIP_parse.y"
    {headerptr->authorization()().fieldName()=FieldName::AUTHORIZATION__E;}
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 525 "SIP_parse.y"
    {headerptr->inReplyTo()().fieldName()=FieldName::IN__REPLY__TO__E;}
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 527 "SIP_parse.y"
    {headerptr->maxForwards()().fieldName()=FieldName::MAX__FORWARDS__E;}
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 528 "SIP_parse.y"
    {headerptr->priority()().fieldName()=FieldName::PRIORITY__E;}
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 530 "SIP_parse.y"
    {headerptr->proxyAuthorization()().fieldName()=FieldName::PROXY__AUTHORIZATION__E;}
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 532 "SIP_parse.y"
    {headerptr->proxyRequire()().fieldName()=FieldName::PROXY__REQUIRE__E;}
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 533 "SIP_parse.y"
    {headerptr->route()().fieldName()=FieldName::ROUTE__E;}
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 534 "SIP_parse.y"
    {headerptr->subject()().fieldName()=FieldName::SUBJECT__E;}
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 535 "SIP_parse.y"
    {headerptr->refer__to()().fieldName()=FieldName::REFER__TO__E;}
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 536 "SIP_parse.y"
    {headerptr->p__served__user()().fieldName()=FieldName::P__SERVED__USER__E;}
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 537 "SIP_parse.y"
    {headerptr->p__profile__key()().fieldName()=FieldName::P__PROFILE__KEY__E;}
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 538 "SIP_parse.y"
    {headerptr->referred__by()().fieldName()=FieldName::REFERRED__BY__E;}
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 539 "SIP_parse.y"
    {headerptr->p__asserted__service()().fieldName()=FieldName::P__ASSERTED__SERVICE__E;}
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 540 "SIP_parse.y"
    {headerptr->p__preferred__service()().fieldName()=FieldName::P__PREFERRED__SERVICE__E;}
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 543 "SIP_parse.y"
    {
        headerptr->referred__by()().referer__uri().nameAddr().displayName() = (yyvsp[(3) - (7)].sv);
        headerptr->referred__by()().referer__uri().nameAddr().addrSpec()= *uriptr;
        headerptr->referred__by()().refererParams()=*paramptr;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;

//        Free($3);
      }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 557 "SIP_parse.y"
    {
        headerptr->referred__by()().referer__uri().nameAddr().displayName() 
                    = OMIT_VALUE;
        headerptr->referred__by()().referer__uri().nameAddr().addrSpec()= *uriptr;
        headerptr->referred__by()().refererParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 569 "SIP_parse.y"
    {
        headerptr->referred__by()().referer__uri().addrSpecUnion()= *uriptr;
        headerptr->referred__by()().refererParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 579 "SIP_parse.y"
    {
        headerptr->referred__by()().referer__uri().nameAddr().displayName() = (yyvsp[(3) - (6)].sv);
        headerptr->referred__by()().referer__uri().nameAddr().addrSpec()= *uriptr;
        headerptr->referred__by()().refererParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
//        Free($3);
      }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 588 "SIP_parse.y"
    {
        headerptr->referred__by()().referer__uri().nameAddr().displayName()
                    = OMIT_VALUE;
        headerptr->referred__by()().referer__uri().nameAddr().addrSpec()= *uriptr;
        headerptr->referred__by()().refererParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 597 "SIP_parse.y"
    {
        headerptr->referred__by()().referer__uri().addrSpecUnion()= *uriptr;
        headerptr->referred__by()().refererParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 608 "SIP_parse.y"
    {
        headerptr->p__profile__key()().profile__key().nameAddr().displayName() = (yyvsp[(3) - (7)].sv);
        headerptr->p__profile__key()().profile__key().nameAddr().addrSpec()= *uriptr;
        headerptr->p__profile__key()().profile__key__params()=*paramptr;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;

//        Free($3);
      }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 622 "SIP_parse.y"
    {
        headerptr->p__profile__key()().profile__key().nameAddr().displayName() 
                    = OMIT_VALUE;
        headerptr->p__profile__key()().profile__key().nameAddr().addrSpec()= *uriptr;
        headerptr->p__profile__key()().profile__key__params()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 634 "SIP_parse.y"
    {
        headerptr->p__profile__key()().profile__key().addrSpecUnion()= *uriptr;
        headerptr->p__profile__key()().profile__key__params()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 644 "SIP_parse.y"
    {
        headerptr->p__profile__key()().profile__key().nameAddr().displayName() = (yyvsp[(3) - (6)].sv);
        headerptr->p__profile__key()().profile__key().nameAddr().addrSpec()= *uriptr;
        headerptr->p__profile__key()().profile__key__params()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
//        Free($3);
      }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 653 "SIP_parse.y"
    {
        headerptr->p__profile__key()().profile__key().nameAddr().displayName()
                    = OMIT_VALUE;
        headerptr->p__profile__key()().profile__key().nameAddr().addrSpec()= *uriptr;
        headerptr->p__profile__key()().profile__key__params()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 662 "SIP_parse.y"
    {
        headerptr->p__profile__key()().profile__key().addrSpecUnion()= *uriptr;
        headerptr->p__profile__key()().profile__key__params()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 672 "SIP_parse.y"
    {
        headerptr->p__served__user()().served__user().nameAddr().displayName() = (yyvsp[(3) - (7)].sv);
        headerptr->p__served__user()().served__user().nameAddr().addrSpec()= *uriptr;
        headerptr->p__served__user()().served__user__params()=*paramptr;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;

//        Free($3);
      }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 686 "SIP_parse.y"
    {
        headerptr->p__served__user()().served__user().nameAddr().displayName() 
                    = OMIT_VALUE;
        headerptr->p__served__user()().served__user().nameAddr().addrSpec()= *uriptr;
        headerptr->p__served__user()().served__user__params()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 698 "SIP_parse.y"
    {
        headerptr->p__served__user()().served__user().addrSpecUnion()= *uriptr;
        headerptr->p__served__user()().served__user__params()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 708 "SIP_parse.y"
    {
        headerptr->p__served__user()().served__user().nameAddr().displayName() = (yyvsp[(3) - (6)].sv);
        headerptr->p__served__user()().served__user().nameAddr().addrSpec()= *uriptr;
        headerptr->p__served__user()().served__user__params()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
//        Free($3);
      }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 717 "SIP_parse.y"
    {
        headerptr->p__served__user()().served__user().nameAddr().displayName()
                    = OMIT_VALUE;
        headerptr->p__served__user()().served__user().nameAddr().addrSpec()= *uriptr;
        headerptr->p__served__user()().served__user__params()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 726 "SIP_parse.y"
    {
        headerptr->p__served__user()().served__user().addrSpecUnion()= *uriptr;
        headerptr->p__served__user()().served__user__params()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 735 "SIP_parse.y"
    {
        headerptr->p__Service__Indication()().service__indication() = trim((yyvsp[(3) - (3)].sv));
//        Free($3);
      }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 741 "SIP_parse.y"
    {
        headerptr->p__Service__Notification()().service__notification() = trim((yyvsp[(3) - (3)].sv));
//        Free($3);
      }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 748 "SIP_parse.y"
    {
        headerptr->refer__to()().addr().nameAddr().displayName() = (yyvsp[(3) - (7)].sv);
        headerptr->refer__to()().addr().nameAddr().addrSpec()= *uriptr;
        headerptr->refer__to()().referToParams()=*paramptr;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;

//        Free($3);
      }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 762 "SIP_parse.y"
    {
        headerptr->refer__to()().addr().nameAddr().displayName() 
                    = OMIT_VALUE;
        headerptr->refer__to()().addr().nameAddr().addrSpec()= *uriptr;
        headerptr->refer__to()().referToParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 774 "SIP_parse.y"
    {
        headerptr->refer__to()().addr().addrSpecUnion()= *uriptr;
        headerptr->refer__to()().referToParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 784 "SIP_parse.y"
    {
        headerptr->refer__to()().addr().nameAddr().displayName() = (yyvsp[(3) - (6)].sv);
        headerptr->refer__to()().addr().nameAddr().addrSpec()= *uriptr;
        headerptr->refer__to()().referToParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
//        Free($3);
      }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 793 "SIP_parse.y"
    {
        headerptr->refer__to()().addr().nameAddr().displayName()
                    = OMIT_VALUE;
        headerptr->refer__to()().addr().nameAddr().addrSpec()= *uriptr;
        headerptr->refer__to()().referToParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 802 "SIP_parse.y"
    {
        headerptr->refer__to()().addr().addrSpecUnion()= *uriptr;
        headerptr->refer__to()().referToParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 811 "SIP_parse.y"
    {}
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 818 "SIP_parse.y"
    {
        headerptr->alertInfo()().alertInfoBody()()[alertinfcount].url()=(yyvsp[(3) - (4)].sv);
        headerptr->alertInfo()().alertInfoBody()()[alertinfcount].
                            genericParams()=OMIT_VALUE;
        alertinfcount++;
//        Free($3);
      }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 825 "SIP_parse.y"
    {
        headerptr->alertInfo()().alertInfoBody()()[alertinfcount].url()=(yyvsp[(3) - (5)].sv);
        headerptr->alertInfo()().alertInfoBody()()[alertinfcount].
                            genericParams()()=*paramptr;
        paramcount=0;
        alertinfcount++;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($3);
      }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 837 "SIP_parse.y"
    {}
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 840 "SIP_parse.y"
    {
        headerptr->authorization()().body().digestResponse()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
      }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 846 "SIP_parse.y"
    {
         headerptr->authorization()().body().otherResponse().authScheme()=(yyvsp[(2) - (3)].sv);
         headerptr->authorization()().body().otherResponse().authParams()=
                            *paramptr;
         paramcount=0;
         delete paramptr;
         paramptr=new GenericParam__List;
//         Free($2);
      }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 857 "SIP_parse.y"
    {}
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 860 "SIP_parse.y"
    {
        headerptr->inReplyTo()().callids()[inreplytocount] = (yyvsp[(1) - (1)].sv);
        inreplytocount++;
//        Free($1);
      }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 865 "SIP_parse.y"
    {
        headerptr->inReplyTo()().callids()[inreplytocount] = (yyvsp[(4) - (4)].sv);
        inreplytocount++;
//        Free($4);
      }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 873 "SIP_parse.y"
    { headerptr->maxForwards()().forwards()=str2int((yyvsp[(3) - (3)].sv));/*Free($3)*/}
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 876 "SIP_parse.y"
    {
        headerptr->priority()().priorityValue() = trim((yyvsp[(2) - (2)].sv));
//        Free($2);
      }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 882 "SIP_parse.y"
    {}
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 885 "SIP_parse.y"
    {
        headerptr->proxyAuthorization()().credentials().digestResponse()=
                              *paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
      }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 892 "SIP_parse.y"
    {
        headerptr->proxyAuthorization()().credentials().otherResponse().
                              authScheme()=(yyvsp[(2) - (3)].sv);
        headerptr->proxyAuthorization()().credentials().otherResponse().
                              authParams()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($2);
      }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 904 "SIP_parse.y"
    {
        if(!proxyreqcount){
          headerptr->proxyRequire()().optionsTags()= *optptr;
          delete optptr;
        }
        proxyreqcount=optioncount;
      }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 913 "SIP_parse.y"
    {
        (*optptr)[optioncount]=(yyvsp[(2) - (2)].sv);
        optioncount++;
//        Free($2);
      }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 918 "SIP_parse.y"
    {
        (*optptr)[optioncount]=(yyvsp[(4) - (4)].sv);
        optioncount++;
//        Free($4);
      }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 925 "SIP_parse.y"
    {
        if(!routecount){
          headerptr->route()().routeBody()= *routeptr;
          delete routeptr;
        }
        routecount=rcount;
      }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 934 "SIP_parse.y"
    {}
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 935 "SIP_parse.y"
    {}
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 938 "SIP_parse.y"
    {
        (*routeptr)[rcount].nameAddr().displayName()=(yyvsp[(2) - (6)].sv);
        (*routeptr)[rcount].nameAddr().addrSpec()= *uriptr;
        (*routeptr)[rcount].rrParam()()= *paramptr;
        rcount++;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
//        Free($2);
      }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 952 "SIP_parse.y"
    {
        (*routeptr)[rcount].nameAddr().displayName()=OMIT_VALUE;
        (*routeptr)[rcount].nameAddr().addrSpec()= *uriptr;
        (*routeptr)[rcount].rrParam()()= *paramptr;
        rcount++;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 965 "SIP_parse.y"
    {
        (*routeptr)[rcount].nameAddr().displayName()=(yyvsp[(2) - (5)].sv);
        (*routeptr)[rcount].nameAddr().addrSpec()= *uriptr;
        (*routeptr)[rcount].rrParam()= OMIT_VALUE;
        rcount++;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
//        Free($2);
      }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 976 "SIP_parse.y"
    {
        (*routeptr)[rcount].nameAddr().displayName()=OMIT_VALUE;
        (*routeptr)[rcount].nameAddr().addrSpec()= *uriptr;
        (*routeptr)[rcount].rrParam()= OMIT_VALUE;
        rcount++;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 988 "SIP_parse.y"
    {
        headerptr->subject()().summary()=trim((yyvsp[(2) - (2)].sv));
//        Free($2);
      }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 1000 "SIP_parse.y"
    {

/*	EPTEBAL */
//	int offset = $<sip_sv>1.offset;
//	int len = $<sip_sv>1.length;
//      stlineptr->sipVersion() = CHARSTRING(len, stream_buffer+offset);
//      stlineptr->sipVersion() = stream_buffer+offset;

        stlineptr->sipVersion() = (yyvsp[(1) - (6)].sv); /* EPTEBAL */
        stlineptr->statusCode() = (yyvsp[(3) - (6)].iv);
        stlineptr->reasonPhrase() = (yyvsp[(5) - (6)].sv);
        /*Free($1);*/
//	Free($5);
      }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 1014 "SIP_parse.y"
    { yyerrok; errorind_loc=255;}
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 1017 "SIP_parse.y"
    {}
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 1018 "SIP_parse.y"
    {}
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 1023 "SIP_parse.y"
    {}
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 1024 "SIP_parse.y"
    {}
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 1025 "SIP_parse.y"
    {}
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 1026 "SIP_parse.y"
    {}
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 1027 "SIP_parse.y"
    { resetptr(); yyerrok; errorind_loc=255;}
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 1030 "SIP_parse.y"
    {headerptr->errorInfo()().fieldName()=FieldName::ERROR__INFO__E;}
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 1032 "SIP_parse.y"
    {headerptr->proxyAuthenticate()().fieldName()=FieldName::PROXY__AUTHENTICATE__E;}
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 1033 "SIP_parse.y"
    {headerptr->retryAfter()().fieldName()=FieldName::RETRY__AFTER__E;}
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 1034 "SIP_parse.y"
    {headerptr->server()().fieldName()=FieldName::SERVER__E;}
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 1035 "SIP_parse.y"
    {headerptr->unsupported()().fieldName()=FieldName::UNSUPPORTED__E;}
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 1037 "SIP_parse.y"
    {headerptr->authenticationInfo()().fieldName()=FieldName::AUTHENTICATION__INFO__E;}
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 1038 "SIP_parse.y"
    {headerptr->warning()().fieldName()=FieldName::WARNING__E;}
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 1040 "SIP_parse.y"
    {headerptr->wwwAuthenticate()().fieldName()=FieldName::WWW__AUTHENTICATE__E;}
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 1043 "SIP_parse.y"
    {
        headerptr->authenticationInfo()().ainfo()=*paramptr;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 1051 "SIP_parse.y"
    { 
      if(headerptr->p__asserted__service().ispresent()){
        int a=headerptr->p__asserted__service()().p__as().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].service_1toN)->size_of();b++){
          headerptr->p__asserted__service()().p__as()[a]=(*(yyvsp[(2) - (2)].service_1toN))[b];
          a++;
          }
      }
      else {
        headerptr->p__asserted__service()().p__as()=*(yyvsp[(2) - (2)].service_1toN);
      }
      delete (yyvsp[(2) - (2)].service_1toN);    
      }
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 1066 "SIP_parse.y"
    { 
      if(headerptr->p__preferred__service().ispresent()){
        int a=headerptr->p__preferred__service()().p__ps().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].service_1toN)->size_of();b++){
          headerptr->p__preferred__service()().p__ps()[a]=(*(yyvsp[(2) - (2)].service_1toN))[b];
          a++;
          }
      }
      else {
        headerptr->p__preferred__service()().p__ps()=*(yyvsp[(2) - (2)].service_1toN);
      }
      delete (yyvsp[(2) - (2)].service_1toN);    
      }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 1081 "SIP_parse.y"
    {
        (yyval.service_1toN) = new Service__ID__List;
        (*(yyval.service_1toN))[0] = (yyvsp[(2) - (2)].sv);
      //  delete $2;
      }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 1086 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (4)].service_1toN)->size_of();
        (yyval.service_1toN)=(yyvsp[(1) - (4)].service_1toN);
        (*(yyval.service_1toN))[a] = (yyvsp[(4) - (4)].sv);
     //   delete $4;
      }
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 1094 "SIP_parse.y"
    {}
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 1095 "SIP_parse.y"
    {}
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 1098 "SIP_parse.y"
    {}
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 1105 "SIP_parse.y"
    {
        headerptr->errorInfo()().errorInfo()()[errorinfocount].uri()=(yyvsp[(3) - (4)].sv);
        headerptr->errorInfo()().errorInfo()()[errorinfocount].genericParams()
                   =OMIT_VALUE;
        errorinfocount++;
//        Free($3);
      }
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 1112 "SIP_parse.y"
    {
        headerptr->errorInfo()().errorInfo()()[errorinfocount].uri()=(yyvsp[(3) - (5)].sv);
        headerptr->errorInfo()().errorInfo()()[errorinfocount].
                  genericParams()()=*paramptr;
        paramcount=0;
        errorinfocount++;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($3);
      }
    break;

  case 145:

/* Line 1806 of yacc.c  */
#line 1124 "SIP_parse.y"
    {
        headerptr->retryAfter()().deltaSec() = (yyvsp[(3) - (3)].sv);
        headerptr->retryAfter()().comment() = OMIT_VALUE;
        headerptr->retryAfter()().retryParams() = OMIT_VALUE;
//        Free($3);
      }
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 1130 "SIP_parse.y"
    {
        headerptr->retryAfter()().deltaSec() = (yyvsp[(3) - (7)].sv);
        headerptr->retryAfter()().comment()() = comment_buf+1;
        headerptr->retryAfter()().retryParams() = OMIT_VALUE;
//        Free($3);
        comment_buf[1]='\0';
      }
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 1138 "SIP_parse.y"
    {
        headerptr->retryAfter()().deltaSec() = (yyvsp[(3) - (8)].sv);
        headerptr->retryAfter()().comment()() = comment_buf+1;
        headerptr->retryAfter()().retryParams()() = *paramptr;
//        Free($3);
        delete paramptr;
        paramptr=new GenericParam__List;
        paramcount=0;
        comment_buf[1]='\0';
      }
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 1148 "SIP_parse.y"
    {
        headerptr->retryAfter()().deltaSec() = (yyvsp[(3) - (4)].sv);
        headerptr->retryAfter()().comment() = OMIT_VALUE;
        headerptr->retryAfter()().retryParams()() = *paramptr;
//        Free($3);
        delete paramptr;
        paramptr=new GenericParam__List;
        paramcount=0;
      }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 1159 "SIP_parse.y"
    {
      strcat(comment_buf+1,(yyvsp[(1) - (1)].sv));
//      Free($1);
    }
    break;

  case 150:

/* Line 1806 of yacc.c  */
#line 1163 "SIP_parse.y"
    {
      strcat(comment_buf+1,(yyvsp[(2) - (2)].sv));
//      Free($2);
    }
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 1169 "SIP_parse.y"
    {}
    break;

  case 152:

/* Line 1806 of yacc.c  */
#line 1171 "SIP_parse.y"
    {
        headerptr->server()().serverBody()[servercount]= (yyvsp[(1) - (1)].sv);
        servercount++;
        Free((yyvsp[(1) - (1)].sv));
      }
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 1176 "SIP_parse.y"
    {
        headerptr->server()().serverBody()[servercount]= (yyvsp[(3) - (3)].sv);
        servercount++;
        Free((yyvsp[(3) - (3)].sv));
      }
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 1183 "SIP_parse.y"
    {
        size_t comment_buflen = strlen(comment_buf+1);
        char *atm=(char *)Malloc(comment_buflen+3);
        atm[0]='(';
        strcpy(atm+1,comment_buf+1);
        atm[comment_buflen+1] = ')';
        atm[comment_buflen+2] = '\0';
        (yyval.sv)=atm;
        comment_buf[1]='\0';
      }
    break;

  case 155:

/* Line 1806 of yacc.c  */
#line 1193 "SIP_parse.y"
    {
        char *atm=(char *)Malloc(strlen((yyvsp[(1) - (1)].sv))+1);
        strcpy(atm,(yyvsp[(1) - (1)].sv));
        (yyval.sv)=atm;
    }
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 1200 "SIP_parse.y"
    {
        if(!unsuppcount){
            headerptr->unsupported()().optionsTags()= *optptr;
            delete optptr;
        }
        unsuppcount=optioncount;
      }
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 1209 "SIP_parse.y"
    {}
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 1210 "SIP_parse.y"
    {}
    break;

  case 159:

/* Line 1806 of yacc.c  */
#line 1213 "SIP_parse.y"
    {}
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 1214 "SIP_parse.y"
    {}
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 1215 "SIP_parse.y"
    {}
    break;

  case 162:

/* Line 1806 of yacc.c  */
#line 1218 "SIP_parse.y"
    {
        headerptr->warning()().warningValue()[warncount].warnCode()=(yyvsp[(1) - (5)].iv);
        (yyvsp[(5) - (5)].sv)[strlen((yyvsp[(5) - (5)].sv))-1]='\0';
        headerptr->warning()().warningValue()[warncount].WarnText()=(yyvsp[(5) - (5)].sv)+1;

        warncount++;
//        Free($5);
      }
    break;

  case 163:

/* Line 1806 of yacc.c  */
#line 1228 "SIP_parse.y"
    {
        headerptr->warning()().warningValue()[warncount].warnAgent().
                hostPort().host()=trimOnIPv6((yyvsp[(1) - (3)].sv));
        headerptr->warning()().warningValue()[warncount].warnAgent().
                hostPort().portField() = (yyvsp[(3) - (3)].iv);
//        Free($1);
      }
    break;

  case 164:

/* Line 1806 of yacc.c  */
#line 1235 "SIP_parse.y"
    {
        if(strchr((yyvsp[(1) - (1)].sv),'.') || strchr((yyvsp[(1) - (1)].sv),':')){
          headerptr->warning()().warningValue()[warncount].warnAgent().
                hostPort().host()=trimOnIPv6((yyvsp[(1) - (1)].sv));
          headerptr->warning()().warningValue()[warncount].warnAgent().
                hostPort().portField() = OMIT_VALUE;
        }
        else{
          headerptr->warning()().warningValue()[warncount].warnAgent().
                pseudonym()=(yyvsp[(1) - (1)].sv);
        }
//        Free($1);
      }
    break;

  case 165:

/* Line 1806 of yacc.c  */
#line 1248 "SIP_parse.y"
    {
        headerptr->warning()().warningValue()[warncount].warnAgent().
                hostPort().host()=trimOnIPv6((yyvsp[(1) - (3)].sv));
        headerptr->warning()().warningValue()[warncount].warnAgent().
                hostPort().portField() = (yyvsp[(3) - (3)].iv);
//        Free($1);
      }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 1255 "SIP_parse.y"
    {
        if(strchr((yyvsp[(1) - (1)].sv),'.') || strchr((yyvsp[(1) - (1)].sv),':')){
          headerptr->warning()().warningValue()[warncount].warnAgent().
                hostPort().host()=trimOnIPv6((yyvsp[(1) - (1)].sv));
          headerptr->warning()().warningValue()[warncount].warnAgent().
                hostPort().portField() = OMIT_VALUE;
        }
        else{
          headerptr->warning()().warningValue()[warncount].warnAgent().
                pseudonym()=(yyvsp[(1) - (1)].sv);
        }
//        Free($1);
      }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 1272 "SIP_parse.y"
    {headerptr->accept()().fieldName()=FieldName::ACCEPT__E;}
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 1274 "SIP_parse.y"
    {headerptr->acceptEncoding()().fieldName()=FieldName::ACCEPT__ENCODING__E;}
    break;

  case 169:

/* Line 1806 of yacc.c  */
#line 1276 "SIP_parse.y"
    {headerptr->acceptLanguage()().fieldName()=FieldName::ACCEPT__LANGUAGE__E;}
    break;

  case 170:

/* Line 1806 of yacc.c  */
#line 1277 "SIP_parse.y"
    {headerptr->callId()().fieldName()=FieldName::CALL__ID__E;}
    break;

  case 171:

/* Line 1806 of yacc.c  */
#line 1278 "SIP_parse.y"
    {headerptr->callInfo()().fieldName()=FieldName::CALL__INFO__E;}
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 1279 "SIP_parse.y"
    {headerptr->contact()().fieldName()=FieldName::CONTACT__E;}
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 1280 "SIP_parse.y"
    {headerptr->cSeq()().fieldName()=FieldName:: CSEQ__E;}
    break;

  case 174:

/* Line 1806 of yacc.c  */
#line 1281 "SIP_parse.y"
    {headerptr->date()().fieldName()=FieldName::DATE__E;}
    break;

  case 175:

/* Line 1806 of yacc.c  */
#line 1282 "SIP_parse.y"
    {headerptr->minExpires()().fieldName()=FieldName::MIN__EXPIRES__E;}
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 1283 "SIP_parse.y"
    {headerptr->event()().fieldName()=FieldName::EVENT__E;}
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 1284 "SIP_parse.y"
    {headerptr->allow__events()().fieldName()=FieldName::ALLOW__EVENTS__E;}
    break;

  case 178:

/* Line 1806 of yacc.c  */
#line 1285 "SIP_parse.y"
    {headerptr->acceptResourcePriority()().fieldName()=FieldName::ACCEPT__RESOURCE__PRIORITY__E;}
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 1286 "SIP_parse.y"
    {headerptr->resourcePriority()().fieldName()=FieldName::RESOURCE__PRIORITY__E;}
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 1287 "SIP_parse.y"
    {headerptr->fromField()().fieldName()=FieldName::FROM__E;errorind_loc&=254;}
    break;

  case 181:

/* Line 1806 of yacc.c  */
#line 1289 "SIP_parse.y"
    {headerptr->historyInfo()().fieldName()=FieldName::HISTORY__INFO__E;}
    break;

  case 182:

/* Line 1806 of yacc.c  */
#line 1291 "SIP_parse.y"
    {headerptr->mimeVersion()().fieldName()=FieldName::MIME__VERSION__E;}
    break;

  case 183:

/* Line 1806 of yacc.c  */
#line 1293 "SIP_parse.y"
    {headerptr->organization()().fieldName()=FieldName::ORGANIZATION__E;}
    break;

  case 184:

/* Line 1806 of yacc.c  */
#line 1294 "SIP_parse.y"
    {headerptr->privacy()().fieldName()=FieldName::PRIVACY__E;}
    break;

  case 185:

/* Line 1806 of yacc.c  */
#line 1296 "SIP_parse.y"
    {headerptr->p__associated__uri()().fieldName()=FieldName::P__ASSOCIATED__URI;}
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 1298 "SIP_parse.y"
    {headerptr->diversion()().fieldName()=FieldName::DIVERSION__E;}
    break;

  case 187:

/* Line 1806 of yacc.c  */
#line 1300 "SIP_parse.y"
    {headerptr->p__called__party__id()().fieldName()=FieldName::P__CALLED__PARTY__ID;}
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 1302 "SIP_parse.y"
    {headerptr->passertedID()().fieldName()=FieldName::P__ASSERTED__ID__E;}
    break;

  case 189:

/* Line 1806 of yacc.c  */
#line 1304 "SIP_parse.y"
    {headerptr->p__visited__network__id()().fieldName()=FieldName::P__VISITED__NETWORK__ID;}
    break;

  case 190:

/* Line 1806 of yacc.c  */
#line 1306 "SIP_parse.y"
    {headerptr->ppreferredID()().fieldName()=FieldName::P__PREFERRED__ID__E;}
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 1308 "SIP_parse.y"
    {headerptr->p__access__network__info()().fieldName()=FieldName::P__ACCESS__NETWORK__INFO;}
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 1310 "SIP_parse.y"
    {headerptr->p__charging__function__address()().fieldName()=FieldName::P__CHARGING__FUNCTION__ADDRESS;}
    break;

  case 193:

/* Line 1806 of yacc.c  */
#line 1312 "SIP_parse.y"
    {headerptr->p__charging__vector()().fieldName()=FieldName::P__CHARGING__VECTOR;}
    break;

  case 194:

/* Line 1806 of yacc.c  */
#line 1313 "SIP_parse.y"
    {headerptr->rack()().fieldName()=FieldName::RACK__E;}
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 1314 "SIP_parse.y"
    {headerptr->reason()().fieldName()=FieldName::REASON__E;}
    break;

  case 196:

/* Line 1806 of yacc.c  */
#line 1315 "SIP_parse.y"
    {headerptr->rseq()().fieldName()=FieldName::RSEQ__E;}
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 1317 "SIP_parse.y"
    {headerptr->recordRoute()().fieldName()=FieldName::RECORD__ROUTE__E;}
    break;

  case 198:

/* Line 1806 of yacc.c  */
#line 1318 "SIP_parse.y"
    {headerptr->replyTo()().fieldName()=FieldName::REPLY__TO__E;}
    break;

  case 199:

/* Line 1806 of yacc.c  */
#line 1319 "SIP_parse.y"
    {headerptr->require()().fieldName()=FieldName::REQUIRE__E;}
    break;

  case 200:

/* Line 1806 of yacc.c  */
#line 1320 "SIP_parse.y"
    {headerptr->supported()().fieldName()=FieldName::SUPPORTED__E;}
    break;

  case 201:

/* Line 1806 of yacc.c  */
#line 1321 "SIP_parse.y"
    {headerptr->min__SE()().fieldName()=FieldName::MIN__SE__E;}
    break;

  case 202:

/* Line 1806 of yacc.c  */
#line 1322 "SIP_parse.y"
    {headerptr->session__expires()().fieldName()=FieldName::SESSION__EXPIRES__E;}
    break;

  case 203:

/* Line 1806 of yacc.c  */
#line 1323 "SIP_parse.y"
    {headerptr->session__id()().fieldName()=FieldName::SESSION__ID__E;}
    break;

  case 204:

/* Line 1806 of yacc.c  */
#line 1324 "SIP_parse.y"
    {headerptr->subscription__state()().fieldName()=FieldName::SUBSCRIPTION__STATE__E;}
    break;

  case 205:

/* Line 1806 of yacc.c  */
#line 1325 "SIP_parse.y"
    {headerptr->timestamp()().fieldName()=FieldName::TIMESTAMP__E;}
    break;

  case 206:

/* Line 1806 of yacc.c  */
#line 1326 "SIP_parse.y"
    {headerptr->toField()().fieldName()=FieldName::TO__E;errorind_loc&=253;}
    break;

  case 207:

/* Line 1806 of yacc.c  */
#line 1327 "SIP_parse.y"
    {headerptr->userAgent()().fieldName()=FieldName::USER__AGENT__E;}
    break;

  case 208:

/* Line 1806 of yacc.c  */
#line 1328 "SIP_parse.y"
    {headerptr->via()().fieldName()=FieldName::VIA__E;errorind_loc&=251;}
    break;

  case 209:

/* Line 1806 of yacc.c  */
#line 1329 "SIP_parse.y"
    {headerptr->accept__contact()().fieldName()=FieldName::ACCEPT__CONTACT__E;}
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 1330 "SIP_parse.y"
    {headerptr->reject__contact()().fieldName()=FieldName::REJECT__CONTACT__E;}
    break;

  case 211:

/* Line 1806 of yacc.c  */
#line 1331 "SIP_parse.y"
    {headerptr->request__disp()().fieldName()=FieldName::REQUEST__DISP__E;}
    break;

  case 212:

/* Line 1806 of yacc.c  */
#line 1332 "SIP_parse.y"
    {headerptr->p__media__auth()().fieldName()=FieldName::P__MEDIA__AUTH__E;}
    break;

  case 213:

/* Line 1806 of yacc.c  */
#line 1333 "SIP_parse.y"
    {headerptr->path()().fieldName()=FieldName::PATH__E;}
    break;

  case 214:

/* Line 1806 of yacc.c  */
#line 1334 "SIP_parse.y"
    {headerptr->security__client()().fieldName()=FieldName::SECURITY__CLIENT__E;}
    break;

  case 215:

/* Line 1806 of yacc.c  */
#line 1335 "SIP_parse.y"
    {headerptr->security__server()().fieldName()=FieldName::SECURITY__SERVER__E;}
    break;

  case 216:

/* Line 1806 of yacc.c  */
#line 1336 "SIP_parse.y"
    {headerptr->security__verify()().fieldName()=FieldName::SECURITY__VERIFY__E;}
    break;

  case 217:

/* Line 1806 of yacc.c  */
#line 1337 "SIP_parse.y"
    {headerptr->p__DCS__trace__pty__id()().fieldName()=FieldName::P__DCS__TRACE__PTY__ID__E;}
    break;

  case 218:

/* Line 1806 of yacc.c  */
#line 1338 "SIP_parse.y"
    {headerptr->p__DCS__OSPS()().fieldName()=FieldName::P__DCS__OSPS__E;}
    break;

  case 219:

/* Line 1806 of yacc.c  */
#line 1339 "SIP_parse.y"
    {headerptr->p__Early__Media()().fieldName()=FieldName::P__EARLY__MEDIA__E;}
    break;

  case 220:

/* Line 1806 of yacc.c  */
#line 1340 "SIP_parse.y"
    {headerptr->p__DCS__billing__info()().fieldName()=FieldName::P__DCS__BILLING__INFO__E;}
    break;

  case 221:

/* Line 1806 of yacc.c  */
#line 1341 "SIP_parse.y"
    {headerptr->p__DCS__LAES()().fieldName()=FieldName::P__DCS__LAES__E;}
    break;

  case 222:

/* Line 1806 of yacc.c  */
#line 1342 "SIP_parse.y"
    {headerptr->p__DCS__redirect()().fieldName()=FieldName::P__DCS__REDIRECT__E;}
    break;

  case 223:

/* Line 1806 of yacc.c  */
#line 1343 "SIP_parse.y"
    {headerptr->p__user__database()().fieldName()=FieldName::P__USER__DATABASE__E;}
    break;

  case 224:

/* Line 1806 of yacc.c  */
#line 1344 "SIP_parse.y"
    {headerptr->service__route()().fieldName()=FieldName::SERVICE__ROUTE__E;}
    break;

  case 225:

/* Line 1806 of yacc.c  */
#line 1345 "SIP_parse.y"
    {headerptr->replaces()().fieldName()=FieldName::REPLACES__E;}
    break;

  case 226:

/* Line 1806 of yacc.c  */
#line 1346 "SIP_parse.y"
    {headerptr->sip__ETag()().fieldName()=FieldName::SIP__ETAG__E;}
    break;

  case 227:

/* Line 1806 of yacc.c  */
#line 1347 "SIP_parse.y"
    {headerptr->sip__If__Match()().fieldName()=FieldName::SIP__IF__MATCH__E;}
    break;

  case 228:

/* Line 1806 of yacc.c  */
#line 1348 "SIP_parse.y"
    {headerptr->join()().fieldName()=FieldName::JOIN__E;}
    break;

  case 229:

/* Line 1806 of yacc.c  */
#line 1349 "SIP_parse.y"
    {headerptr->p__Service__Indication()().fieldName()=FieldName::P__SERVICE__INDICATION__E;}
    break;

  case 230:

/* Line 1806 of yacc.c  */
#line 1350 "SIP_parse.y"
    {headerptr->p__Service__Notification()().fieldName()=FieldName::P__SERVICE__NOTIFICATION__E;}
    break;

  case 231:

/* Line 1806 of yacc.c  */
#line 1352 "SIP_parse.y"
    {headerptr->answer__mode()().fieldName()=FieldName::ANSWER__MODE__E;}
    break;

  case 232:

/* Line 1806 of yacc.c  */
#line 1354 "SIP_parse.y"
    {headerptr->priv__answer__mode()().fieldName()=FieldName::PRIV__ANSWER__MODE__E;}
    break;

  case 233:

/* Line 1806 of yacc.c  */
#line 1356 "SIP_parse.y"
    {headerptr->alert__mode()().fieldName()=FieldName::ALERT__MODE__E;}
    break;

  case 234:

/* Line 1806 of yacc.c  */
#line 1358 "SIP_parse.y"
    {headerptr->refer__sub()().fieldName()=FieldName::REFER__SUB__E;}
    break;

  case 235:

/* Line 1806 of yacc.c  */
#line 1360 "SIP_parse.y"
    {headerptr->p__alerting__mode()().fieldName()=FieldName::P__ALERTING__MODE__E;}
    break;

  case 236:

/* Line 1806 of yacc.c  */
#line 1362 "SIP_parse.y"
    {headerptr->p__answer__state()().fieldName()=FieldName::P__ANSWER__STATE__E;}
    break;

  case 237:

/* Line 1806 of yacc.c  */
#line 1363 "SIP_parse.y"
    {headerptr->geolocation()().fieldName()=FieldName::GEOLOCATION__E;}
    break;

  case 238:

/* Line 1806 of yacc.c  */
#line 1364 "SIP_parse.y"
    {headerptr->geolocation__routing()().fieldName()=FieldName::GEOLOCATION__ROUTING__E;}
    break;

  case 239:

/* Line 1806 of yacc.c  */
#line 1365 "SIP_parse.y"
    {headerptr->geolocation__error()().fieldName()=FieldName::GEOLOCATION__ERROR__E;}
    break;

  case 240:

/* Line 1806 of yacc.c  */
#line 1366 "SIP_parse.y"
    {headerptr->target__dialog()().fieldName()=FieldName::TARGET__DIALOG__E;}
    break;

  case 241:

/* Line 1806 of yacc.c  */
#line 1367 "SIP_parse.y"
    {headerptr->feature__caps()().fieldName()=FieldName::FEATURE__CAPS__E;}
    break;

  case 242:

/* Line 1806 of yacc.c  */
#line 1368 "SIP_parse.y"
    {headerptr->info__Package()().fieldName()=FieldName::INFO__PACKAGE__E;}
    break;

  case 243:

/* Line 1806 of yacc.c  */
#line 1369 "SIP_parse.y"
    {headerptr->recv__Info()().fieldName()=FieldName::RECV__INFO__E;}
    break;

  case 244:

/* Line 1806 of yacc.c  */
#line 1370 "SIP_parse.y"
    {headerptr->x__AUT()().fieldName()=FieldName::X__AUT__E;}
    break;

  case 245:

/* Line 1806 of yacc.c  */
#line 1371 "SIP_parse.y"
    {headerptr->x__Carrier__Info()().fieldName()=FieldName::X__CARRIER__INFO__E;}
    break;

  case 246:

/* Line 1806 of yacc.c  */
#line 1372 "SIP_parse.y"
    {headerptr->x__CHGDelay()().fieldName()=FieldName::X__CHGDELAY__E;}
    break;

  case 247:

/* Line 1806 of yacc.c  */
#line 1373 "SIP_parse.y"
    {headerptr->p__Area__Info()().fieldName()=FieldName::P__AREA__INFO__E;}
    break;

  case 248:

/* Line 1806 of yacc.c  */
#line 1374 "SIP_parse.y"
    {headerptr->x__CHGInfo()().fieldName()=FieldName::X__CHGINFO__E;}
    break;

  case 250:

/* Line 1806 of yacc.c  */
#line 1380 "SIP_parse.y"
    {
        headerptr->recv__Info()().info__Package__List()=OMIT_VALUE;
      }
    break;

  case 251:

/* Line 1806 of yacc.c  */
#line 1383 "SIP_parse.y"
    {}
    break;

  case 254:

/* Line 1806 of yacc.c  */
#line 1390 "SIP_parse.y"
    {
        headerptr->recv__Info()().info__Package__List()()[recvinfocount].
                                  info__package__name()=(yyvsp[(2) - (2)].sv);	
	headerptr->recv__Info()().info__Package__List()()[recvinfocount].
                                  info__package__params()=OMIT_VALUE;
				  
	recvinfocount++;
//        Free($2);				  
   }
    break;

  case 255:

/* Line 1806 of yacc.c  */
#line 1399 "SIP_parse.y"
    {
        headerptr->recv__Info()().info__Package__List()()[recvinfocount].
                                  info__package__name()=(yyvsp[(2) - (3)].sv);	
        headerptr->recv__Info()().info__Package__List()()[recvinfocount].
                                  info__package__params()=*paramptr;
        paramcount=0;
        recvinfocount++;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($2);
      }
    break;

  case 256:

/* Line 1806 of yacc.c  */
#line 1412 "SIP_parse.y"
    {
        headerptr->info__Package()().info__Package__Type().info__package__name() = (yyvsp[(3) - (3)].sv);
        headerptr->info__Package()().info__Package__Type().info__package__params() = OMIT_VALUE;
//        Free($3);
      }
    break;

  case 257:

/* Line 1806 of yacc.c  */
#line 1417 "SIP_parse.y"
    {
        headerptr->info__Package()().info__Package__Type().info__package__name() = (yyvsp[(3) - (4)].sv);
        headerptr->info__Package()().info__Package__Type().info__package__params() = *paramptr;
//        Free($3);
        delete paramptr;
        paramptr=new GenericParam__List;
        paramcount=0;
      }
    break;

  case 258:

/* Line 1806 of yacc.c  */
#line 1427 "SIP_parse.y"
    {
      headerptr->target__dialog()().callid() =(yyvsp[(3) - (3)].sv);
      headerptr->target__dialog()().td__params()= OMIT_VALUE;
//      Free($3);
    }
    break;

  case 259:

/* Line 1806 of yacc.c  */
#line 1432 "SIP_parse.y"
    {
      headerptr->target__dialog()().callid() =(yyvsp[(3) - (4)].sv);
      headerptr->target__dialog()().td__params()= *paramptr;
      paramcount=0;
      delete paramptr;
      paramptr=new GenericParam__List;

//      Free($3);
    }
    break;

  case 260:

/* Line 1806 of yacc.c  */
#line 1444 "SIP_parse.y"
    {}
    break;

  case 263:

/* Line 1806 of yacc.c  */
#line 1451 "SIP_parse.y"
    {
        headerptr->feature__caps()().fc__values()[featureCapscount].fc__string()=(yyvsp[(2) - (2)].sv);
        headerptr->feature__caps()().fc__values()[featureCapscount].feature__param()=
                                                                     OMIT_VALUE;
        featureCapscount++;
//        Free($3);
      }
    break;

  case 264:

/* Line 1806 of yacc.c  */
#line 1458 "SIP_parse.y"
    {
        headerptr->feature__caps()().fc__values()[featureCapscount].fc__string()=(yyvsp[(2) - (3)].sv);
        headerptr->feature__caps()().fc__values()[featureCapscount].feature__param()=
                                                                      *paramptr;
        paramcount=0;
        featureCapscount++;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($3);
      }
    break;

  case 265:

/* Line 1806 of yacc.c  */
#line 1470 "SIP_parse.y"
    {
      headerptr->geolocation__error()().location__error__code()=str2int((yyvsp[(3) - (3)].sv));
      headerptr->geolocation__error()().location__error__params()=OMIT_VALUE;
    }
    break;

  case 266:

/* Line 1806 of yacc.c  */
#line 1474 "SIP_parse.y"
    {
      headerptr->geolocation__error()().location__error__code()=str2int((yyvsp[(3) - (4)].sv));
      headerptr->geolocation__error()().location__error__params()=*paramptr;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
    }
    break;

  case 267:

/* Line 1806 of yacc.c  */
#line 1483 "SIP_parse.y"
    {
     headerptr->geolocation__routing()().georouting__param()=(yyvsp[(3) - (3)].sv);
     headerptr->geolocation__routing()().georouting__value()=OMIT_VALUE;
   }
    break;

  case 268:

/* Line 1806 of yacc.c  */
#line 1487 "SIP_parse.y"
    {
     headerptr->geolocation__routing()().georouting__param()=(yyvsp[(3) - (5)].sv);
     headerptr->geolocation__routing()().georouting__value()=(yyvsp[(5) - (5)].sv);
   }
    break;

  case 269:

/* Line 1806 of yacc.c  */
#line 1494 "SIP_parse.y"
    {
      if(headerptr->geolocation().ispresent()){
        int a=headerptr->geolocation()().location__values().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].g_val_list)->size_of();b++){
          headerptr->geolocation()().location__values()[a]=(*(yyvsp[(2) - (2)].g_val_list))[b];
          a++;
          }
      }
      else {
        headerptr->geolocation()().location__values()=*(yyvsp[(2) - (2)].g_val_list);
      }
      delete (yyvsp[(2) - (2)].g_val_list);
    }
    break;

  case 270:

/* Line 1806 of yacc.c  */
#line 1509 "SIP_parse.y"
    {
        (yyval.g_val_list)= new Location__value__list;
        (*(yyval.g_val_list))[0]=*(yyvsp[(1) - (1)].g_val);
        delete (yyvsp[(1) - (1)].g_val);
      }
    break;

  case 271:

/* Line 1806 of yacc.c  */
#line 1515 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (3)].g_val_list)->size_of();
        (yyval.g_val_list)=(yyvsp[(1) - (3)].g_val_list);
        (*(yyval.g_val_list))[a] = *(yyvsp[(3) - (3)].g_val);
        delete (yyvsp[(3) - (3)].g_val);
      }
    break;

  case 272:

/* Line 1806 of yacc.c  */
#line 1523 "SIP_parse.y"
    {
     (yyval.g_val) = new Location__value;
     (yyval.g_val)->location__uri() = *uriptr;
     (yyval.g_val)->location__params() = OMIT_VALUE;
     
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
   }
    break;

  case 273:

/* Line 1806 of yacc.c  */
#line 1532 "SIP_parse.y"
    {
     (yyval.g_val) = new Location__value;
     (yyval.g_val)->location__uri() = *uriptr;
     (yyval.g_val)->location__params() = *paramptr;
     
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
   }
    break;

  case 274:

/* Line 1806 of yacc.c  */
#line 1548 "SIP_parse.y"
    {
      if(headerptr->acceptResourcePriority().ispresent()){
        int a=headerptr->acceptResourcePriority()().rvalues()().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].r_val_list)->size_of();b++){
          headerptr->acceptResourcePriority()().rvalues()()[a]=(*(yyvsp[(2) - (2)].r_val_list))[b];
          a++;
          }
      }
      else {
        headerptr->acceptResourcePriority()().rvalues()()=*(yyvsp[(2) - (2)].r_val_list);
      }
      delete (yyvsp[(2) - (2)].r_val_list);
   }
    break;

  case 275:

/* Line 1806 of yacc.c  */
#line 1563 "SIP_parse.y"
    {
      if(headerptr->resourcePriority().ispresent()){
        int a=headerptr->resourcePriority()().rvalues()().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].r_val_list)->size_of();b++){
          headerptr->resourcePriority()().rvalues()()[a]=(*(yyvsp[(2) - (2)].r_val_list))[b];
          a++;
          }
      }
      else {
        headerptr->resourcePriority()().rvalues()()=*(yyvsp[(2) - (2)].r_val_list);
      }
      delete (yyvsp[(2) - (2)].r_val_list);
   }
    break;

  case 276:

/* Line 1806 of yacc.c  */
#line 1578 "SIP_parse.y"
    {
        (yyval.r_val_list) = new Rvalue__List;
        (*(yyval.r_val_list))[0] = *(yyvsp[(2) - (2)].rval);
        delete (yyvsp[(2) - (2)].rval);
      }
    break;

  case 277:

/* Line 1806 of yacc.c  */
#line 1583 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (4)].r_val_list)->size_of();
        (yyval.r_val_list)=(yyvsp[(1) - (4)].r_val_list);
        (*(yyval.r_val_list))[a] = *(yyvsp[(4) - (4)].rval);
        delete (yyvsp[(4) - (4)].rval);
      }
    break;

  case 278:

/* Line 1806 of yacc.c  */
#line 1591 "SIP_parse.y"
    {
      (yyval.rval) = new Rvalue;
      (yyval.rval)->namespace_()= (yyvsp[(1) - (3)].sv);
      (yyval.rval)->r__priority()=(yyvsp[(3) - (3)].sv);
//      delete $3;
//      Free($1);
      }
    break;

  case 279:

/* Line 1806 of yacc.c  */
#line 1601 "SIP_parse.y"
    {
      if(!headerptr->p__Early__Media().ispresent()){
        headerptr->p__Early__Media()().em__param__list()= OMIT_VALUE;
      }
    }
    break;

  case 280:

/* Line 1806 of yacc.c  */
#line 1606 "SIP_parse.y"
    {
      if(headerptr->p__Early__Media().ispresent()){
        int a=headerptr->p__Early__Media()().em__param__list().ispresent()?
                 headerptr->p__Early__Media()().em__param__list()().size_of():0;
        for(int b=0;b<(yyvsp[(2) - (2)].em_bdy_list)->size_of();b++){
          headerptr->p__Early__Media()().em__param__list()()[a]=(*(yyvsp[(2) - (2)].em_bdy_list))[b];
          a++;
          }
      }
      else {
        headerptr->p__Early__Media()().em__param__list()()=*(yyvsp[(2) - (2)].em_bdy_list);
      }
      delete (yyvsp[(2) - (2)].em_bdy_list);
   }
    break;

  case 281:

/* Line 1806 of yacc.c  */
#line 1622 "SIP_parse.y"
    {
        (yyval.em_bdy_list)= new Em__param__List;
        (*(yyval.em_bdy_list))[0]=(yyvsp[(2) - (2)].sv);
//        Free($2);
      }
    break;

  case 282:

/* Line 1806 of yacc.c  */
#line 1627 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (4)].em_bdy_list)->size_of();
        (yyval.em_bdy_list)=(yyvsp[(1) - (4)].em_bdy_list);
        (*(yyval.em_bdy_list))[a] = (yyvsp[(4) - (4)].sv);
//        Free($4);
      }
    break;

  case 283:

/* Line 1806 of yacc.c  */
#line 1635 "SIP_parse.y"
    {
        headerptr->join()().callid()=(yyvsp[(3) - (3)].sv);
        headerptr->join()().joinParams()=OMIT_VALUE;
//        Free($3);
      }
    break;

  case 284:

/* Line 1806 of yacc.c  */
#line 1640 "SIP_parse.y"
    {
        headerptr->join()().callid()=(yyvsp[(3) - (4)].sv);
        headerptr->join()().joinParams()=*paramptr;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
//        Free($3);
      }
    break;

  case 285:

/* Line 1806 of yacc.c  */
#line 1650 "SIP_parse.y"
    {
        headerptr->sip__ETag()().entity__tag()=(yyvsp[(3) - (3)].sv);
//        Free($3);
      }
    break;

  case 286:

/* Line 1806 of yacc.c  */
#line 1656 "SIP_parse.y"
    {
        headerptr->sip__If__Match()().entity__tag()=(yyvsp[(3) - (3)].sv);
//        Free($3);
      }
    break;

  case 287:

/* Line 1806 of yacc.c  */
#line 1662 "SIP_parse.y"
    {
        headerptr->replaces()().callid()=(yyvsp[(3) - (3)].sv);
        headerptr->replaces()().replacesParams()=OMIT_VALUE;
//        Free($3);
      }
    break;

  case 288:

/* Line 1806 of yacc.c  */
#line 1667 "SIP_parse.y"
    {
        headerptr->replaces()().callid()=(yyvsp[(3) - (4)].sv);
        headerptr->replaces()().replacesParams()=*paramptr;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
//        Free($3);
      }
    break;

  case 289:

/* Line 1806 of yacc.c  */
#line 1677 "SIP_parse.y"
    {
      if(headerptr->service__route().ispresent()){
        int a=headerptr->service__route()().routeBody().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].route_list)->size_of();b++){
          headerptr->service__route()().routeBody()[a]=(*(yyvsp[(2) - (2)].route_list))[b];
          a++;
          }
      }
      else {
        headerptr->service__route()().routeBody()=*(yyvsp[(2) - (2)].route_list);
      }
      delete (yyvsp[(2) - (2)].route_list);
   }
    break;

  case 290:

/* Line 1806 of yacc.c  */
#line 1692 "SIP_parse.y"
    {
        headerptr->p__user__database()().database()=*uriptr;
        headerptr->p__user__database()().params()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;

      }
    break;

  case 291:

/* Line 1806 of yacc.c  */
#line 1700 "SIP_parse.y"
    {
        headerptr->p__user__database()().database()=*uriptr;
          headerptr->p__user__database()().params()=*paramptr;
          delete paramptr;
          paramptr= new GenericParam__List;
          paramcount=0;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 292:

/* Line 1806 of yacc.c  */
#line 1712 "SIP_parse.y"
    {
        headerptr->p__DCS__redirect()().caller__ID()=*uriptr;
        headerptr->p__DCS__redirect()().redir__params()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;

      }
    break;

  case 293:

/* Line 1806 of yacc.c  */
#line 1720 "SIP_parse.y"
    {
        headerptr->p__DCS__redirect()().caller__ID()=*uriptr;
          headerptr->p__DCS__redirect()().redir__params()=*paramptr;
          delete paramptr;
          paramptr= new GenericParam__List;
          paramcount=0;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 294:

/* Line 1806 of yacc.c  */
#line 1732 "SIP_parse.y"
    {
        headerptr->p__DCS__LAES()().laes__sig()=*(yyvsp[(3) - (3)].host_p);
        headerptr->p__DCS__LAES()().laes__params()=OMIT_VALUE;
        delete (yyvsp[(3) - (3)].host_p);
      }
    break;

  case 295:

/* Line 1806 of yacc.c  */
#line 1737 "SIP_parse.y"
    {
        headerptr->p__DCS__LAES()().laes__sig()=*(yyvsp[(3) - (5)].host_p);
          headerptr->p__DCS__LAES()().laes__params()=*paramptr;
          delete paramptr;
          paramptr= new GenericParam__List;
          paramcount=0;
        delete (yyvsp[(3) - (5)].host_p);
      }
    break;

  case 300:

/* Line 1806 of yacc.c  */
#line 1757 "SIP_parse.y"
    {
        (*paramptr)[paramcount].id()="content";
        if((yyvsp[(4) - (4)].host_p)->portField().ispresent()){
          (*paramptr)[paramcount].paramValue()=(yyvsp[(4) - (4)].host_p)->host()() + ":" + int2str((yyvsp[(4) - (4)].host_p)->portField()());
        } else {
          (*paramptr)[paramcount].paramValue()=(yyvsp[(4) - (4)].host_p)->host()();
        }
        paramcount++;
        delete (yyvsp[(4) - (4)].host_p);
    }
    break;

  case 301:

/* Line 1806 of yacc.c  */
#line 1769 "SIP_parse.y"
    {
        (yyval.host_p)= new HostPort;
        (yyval.host_p)->host()=(yyvsp[(1) - (3)].sv);
        (yyval.host_p)->portField() = (yyvsp[(3) - (3)].iv);
//        Free($1);
      }
    break;

  case 302:

/* Line 1806 of yacc.c  */
#line 1775 "SIP_parse.y"
    {
        (yyval.host_p)= new HostPort;
        (yyval.host_p)->host()=(yyvsp[(1) - (1)].sv);
        (yyval.host_p)->portField() = OMIT_VALUE;
//        Free($1);
      }
    break;

  case 303:

/* Line 1806 of yacc.c  */
#line 1783 "SIP_parse.y"
    {
        headerptr->p__DCS__billing__info()().billing__correlation__ID()=str2hex((yyvsp[(3) - (7)].sv));
        headerptr->p__DCS__billing__info()().FEID__ID()=str2hex((yyvsp[(4) - (7)].cv));
        headerptr->p__DCS__billing__info()().FEID__host()=trimOnIPv6((yyvsp[(7) - (7)].sv));
        headerptr->p__DCS__billing__info()().billing__info__params()=OMIT_VALUE;
        
//        Free($3);
//        Free($5);
//        Free($7);
      }
    break;

  case 304:

/* Line 1806 of yacc.c  */
#line 1793 "SIP_parse.y"
    {
        headerptr->p__DCS__billing__info()().billing__correlation__ID()=str2hex((yyvsp[(3) - (8)].sv));
        headerptr->p__DCS__billing__info()().FEID__ID()=str2hex((yyvsp[(5) - (8)].sv));
        headerptr->p__DCS__billing__info()().FEID__host()=trimOnIPv6((yyvsp[(7) - (8)].sv));
        headerptr->p__DCS__billing__info()().billing__info__params()=*paramptr;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;        
//        Free($3);
//        Free($5);
//        Free($7);
      }
    break;

  case 305:

/* Line 1806 of yacc.c  */
#line 1807 "SIP_parse.y"
    {
        headerptr->p__DCS__OSPS()().OSPS__tag()=(yyvsp[(3) - (3)].sv);
//        Free($3);
      }
    break;

  case 306:

/* Line 1806 of yacc.c  */
#line 1813 "SIP_parse.y"
    {
        headerptr->p__DCS__trace__pty__id()().name__addr()=*(yyvsp[(2) - (2)].naddr);
        delete (yyvsp[(2) - (2)].naddr);
      }
    break;

  case 307:

/* Line 1806 of yacc.c  */
#line 1820 "SIP_parse.y"
    {
      if(headerptr->security__client().ispresent()){
        int a=headerptr->security__client()().sec__mechanism__list().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].sec_mech_list)->size_of();b++){
          headerptr->security__client()().sec__mechanism__list()[a]=(*(yyvsp[(2) - (2)].sec_mech_list))[b];
          a++;
          }
      }
      else {
        headerptr->security__client()().sec__mechanism__list()=*(yyvsp[(2) - (2)].sec_mech_list);
      }
      delete (yyvsp[(2) - (2)].sec_mech_list);
    }
    break;

  case 308:

/* Line 1806 of yacc.c  */
#line 1835 "SIP_parse.y"
    {
      if(headerptr->security__server().ispresent()){
        int a=headerptr->security__server()().sec__mechanism__list().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].sec_mech_list)->size_of();b++){
          headerptr->security__server()().sec__mechanism__list()[a]=(*(yyvsp[(2) - (2)].sec_mech_list))[b];
          a++;
          }
      }
      else {
        headerptr->security__server()().sec__mechanism__list()=*(yyvsp[(2) - (2)].sec_mech_list);
      }
      delete (yyvsp[(2) - (2)].sec_mech_list);
    }
    break;

  case 309:

/* Line 1806 of yacc.c  */
#line 1850 "SIP_parse.y"
    {
      if(headerptr->security__verify().ispresent()){
        int a=headerptr->security__verify()().sec__mechanism__list().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].sec_mech_list)->size_of();b++){
          headerptr->security__verify()().sec__mechanism__list()[a]=(*(yyvsp[(2) - (2)].sec_mech_list))[b];
          a++;
          }
      }
      else {
        headerptr->security__verify()().sec__mechanism__list()=*(yyvsp[(2) - (2)].sec_mech_list);
      }
      delete (yyvsp[(2) - (2)].sec_mech_list);
    }
    break;

  case 310:

/* Line 1806 of yacc.c  */
#line 1865 "SIP_parse.y"
    {
        (yyval.sec_mech_list)= new Security__mechanism__list;
        (*(yyval.sec_mech_list))[0]=*(yyvsp[(1) - (1)].sec_mech);
        delete (yyvsp[(1) - (1)].sec_mech);
      }
    break;

  case 311:

/* Line 1806 of yacc.c  */
#line 1870 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (3)].sec_mech_list)->size_of();
        (yyval.sec_mech_list)=(yyvsp[(1) - (3)].sec_mech_list);
        (*(yyval.sec_mech_list))[a] = *(yyvsp[(3) - (3)].sec_mech);
        delete (yyvsp[(3) - (3)].sec_mech);
      }
    break;

  case 312:

/* Line 1806 of yacc.c  */
#line 1878 "SIP_parse.y"
    {
        (yyval.sec_mech) = new Security__mechanism;
        (yyval.sec_mech)->mechanism__name()= (yyvsp[(2) - (2)].sv);
        (yyval.sec_mech)->mechanism__params()=OMIT_VALUE;
//        Free($2);
      }
    break;

  case 313:

/* Line 1806 of yacc.c  */
#line 1884 "SIP_parse.y"
    {
        (yyval.sec_mech) = new Security__mechanism;
        (yyval.sec_mech)->mechanism__name()= (yyvsp[(2) - (3)].sv);
        (yyval.sec_mech)->mechanism__params()=*paramptr;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
//        Free($2);
      }
    break;

  case 314:

/* Line 1806 of yacc.c  */
#line 1894 "SIP_parse.y"
    {
      if(headerptr->path().ispresent()){
        int a=headerptr->path()().routeBody().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].route_list)->size_of();b++){
          headerptr->path()().routeBody()[a]=(*(yyvsp[(2) - (2)].route_list))[b];
          a++;
          }
      }
      else {
        headerptr->path()().routeBody()=*(yyvsp[(2) - (2)].route_list);
      }
      delete (yyvsp[(2) - (2)].route_list);
   }
    break;

  case 315:

/* Line 1806 of yacc.c  */
#line 1909 "SIP_parse.y"
    {
        (yyval.route_list)= new RouteBody__List;
        (*(yyval.route_list))[0]=*(yyvsp[(1) - (1)].route_val);
        delete (yyvsp[(1) - (1)].route_val);
      }
    break;

  case 316:

/* Line 1806 of yacc.c  */
#line 1914 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (3)].route_list)->size_of();
        (yyval.route_list)=(yyvsp[(1) - (3)].route_list);
        (*(yyval.route_list))[a] = *(yyvsp[(3) - (3)].route_val);
        delete (yyvsp[(3) - (3)].route_val);
      }
    break;

  case 317:

/* Line 1806 of yacc.c  */
#line 1922 "SIP_parse.y"
    {
        (yyval.route_val) = new RouteBody;
        (yyval.route_val)->nameAddr().displayName()=(yyvsp[(2) - (6)].sv);
        (yyval.route_val)->nameAddr().addrSpec()= *uriptr;
        (yyval.route_val)->rrParam()()= *paramptr;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
//        Free($2);
      }
    break;

  case 318:

/* Line 1806 of yacc.c  */
#line 1936 "SIP_parse.y"
    {
        (yyval.route_val) = new RouteBody;
        (yyval.route_val)->nameAddr().displayName()=OMIT_VALUE;
        (yyval.route_val)->nameAddr().addrSpec()= *uriptr;
        (yyval.route_val)->rrParam()()= *paramptr;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 319:

/* Line 1806 of yacc.c  */
#line 1949 "SIP_parse.y"
    {
        (yyval.route_val) = new RouteBody;
        (yyval.route_val)->nameAddr().displayName()=(yyvsp[(2) - (5)].sv);
        (yyval.route_val)->nameAddr().addrSpec()= *uriptr;
        (yyval.route_val)->rrParam()= OMIT_VALUE;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
//        Free($2);
      }
    break;

  case 320:

/* Line 1806 of yacc.c  */
#line 1960 "SIP_parse.y"
    {
        (yyval.route_val) = new RouteBody;
        (yyval.route_val)->nameAddr().displayName()=OMIT_VALUE;
        (yyval.route_val)->nameAddr().addrSpec()= *uriptr;
        (yyval.route_val)->rrParam()= OMIT_VALUE;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 321:

/* Line 1806 of yacc.c  */
#line 1973 "SIP_parse.y"
    {
      if(headerptr->p__media__auth().ispresent()){
        int a=headerptr->p__media__auth()().token__list().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].a_t_list)->size_of();b++){
          headerptr->p__media__auth()().token__list()[a]=(*(yyvsp[(2) - (2)].a_t_list))[b];
          a++;
          }
      }
      else {
        headerptr->p__media__auth()().token__list()=*(yyvsp[(2) - (2)].a_t_list);
      }
      delete (yyvsp[(2) - (2)].a_t_list);
   }
    break;

  case 322:

/* Line 1806 of yacc.c  */
#line 1988 "SIP_parse.y"
    {
        (yyval.a_t_list)= new Media__auth__token__list;
        (*(yyval.a_t_list))[0]=str2hex((yyvsp[(2) - (2)].sv));
        //Free $2;
      }
    break;

  case 323:

/* Line 1806 of yacc.c  */
#line 1993 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (4)].a_t_list)->size_of();
        (yyval.a_t_list)=(yyvsp[(1) - (4)].a_t_list);
        (*(yyval.a_t_list))[a] = str2hex((yyvsp[(4) - (4)].sv));
        //Free $4;
      }
    break;

  case 324:

/* Line 1806 of yacc.c  */
#line 2001 "SIP_parse.y"
    {
      if(headerptr->request__disp().ispresent()){
        int a=headerptr->request__disp()().directive__list().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].d_list)->size_of();b++){
          headerptr->request__disp()().directive__list()[a]=(*(yyvsp[(2) - (2)].d_list))[b];
          a++;
          }
      }
      else {
        headerptr->request__disp()().directive__list()=*(yyvsp[(2) - (2)].d_list);
      }
      delete (yyvsp[(2) - (2)].d_list);
   }
    break;

  case 325:

/* Line 1806 of yacc.c  */
#line 2016 "SIP_parse.y"
    {
        (yyval.d_list)= new Request__disp__directive__list;
        (*(yyval.d_list))[0]=(yyvsp[(2) - (2)].sv);
//        Free($2);
      }
    break;

  case 326:

/* Line 1806 of yacc.c  */
#line 2021 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (4)].d_list)->size_of();
        (yyval.d_list)=(yyvsp[(1) - (4)].d_list);
        (*(yyval.d_list))[a] = (yyvsp[(4) - (4)].sv);
//        Free($4);
      }
    break;

  case 327:

/* Line 1806 of yacc.c  */
#line 2029 "SIP_parse.y"
    {
      if(headerptr->reject__contact().ispresent()){
        int a=headerptr->reject__contact()().rc__values().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].c_list)->size_of();b++){
          headerptr->reject__contact()().rc__values()[a]=(*(yyvsp[(2) - (2)].c_list))[b];
          a++;
          }
      }
      else {
        headerptr->reject__contact()().rc__values()=*(yyvsp[(2) - (2)].c_list);
      }
      delete (yyvsp[(2) - (2)].c_list);
   }
    break;

  case 328:

/* Line 1806 of yacc.c  */
#line 2044 "SIP_parse.y"
    {
      if(headerptr->accept__contact().ispresent()){
        int a=headerptr->accept__contact()().ac__values().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].c_list)->size_of();b++){
          headerptr->accept__contact()().ac__values()[a]=(*(yyvsp[(2) - (2)].c_list))[b];
          a++;
          }
      }
      else {
        headerptr->accept__contact()().ac__values()=*(yyvsp[(2) - (2)].c_list);
      }
      delete (yyvsp[(2) - (2)].c_list);
   }
    break;

  case 329:

/* Line 1806 of yacc.c  */
#line 2059 "SIP_parse.y"
    {
        (yyval.c_list)= new Contact__list;
        (*(yyval.c_list))[0]=*(yyvsp[(1) - (1)].p_list);
        delete (yyvsp[(1) - (1)].p_list);
      }
    break;

  case 330:

/* Line 1806 of yacc.c  */
#line 2064 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (3)].c_list)->size_of();
        (yyval.c_list)=(yyvsp[(1) - (3)].c_list);
        (*(yyval.c_list))[a] = *(yyvsp[(3) - (3)].p_list);
        delete (yyvsp[(3) - (3)].p_list);
      }
    break;

  case 331:

/* Line 1806 of yacc.c  */
#line 2072 "SIP_parse.y"
    {
        (yyval.p_list)= new GenericParam__List(NULL_VALUE);
//        Free($2);
      }
    break;

  case 332:

/* Line 1806 of yacc.c  */
#line 2076 "SIP_parse.y"
    {
//        Free($2);
        (yyval.p_list)=paramptr;
        paramptr=new GenericParam__List;
        paramcount=0;
      }
    break;

  case 333:

/* Line 1806 of yacc.c  */
#line 2084 "SIP_parse.y"
    {
        headerptr->session__expires()().deltaSec() = (yyvsp[(3) - (3)].sv);
        headerptr->session__expires()().se__params() = OMIT_VALUE;
//        Free($3);
      }
    break;

  case 334:

/* Line 1806 of yacc.c  */
#line 2089 "SIP_parse.y"
    {
        headerptr->session__expires()().deltaSec() = (yyvsp[(3) - (4)].sv);
        headerptr->session__expires()().se__params()() = *paramptr;
//        Free($3);
        delete paramptr;
        paramptr=new GenericParam__List;
        paramcount=0;
      }
    break;

  case 335:

/* Line 1806 of yacc.c  */
#line 2099 "SIP_parse.y"
    {
        headerptr->session__id()().sessionID() = (yyvsp[(3) - (3)].sv);
        headerptr->session__id()().se__params() = OMIT_VALUE;
//        Free($3);
      }
    break;

  case 336:

/* Line 1806 of yacc.c  */
#line 2104 "SIP_parse.y"
    {
        headerptr->session__id()().sessionID() = (yyvsp[(3) - (4)].sv);
        headerptr->session__id()().se__params()() = *paramptr;
//        Free($3);
        delete paramptr;
        paramptr=new GenericParam__List;
        paramcount=0;
      }
    break;

  case 337:

/* Line 1806 of yacc.c  */
#line 2114 "SIP_parse.y"
    {
        headerptr->min__SE()().deltaSec() = (yyvsp[(3) - (3)].sv);
        headerptr->min__SE()().params() = OMIT_VALUE;
//        Free($3);
      }
    break;

  case 338:

/* Line 1806 of yacc.c  */
#line 2119 "SIP_parse.y"
    {
        headerptr->min__SE()().deltaSec() = (yyvsp[(3) - (4)].sv);
        headerptr->min__SE()().params()() = *paramptr;
//        Free($3);
        delete paramptr;
        paramptr=new GenericParam__List;
        paramcount=0;
      }
    break;

  case 339:

/* Line 1806 of yacc.c  */
#line 2129 "SIP_parse.y"
    {
      if(headerptr->historyInfo().ispresent()){
        int a=headerptr->historyInfo()().hi__entries().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].history_entry_list)->size_of();b++){
          headerptr->historyInfo()().hi__entries()[a]=(*(yyvsp[(2) - (2)].history_entry_list))[b];
          a++;
          }
      }
      else {
        headerptr->historyInfo()().hi__entries()=*(yyvsp[(2) - (2)].history_entry_list);
      }
      delete (yyvsp[(2) - (2)].history_entry_list);
   }
    break;

  case 340:

/* Line 1806 of yacc.c  */
#line 2144 "SIP_parse.y"
    {
        (yyval.history_entry_list) = new Hi__Entry__list;
        (*(yyval.history_entry_list))[0] = *(yyvsp[(1) - (1)].history_entry);
        delete (yyvsp[(1) - (1)].history_entry);
      }
    break;

  case 341:

/* Line 1806 of yacc.c  */
#line 2149 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (3)].history_entry_list)->size_of();
        (yyval.history_entry_list)=(yyvsp[(1) - (3)].history_entry_list);
        (*(yyval.history_entry_list))[a] = *(yyvsp[(3) - (3)].history_entry);
        delete (yyvsp[(3) - (3)].history_entry);
      }
    break;

  case 342:

/* Line 1806 of yacc.c  */
#line 2157 "SIP_parse.y"
    {
        (yyval.history_entry)= new Hi__Entry;
        (yyval.history_entry)->nameAddr()=*(yyvsp[(1) - (1)].naddr);
        (yyval.history_entry)->hi__params()=OMIT_VALUE;
        delete (yyvsp[(1) - (1)].naddr);
      }
    break;

  case 343:

/* Line 1806 of yacc.c  */
#line 2163 "SIP_parse.y"
    {
        (yyval.history_entry)= new Hi__Entry;
        (yyval.history_entry)->nameAddr()=*(yyvsp[(1) - (2)].naddr);
        (yyval.history_entry)->hi__params()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
        delete (yyvsp[(1) - (2)].naddr);
      }
    break;

  case 344:

/* Line 1806 of yacc.c  */
#line 2174 "SIP_parse.y"
    {
        int b=0;
        int a;
        for(a=0;a<paramptr->size_of();a++){
          if(strcasecmp((const char*)(*paramptr)[a].id(),"icid-value")){
            headerptr->p__charging__vector()().charge__params()()[b].id()=(*paramptr)[a].id();
            headerptr->p__charging__vector()().charge__params()()[b].paramValue()=(*paramptr)[a].paramValue();
            b++;
          }
          else {
            headerptr->p__charging__vector()().icid__value()=(*paramptr)[a].paramValue();
          }
        }
        if(b==0) headerptr->p__charging__vector()().charge__params()=OMIT_VALUE;
        if(a==b) headerptr->p__charging__vector()().icid__value()="";
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;

      }
    break;

  case 345:

/* Line 1806 of yacc.c  */
#line 2197 "SIP_parse.y"
    {
        headerptr->p__charging__function__address()().charge__addr__params()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;

      }
    break;

  case 346:

/* Line 1806 of yacc.c  */
#line 2206 "SIP_parse.y"
    {}
    break;

  case 347:

/* Line 1806 of yacc.c  */
#line 2207 "SIP_parse.y"
    {}
    break;

  case 348:

/* Line 1806 of yacc.c  */
#line 2211 "SIP_parse.y"
    {
      if(headerptr->p__access__network__info().ispresent()){
        int a=headerptr->p__access__network__info()().access__net__specs().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].an_spec_list)->size_of();b++){
          headerptr->p__access__network__info()().access__net__specs()[a]=(*(yyvsp[(2) - (2)].an_spec_list))[b];
          a++;
          }
      }
      else {
        headerptr->p__access__network__info()().access__net__specs()=*(yyvsp[(2) - (2)].an_spec_list);
      }
      delete (yyvsp[(2) - (2)].an_spec_list);
   }
    break;

  case 349:

/* Line 1806 of yacc.c  */
#line 2226 "SIP_parse.y"
    {
        (yyval.an_spec_list)= new Access__net__spec__list;
        (*(yyval.an_spec_list))[0]=*(yyvsp[(1) - (1)].an_spec);
        delete (yyvsp[(1) - (1)].an_spec);
      }
    break;

  case 350:

/* Line 1806 of yacc.c  */
#line 2231 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (3)].an_spec_list)->size_of();
        (yyval.an_spec_list)=(yyvsp[(1) - (3)].an_spec_list);
        (*(yyval.an_spec_list))[a] = *(yyvsp[(3) - (3)].an_spec);
        delete (yyvsp[(3) - (3)].an_spec);
      }
    break;

  case 351:

/* Line 1806 of yacc.c  */
#line 2239 "SIP_parse.y"
    {
        (yyval.an_spec)= new Access__net__spec;
        (yyval.an_spec)->access__type()=(yyvsp[(2) - (2)].sv);
        (yyval.an_spec)->access__info()=OMIT_VALUE;
//        Free($2);
      }
    break;

  case 352:

/* Line 1806 of yacc.c  */
#line 2245 "SIP_parse.y"
    {
        (yyval.an_spec)= new Access__net__spec;
        (yyval.an_spec)->access__type()=(yyvsp[(2) - (3)].sv);
        (yyval.an_spec)->access__info()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($2);
      }
    break;

  case 353:

/* Line 1806 of yacc.c  */
#line 2257 "SIP_parse.y"
    {
        headerptr->answer__mode()().answer__mode()=(yyvsp[(3) - (3)].sv);
        headerptr->answer__mode()().answer__mode__param()=OMIT_VALUE;
//        Free($3);
      }
    break;

  case 354:

/* Line 1806 of yacc.c  */
#line 2262 "SIP_parse.y"
    {
        headerptr->answer__mode()().answer__mode()=(yyvsp[(3) - (4)].sv);
        headerptr->answer__mode()().answer__mode__param()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($3);
      }
    break;

  case 355:

/* Line 1806 of yacc.c  */
#line 2272 "SIP_parse.y"
    {
        headerptr->priv__answer__mode()().answer__mode()=(yyvsp[(3) - (3)].sv);
        headerptr->priv__answer__mode()().answer__mode__param()=OMIT_VALUE;
//        Free($3);
      }
    break;

  case 356:

/* Line 1806 of yacc.c  */
#line 2277 "SIP_parse.y"
    {
        headerptr->priv__answer__mode()().answer__mode()=(yyvsp[(3) - (4)].sv);
        headerptr->priv__answer__mode()().answer__mode__param()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($3);
      }
    break;

  case 357:

/* Line 1806 of yacc.c  */
#line 2287 "SIP_parse.y"
    {
        headerptr->alert__mode()().alert__mode()=(yyvsp[(3) - (3)].sv);
        headerptr->alert__mode()().alert__mode__param()=OMIT_VALUE;
//        Free($3);
      }
    break;

  case 358:

/* Line 1806 of yacc.c  */
#line 2292 "SIP_parse.y"
    {
        headerptr->alert__mode()().alert__mode()=(yyvsp[(3) - (4)].sv);
        headerptr->alert__mode()().alert__mode__param()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($3);
      }
    break;

  case 359:

/* Line 1806 of yacc.c  */
#line 2302 "SIP_parse.y"
    {
        headerptr->refer__sub()().refer__sub__value()=(yyvsp[(3) - (3)].sv);
        headerptr->refer__sub()().refer__sub__param()=OMIT_VALUE;
//        Free($3);
      }
    break;

  case 360:

/* Line 1806 of yacc.c  */
#line 2307 "SIP_parse.y"
    {
        headerptr->refer__sub()().refer__sub__value()=(yyvsp[(3) - (4)].sv);
        headerptr->refer__sub()().refer__sub__param()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($3);
      }
    break;

  case 361:

/* Line 1806 of yacc.c  */
#line 2317 "SIP_parse.y"
    {
        headerptr->p__alerting__mode()().alerting__type()=(yyvsp[(3) - (3)].sv);
        headerptr->p__alerting__mode()().alerting__info()=OMIT_VALUE;
//        Free($3);
      }
    break;

  case 362:

/* Line 1806 of yacc.c  */
#line 2322 "SIP_parse.y"
    {
        headerptr->p__alerting__mode()().alerting__type()=(yyvsp[(3) - (4)].sv);
        headerptr->p__alerting__mode()().alerting__info()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($3);
      }
    break;

  case 363:

/* Line 1806 of yacc.c  */
#line 2332 "SIP_parse.y"
    {
        headerptr->p__answer__state()().answer__type()=(yyvsp[(3) - (3)].sv);
        headerptr->p__answer__state()().answer__info()=OMIT_VALUE;
//        Free($3);
      }
    break;

  case 364:

/* Line 1806 of yacc.c  */
#line 2337 "SIP_parse.y"
    {
        headerptr->p__answer__state()().answer__type()=(yyvsp[(3) - (4)].sv);
        headerptr->p__answer__state()().answer__info()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($3);
      }
    break;

  case 365:

/* Line 1806 of yacc.c  */
#line 2347 "SIP_parse.y"
    {
        headerptr->p__Area__Info()().p__Area__Info__Value()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
      }
    break;

  case 366:

/* Line 1806 of yacc.c  */
#line 2355 "SIP_parse.y"
    {
      if(headerptr->p__visited__network__id().ispresent()){
        int a=headerptr->p__visited__network__id()().vnetworks().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].n_spec_list)->size_of();b++){
          headerptr->p__visited__network__id()().vnetworks()[a]=(*(yyvsp[(2) - (2)].n_spec_list))[b];
          a++;
          }
      }
      else {
        headerptr->p__visited__network__id()().vnetworks()=*(yyvsp[(2) - (2)].n_spec_list);
      }
      delete (yyvsp[(2) - (2)].n_spec_list);
   }
    break;

  case 367:

/* Line 1806 of yacc.c  */
#line 2370 "SIP_parse.y"
    {
        (yyval.n_spec_list)= new Network__spec__list;
        (*(yyval.n_spec_list))[0]=*(yyvsp[(1) - (1)].n_spec);
        delete (yyvsp[(1) - (1)].n_spec);
      }
    break;

  case 368:

/* Line 1806 of yacc.c  */
#line 2375 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (3)].n_spec_list)->size_of();
        (yyval.n_spec_list)=(yyvsp[(1) - (3)].n_spec_list);
        (*(yyval.n_spec_list))[a] = *(yyvsp[(3) - (3)].n_spec);
        delete (yyvsp[(3) - (3)].n_spec);
      }
    break;

  case 369:

/* Line 1806 of yacc.c  */
#line 2382 "SIP_parse.y"
    {
        (yyval.n_spec)= new Network__spec;
        (yyval.n_spec)->network__id()=(yyvsp[(2) - (2)].sv);
        (yyval.n_spec)->network__par()=OMIT_VALUE;
//        Free($2);
      }
    break;

  case 370:

/* Line 1806 of yacc.c  */
#line 2388 "SIP_parse.y"
    {
        (yyval.n_spec)= new Network__spec;
        (yyval.n_spec)->network__id()=(yyvsp[(2) - (3)].sv);
        (yyval.n_spec)->network__par()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($2);
      }
    break;

  case 371:

/* Line 1806 of yacc.c  */
#line 2400 "SIP_parse.y"
    {
        headerptr->p__called__party__id()().called__pty__id()=*(yyvsp[(2) - (2)].naddr);
        headerptr->p__called__party__id()().cpid__param()=OMIT_VALUE;
        delete (yyvsp[(2) - (2)].naddr);
      }
    break;

  case 372:

/* Line 1806 of yacc.c  */
#line 2405 "SIP_parse.y"
    {
        headerptr->p__called__party__id()().called__pty__id()=*(yyvsp[(2) - (3)].naddr);
        headerptr->p__called__party__id()().cpid__param()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
        delete (yyvsp[(2) - (3)].naddr);
      }
    break;

  case 373:

/* Line 1806 of yacc.c  */
#line 2415 "SIP_parse.y"
    {
      if(headerptr->p__associated__uri().ispresent()){
        int a=headerptr->p__associated__uri()().p__assoc__uris().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].u_spec_list)->size_of();b++){
          headerptr->p__associated__uri()().p__assoc__uris()[a]=(*(yyvsp[(2) - (2)].u_spec_list))[b];
          a++;
          }
      }
      else {
        headerptr->p__associated__uri()().p__assoc__uris()=*(yyvsp[(2) - (2)].u_spec_list);
      }
      delete (yyvsp[(2) - (2)].u_spec_list);
   }
    break;

  case 374:

/* Line 1806 of yacc.c  */
#line 2430 "SIP_parse.y"
    {
        (yyval.u_spec_list) = new P__Assoc__uri__spec__list;
        (*(yyval.u_spec_list))[0] = *(yyvsp[(1) - (1)].u_spec);
        delete (yyvsp[(1) - (1)].u_spec);
      }
    break;

  case 375:

/* Line 1806 of yacc.c  */
#line 2435 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (3)].u_spec_list)->size_of();
        (yyval.u_spec_list)=(yyvsp[(1) - (3)].u_spec_list);
        (*(yyval.u_spec_list))[a] = *(yyvsp[(3) - (3)].u_spec);
        delete (yyvsp[(3) - (3)].u_spec);
      }
    break;

  case 376:

/* Line 1806 of yacc.c  */
#line 2443 "SIP_parse.y"
    {
        (yyval.u_spec)= new P__Assoc__uri__spec;
        (yyval.u_spec)->p__asso__uri()=*(yyvsp[(1) - (1)].naddr);
        (yyval.u_spec)->ai__params()=OMIT_VALUE;
        delete (yyvsp[(1) - (1)].naddr);
      }
    break;

  case 377:

/* Line 1806 of yacc.c  */
#line 2449 "SIP_parse.y"
    {
        (yyval.u_spec)= new P__Assoc__uri__spec;
        (yyval.u_spec)->p__asso__uri()=*(yyvsp[(1) - (2)].naddr);
        (yyval.u_spec)->ai__params()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
        delete (yyvsp[(1) - (2)].naddr);
      }
    break;

  case 378:

/* Line 1806 of yacc.c  */
#line 2460 "SIP_parse.y"
    {
      if(headerptr->diversion().ispresent()){
        int a=headerptr->diversion()().divParams().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].divparam_list)->size_of();b++){
          headerptr->diversion()().divParams()[a]=(*(yyvsp[(2) - (2)].divparam_list))[b];
          a++;
          }
      }
      else {
        headerptr->diversion()().divParams()=*(yyvsp[(2) - (2)].divparam_list);
      }
      delete (yyvsp[(2) - (2)].divparam_list);
   }
    break;

  case 379:

/* Line 1806 of yacc.c  */
#line 2475 "SIP_parse.y"
    {
        (yyval.divparam_list) = new Diversion__params__list;
        (*(yyval.divparam_list))[0] = *(yyvsp[(1) - (1)].divparam);
        delete (yyvsp[(1) - (1)].divparam);
      }
    break;

  case 380:

/* Line 1806 of yacc.c  */
#line 2480 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (3)].divparam_list)->size_of();
        (yyval.divparam_list)=(yyvsp[(1) - (3)].divparam_list);
        (*(yyval.divparam_list))[a] = *(yyvsp[(3) - (3)].divparam);
        delete (yyvsp[(3) - (3)].divparam);
      }
    break;

  case 381:

/* Line 1806 of yacc.c  */
#line 2488 "SIP_parse.y"
    {
        (yyval.divparam)= new Diversion__params;
        (yyval.divparam)->nameAddr()=*(yyvsp[(1) - (1)].naddr);
        (yyval.divparam)->div__params()=OMIT_VALUE;
        delete (yyvsp[(1) - (1)].naddr);
      }
    break;

  case 382:

/* Line 1806 of yacc.c  */
#line 2494 "SIP_parse.y"
    {
        (yyval.divparam)= new Diversion__params;
        (yyval.divparam)->nameAddr()=*(yyvsp[(1) - (2)].naddr);
        (yyval.divparam)->div__params()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
        delete (yyvsp[(1) - (2)].naddr);
      }
    break;

  case 383:

/* Line 1806 of yacc.c  */
#line 2505 "SIP_parse.y"
    {
        (yyval.naddr)= new NameAddr;
        (yyval.naddr)->displayName() = (yyvsp[(2) - (5)].sv);
        (yyval.naddr)->addrSpec()= *uriptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
//        Free($2);
      }
    break;

  case 384:

/* Line 1806 of yacc.c  */
#line 2514 "SIP_parse.y"
    {
        (yyval.naddr)= new NameAddr;
        (yyval.naddr)->displayName() = OMIT_VALUE;
        (yyval.naddr)->addrSpec()= *uriptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 385:

/* Line 1806 of yacc.c  */
#line 2525 "SIP_parse.y"
    {
      headerptr->subscription__state()().substate__value() =(yyvsp[(3) - (3)].sv);
      headerptr->subscription__state()().subexp__params()= OMIT_VALUE;
//      Free($3);
    }
    break;

  case 386:

/* Line 1806 of yacc.c  */
#line 2530 "SIP_parse.y"
    {
      headerptr->subscription__state()().substate__value() =(yyvsp[(3) - (4)].sv);
      headerptr->subscription__state()().subexp__params()= *paramptr;
      paramcount=0;
      delete paramptr;
      paramptr=new GenericParam__List;

//      Free($3);
    }
    break;

  case 387:

/* Line 1806 of yacc.c  */
#line 2541 "SIP_parse.y"
    {
      if(headerptr->allow__events().ispresent()){
        int a=headerptr->allow__events()().events().size_of();
        for(int b=0;b<(yyvsp[(2) - (2)].e_list)->size_of();b++){
          headerptr->allow__events()().events()[a]=(*(yyvsp[(2) - (2)].e_list))[b];
          a++;
          }
      }
      else {
        headerptr->allow__events()().events()=*(yyvsp[(2) - (2)].e_list);
      }
      delete (yyvsp[(2) - (2)].e_list);
   }
    break;

  case 388:

/* Line 1806 of yacc.c  */
#line 2556 "SIP_parse.y"
    {
        (yyval.e_list) = new Event__type__list;
        (*(yyval.e_list))[0] = *(yyvsp[(2) - (2)].e_type);
        delete (yyvsp[(2) - (2)].e_type);
      }
    break;

  case 389:

/* Line 1806 of yacc.c  */
#line 2561 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (4)].e_list)->size_of();
        (yyval.e_list)=(yyvsp[(1) - (4)].e_list);
        (*(yyval.e_list))[a] = *(yyvsp[(4) - (4)].e_type);
        delete (yyvsp[(4) - (4)].e_type);
      }
    break;

  case 390:

/* Line 1806 of yacc.c  */
#line 2570 "SIP_parse.y"
    {
        headerptr->event()().event__type()=*(yyvsp[(3) - (3)].e_type);
        headerptr->event()().event__params()=OMIT_VALUE;
        delete (yyvsp[(3) - (3)].e_type);
      }
    break;

  case 391:

/* Line 1806 of yacc.c  */
#line 2575 "SIP_parse.y"
    {
        headerptr->event()().event__type()=*(yyvsp[(3) - (4)].e_type);
        headerptr->event()().event__params()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
        delete (yyvsp[(3) - (4)].e_type);
      }
    break;

  case 392:

/* Line 1806 of yacc.c  */
#line 2585 "SIP_parse.y"
    {
      (yyval.e_type) = new Event__type;
      (yyval.e_type)->event__package()= (yyvsp[(1) - (1)].sv);
      (yyval.e_type)->event__templates()=OMIT_VALUE;
//      Free($1);
      }
    break;

  case 393:

/* Line 1806 of yacc.c  */
#line 2591 "SIP_parse.y"
    {
      (yyval.e_type) = new Event__type;
      (yyval.e_type)->event__package()= (yyvsp[(1) - (3)].sv);
      (yyval.e_type)->event__templates()=*(yyvsp[(3) - (3)].e_template_list);
      delete (yyvsp[(3) - (3)].e_template_list);
//      Free($1);
      }
    break;

  case 394:

/* Line 1806 of yacc.c  */
#line 2600 "SIP_parse.y"
    {
        (yyval.e_template_list) = new Event__template__list;
        (*(yyval.e_template_list))[0] = (yyvsp[(2) - (2)].sv);
//        Free($2);
      }
    break;

  case 395:

/* Line 1806 of yacc.c  */
#line 2605 "SIP_parse.y"
    {
        int a=(yyvsp[(1) - (4)].e_template_list)->size_of();
        (yyval.e_template_list)=(yyvsp[(1) - (4)].e_template_list);
        (*(yyval.e_template_list))[a] = (yyvsp[(4) - (4)].sv);
//        Free($4);
      }
    break;

  case 399:

/* Line 1806 of yacc.c  */
#line 2620 "SIP_parse.y"
    {
        headerptr->reason()().reasons()[reasoncount].protocol()= (yyvsp[(2) - (2)].sv);
        headerptr->reason()().reasons()[reasoncount].reasonValues()=OMIT_VALUE;
        reasoncount++;
//        Free($2);
      }
    break;

  case 400:

/* Line 1806 of yacc.c  */
#line 2626 "SIP_parse.y"
    {
        headerptr->reason()().reasons()[reasoncount].protocol()= (yyvsp[(2) - (3)].sv);
        headerptr->reason()().reasons()[reasoncount].reasonValues()()=*paramptr;
        reasoncount++;
        delete paramptr;
        paramptr=new GenericParam__List;
        paramcount=0;
//        Free($2);
      }
    break;

  case 401:

/* Line 1806 of yacc.c  */
#line 2637 "SIP_parse.y"
    {}
    break;

  case 405:

/* Line 1806 of yacc.c  */
#line 2644 "SIP_parse.y"
    {
        headerptr->passertedID()().ids()[passertedidcount].
                    nameAddr().displayName()= (yyvsp[(2) - (5)].sv);
        headerptr->passertedID()().ids()[passertedidcount].
                    nameAddr().addrSpec()= *uriptr;
        passertedidcount++;
//        Free($2);
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 406:

/* Line 1806 of yacc.c  */
#line 2655 "SIP_parse.y"
    {
        headerptr->passertedID()().ids()[passertedidcount].
                    nameAddr().displayName()= OMIT_VALUE;
        headerptr->passertedID()().ids()[passertedidcount].
                    nameAddr().addrSpec()= *uriptr;
        passertedidcount++;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 407:

/* Line 1806 of yacc.c  */
#line 2665 "SIP_parse.y"
    {
        headerptr->passertedID()().ids()[passertedidcount].
                    addrSpecUnion()= *uriptr;
        passertedidcount++;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 408:

/* Line 1806 of yacc.c  */
#line 2675 "SIP_parse.y"
    {}
    break;

  case 412:

/* Line 1806 of yacc.c  */
#line 2682 "SIP_parse.y"
    {
        headerptr->ppreferredID()().ids()[ppreferredidcount].
                    nameAddr().displayName()= (yyvsp[(2) - (5)].sv);
        headerptr->ppreferredID()().ids()[ppreferredidcount].
                    nameAddr().addrSpec()= *uriptr;
        ppreferredidcount++;
//        Free($2);
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 413:

/* Line 1806 of yacc.c  */
#line 2693 "SIP_parse.y"
    {
        headerptr->ppreferredID()().ids()[ppreferredidcount].
                    nameAddr().displayName()= OMIT_VALUE;
        headerptr->ppreferredID()().ids()[ppreferredidcount].
                    nameAddr().addrSpec()= *uriptr;
        ppreferredidcount++;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 414:

/* Line 1806 of yacc.c  */
#line 2703 "SIP_parse.y"
    {
        headerptr->ppreferredID()().ids()[ppreferredidcount].
                    addrSpecUnion()= *uriptr;
        ppreferredidcount++;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 415:

/* Line 1806 of yacc.c  */
#line 2713 "SIP_parse.y"
    {}
    break;

  case 418:

/* Line 1806 of yacc.c  */
#line 2720 "SIP_parse.y"
    {
      headerptr->privacy()().privacyValues()[privacycount]=(yyvsp[(1) - (1)].sv);
      privacycount++;
//      Free($1);
    }
    break;

  case 419:

/* Line 1806 of yacc.c  */
#line 2727 "SIP_parse.y"
    {
        headerptr->rack()().response__num()= str2int((yyvsp[(3) - (7)].sv)); /*Free($3);*/
        headerptr->rack()().seqNumber()= str2int((yyvsp[(5) - (7)].sv)); /*Free($5);*/
        headerptr->rack()().method()= (yyvsp[(7) - (7)].sv);
//        Free($7);
      }
    break;

  case 420:

/* Line 1806 of yacc.c  */
#line 2735 "SIP_parse.y"
    {
        headerptr->rseq()().response__num() = str2int((yyvsp[(3) - (3)].sv)); /*Free($3);*/
      }
    break;

  case 421:

/* Line 1806 of yacc.c  */
#line 2740 "SIP_parse.y"
    {}
    break;

  case 422:

/* Line 1806 of yacc.c  */
#line 2743 "SIP_parse.y"
    {
        headerptr->replyTo()().addressField().nameAddr().displayName() = (yyvsp[(2) - (6)].sv);
        headerptr->replyTo()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->replyTo()().replyToParams()=*paramptr;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;

//        Free($2);
      }
    break;

  case 423:

/* Line 1806 of yacc.c  */
#line 2757 "SIP_parse.y"
    {
        headerptr->replyTo()().addressField().nameAddr().displayName() = 
                                                                     OMIT_VALUE;
        headerptr->replyTo()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->replyTo()().replyToParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 424:

/* Line 1806 of yacc.c  */
#line 2769 "SIP_parse.y"
    {
        headerptr->replyTo()().addressField().addrSpecUnion()= *uriptr;
        headerptr->replyTo()().replyToParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 425:

/* Line 1806 of yacc.c  */
#line 2779 "SIP_parse.y"
    {
        headerptr->replyTo()().addressField().nameAddr().displayName() = (yyvsp[(2) - (5)].sv);
        headerptr->replyTo()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->replyTo()().replyToParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
//        Free($2);
      }
    break;

  case 426:

/* Line 1806 of yacc.c  */
#line 2788 "SIP_parse.y"
    {
        headerptr->replyTo()().addressField().nameAddr().displayName() = 
                                                                    OMIT_VALUE;
        headerptr->replyTo()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->replyTo()().replyToParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 427:

/* Line 1806 of yacc.c  */
#line 2797 "SIP_parse.y"
    {
        headerptr->replyTo()().addressField().addrSpecUnion()= *uriptr;
        headerptr->replyTo()().replyToParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 428:

/* Line 1806 of yacc.c  */
#line 2806 "SIP_parse.y"
    {
        headerptr->undefinedHeader__List()()[undefcount].headerName() = (yyvsp[(1) - (2)].sv);
        headerptr->undefinedHeader__List()()[undefcount].headerValue()=trim((yyvsp[(2) - (2)].sv));
        undefcount++;
//        Free($1);Free($2);
      }
    break;

  case 429:

/* Line 1806 of yacc.c  */
#line 2814 "SIP_parse.y"
    {}
    break;

  case 432:

/* Line 1806 of yacc.c  */
#line 2821 "SIP_parse.y"
    {}
    break;

  case 434:

/* Line 1806 of yacc.c  */
#line 2826 "SIP_parse.y"
    {
        headerptr->via()().viaBody()[viacount].viaParams()=*paramptr;
        viacount++;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 435:

/* Line 1806 of yacc.c  */
#line 2833 "SIP_parse.y"
    {
        headerptr->via()().viaBody()[viacount].viaParams()=OMIT_VALUE;
        viacount++;
      }
    break;

  case 436:

/* Line 1806 of yacc.c  */
#line 2840 "SIP_parse.y"
    {
       headerptr->via()().viaBody()[viacount].sentProtocol().protocolName()=(yyvsp[(1) - (5)].sv);
       headerptr->via()().viaBody()[viacount].sentProtocol().protocolVersion()=(yyvsp[(3) - (5)].sv);
       headerptr->via()().viaBody()[viacount].sentProtocol().transport()=(yyvsp[(5) - (5)].sv);
//       Free($1);Free($3);Free($5);
     }
    break;

  case 437:

/* Line 1806 of yacc.c  */
#line 2848 "SIP_parse.y"
    {
        headerptr->via()().viaBody()[viacount].sentBy().host()=trimOnIPv6((yyvsp[(1) - (5)].sv));
        headerptr->via()().viaBody()[viacount].sentBy().portField()=(yyvsp[(5) - (5)].iv);
//        Free($1);
      }
    break;

  case 438:

/* Line 1806 of yacc.c  */
#line 2853 "SIP_parse.y"
    {
        headerptr->via()().viaBody()[viacount].sentBy().host()=trimOnIPv6((yyvsp[(1) - (4)].sv));
        headerptr->via()().viaBody()[viacount].sentBy().portField()=(yyvsp[(4) - (4)].iv);
//        Free($1);
      }
    break;

  case 439:

/* Line 1806 of yacc.c  */
#line 2858 "SIP_parse.y"
    {
        headerptr->via()().viaBody()[viacount].sentBy().host()=trimOnIPv6((yyvsp[(1) - (4)].sv));
        headerptr->via()().viaBody()[viacount].sentBy().portField()=(yyvsp[(4) - (4)].iv);
//        Free($1);
      }
    break;

  case 440:

/* Line 1806 of yacc.c  */
#line 2863 "SIP_parse.y"
    {
        headerptr->via()().viaBody()[viacount].sentBy().host()=trimOnIPv6((yyvsp[(1) - (3)].sv));
        headerptr->via()().viaBody()[viacount].sentBy().portField()=(yyvsp[(3) - (3)].iv);
//        Free($1);
      }
    break;

  case 441:

/* Line 1806 of yacc.c  */
#line 2868 "SIP_parse.y"
    {
        headerptr->via()().viaBody()[viacount].sentBy().host()=trimOnIPv6((yyvsp[(1) - (1)].sv));
        headerptr->via()().viaBody()[viacount].sentBy().portField()=OMIT_VALUE;
//        Free($1);
      }
    break;

  case 442:

/* Line 1806 of yacc.c  */
#line 2875 "SIP_parse.y"
    {}
    break;

  case 443:

/* Line 1806 of yacc.c  */
#line 2876 "SIP_parse.y"
    {}
    break;

  case 444:

/* Line 1806 of yacc.c  */
#line 2879 "SIP_parse.y"
    {
        headerptr->minExpires()().deltaSec()= (yyvsp[(3) - (3)].sv);
//        Free($3);
      }
    break;

  case 445:

/* Line 1806 of yacc.c  */
#line 2885 "SIP_parse.y"
    {
        headerptr->mimeVersion()().majorNumber()=str2int((yyvsp[(3) - (5)].sv)); /*Free($3);*/
        headerptr->mimeVersion()().minorNumber()=str2int((yyvsp[(5) - (5)].sv)); /*Free($5); */
      }
    break;

  case 446:

/* Line 1806 of yacc.c  */
#line 2891 "SIP_parse.y"
    {}
    break;

  case 447:

/* Line 1806 of yacc.c  */
#line 2894 "SIP_parse.y"
    {
        headerptr->userAgent()().userAgentBody()[useragentcount]= (yyvsp[(1) - (1)].sv);
        useragentcount++;
        Free((yyvsp[(1) - (1)].sv));
      }
    break;

  case 448:

/* Line 1806 of yacc.c  */
#line 2899 "SIP_parse.y"
    {
        headerptr->userAgent()().userAgentBody()[useragentcount]= (yyvsp[(3) - (3)].sv);
        useragentcount++;
        Free((yyvsp[(3) - (3)].sv));
      }
    break;

  case 449:

/* Line 1806 of yacc.c  */
#line 2906 "SIP_parse.y"
    {
        headerptr->timestamp()().timeValue()().majorDigit()=str2int((yyvsp[(3) - (3)].sv)); /*Free($3);*/
        headerptr->timestamp()().timeValue()().minorDigit()=OMIT_VALUE;
        headerptr->timestamp()().delay() = OMIT_VALUE;
      }
    break;

  case 450:

/* Line 1806 of yacc.c  */
#line 2911 "SIP_parse.y"
    {
        headerptr->timestamp()().timeValue()().majorDigit()=str2int((yyvsp[(3) - (5)].sv)); /*Free($3);*/
        headerptr->timestamp()().timeValue()().minorDigit()=str2int((yyvsp[(5) - (5)].sv)); /*Free($5);*/
        headerptr->timestamp()().delay() = OMIT_VALUE;
      }
    break;

  case 451:

/* Line 1806 of yacc.c  */
#line 2916 "SIP_parse.y"
    {
        headerptr->timestamp()().timeValue()().majorDigit()=str2int((yyvsp[(3) - (5)].sv)); /*Free($3);*/
        headerptr->timestamp()().timeValue()().minorDigit()=OMIT_VALUE;
        headerptr->timestamp()().delay()().majorDigit()=str2int((yyvsp[(5) - (5)].sv)); /*Free($5);*/
        headerptr->timestamp()().delay()().minorDigit()=OMIT_VALUE;
      }
    break;

  case 452:

/* Line 1806 of yacc.c  */
#line 2922 "SIP_parse.y"
    {
        headerptr->timestamp()().timeValue()().majorDigit()=str2int((yyvsp[(3) - (7)].sv)); //Free($3);
        headerptr->timestamp()().timeValue()().minorDigit()=str2int((yyvsp[(5) - (7)].sv)); //Free($5);
        headerptr->timestamp()().delay()().majorDigit()=str2int((yyvsp[(7) - (7)].sv)); //Free($7);
        headerptr->timestamp()().delay()().minorDigit()=OMIT_VALUE;
      }
    break;

  case 453:

/* Line 1806 of yacc.c  */
#line 2928 "SIP_parse.y"
    {
        headerptr->timestamp()().timeValue()().majorDigit()=str2int((yyvsp[(3) - (7)].sv)); //Free($3);
        headerptr->timestamp()().timeValue()().minorDigit()=OMIT_VALUE;
        headerptr->timestamp()().delay()().majorDigit()=str2int((yyvsp[(5) - (7)].sv)); //Free($5);
        headerptr->timestamp()().delay()().minorDigit()=str2int((yyvsp[(7) - (7)].sv)); //Free($7);
      }
    break;

  case 454:

/* Line 1806 of yacc.c  */
#line 2935 "SIP_parse.y"
    {
        headerptr->timestamp()().timeValue()().majorDigit()=str2int((yyvsp[(3) - (9)].sv)); //Free($3);
        headerptr->timestamp()().timeValue()().minorDigit()=str2int((yyvsp[(5) - (9)].sv)); //Free($5);
        headerptr->timestamp()().delay()().majorDigit()=str2int((yyvsp[(7) - (9)].sv)); //Free($7);
        headerptr->timestamp()().delay()().minorDigit()=str2int((yyvsp[(9) - (9)].sv)); //Free($9);
      }
    break;

  case 455:

/* Line 1806 of yacc.c  */
#line 2943 "SIP_parse.y"
    {
        headerptr->supported()().optionsTags()=OMIT_VALUE;
      }
    break;

  case 456:

/* Line 1806 of yacc.c  */
#line 2946 "SIP_parse.y"
    {
        if(!suppcount){
          headerptr->supported()().optionsTags()= *optptr;
          delete optptr;
        }
        suppcount=optioncount;
      }
    break;

  case 457:

/* Line 1806 of yacc.c  */
#line 2955 "SIP_parse.y"
    {
        if(!reqcount){
          headerptr->require()().optionsTags()= *optptr;
          delete optptr;
        }
        reqcount=optioncount;
      }
    break;

  case 458:

/* Line 1806 of yacc.c  */
#line 2964 "SIP_parse.y"
    {
        if(!recroutecount){
          headerptr->recordRoute()().routeBody()= *routeptr;
          delete routeptr;
        }
        recroutecount=rcount;
      }
    break;

  case 459:

/* Line 1806 of yacc.c  */
#line 2973 "SIP_parse.y"
    {
        headerptr->organization()().organization() = trim((yyvsp[(2) - (2)].sv));
//        Free($2);
      }
    break;

  case 460:

/* Line 1806 of yacc.c  */
#line 2979 "SIP_parse.y"
    {
        headerptr->date()().sipDate() = trim((yyvsp[(3) - (3)].sv));
//        Free($3);
      }
    break;

  case 461:

/* Line 1806 of yacc.c  */
#line 2985 "SIP_parse.y"
    {
        headerptr->cSeq()().seqNumber() = str2int((yyvsp[(3) - (5)].sv)); //Free($3);
        headerptr->cSeq()().method()= (yyvsp[(5) - (5)].sv);
//        Free($5);
      }
    break;

  case 462:

/* Line 1806 of yacc.c  */
#line 2992 "SIP_parse.y"
    {}
    break;

  case 465:

/* Line 1806 of yacc.c  */
#line 2999 "SIP_parse.y"
    {
        headerptr->callInfo()().callInfoBody()()[callinfocount].url()=(yyvsp[(3) - (4)].sv);
        headerptr->callInfo()().callInfoBody()()[callinfocount].infoParams()=
                                                                     OMIT_VALUE;
        callinfocount++;
//        Free($3);
      }
    break;

  case 466:

/* Line 1806 of yacc.c  */
#line 3006 "SIP_parse.y"
    {
        headerptr->callInfo()().callInfoBody()()[callinfocount].url()=(yyvsp[(3) - (5)].sv);
        headerptr->callInfo()().callInfoBody()()[callinfocount].infoParams()=
                                                                      *paramptr;
        paramcount=0;
        callinfocount++;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($3);
      }
    break;

  case 467:

/* Line 1806 of yacc.c  */
#line 3018 "SIP_parse.y"
    {
        headerptr->callId()().callid()=(yyvsp[(3) - (3)].sv);
//        Free($3);
      }
    break;

  case 468:

/* Line 1806 of yacc.c  */
#line 3024 "SIP_parse.y"
    {}
    break;

  case 469:

/* Line 1806 of yacc.c  */
#line 3025 "SIP_parse.y"
    {}
    break;

  case 470:

/* Line 1806 of yacc.c  */
#line 3028 "SIP_parse.y"
    {
        if(!acceptcount){headerptr->accept()().acceptArgs()= OMIT_VALUE;};
      }
    break;

  case 471:

/* Line 1806 of yacc.c  */
#line 3031 "SIP_parse.y"
    {}
    break;

  case 474:

/* Line 1806 of yacc.c  */
#line 3038 "SIP_parse.y"
    {
        size_t len2 = strlen((yyvsp[(2) - (4)].sv));
        size_t buflen = len2+strlen((yyvsp[(4) - (4)].sv))+2;
        char *buff=(char *)Malloc(buflen);
        /*buff[0]='\0';
        strcat(buff,$2);
        strcat(buff,"/");
        strcat(buff,$4);*/
        // avoid strcat+strcat+...
        strcpy(buff, (yyvsp[(2) - (4)].sv));
        buff[len2] = '/';
        strcpy(buff+len2+1, (yyvsp[(4) - (4)].sv));
        buff[buflen-1] = '\0'; // safety
        headerptr->accept()().acceptArgs()()[acceptcount].mediaRange()=buff;
        headerptr->accept()().acceptArgs()()[acceptcount].acceptParam()
                      =OMIT_VALUE;
        acceptcount++;
        //Free($2);Free($4);
	Free(buff);
      }
    break;

  case 475:

/* Line 1806 of yacc.c  */
#line 3058 "SIP_parse.y"
    {
        size_t len2 = strlen((yyvsp[(2) - (5)].sv));
        size_t buflen = len2+strlen((yyvsp[(4) - (5)].sv))+2;
        char *buff=(char *)Malloc(buflen);
        /*buff[0]='\0';
        strcat(buff,$2);
        strcat(buff,"/");
        strcat(buff,$4);*/
        // avoid strcat+strcat+...
        strcpy(buff, (yyvsp[(2) - (5)].sv));
        buff[len2] = '/';
        strcpy(buff+len2+1, (yyvsp[(4) - (5)].sv));
        buff[buflen-1] = '\0'; // safety
        headerptr->accept()().acceptArgs()()[acceptcount].mediaRange()=buff;
        headerptr->accept()().acceptArgs()()[acceptcount].acceptParam()
                      =*paramptr;
        paramcount=0;
        acceptcount++;
        delete paramptr;
        paramptr=new GenericParam__List;
        //Free($2);Free($4);
	Free(buff);
      }
    break;

  case 476:

/* Line 1806 of yacc.c  */
#line 3084 "SIP_parse.y"
    {
        if(!aceptenccount){headerptr->acceptEncoding()().contentCoding()
                    = OMIT_VALUE;};
      }
    break;

  case 477:

/* Line 1806 of yacc.c  */
#line 3088 "SIP_parse.y"
    {}
    break;

  case 478:

/* Line 1806 of yacc.c  */
#line 3091 "SIP_parse.y"
    {
        headerptr->acceptEncoding()().contentCoding()()[aceptenccount]=trim((yyvsp[(2) - (2)].sv));
        aceptenccount++;
//        Free($2);
      }
    break;

  case 479:

/* Line 1806 of yacc.c  */
#line 3096 "SIP_parse.y"
    {
        headerptr->acceptEncoding()().contentCoding()()[aceptenccount]=trim((yyvsp[(4) - (4)].sv));
        aceptenccount++;
//        Free($4);
      }
    break;

  case 480:

/* Line 1806 of yacc.c  */
#line 3103 "SIP_parse.y"
    {
        headerptr->acceptLanguage()().languageBody()=OMIT_VALUE;
      }
    break;

  case 481:

/* Line 1806 of yacc.c  */
#line 3106 "SIP_parse.y"
    {}
    break;

  case 484:

/* Line 1806 of yacc.c  */
#line 3113 "SIP_parse.y"
    {
        headerptr->acceptLanguage()().languageBody()()[acceptlangcount].
                      languageRange()=(yyvsp[(2) - (2)].sv);
        headerptr->acceptLanguage()().languageBody()()[acceptlangcount].
                      acceptParam()=OMIT_VALUE;
        acceptlangcount++;
//        Free($2);
      }
    break;

  case 485:

/* Line 1806 of yacc.c  */
#line 3121 "SIP_parse.y"
    {
        headerptr->acceptLanguage()().languageBody()()[acceptlangcount].
                      languageRange()=(yyvsp[(2) - (3)].sv);
        headerptr->acceptLanguage()().languageBody()()[acceptlangcount].
                      acceptParam()=*paramptr;
        paramcount=0;
        acceptlangcount++;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($2);
      }
    break;

  case 486:

/* Line 1806 of yacc.c  */
#line 3136 "SIP_parse.y"
    {}
    break;

  case 487:

/* Line 1806 of yacc.c  */
#line 3139 "SIP_parse.y"
    {
        int idx;
        if(headerptr->wwwAuthenticate().ispresent()) 
          idx=headerptr->wwwAuthenticate()().challenge().size_of();
        else idx=0;
        headerptr->wwwAuthenticate()().challenge()[idx].digestCln()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
      }
    break;

  case 488:

/* Line 1806 of yacc.c  */
#line 3149 "SIP_parse.y"
    {
        int idx;
        if(headerptr->wwwAuthenticate().ispresent()) 
          idx=headerptr->wwwAuthenticate()().challenge().size_of();
        else idx=0;
        headerptr->wwwAuthenticate()().challenge()[idx].otherChallenge().
                      authScheme()=(yyvsp[(2) - (3)].sv);
        headerptr->wwwAuthenticate()().challenge()[idx].otherChallenge().
                      authParams()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($2);
      }
    break;

  case 489:

/* Line 1806 of yacc.c  */
#line 3165 "SIP_parse.y"
    {}
    break;

  case 490:

/* Line 1806 of yacc.c  */
#line 3168 "SIP_parse.y"
    {
        int idx;
        if(headerptr->proxyAuthenticate().ispresent()) 
          idx=headerptr->proxyAuthenticate()().challenge().size_of();
        else idx=0;
        headerptr->proxyAuthenticate()().challenge()[idx].digestCln()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
      }
    break;

  case 491:

/* Line 1806 of yacc.c  */
#line 3178 "SIP_parse.y"
    {
        int idx;
        if(headerptr->proxyAuthenticate().ispresent()) 
          idx=headerptr->proxyAuthenticate()().challenge().size_of();
        else idx=0;
        headerptr->proxyAuthenticate()().challenge()[idx].otherChallenge().
                authScheme()=(yyvsp[(2) - (3)].sv);
        headerptr->proxyAuthenticate()().challenge()[idx].otherChallenge().
                authParams()=*paramptr;
        paramcount=0;
        delete paramptr;
        paramptr=new GenericParam__List;
//        Free($2);
      }
    break;

  case 492:

/* Line 1806 of yacc.c  */
#line 3196 "SIP_parse.y"
    {
        headerptr->toField()().addressField().nameAddr().displayName() = (yyvsp[(3) - (7)].sv);
        headerptr->toField()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->toField()().toParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
//        Free($3);
      }
    break;

  case 493:

/* Line 1806 of yacc.c  */
#line 3208 "SIP_parse.y"
    {
        headerptr->toField()().addressField().nameAddr().displayName()=OMIT_VALUE;
        headerptr->toField()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->toField()().toParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 494:

/* Line 1806 of yacc.c  */
#line 3219 "SIP_parse.y"
    {
        headerptr->toField()().addressField().addrSpecUnion()= *uriptr;
        headerptr->toField()().toParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 495:

/* Line 1806 of yacc.c  */
#line 3229 "SIP_parse.y"
    {
        headerptr->toField()().addressField().nameAddr().displayName() = (yyvsp[(3) - (6)].sv);
        headerptr->toField()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->toField()().toParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
//        Free($3);
      }
    break;

  case 496:

/* Line 1806 of yacc.c  */
#line 3238 "SIP_parse.y"
    {
        headerptr->toField()().addressField().nameAddr().displayName()=OMIT_VALUE;
        headerptr->toField()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->toField()().toParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 497:

/* Line 1806 of yacc.c  */
#line 3246 "SIP_parse.y"
    {
        headerptr->toField()().addressField().addrSpecUnion()= *uriptr;
        headerptr->toField()().toParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 498:

/* Line 1806 of yacc.c  */
#line 3255 "SIP_parse.y"
    {}
    break;

  case 499:

/* Line 1806 of yacc.c  */
#line 3256 "SIP_parse.y"
    {}
    break;

  case 500:

/* Line 1806 of yacc.c  */
#line 3259 "SIP_parse.y"
    {
        headerptr->contact()().contactBody().wildcard()=(yyvsp[(3) - (3)].sv);
//        Free($3);
      }
    break;

  case 501:

/* Line 1806 of yacc.c  */
#line 3263 "SIP_parse.y"
    {}
    break;

  case 502:

/* Line 1806 of yacc.c  */
#line 3266 "SIP_parse.y"
    {}
    break;

  case 503:

/* Line 1806 of yacc.c  */
#line 3267 "SIP_parse.y"
    {}
    break;

  case 504:

/* Line 1806 of yacc.c  */
#line 3270 "SIP_parse.y"
    {
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    addressField().nameAddr().displayName()= (yyvsp[(2) - (6)].sv);
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    contactParams()= *paramptr;
        contactcount++;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
//        Free($2);
      }
    break;

  case 505:

/* Line 1806 of yacc.c  */
#line 3286 "SIP_parse.y"
    {
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    addressField().nameAddr().displayName()= OMIT_VALUE;
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    contactParams()= *paramptr;
        contactcount++;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 506:

/* Line 1806 of yacc.c  */
#line 3301 "SIP_parse.y"
    {
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    addressField().addrSpecUnion()= *uriptr;
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    contactParams()= *paramptr;
        contactcount++;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 507:

/* Line 1806 of yacc.c  */
#line 3314 "SIP_parse.y"
    {
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    addressField().nameAddr().displayName()= (yyvsp[(2) - (5)].sv);
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    contactParams()= OMIT_VALUE;
        contactcount++;
//        Free($2);
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 508:

/* Line 1806 of yacc.c  */
#line 3327 "SIP_parse.y"
    {
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    addressField().nameAddr().displayName()= OMIT_VALUE;
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    contactParams()= OMIT_VALUE;
        contactcount++;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 509:

/* Line 1806 of yacc.c  */
#line 3339 "SIP_parse.y"
    {
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    addressField().addrSpecUnion()= *uriptr;
        headerptr->contact()().contactBody().contactAddresses()[contactcount].
                    contactParams()= OMIT_VALUE;
        contactcount++;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 510:

/* Line 1806 of yacc.c  */
#line 3352 "SIP_parse.y"
    {
        headerptr->fromField()().addressField().nameAddr().displayName() = (yyvsp[(3) - (7)].sv);
        headerptr->fromField()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->fromField()().fromParams()=*paramptr;

        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;

//        Free($3);
      }
    break;

  case 511:

/* Line 1806 of yacc.c  */
#line 3366 "SIP_parse.y"
    {
        headerptr->fromField()().addressField().nameAddr().displayName() 
                    = OMIT_VALUE;
        headerptr->fromField()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->fromField()().fromParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 512:

/* Line 1806 of yacc.c  */
#line 3378 "SIP_parse.y"
    {
        headerptr->fromField()().addressField().addrSpecUnion()= *uriptr;
        headerptr->fromField()().fromParams()=*paramptr;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
        delete paramptr;
        paramptr= new GenericParam__List;
        paramcount=0;
      }
    break;

  case 513:

/* Line 1806 of yacc.c  */
#line 3388 "SIP_parse.y"
    {
        headerptr->fromField()().addressField().nameAddr().displayName() = (yyvsp[(3) - (6)].sv);
        headerptr->fromField()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->fromField()().fromParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
//        Free($3);
      }
    break;

  case 514:

/* Line 1806 of yacc.c  */
#line 3397 "SIP_parse.y"
    {
        headerptr->fromField()().addressField().nameAddr().displayName()
                    = OMIT_VALUE;
        headerptr->fromField()().addressField().nameAddr().addrSpec()= *uriptr;
        headerptr->fromField()().fromParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 515:

/* Line 1806 of yacc.c  */
#line 3406 "SIP_parse.y"
    {
        headerptr->fromField()().addressField().addrSpecUnion()= *uriptr;
        headerptr->fromField()().fromParams()=OMIT_VALUE;
        delete uriptr;
        uriptr= new SipUrl;
        urlparamcount=headercount=0;
      }
    break;

  case 516:

/* Line 1806 of yacc.c  */
#line 3415 "SIP_parse.y"
    {}
    break;

  case 517:

/* Line 1806 of yacc.c  */
#line 3416 "SIP_parse.y"
    {}
    break;

  case 518:

/* Line 1806 of yacc.c  */
#line 3419 "SIP_parse.y"
    {
        (*paramptr)[paramcount].id()=(yyvsp[(2) - (4)].sv);
        (*paramptr)[paramcount].paramValue()=(yyvsp[(4) - (4)].sv);
        paramcount++;
//        Free($2);Free($4);
      }
    break;

  case 519:

/* Line 1806 of yacc.c  */
#line 3425 "SIP_parse.y"
    {
        (*paramptr)[paramcount].id()=(yyvsp[(1) - (3)].sv);
        (*paramptr)[paramcount].paramValue()=(yyvsp[(3) - (3)].sv);
        paramcount++;
//        Free($1);Free($3);
      }
    break;

  case 520:

/* Line 1806 of yacc.c  */
#line 3431 "SIP_parse.y"
    {}
    break;

  case 521:

/* Line 1806 of yacc.c  */
#line 3432 "SIP_parse.y"
    {}
    break;

  case 522:

/* Line 1806 of yacc.c  */
#line 3437 "SIP_parse.y"
    {
        (*paramptr)[paramcount].id()=(yyvsp[(1) - (2)].sv);
        (*paramptr)[paramcount].paramValue()=(yyvsp[(2) - (2)].sv);
        paramcount++;
//        Free($1);
//        Free($2);
      }
    break;

  case 523:

/* Line 1806 of yacc.c  */
#line 3444 "SIP_parse.y"
    {
        (*paramptr)[paramcount].id()=(yyvsp[(1) - (1)].sv);
        (*paramptr)[paramcount].paramValue()=OMIT_VALUE;
        paramcount++;
//        Free($1);
      }
    break;

  case 524:

/* Line 1806 of yacc.c  */
#line 3452 "SIP_parse.y"
    {(yyval.sv)=(yyvsp[(3) - (3)].sv);}
    break;

  case 525:

/* Line 1806 of yacc.c  */
#line 3453 "SIP_parse.y"
    {(yyval.sv)=(yyvsp[(2) - (2)].sv);}
    break;

  case 529:

/* Line 1806 of yacc.c  */
#line 3461 "SIP_parse.y"
    {
        uriptr->urlParameters()()[urlparamcount].id()=(yyvsp[(1) - (2)].sv);
        uriptr->urlParameters()()[urlparamcount].paramValue()=(yyvsp[(2) - (2)].sv);
        urlparamcount++;
//        Free($1);Free($2);
      }
    break;

  case 530:

/* Line 1806 of yacc.c  */
#line 3467 "SIP_parse.y"
    {
        uriptr->urlParameters()()[urlparamcount].id()=(yyvsp[(1) - (1)].sv);
        uriptr->urlParameters()()[urlparamcount].paramValue()=OMIT_VALUE;
        urlparamcount++;
//        Free($1);
    }
    break;

  case 531:

/* Line 1806 of yacc.c  */
#line 3475 "SIP_parse.y"
    {(yyval.sv)=(yyvsp[(2) - (2)].sv);}
    break;

  case 533:

/* Line 1806 of yacc.c  */
#line 3481 "SIP_parse.y"
    {
        uriptr->scheme() = (yyvsp[(1) - (3)].sv);
        uriptr->urlParameters() = OMIT_VALUE;
        uriptr->headers() = OMIT_VALUE;
        if(!strcasecmp((yyvsp[(1) - (3)].sv),"tel")){
          if(!uriptr->userInfo().ispresent()){
            uriptr->userInfo()().userOrTelephoneSubscriber()=uriptr->hostPort().host()();
            uriptr->userInfo()().password()=OMIT_VALUE;
            uriptr->hostPort().host()=OMIT_VALUE;
          }
        }
        /*Free($1);*/
      }
    break;

  case 537:

/* Line 1806 of yacc.c  */
#line 3501 "SIP_parse.y"
    {}
    break;

  case 538:

/* Line 1806 of yacc.c  */
#line 3503 "SIP_parse.y"
    {
        uriptr->scheme() = (yyvsp[(1) - (5)].sv);
        if(!strcasecmp((yyvsp[(1) - (5)].sv),"tel")){
          if(!uriptr->userInfo().ispresent()){
            uriptr->userInfo()().userOrTelephoneSubscriber()=uriptr->hostPort().host()();
            uriptr->userInfo()().password()=OMIT_VALUE;
            uriptr->hostPort().host()=OMIT_VALUE;
          }
        }
        /*Free($1);*/
      }
    break;

  case 539:

/* Line 1806 of yacc.c  */
#line 3514 "SIP_parse.y"
    {
        uriptr->scheme() = (yyvsp[(1) - (4)].sv);
        uriptr->headers() = OMIT_VALUE;
        if(!strcasecmp((yyvsp[(1) - (4)].sv),"tel")){
          if(!uriptr->userInfo().ispresent()){
            uriptr->userInfo()().userOrTelephoneSubscriber()=uriptr->hostPort().host()();
            uriptr->userInfo()().password()=OMIT_VALUE;
            uriptr->hostPort().host()=OMIT_VALUE;
          }
        }
        /*Free($1);*/
      }
    break;

  case 540:

/* Line 1806 of yacc.c  */
#line 3526 "SIP_parse.y"
    {
        uriptr->scheme() = (yyvsp[(1) - (4)].sv);
        uriptr->urlParameters() = OMIT_VALUE;
        if(!strcasecmp((yyvsp[(1) - (4)].sv),"tel")){
          if(!uriptr->userInfo().ispresent()){
            uriptr->userInfo()().userOrTelephoneSubscriber()=uriptr->hostPort().host()();
            uriptr->userInfo()().password()=OMIT_VALUE;
            uriptr->hostPort().host()=OMIT_VALUE;
          }
        }
        /*Free($1);*/
      }
    break;

  case 541:

/* Line 1806 of yacc.c  */
#line 3538 "SIP_parse.y"
    {
        uriptr->scheme() = (yyvsp[(1) - (3)].sv);
        uriptr->urlParameters() = OMIT_VALUE;
        uriptr->headers() = OMIT_VALUE;
        if(!strcasecmp((yyvsp[(1) - (3)].sv),"tel")){
          if(!uriptr->userInfo().ispresent()){
            uriptr->userInfo()().userOrTelephoneSubscriber()=uriptr->hostPort().host()();
            uriptr->userInfo()().password()=OMIT_VALUE;
            uriptr->hostPort().host()=OMIT_VALUE;
          }
        }
        /*Free($1);*/
      }
    break;

  case 542:

/* Line 1806 of yacc.c  */
#line 3554 "SIP_parse.y"
    { uriptr->userInfo() = OMIT_VALUE;}
    break;

  case 543:

/* Line 1806 of yacc.c  */
#line 3555 "SIP_parse.y"
    {
        char* s=strchr((yyvsp[(1) - (1)].sv),':');
        if(s==NULL){
          uriptr->userInfo()().password() = OMIT_VALUE;
          uriptr->userInfo()().userOrTelephoneSubscriber() = (yyvsp[(1) - (1)].sv);
        }
        else{
          *s='\0';
          s++;
          uriptr->userInfo()().password() = s;
          uriptr->userInfo()().userOrTelephoneSubscriber() = (yyvsp[(1) - (1)].sv);
        }
//        Free($1);
      }
    break;

  case 544:

/* Line 1806 of yacc.c  */
#line 3571 "SIP_parse.y"
    {
        uriptr->hostPort().host() = trimOnIPv6((yyvsp[(1) - (2)].sv));
//        Free($1);
      }
    break;

  case 545:

/* Line 1806 of yacc.c  */
#line 3577 "SIP_parse.y"
    {uriptr->hostPort().portField() = OMIT_VALUE;}
    break;

  case 546:

/* Line 1806 of yacc.c  */
#line 3578 "SIP_parse.y"
    {uriptr->hostPort().portField() = (yyvsp[(2) - (2)].iv);}
    break;

  case 549:

/* Line 1806 of yacc.c  */
#line 3585 "SIP_parse.y"
    {}
    break;

  case 550:

/* Line 1806 of yacc.c  */
#line 3586 "SIP_parse.y"
    {}
    break;

  case 552:

/* Line 1806 of yacc.c  */
#line 3592 "SIP_parse.y"
    {}
    break;

  case 557:

/* Line 1806 of yacc.c  */
#line 3603 "SIP_parse.y"
    {}
    break;

  case 558:

/* Line 1806 of yacc.c  */
#line 3606 "SIP_parse.y"
    {
        uriptr->headers()()[headercount].id()=(yyvsp[(1) - (3)].sv);
        uriptr->headers()()[headercount].paramValue()=(yyvsp[(3) - (3)].sv);
        headercount++;
//        Free($1);Free($3);
      }
    break;



/* Line 1806 of yacc.c  */
#line 8795 "SIP_parse_.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 3617 "SIP_parse.y"

                    
/* Additional C code */
char *trim(char *string){

    /* trims leading blanks and removes line breaks*/

    char *j;
    int a=0;
    int b=0;
    j = string+strlen(string);
    //leading blanks
    while ((string[0] <= ' ') && (string[0] > 0) && (string < j)) string++; 
    while (string[a]){                                       // line breaks
        if((string[a]=='\n') || (string[a]=='\r')){
            while ((string[a] <= ' ') && (string[a] > 0) && (string[a])) a++;
            string[b]=' ';
            b++;
        }
        if(string[a]){
            string[b]=string[a];
            b++;
            a++;
        }
    }
    string[b]='\0';
    b--;
    //ending blanks
    while ((string[b] <= ' ') && (string[b] > 0) && (b)){string[b]='\0';b--;}
    return(string);
}

char *trimOnIPv6(char *str){
  if(ipv6enabled){
    if(str[0]=='['){
      str++;
      str[strlen(str)-1]='\0';
    }
  }
  return str;
}

void resetptr(){
    if(paramcount){
      delete paramptr;
      paramptr= new GenericParam__List;
      paramcount=0;
    }
    if(urlparamcount+headercount){
      delete uriptr;
      uriptr= new SipUrl;
      urlparamcount=headercount=0;
    }
}

extern char * getstream();

void parsing(const char* buff, int len, bool p_ipv6enabled){
  SIP_parse_lex_destroy();
  yy_buffer_state * flex_buffer = SIP_parse__scan_bytes (buff, len);
//  stream_buffer = getstream(); // EPTEBAL
  stream_buffer = (char *)Malloc((len*2+2)*sizeof(char)); // EPTEBAL
  if (flex_buffer == NULL) {
    TTCN_error("Flex buffer creation failed.");
  }
  ipv6enabled = p_ipv6enabled;
      SIP_parse_debug=0;
//      void resetptr();
      initcounters();
      SIP_parse_parse();  // also sets appropriate fields of msg through pointers..
        delete uriptr;
        uriptr=NULL;
        delete paramptr;
        paramptr=NULL;
  SIP_parse__delete_buffer(flex_buffer);
  Free(stream_buffer);
}

