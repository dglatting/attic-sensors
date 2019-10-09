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
 * This file holds common routines and definitions used within the
 * long-string TLS example programs.
 *
 *
 * $Log: util.cc,v $
 * Revision 1.40  2019/10/07 03:42:36  root
 * Shortened the function quoteStr().
 *
 * Revision 1.39  2019/10/01 04:50:51  root
 * Changed a couple of decls.
 *
 * Revision 1.38  2019/09/10 04:34:16  root
 * Added a function similar to Python's round().
 *
 * Revision 1.37  2019/09/04 04:23:47  root
 * Added MIT copyright notice.
 *
 * Revision 1.36  2019/08/31 06:04:57  root
 * Fixed width problem in _hex() template.
 *
 * Revision 1.35  2019/08/29 07:06:40  root
 * Got rid of the SMBUS code - it doesn't work.
 * Added more functions to the ads1015.
 * Code now freaking works.
 *
 * Revision 1.34  2019/08/28 07:57:21  root
 * Daily check-in. Things improving. Kernel trap gone.
 *
 * Revision 1.33  2019/08/27 07:09:13  root
 * It crashes the kernel, so that's nice.
 *
 * Revision 1.32  2019/08/27 01:01:34  root
 * Adding additional utilities to source.
 *
 * Revision 1.31  2018/08/21 21:47:30  root
 * Created/implemented a bunch of utility subroutines to standardize 
 * print output and reduce memory errors.
 *
 * Revision 1.30  2018/08/20 09:18:41  root
 * Logging subsysten to reduce the boost bloat.
 *
 * Revision 1.29  2018/08/17 08:40:13  root
 * Modified string_for() for better memory utilization. Added function
 * to dump the string map for debug.
 *
 * Revision 1.28  2018/08/16 10:38:57  root
 * many things have changed.
 *
 * Revision 1.27  2018/08/16 10:35:53  root
 * Deleted an unnecessary load() for SQLite.
 *
 * Revision 1.26  2018/08/11 04:33:07  root
 * Lazy keys were a bad idea.
 *
 * Revision 1.25  2018/08/04 04:40:08  root
 * First integration of lazy key pre-debug.
 *
 * Revision 1.24  2018/07/20 06:09:02  root
 * IPv4 now works.
 * Added template wrappers for the Berkeley socket functions.
 *
 * Revision 1.23  2018/07/19 04:18:07  root
 * Check-in before company meeting.
 *
 * Revision 1.22  2018/07/18 09:30:47  root
 * Too many changes to comment on.
 *
 * Revision 1.21  2018/07/11 09:15:19  root
 * I moved usage() to opts.cc where it was closer to the options.
 *
 * Revision 1.20  2018/07/04 08:44:34  root
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
 * Revision 1.19  2018/07/02 02:57:48  root
 * Client and server are now talking TCP/TLS although some more
 * debugging is necessary.
 *
 * Revision 1.18  2018/06/29 07:06:12  root
 * Added more utility functions and const-fy some things.
 *
 * Revision 1.17  2018/06/27 17:49:47  root
 * In c++ threads cannot create threads. That means redesign of the server.
 *
 * Revision 1.16  2018/06/26 00:00:21  root
 * Added more utility routines.
 *
 * Revision 1.15  2018/06/21 06:03:10  root
 * I removed the static from the ident structure declaration so its
 * content can be output by main().
 *
 * Revision 1.14  2018/06/16 00:56:49  root
 * Added atfCYBER copyright.
 *
 * Revision 1.13  2018/06/15 09:26:10  root
 * The stitching of the keys identity was flawed.
 *
 * Revision 1.12  2018/06/15 07:54:02  root
 * Fixed text within the usage() function.
 *
 * Revision 1.11  2018/06/15 07:23:19  root
 * The client and server now talk to each other.
 *
 * Revision 1.10  2018/06/12 05:48:41  root
 * Everything compiles and links. Now on to actual client/server testing.
 * Will enable selection between IPv6/IPv4 in future.
 * Will optimize threading in server in future.
 *
 * Revision 1.9  2018/06/11 03:49:32  root
 * Added threading to the client and expanded options.
 *
 * Revision 1.8  2018/06/06 05:22:50  root
 * The client compiles, links, and attempts to connect. Onward!
 *
 * Revision 1.7  2018/06/05 18:14:31  root
 * At this point the client compiles but does not link, which is
 * expected. I'm saving a copy of the files before I start that
 * part of the work.
 *
 * Revision 1.5  2016/07/14 00:09:57  root
 * Modified the key extractor to modulo key size.
 *
 * Revision 1.4  2016/01/12 04:44:48  root
 * Minor text fix.
 *
 * Revision 1.3  2015/12/22 02:00:50  root
 * Saving a working copy.
 *
 * Revision 1.2  2015/12/21 00:34:26  root
 * Saving a copy.
 *
 * Revision 1.1  2015/12/14 01:29:25  root
 * Initial revision
 *
 */

