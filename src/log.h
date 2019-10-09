/* -*- c++ -*- */

/*
 * Copyright (c) 2018,2019 Dennis Glatting.
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
 * This file contains declarations and function prototypes for the
 * logging subsystem used by other classes and routines.
 *
 *
 * $Log: log.h,v $
 * Revision 1.13  2019/09/04 04:23:47  root
 * Added MIT copyright notice.
 *
 * Revision 1.12  2019/08/27 07:09:13  root
 * It crashes the kernel, so that's nice.
 *
 * Revision 1.11  2019/08/27 01:01:34  root
 * Adding additional utilities to source.
 *
 * Revision 1.10  2018/08/21 21:47:30  root
 * Created/implemented a bunch of utility subroutines to standardize 
 * print output and reduce memory errors.
 *
 * Revision 1.9  2018/08/20 09:17:54  root
 * Logging subsysten to reduce the boost bloat.
 *
 * Revision 1.8  2018/08/19 21:25:05  root
 * Some test debug changes that meant nothing. About to make
 * considerable changes.
 *
 * Revision 1.7  2018/08/17 21:25:21  root
 * _log() was unnecessarily creating temporary objects on invocation.
 *
 * Revision 1.6  2018/07/18 09:30:47  root
 * Too many changes to comment on.
 *
 * Revision 1.5  2018/07/04 08:44:34  root
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
 * Revision 1.4  2018/06/16 00:56:49  root
 * Added atfCYBER copyright.
 *
 * Revision 1.3  2018/06/15 07:23:19  root
 * The client and server now talk to each other.
 *
 * Revision 1.2  2018/06/06 05:22:50  root
 * The client compiles, links, and attempts to connect. Onward!
 *
 * Revision 1.1  2018/06/04 23:08:01  root
 * Initial revision
 *
 */


#ifndef __LOG_H__
#define __LOG_H__

extern "C" {

#include <syslog.h>
  
#include <sys/types.h>

}

#include <string>
#include <sstream>


#define _LOG_H_ID "$Id: log.h,v 1.13 2019/09/04 04:23:47 root Exp $"

// These are the log support functions and templates.

inline
std::stringstream& _log_support( std::stringstream& t ) {

  static std::stringstream s;

  return s;

}

template< typename T, typename ... Types >
inline
std::stringstream& _log_support( std::stringstream& t,
				 T& firstArg,
				 Types... args ) {

  t << firstArg;
  return _log_support( t, args... );
}

// This is a variadic template used with the _log() function.

template< typename T, typename ... Types >
std::string _log_interface( const T& firstArg, Types... args ) {

  std::stringstream stre;

  stre << firstArg << _log_support( stre, args... ).str();

  return stre.str();
}

// This is the function responsible for the actual logging. It is
// meant to be a hidden interface.

void
_log( const char* file, const char* func, int line, int lvl,
      const std::string& );


// There is no such thing as log verbose in syslog, so I am defining
// it.

#define LOG_VERB (LOG_DEBUG + 3)

// These are logging messages conceptually built like syslog() except
// that I shortened WARNING label and ABORT/ALERT exits the
// application. 

#ifdef _DPG_DEBUG
#define _LOG_DEBUG(x)  _log(__FILE__, __FUNCTION__, __LINE__, \
			    LOG_DEBUG, _log_interface x )
#else
#define _LOG_DEBUG(x)
#endif

#ifdef _DPG_DEBUG_VERBOSE
#define _LOG_VERB(x)  _log(__FILE__, __FUNCTION__, __LINE__,  \
			   LOG_VERB, _log_interface x )
#else
#define _LOG_VERB(x)
#endif

#define _LOG_INFO(x)   _log(__FILE__, __FUNCTION__, __LINE__, \
			    LOG_INFO, _log_interface x )

#define _LOG_NOTICE(x) _log(__FILE__, __FUNCTION__, __LINE__, \
			    LOG_NOTICE, _log_interface x )
#define _LOG_WARN(x)   _log(__FILE__, __FUNCTION__, __LINE__, \
			    LOG_WARNING,_log_interface x )
#define _LOG_ERR(x)    _log(__FILE__, __FUNCTION__, __LINE__, \
			    LOG_ERR, _log_interface x )
#define _LOG_ABORT(x)  _log(__FILE__, __FUNCTION__, __LINE__, \
			    LOG_CRIT, _log_interface x )

// These primitives are used to indicate whether the logging level
// permits the possible output of a log message and NOT an indication
// of a given logging level. Use with caution.

#define _IS_LOG_VERB   ( logging_level >= LOG_VERB   )
#define _IS_LOG_DEBUG  ( logging_level >= LOG_DEBUG  )
#define _IS_LOG_INFO   ( logging_level >= LOG_INFO   )
#define _IS_LOG_NOTICE ( logging_level >= LOG_NOTICE )
#define _IS_LOG_WARN   ( logging_level >= LOG_WARN   )
#define _IS_LOG_ERR    ( logging_level >= LOG_ERR    )
#define _IS_LOG_ABORT  ( logging_level >= LOG_CRIT   )


#endif


//  LocalWords:  PSK crypto BeagleBone syslog variadic
