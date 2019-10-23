/* -*- c++ -*- */

/*
 * Copyright (c) 2015,2018,2019 Dennis Glatting.
 * Copyright (c) 2018 atfCYBER. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 *
 * This is a header file that holds common definitions, prototypes,
 * and a few inline functions used across the the client/server
 * objects and routines. The "common" are a sort of grab bag.
 *
 *
 * $Log: util.h,v $
 * Revision 1.37  2019/10/19 01:07:19  root
 * Fixed minor formatting bug.
 *
 * Revision 1.36  2019/10/07 03:42:36  root
 * Shortened the function quoteStr().
 *
 * Revision 1.35  2019/10/01 04:50:51  root
 * Changed a couple of decls.
 *
 * Revision 1.34  2019/09/27 07:41:57  root
 * Introduced "structured bindings." e=enum,i=int,f=float,b=bits,m=mask.
 *
 * Revision 1.33  2019/09/26 19:04:30  root
 * Added errno2str() with void param because sometimes the param is
 * errno itself.
 *
 * Revision 1.32  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.31  2019/09/10 04:34:16  root
 * Added a function similar to Python's round().
 *
 * Revision 1.30  2019/09/04 04:23:47  root
 * Added MIT copyright notice.
 *
 * Revision 1.29  2019/08/27 07:09:13  root
 * It crashes the kernel, so that's nice.
 *
 * Revision 1.28  2019/08/27 01:01:34  root
 * Adding additional utilities to source.
 *
 * Revision 1.27  2018/08/21 21:47:30  root
 * Created/implemented a bunch of utility subroutines to standardize 
 * print output and reduce memory errors.
 *
 * Revision 1.26  2018/08/20 09:18:41  root
 * Logging subsysten to reduce the boost bloat.
 * /
 *
 * Revision 1.25  2018/08/17 08:40:13  root
 * Modified string_for() for better memory utilization. Added function
 * to dump the string map for debug.
 *
 * Revision 1.24  2018/08/16 10:34:08  root
 * Added a common regex flag def for identities. Added a string_for()
 * for char*.
 *
 * Revision 1.23  2018/08/11 04:33:07  root
 * Lazy keys were a bad idea.
 *
 * Revision 1.22  2018/08/04 04:40:08  root
 * First integration of lazy key pre-debug.
 *
 * Revision 1.21  2018/07/21 06:47:01  root
 * Removed unneeded #include statements.
 *
 * Revision 1.20  2018/07/20 06:09:02  root
 * IPv4 now works.
 * Added template wrappers for the Berkeley socket functions.
 *
 * Revision 1.19  2018/07/18 09:30:47  root
 * Too many changes to comment on.
 *
 * Revision 1.18  2018/07/11 09:15:19  root
 * I moved usage() to opts.cc where it was closer to the options.
 *
 * Revision 1.17  2018/07/04 08:44:34  root
 * Forced check-in of a working branch of code before the next round
 * of revisions. This version ran for 24 hours across a 3x3 node WiFi
 * network, consuming ~12Mb/sec of bandwidth and showing no apparent
 * memory of leakages. The next version of this code is to organise
 * key from tiered arrays into sets with individual keys (elements)
 * belonging to a set, resulting in sets of sets. The "array" and
 * "name" identifiers will be respectably replaced with "set" and
 * "elem"(element). Code will be added to form intersections of
 * sets. Code will also be added (i.e., SQLite) to store elements
 * (keys) in a database where should be more manageable in standard
 * form.
 *
 * Revision 1.16  2018/07/02 02:57:48  root
 * Client and server are now talking TCP/TLS although some more
 * debugging is necessary.
 *
 * Revision 1.15  2018/06/29 07:05:07  root
 * Restructured the _ntoa() routines and added some new utility
 * function declarations.
 *
 * Revision 1.14  2018/06/27 17:49:47  root
 * In c++ threads cannot create threads. That means redesign of the server.
 *
 * Revision 1.13  2018/06/26 00:33:06  root
 * I forgot a header file.
 *
 * Revision 1.12  2018/06/26 00:00:21  root
 * Added more utility routines.
 *
 * Revision 1.11  2018/06/16 00:56:49  root
 * Added atfCYBER copyright.
 *
 * Revision 1.10  2018/06/15 07:23:19  root
 * The client and server now talk to each other.
 *
 * Revision 1.9  2018/06/12 05:48:41  root
 * Everything compiles and links. Now on to actual client/server testing.
 * Will enable selection between IPv6/IPv4 in future.
 * Will optimize threading in server in future.
 *
 * Revision 1.8  2018/06/06 01:53:45  root
 * The client code compiles and links. Archiving before debug.
 *
 * Revision 1.7  2018/06/05 18:14:31  root
 * At this point the client compiles but does not link, which is
 * expected. I'm saving a copy of the files before I start that
 * part of the work.
 *
 * Revision 1.5  2016/07/14 00:09:57  root
 * Modified the key extractor to modulo key size.
 *
 * Revision 1.4  2015/12/27 08:28:14  root
 * *** empty log message ***
 *
 * Revision 1.3  2015/12/22 02:00:50  root
 * Saving a working copy.
 *
 * Revision 1.2  2015/12/14 01:30:58  root
 * *** empty log message ***
 *
 * Revision 1.1  2015/12/07 00:08:21  root
 * Initial revision
 *
 */


