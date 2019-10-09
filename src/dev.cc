/* -*- c++ -*- */

/*
 * Copyright (c) 2019, Dennis Glatting.
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
 * $Log: dev.cc,v $
 * Revision 1.18  2019/09/29 03:56:17  root
 * Various syntax and text updates.
 *
 * Revision 1.17  2019/09/27 04:10:26  root
 * Cleaned up errno2str() calls.
 *
 * Revision 1.16  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.15  2019/09/21 09:09:39  root
 * Made use of new utility functions.
 *
 * Revision 1.14  2019/09/21 08:57:19  root
 * Deleted most virtual function specs. Added utility functions.
 *
 * Revision 1.13  2019/09/13 06:19:02  root
 * Changed the address type from "short" to __s16. Fixed some log
 * statements. Deleted _test_read() in find() because some devices are
 * write-only and a read fails. :(
 *
 * Revision 1.12  2019/09/09 08:27:23  root
 * Fixed several log statements.
 *
 * Revision 1.11  2019/09/07 07:07:51  root
 * Added a reset() function for subclassing. Moved read/write into a
 * common routine to simplify subclasses, including differing data
 * structures. Added silly helper function.
 *
 * Revision 1.10  2019/09/04 04:23:47  root
 * Added MIT copyright notice.
 *
 * Revision 1.9  2019/08/31 06:04:25  root
 * Fixed a debug message.
 *
 * Revision 1.8  2019/08/29 07:06:40  root
 * Got rid of the SMBUS code - it doesn't work.
 * Added more functions to the ads1015.
 * Code now freaking works.
 *
 * Revision 1.7  2019/08/28 07:57:21  root
 * Daily check-in. Things improving. Kernel trap gone.
 *
 * Revision 1.6  2019/08/28 04:59:38  root
 * Discovered I was using the wrong function to write to i2c which was
 * causing the kernel to trap.
 *
 * Revision 1.5  2019/08/27 07:09:13  root
 * It crashes the kernel, so that's nice.
 *
 * Revision 1.4  2019/08/27 02:41:34  root
 * Does anyone know why I named the dev file with a capital D? Stupid.
 *
 * Revision 1.3  2019/08/26 19:37:49  root
 * Saving work in-progress.
 *
 * Revision 1.2  2019/08/24 23:20:05  root
 * Test check-in.
 *
 * Revision 1.1  2019/08/24 07:58:14  root
 * Initial revision
 *
 */

extern "C" {

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
  
#include <linux/i2c-dev.h>
  
}

#include <iostream>
#include <memory>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include "dev.h"
#include "log.h"
#include "util.h"


extern const std::vector< std::string > i2c_ident {
  _I2CDEV_H_ID, "$Id: dev.cc,v 1.18 2019/09/29 03:56:17 root Exp $"
};


namespace i2c {
  
  // The prefix name of an i2c device. On the Raspberry PI, there is
  // only one i2c bus. Therefore, once found, it is considered rather
  // static. Consequently, this code has on one bus name, one bus, one
  // file descriptor to the bus, and one locking Mutex for bus
  // arbitration.
  
  static const std::string i2c_fname = "/dev/i2c-";
                       int i2c_fd = -1;
                std::mutex i2c_mtx;


  Dev::Dev( void )
    : myAddr( -1 ) {

    _find( myAddr );
  }

  Dev::Dev( int16_t addr )
    : myAddr( addr ) {

    _find( addr );
  }

  Dev::Dev( const std::string& s, int16_t addr )
    : myAddr( addr ) {

    _find( s, addr );
  }

  Dev::~Dev( void ) {
    
    myAddr = -1;
    
  }
  
  Dev&
  Dev::operator=( Dev& d ) {
    
    myFD      = d.myFD;
    myAddr    = d.myAddr;
    
    return *this;
  }
  
  Dev&
  Dev::operator=( Dev&& d ) {
    
    myFD      = std::move( d.myFD );
    myAddr    = d.myAddr;
    
    d.myAddr = -1;
    
    return *this;
  }
  
  bool
  Dev::_find( int16_t addr ) {
    
    myAddr = addr;
    myFD.reset();

    if( myAddr >= 0 ) 
      for( u_int adapter = 0; adapter < 10; ++adapter ) {
	
	std::stringstream s;
	
	s << i2c_fname << adapter;
	
	if( _find( s.str(), addr ))
	  break;
	
      }

    bool found = myFD ? true : false;
    
    _LOG_VERB(( "Device at addr 0x", t2hex( found ? myAddr : -1 ), 
		", ", ( found ? "" : "not " ), "found." ));
      
    return found;
  }

