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
 * This file holds common routines and definitions used against the
 * long-string DTLS example programs.
 *
 *
 * $Log: log.cc,v $
 * Revision 1.18  2019/10/19 01:25:00  root
 * Fixed a clang complaint.
 *
 * Revision 1.17  2019/10/07 03:42:36  root
 * Shortened the function quoteStr().
 *
 * Revision 1.16  2019/10/05 04:09:32  root
 * Added ability to log to syslog().
 *
 * Revision 1.15  2019/09/04 04:23:47  root
 * Added MIT copyright notice.
 *
 * Revision 1.14  2019/08/27 01:01:34  root
 * Adding additional utilities to source.
 *
 * Revision 1.13  2018/08/21 21:47:30  root
 * Created/implemented a bunch of utility subroutines to standardize 
 * print output and reduce memory errors.
 *
 * Revision 1.12  2018/08/20 09:18:41  root
 * Logging subsysten to reduce the boost bloat.
 * /
 *
 * Revision 1.11  2018/08/17 21:25:21  root
 * _log() was unnecessarily creating temporary objects on invocation.
 *
 * Revision 1.10  2018/07/19 04:18:07  root
 * Check-in before company meeting.
 *
 * Revision 1.9  2018/07/12 03:32:50  root
 * Changed a < to <=.
 *
 * Revision 1.8  2018/07/04 08:44:34  root
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
 * Revision 1.7  2018/06/27 17:49:47  root
 * In c++ threads cannot create threads. That means redesign of the
 * server.
 *
 * Revision 1.6  2018/06/21 06:03:10  root
 * I removed the static from the ident structure declaration so its
 * content can be output by main().
 *
 * Revision 1.5  2018/06/16 00:56:49  root
 * Added atfCYBER copyright.
 *
 * Revision 1.4  2018/06/15 07:58:43  root
 * Has to change a <= to < in determining whether a message is output.
 *
 * Revision 1.3  2018/06/11 03:49:32  root
 * Added threading to the client and expanded options.
 *
 * Revision 1.2  2018/06/06 05:22:50  root
 * The client compiles, links, and attempts to connect. Onward!
 *
 * Revision 1.1  2018/06/05 18:14:31  root
 * Initial revision
 *
 */

extern "C" {

#include <assert.h>
#include <stdarg.h>
#include <syslog.h>
  
}

#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include "log.h"
#include "opts.h"
#include "util.h"


extern const
std::vector< std::string > log_ident {
  _LOG_H_ID, "$Id: log.cc,v 1.18 2019/10/19 01:25:00 root Exp $"
};


static std::mutex mtx;


void
_log( const char* file, const char* func, int line, int lvl,
      const std::string& s ) {

  assert( file && func );

  if( lvl <= logging_level ) {

    // Prepare the log message.
    
    std::stringstream ss;

    ss << _log_interface( "File ", quote( file ), ", "
			  "Function ", quote( func ), ", "
			  "Line ", line, ": ", s );

    // Mutual exclusion on output.
    
    std::unique_lock< std::mutex > l( mtx );

    // Output the message.
    
    if( logDev == "stdout" )
      std::cout << ss.str() << std::endl;
    else
      if( logDev == "syslog" ) {

	// Open the log if not yet open.
	
	static bool isOpen = false;
	if( isOpen == false ) {

	  ::openlog( "", LOG_NDELAY | LOG_PID, LOG_DAEMON );
	  ::atexit( closelog );

	  isOpen = true;
	  
	}
	
	// Normalize the logging level.
	
	int l = ( logging_level > LOG_DEBUG ? LOG_DEBUG : logging_level );

	// Output.
	
	::syslog( l, "%s", ss.str().c_str());
	
      } else {

	// There's a fault: the log device is not recognized. Output
	// an error message, the log message, then abort the program.
	
	std::cerr << "Unsupported log device " << quote( logDev )
		  << ". Aborting."
		  << std::endl
		  << "Log message was: " << ss.str() << std::endl;
	abort();

      }
  }

  if( lvl <= LOG_CRIT )
    exit( -1 );
  
}