#ifndef __UTIL_H__
#define __UTIL_H__

extern "C" {

#include <assert.h>
  
#include <sys/types.h>
  
}

#include <chrono>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <thread>

#include "opts.h"


#define _UTIL_H_ID "$Id: util.h,v 1.37 2019/10/19 01:07:19 root Exp $"


// Make modulo math work the way I want it to work.

inline int
modulo( int m, int n ) {
  return m >= 0 ? m % n : ( n - std::abs( m % n )) % n;
}


// Round up a number to the number of digits after the decimal
// point. For example, roundup( 1.37, 1 ) = 1.4.

float roundz( const float num, const u_int digits );


// A handle little routine I like to use to manage dirty work.

inline std::time_t
t_now( void ) {

  return std::chrono::system_clock::to_time_t
    ( std::chrono::system_clock::now());
}


// Return a thread ID is a commonly used printable form.

const std::string _tid( std::thread::id );

inline
const
std::string _tid( void ) {
  return _tid( std::this_thread::get_id());
}


// Convert a binary string into ASCII hexadecimal form

const std::string _vtoa( const std::vector<uint8_t>& v );
const std::string _vtoa( const u_char* v, size_t len );

// Convert certain data types into hexadecimal strings suitable for
// printing - "type to hex."

const std::string t2hex( u_int );
const std::string t2hex( u_short );
const std::string t2hex( u_char );

inline
const std::string
t2hex( int w ) {

  return t2hex( u_int( w ));
}

inline
const std::string
t2Hex( short w ) {

  return t2hex( u_short( w ));
}

inline
const std::string
t2hex( char w ) {

  return t2hex( u_char( w ));
}


// This function is used by routines to log regex match
// information. This routine logs as verbose.

void _log_match( const std::cmatch& m );


// Return a commonly formatted errno string for inclusion in a log
// line.

const std::string errno2str( const int );

inline
const std::string
errno2str( void ) {

  return errno2str( errno );
}


// Return something quoted.

const std::string quote( const std::string& );
const std::string quote( const int& );
const std::string quote( const char& );

inline
const
std::string
quote( const char* s ) {

  assert( s );
  return quote( std::string( s ));
}


// Bit shift functions.
//
// Since this application is i2c oriented, the shift is limited to 8
// bits.

inline
uint8_t
_bit_shift_left( int s ) {

  assert(( s >= 0 ) && ( s <= 7 ));

  static const std::vector< uint8_t > b {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
  };

  return b[ s ];
}

inline
uint8_t
_bit_shift_right( int s ) {

  assert(( s >= 0 ) && ( s <= 7 ));

  static const std::vector< uint8_t > b {
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
  };

  return b[ s ];
}



// Return a thing inside parenthesis.

const std::string parenQuote( const char* );
const std::string parenQuote( const std::string& );

const std::string paren( const int    );
const std::string paren( const u_int  );
const std::string paren( const long   );
const std::string paren( const u_long );

const std::string paren( const char* );
const std::string paren( const std::string& );


// Template used to create a std::unique_ptr<> with a custom deleter

template< typename T >
using deleted_unique_ptr = std::unique_ptr< T, std::function<void(T*)>>;


#endif


//  LocalWords:  PSK crypto BeagleBone  getpeername URI URIs errno
