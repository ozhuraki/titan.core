/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               TCCDateTime.cc
//  Description:        TCC Useful Functions: DateTime Functions
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 472
//  Updated:            2011-06-29
//  Contact:            http://ttcn.ericsson.se
//
///////////////////////////////////////////////////////////////////////////////

#include "TCCDateTime_Functions.hh"
#include <time.h>
#include <stdlib.h>

namespace TCCDateTime__Functions 
{

///////////////////////////////////////////////////////////////////////////////
//  Function: f__time
// 
//  Purpose:
//    Current calendar time of the system in seconds
//
//  Parameters:
//    -
// 
//  Return Value:
//    integer - time value
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
  INTEGER f__time()
  {
    time_t cur_time;
    time( &cur_time );
    INTEGER i;
    i.set_long_long_val(cur_time);
    return i;
  }

///////////////////////////////////////////////////////////////////////////////
//  Function: f__ctime
// 
//  Purpose:
//    Convert a time value in seconds to human readable string.
//    The time represented as local time
//
//  Parameters:
//    pl__sec - *in* *integer* - time value
// 
//  Return Value:
//    integer - converted time value
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
CHARSTRING f__ctime(const INTEGER& pl__sec)
{
    time_t cur_time = pl__sec.get_long_long_val();
    return ctime(&cur_time);
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f__ctime__UTC
// 
//  Purpose:
//    Convert a time value in seconds to human readable string.
//    The time represented as UTC
//
//  Parameters:
//    pl__sec - *in* *integer* - time value
// 
//  Return Value:
//    integer - converted time value
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
CHARSTRING f__ctime__UTC(const INTEGER& pl__sec)
{
    time_t cur_time = pl__sec.get_long_long_val();
    return asctime(gmtime(&cur_time));
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f__getTimeFormatted
// 
//  Purpose:
//    Return the current calendar time in a formatted way
//
//  Parameters:
//    pl__sec - *in* *integer* - time value
//    pl__format - *in* *charstring* - format string
// 
//  Return Value:
//    charstring - formatted time in string format
//
//  Errors:
//    - 
// 
//  Detailed description:
//    *Specifier / Replaced by / Example*
//
//    ----------------------------------------------------------------------------------------------------------
//
//    %a - Abbreviated weekday name * - Thu
//
//    %A - Full weekday name * - Thursday
//
//    %b - Abbreviated month name * - Aug
//
//    %B - Full month name * - August
//
//    %c - Date and time representation * - Thu Aug 23 14:55:02 2001
//
//    %d - Day of the month (01-31) - 23
//
//    %H - Hour in 24h format (00-23) - 14
//
//    %I - Hour in 12h format (01-12) - 02
//
//    %j - Day of the year (001-366) - 235
//
//    %m - Month as a decimal number (01-12) - 08
//
//    %M - Minute (00-59) - 55
//
//    %p - AM or PM designation - PM
//
//    %S - Second (00-61) - 02
//
//    %U - Week number with the first Sunday as the first day of week one (00-53) - 33
//
//    %w - Weekday as a decimal number with Sunday as 0 (0-6) - 4
//
//    %W - Week number with the first Monday as the first day of week one (00-53) - 34
//
//    %x - Date representation * - 08/23/01
//
//    %X - Time representation * - 14:55:02
//
//    %y - Year, last two digits (00-99) - 01
//
//    %Y - Year - 2001
//
//    %Z - Timezone name or abbreviation - CDT
//
//    %% - A % sign - %
//
//    ----------------------------------------------------------------------------------------------------------
//
//    * The specifiers whose description is marked with an asterisk (*) are locale-dependent.
// 
///////////////////////////////////////////////////////////////////////////////
  CHARSTRING f__getTimeFormatted(const INTEGER& pl__sec, const CHARSTRING& pl__format)
  {
    time_t in_time = pl__sec.get_long_long_val();
    size_t str_len = 255;
    char ret_val[str_len];
    strftime (ret_val, str_len, (const char *)pl__format, localtime(&in_time));
    return ret_val; 
  }

///////////////////////////////////////////////////////////////////////////////
//  Function: f__time2sec
// 
//  Purpose:
//    Function to convert a formated time value to seconds.
//    The time is expressed as local time.
//
//  Parameters:
//    pl__year - *in* *integer* - year (e.g. 2007)
//    pl__mon - *in* *integer* - month (e.g. 3)
//    pl__day - *in* *integer* - day (e.g. 7)
//    pl__hour - *in* *integer* - day (e.g. 12)
//    pl__min - *in* *integer* - day (e.g. 50)
//    pl__sec - *in* *integer* - day (e.g. 7)
// 
//  Return Value:
//    integer - time in seconds
//
//  Errors:
//    - 
// 
//  Detailed description:
//    time in seconds since January 1, 1900
// 
///////////////////////////////////////////////////////////////////////////////
INTEGER f__time2sec(const INTEGER& pl__year,
		      const INTEGER& pl__mon,
		      const INTEGER& pl__mday,
		      const INTEGER& pl__hour,
		      const INTEGER& pl__min,
		      const INTEGER& pl__sec) 
  {
    struct tm tms;
    tms.tm_sec = pl__sec;
    tms.tm_min = pl__min;
    tms.tm_hour = pl__hour;
    tms.tm_mday = pl__mday;
    tms.tm_mon = pl__mon - 1;
    tms.tm_year = pl__year - 1900;
    tms.tm_wday = 0;
    tms.tm_yday = 0;
    tms.tm_isdst = -1;

    time_t t = mktime(&tms);
    INTEGER i;
    i.set_long_long_val(t);
    return i;
  }


///////////////////////////////////////////////////////////////////////////////
//  Function: f__time2sec__UTC
// 
//  Purpose:
//    Function to convert a formated time value to seconds.
//    The time is expressed as UTC.
//
//  Parameters:
//    pl__year - *in* *integer* - year (e.g. 2007)
//    pl__mon - *in* *integer* - month (e.g. 3)
//    pl__day - *in* *integer* - day (e.g. 7)
//    pl__hour - *in* *integer* - day (e.g. 12)
//    pl__min - *in* *integer* - day (e.g. 50)
//    pl__sec - *in* *integer* - day (e.g. 7)
// 
//  Return Value:
//    integer - time in seconds
//
//  Errors:
//    - 
// 
//  Detailed description:
//    time in seconds since January 1, 1900
// 
///////////////////////////////////////////////////////////////////////////////

INTEGER f__time2sec__UTC(const INTEGER& pl__year,
		      const INTEGER& pl__mon,
		      const INTEGER& pl__mday,
		      const INTEGER& pl__hour,
		      const INTEGER& pl__min,
		      const INTEGER& pl__sec) 
  {
    struct tm tms;
    tms.tm_sec = pl__sec;
    tms.tm_min = pl__min;
    tms.tm_hour = pl__hour;
    tms.tm_mday = pl__mday;
    tms.tm_mon = pl__mon - 1;
    tms.tm_year = pl__year - 1900;
    tms.tm_wday = 0;
    tms.tm_yday = 0;
    tms.tm_isdst = 0;

    time_t t = mktime(&tms);
    t-= timezone;

    INTEGER i;
    i.set_long_long_val(t);
    return i;
  }

///////////////////////////////////////////////////////////////////////////////
//  Function: f__getCurrentDateWithOffset
// 
//  Purpose:
//    Generate a date from the actual date and time plus the parameter
//    in seconds e.g. getSdate(30) will return a charstring containing 
//    the date and time of 30 seconds later
//
//  Parameters:
//    pl__sec - *in* *integer* - offset time value
// 
//  Return Value:
//    charstring - formatted time in string format
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////  
  CHARSTRING f__getCurrentDateWithOffset(const INTEGER& pl__sec)
  {
    time_t cur_time;
    time( &cur_time );
    cur_time += pl__sec.get_long_long_val();
    return ctime (&cur_time);
  }

///////////////////////////////////////////////////////////////////////////////
//  Function: f__getCurrentGMTDate
// 
//  Purpose:
//    Return the current GMT date in format RFC 1123-Date 
//    e.g.:Sat, 13 Nov 2010 23:30:02 GMT
//
//  Parameters:
//    -
// 
//  Return Value:
//    charstring - formatted time in string format
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////  
  CHARSTRING f__getCurrentGMTDate()
  {
    time_t cur_time;
    time( &cur_time );

    return asctime( gmtime( &cur_time ) );
  }
  
  
//////////////////////////////////////////////////////////////////////////////
//  Function: f__tic
// 
//  Purpose:
//    Return the number of clock ticks used by the application since 
//    the program was launched
//
//    OR
//
//    Return the amount of CPU time in microseconds since the last call of f__tic
//
//    OR ...
//
//    Warning! This function depends on used library version. Be careful!
//
//  Parameters:
//    -
// 
//  Return Value:
//    integer - tics since program start
//
//  Errors:
//    -
// 
//  Detailed description:
//    -
// 
/////////////////////////////////////////////////////////////////////////////// 
INTEGER f__tic()
{
    INTEGER i;
    i.set_long_long_val(clock());
    return i;
}

//////////////////////////////////////////////////////////////////////////////
//  Function: f__toc
// 
//  Purpose:
//    Elapsed seconds since time t (only when f__tic() returns the number of 
//    clock ticks elapsed since the program was launched)
//
//    Warning! This function depends on used library version. Be careful!
//
//  Parameters:
//    t - *in* *integer* - time value
// 
//  Return Value:
//    float - elapsed seconds
//
//  Errors:
//    -
// 
//  Detailed description:
//    f__tic counts clock tics since program start. f__toc counts seconds
//    since clock() readout in t till current time
// 
///////////////////////////////////////////////////////////////////////////////
FLOAT f__toc(const INTEGER& t)
{
    clock_t tt = (clock_t)t.get_long_long_val();
    return FLOAT((double)(clock()-tt)/CLOCKS_PER_SEC);
}
  
//////////////////////////////////////////////////////////////////////////////
//  Function: f__timeDiff
// 
//  Purpose:
//    Difference between two time
//
//  Parameters:
//    t_start - *in* *integer* - start time
//    t_stop - *in* *integer* - stop time
// 
//  Return Value:
//    integer - t_stop-t_start
//
//  Errors:
//    - 
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
INTEGER f__timeDiff(const INTEGER& t_stop, const INTEGER& t_start)
{
    if(!t_stop.is_bound())
    {
        TTCN_error("Stop time is unbound in call to function TimeDiff");
    }
    if(!t_start.is_bound())
    {
        TTCN_error("Start time is unbound in call to function TimeDiff");
    }
    return t_stop-t_start;
}

} // end of Namespace