extern "C" {

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <mntent.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

}

#include <algorithm>
#include <cassert>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <typeinfo>
#include <vector>

#include "log.h"
#include "opts.h"
#include "util.h"


extern const
std::vector< std::string > util_ident {
  _UTIL_H_ID, "$Id: util.cc,v 1.40 2019/10/07 03:42:36 root Exp root $"
};


float
roundz( const float n, const u_int d ) {

  assert( d >= 0 );

        float rVal;
  const float p = ::pow( 10.0, d );

  if( n >= 0 )
    rVal = ( int( ::floor((( n ) * p ) + 0.5 )) / p );
  else
    rVal = ( int( ::ceil(((  n ) * p ) - 0.5 )) / p );

  return rVal;
}


const std::string
_vtoa( const std::vector<uint8_t>& v ) {

  std::stringstream s;

  for( size_t i = 0; i < v.size(); ++i )
    s << std::hex << std::setfill('0') << std::setw( 2 )
      << u_int( v[i]) << " ";

  return s.str();
}


const std::string
_vtoa( const u_char* v, size_t len ) {

  std::stringstream s;

  for( size_t i = 0; i < len; ++i )
    s << t2hex( v[i]) << " ";

  return s.str();
}


template< typename T >
std::string _hex( const T& Arg ) {

  std::stringstream s;

  s << std::hex << std::setfill('0')
    << std::setw( sizeof( T ))
    << int( Arg );

  return s.str();
}

const std::string
t2hex( u_int w ) {

  return _hex< u_int >( w );
}

const std::string
t2hex( u_short w ) {

  return _hex< u_short >( w );
}

const std::string
t2hex( u_char w ) {

  return _hex< u_char >( w );
}


const std::string
_tid( std::thread::id tid ) {

  std::stringstream s;

  s << "Thread " << std::hex << std::showbase << tid;
  
  return s.str();
}


// A simple routine to print out regex matches for diagnoses.

void
_log_match( const std::cmatch& m ) {

  std::stringstream s;

  s << "m.size()=" << m.size() << ", "
       "m.empty()=" << m.empty() << ", "
       "m.ready()=" << m.ready() << ", "
       "m.length()=" << m.length() << ", "
       "m.position()=" << m.position() << ", "
       "m.prefix().str()=" << quote( m.prefix().str()) << ", "
       "m.suffix().str()=" << quote( m.suffix().str()) << ", "
       "m.str()=" << m.str() << ", ";
  for( size_t i = 0; i < m.size(); ++i )
    s << "m.str(" << i << ")=" << quote( m.str(i)) << ", "
         "m[" << i << "].str()=" << quote( m[i].str()) << ", "
         "m.position(" << i << ")=" << m.position(i) << ", ";
  for( auto pos = m.begin(); pos != m.end(); ++pos )
    s << "pos=" << quote( *pos ) << ", ";

  _LOG_VERB(( s.str()));

}


const std::string
quote( const int& i ) {

  std::stringstream ss;

  ss << i;

  return quote( ss.str());
}

const std::string
quote( const char& c ) {

  std::stringstream ss;

  ss << c;

  return quote( ss.str());
}

const std::string
quote( const std::string& s ) {

  std::stringstream ss;

  ss << "\"" << s << "\"";

  return ss.str();
}

template< typename T >
std::string _paren( const T& Arg ) {

  std::stringstream ss;

  ss << '(' << Arg << ')';

  return ss.str();
}

const std::string
errno2str( const int e ) {

  std::stringstream ss;

  ss << ", errno=" << e << " " << parenQuote( ::strerror( e ));

  return ss.str();
}


const
std::string
parenQuote( const std::string& s ) {

  return paren( quote( s ));
}


const
std::string
parenQuote( const char* s ) {

  assert( s );
  return parenQuote( std::string( s ));
}

const
std::string
paren( const int i ) {

  return _paren< decltype( i )>( i );
}

const
std::string
paren( const u_int i ) {

  return _paren< decltype( i )>( i );
}

const
std::string
paren( const long i ) {

  return _paren< decltype( i )>( i );
}

const
std::string
paren( const u_long i ) {

  return _paren< decltype( i )>( i );
}

const
std::string
paren( const std::string& s ) {

  return _paren< decltype( s )>( s );
}

const
std::string
paren( const char* s ) {

  assert( s );
  return paren( std::string( s ));
}



// LocalWords:  endl IPv URI SQLite