  bool
  Dev::_find( const std::string& s, int16_t addr ) {

    assert( s.size());

    bool retVal = false;
    
    myAddr = addr;
    myFD.reset();

    if( myAddr >= 0 ) 
      if( int fd = -1; ( fd = ::open( s.c_str(), O_RDWR )) >= 0 ) {
	
	std::shared_ptr< int >
	  tFD( new int( fd ),
	       [](int* fd) {
		 if( fd ) {
		   if( *fd >= 0 )
		     ::close( *fd );
		   delete fd;
		 }
	       });
	assert( tFD );

	myFD = tFD;
	
	// Set up the slave.
	
	if( int err; (err = ::ioctl( *myFD, I2C_SLAVE, myAddr )) < 0 ) {

	  _LOG_WARN(( "Unable to acquire bus access and/or talk to slave "
		      "(addr=0x", t2hex( myAddr ), ", fd=", *myFD, "), "
		      "err=", err, errno2str()));
									
	} else

	  // All is well.

	  retVal = true;
	
      }
    
    return ( myFD && *myFD >= 0 ) ? retVal : false;
  }
  
  bool
  Dev::operator==( const Dev& d ) const noexcept {

    bool rVal = false;

    if( myAddr == d.myAddr ) {
      if( myFD ) {
	if( d.myFD && ( *myFD == *d.myFD ))
	  rVal = true;
      } else
	if( ! d.myFD )
	  rVal = true;
    }
    
    return rVal;      
  }
  
  bool
  Dev::operator!=( const Dev& d ) const noexcept {
    
    return !operator==( d );
  }

  bool
  Dev::isInit( void ) const noexcept {

    bool rVal = false;

    if( myAddr != -1 ) 
      if( myFD && ( *myFD >= 0 ))
	if( _test_read())
	  rVal = true;
    
    return rVal;
  }

  bool
  Dev::doInit( void ) {

    bool rVal = false;
    
    if( addr() != -1 ) {
      if( myFD && ( *myFD >= 0 )) 
	rVal = true;
      else 
	rVal = _find( addr());
    }

    if( rVal == false )
      _LOG_WARN(( "Initialization of device addr=0x",
		  t2hex( addr()), ", fd=", fd(), "), ", " failed" ));
    
    return rVal;
  }


  bool
  Dev::_test_read( void ) const noexcept {

    u_char  r_buf[] = { 0x00 };
    ssize_t r_num = ::read(  *myFD, r_buf, sizeof( r_buf ));

    return ( r_num == sizeof( r_buf )) ? true : false;
  }

  int
  Dev::reset( void ) {

    return 0;
  }

  
  size_t
  Dev::_write( const std::vector< uint8_t >& b ) const noexcept {

    size_t rVal = 0;
    
    if( b.size()) {

      // Allocate memory to write out the vector's content
      
      std::shared_ptr< uint8_t >
	tBuf((uint8_t*)::malloc( b.size()),
	      []( uint8_t* p ) {
		if( p )
		  ::free( p );
	      });
      assert( tBuf.get());

      // copy the vector into memory. 

      for( size_t i = 0; i < b.size(); ++i )
	tBuf.get()[i] = b[i];

      // Write the vector to the device.
      
      rVal = _write( tBuf.get(), b.size());
      
    }
    
    return rVal;  
  }


  size_t
  Dev::_write( const uint8_t* b, const size_t l ) const noexcept {

    size_t rVal = l;

    if( l ) {
      
      assert( b );

      ssize_t w_num = ::write( fd(), b, l );

      if( w_num != ssize_t( l )) {

	_LOG_WARN(( "Failure to write to device (addr=0x",
		    t2hex( addr()), ", fd=", fd(),
		    "), w_num=", w_num, ", vals: ",
		    _vtoa( b, l ), errno2str()));

	rVal = 0;

      }
    }

    _LOG_VERB(( "fd=", fd(), " addr=0x", t2hex( addr()),
		" ret=", rVal, " w: ", _vtoa( b, l )));

    return rVal;
  }


  size_t
  Dev::_read( std::vector< uint8_t >& b, const size_t l ) const noexcept {

    // Allocate memory to fetch data from the device.

    std::shared_ptr< uint8_t >
      tBuf((uint8_t*)::malloc( l ),
	   []( uint8_t* p ) {
	     if( p )
	       ::free( p );
	   });
    assert( tBuf.get());

    // Read from the device.
    
    return _read( tBuf.get(), l );
  }


  size_t
  Dev::_read( uint8_t* b, const size_t l ) const noexcept {

    size_t rVal = 0;
    
    if( l ) {

      assert( b );
      
      ssize_t r_num = ::read( fd(), b, l );

      if( r_num != ssize_t( l )) {

        _LOG_WARN(( "Failure to read bytes from device (addr=0x",
                    t2hex( addr()), ", fd=", fd(), "), r_num=", r_num, errno2str()));

      } else
	rVal = l;

    }

    _LOG_VERB(( "fd=", fd(), " addr=0x", t2hex( addr()),
		" ret=", rVal, " r: ", _vtoa( b, l )));

    return rVal;
  }

  
  const std::string
  Dev::_id( const char* m ) const noexcept {

    std::stringstream s;

    if( m )
      s << m;

    s << " (addr=0x" << t2hex( addr())
      << ", f=" << fd() << ") ";
      
    return s.str();
  }

}

  //  LocalWords:  Mutex mutex
  
