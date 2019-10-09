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
 * This file contains declarations related to options (i.e., command
 * line arguments) processing and global variable declarations.
 *
 *
 * $Log: opts.h,v $
 * Revision 1.16  2019/10/05 04:08:20  root
 * Added decision logic for foreground/background and logging.
 *
 * Revision 1.15  2019/10/01 05:48:49  root
 * Added daemon flag. Cleaned up Help message.
 *
 * Revision 1.14  2019/09/04 04:23:47  root
 * Added MIT copyright notice.
 *
 * Revision 1.13  2019/08/27 01:01:34  root
 * Adding additional utilities to source.
 *
 * Revision 1.12  2018/08/17 08:45:46  root
 * Added new client option to limit connections for debugging.
 *
 * Revision 1.11  2018/08/04 04:40:08  root
 * First integration of lazy key pre-debug.
 *
 * Revision 1.10  2018/07/21 06:47:01  root
 * Removed unneeded #include statements.
 *
 * Revision 1.9  2018/07/18 09:30:47  root
 * Too many changes to comment on.
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
 * Revision 1.7  2018/07/02 02:57:48  root
 * Client and server are now talking TCP/TLS although some more
 * debugging is necessary.
 *
 * Revision 1.6  2018/06/16 00:56:49  root
 * Added atfCYBER copyright.
 *
 * Revision 1.5  2018/06/15 07:23:19  root
 * The client and server now talk to each other.
 *
 * Revision 1.4  2018/06/12 05:48:41  root
 * Everything compiles and links. Now on to actual client/server testing.
 * Will enable selection between IPv6/IPv4 in future.
 * Will optimize threading in server in future.
 *
 * Revision 1.3  2018/06/11 03:49:32  root
 * Added threading to the client and expanded options.
 *
 * Revision 1.2  2018/06/06 01:53:45  root
 * The client code compiles and links. Archiving before debug.
 *
 * Revision 1.1  2018/06/05 18:14:31  root
 * Initial revision
 *
 */


#ifndef __OPTS_H__
#define __OPTS_H__

extern "C" {

#include <sys/types.h>
  
}

#include <string>
#include <vector>


#define _OPT_H_ID "$Id: opts.h,v 1.16 2019/10/05 04:08:20 root Exp $"


// Current level of debug.

extern int logging_level;

// Where to send the log messages (either "stdout" or "syslog" ).

extern std::string logDev;

// Whether or not to become a daemon. Default is TRUE.

extern bool doDaemon;

// The routine that parses the argc/argv options.

bool parse_opts( int , char**  );


// Obvious.

extern void usage( void );


#endif




//  LocalWords:  TCP TLS URI
