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
 * $Log: i2c.cc,v $
 * Revision 1.22  2019/10/22 05:47:38  root
 * Minor text adjustments.
 *
 * Revision 1.21  2019/10/20 02:57:32  root
 * Deleted doInit() which was previously renamed but the old decl not
 * deleted. Moved _reset() to protected.
 *
 * Revision 1.20  2019/10/15 07:41:23  root
 * Substantial improvements.
 *
 * Revision 1.19  2019/10/15 04:49:19  root
 * Deleted name space "i2c" and renamed class "Dev" to class "i2c" -
 * Dev didn't fit into any model when I started work on a SPI base
 * class.
 *
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

#include <functional>
#include <iostream>
#include <memory>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include "i2c.h"
#include "log.h"
#include "util.h"


extern const std::vector< std::string > i2c_ident {
  _I2C_H_ID, "$Id: i2c.cc,v 1.22 2019/10/22 05:47:38 root Exp $"
};


// The prefix name of an i2c device. On the Raspberry PI, there is
// only one i2c bus. Therefore, once found, it is considered rather
// static. Consequently, this code has one bus name, one bus, and one
// file descriptor to the bus.
  
static const std::string i2c_fname = "/dev/i2c-";


i2c::i2c( int16_t a )
  : myAddr( a ) {

  _find( addr());
  _doInit();
}

i2c::i2c( const std::string& b, int16_t a )
  : myAddr( a ), myBus( b ) {
  
  _find( b, a );
  _doInit();
}

i2c::~i2c( void ) {
  
  myAddr = -1;
  myBus.clear();
  
}

i2c&
i2c::operator=( i2c& d ) {
  
  myFD   = d.myFD;
  myAddr = d.myAddr;
  myBus  = d.myBus;
  
  return *this;
}

i2c&
i2c::operator=( i2c&& d ) {
  
  myFD   = std::move( d.myFD );
  myAddr = d.myAddr;
  myBus  = std::move( d.myBus );
  
  d.myAddr = -1;
  
  return *this;
}

bool
i2c::_doInit( void ) noexcept {

  return myFD.get() && ( addr() >= 0 ) && bus().length();
}


bool
i2c::_find( void ) noexcept {

  return _find( addr());
}


bool
i2c::_find( int16_t a ) noexcept {

  assert( a >= 0 );
  
  bool found = false;
  
  for( u_int adapter = 0; adapter < 10; ++adapter ) {

    // Form the device file path.
    
    std::stringstream ss;
    
    ss << i2c_fname << adapter;

    // Reset the search variables.
    
    myAddr = a;
    myBus  = ss.str();

    // Try and find the device.
    
    found = _find( bus(), addr());
    
    if( found )
      break;
    
  }
  
  return found;
}


bool
i2c::_find( const std::string& s, int16_t a ) noexcept {
  
  assert( s.size());
  assert( a >= 0 );
  
  myAddr = a;
  myBus  = s;
  myFD.reset();

  bool retVal = false;

  if( int tFD = -1; ( tFD = ::open( bus().c_str(), O_RDWR )) >= 0 ) {
    
    myFD = std::shared_ptr< int >
      ( new int( tFD ),
	[](int* f) {
	  if( f ) {
	    if( *f >= 0 )
	      ::close( *f );
	    delete f;
	  }
	});
    assert( myFD.get());
    
    // Set up the slave.
    
    if( int err; (err = ::ioctl( fd(), I2C_SLAVE, myAddr )) >= 0 )
      retVal = true;
    else 
      _LOG_WARN(( _id( "Unable to acquire bus and/or talk to slave" ),
		  "err=", err, errno2str()));
      
  }

  // If nothing was found then remember nothing.
  
  if( retVal == false ) {
    
    myAddr = -1;
    myBus.clear();
    myFD.reset();
    
  }
  
  return retVal;
}


bool
i2c::operator==( const i2c& d ) const noexcept {
  
  bool rVal = false;
  
  if( addr() == d.addr()) {
    if( bus() == d.bus()) {
      if( myFD ) {
	if( d.myFD && ( *myFD == *d.myFD ))
	  rVal = true;
      } else
	if( ! d.myFD )
	  rVal = true;
    }
  }
  
  return rVal;      
}


bool
i2c::operator!=( const i2c& d ) const noexcept {
  
  return !operator==( d );
}


void
i2c::_check( void ) noexcept {

#ifdef _DPG_DEBUG
  assert( myFD.get() && ( fd() >= 0 ));
  assert(( addr() >= 0 )  && ( addr() < 0x78 ));
  assert( bus().length());

  _LOG_VERB(( "Data structures tests passed" ));
#endif
}


int
i2c::_reset( void ) {
  
  return 0;
}


size_t
i2c::_write( const std::vector< uint8_t >& b ) const noexcept {
  
  size_t rVal = 0;
  
  if( b.size()) {
    
    // Allocate memory to write out the vector's content
    
    std::unique_ptr< uint8_t, std::function< void( uint8_t* ) >>
      tBuf((uint8_t*)::calloc( b.size() + 8, sizeof( uint8_t )),
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
i2c::_write( const uint8_t* b, const size_t l ) const noexcept {

  size_t rVal = l;
  
  if( l ) {
    
    assert( b );
    
    ssize_t w_num = ::write( fd(), b, l );
    
    if( w_num != ssize_t( l )) {
      
      _LOG_WARN(( _id( "Write failure" ), "w_num=", w_num, ", vals: ",
		  _vtoa( b, l ), errno2str()));
      
      rVal = 0;
      
    }
  }
  
  _LOG_VERB(( _id(), "ret=", rVal, " w: ", _vtoa( b, l )));
  
  return rVal;
}


size_t
i2c::_read( std::vector< uint8_t >& b, const size_t l ) const noexcept {
  
  // Allocate memory to fetch data from the device.
  
  std::unique_ptr< uint8_t, std::function< void( uint8_t* )>>
    tBuf((uint8_t*)::calloc( l + 8, sizeof( uint8_t )),
	 []( uint8_t* p ) {
	   if( p )
	     ::free( p );
	 });
  assert( tBuf.get());
  
  // Read from the device.
  
  return _read( tBuf.get(), l );
}


size_t
i2c::_read( uint8_t* b, const size_t l ) const noexcept {
  
  size_t rVal = 0;
  
  if( l ) {
    
    assert( b );
    
    ssize_t r_num = ::read( fd(), b, l );
    
    if( r_num != ssize_t( l )) {
      
      _LOG_WARN(( _id( "Read failure" ), "r_num=", r_num, errno2str()));
      
    } else
      rVal = l;
    
  }
  
  _LOG_VERB(( _id(), " ret=", rVal, " r: ", _vtoa( b, l )));
  
  return rVal;
}


ssize_t
i2c::_read_reg(  const uint8_t reg,
		 uint8_t* buf, size_t len ) const noexcept {

  assert( buf && len );

  const uint8_t w_buf[] { reg };

  std::unique_ptr< uint8_t, std::function< void( uint8_t* )>>
    r_buf((uint8_t*)::calloc( len + 8, sizeof( uint8_t )),
	  []( uint8_t* p ) {
	    if( p )
	      ::free( p );
	  });
  assert( r_buf.get());

  ssize_t w_num = _write( w_buf, sizeof( w_buf ));
  ssize_t r_num = _read( r_buf.get(), len );

  _LOG_VERB(( _id(),
              "w=", w_num, " ", _vtoa( w_buf, sizeof( w_buf )),
              "r=", r_num, " ", _vtoa( r_buf.get(), len )));
  
  if( w_num != sizeof( w_buf )) {
    
    _LOG_WARN(( _id( "Unable to select register" ),
                "w_num=", w_num, "val=", _vtoa( w_buf, sizeof( w_buf )),
                errno2str()));
    
  } else
    if( r_num != ssize_t( len )) {

      _LOG_WARN(( _id( "Unable to read register" ),
                  "r_num=", r_num, errno2str()));
      
    } else {

      // Copy back the read data.
      
      for( size_t i = 0; i < len; ++i )
	buf[i] = r_buf.get()[i];
      
    }

  return ssize_t( r_num );
}


ssize_t
i2c::_write_reg( const uint8_t reg, const uint8_t* buf ) const noexcept {

  assert( buf );

  const uint8_t w_buf[] { reg, buf[0] };
  
  // Write the buffer's content to the device.
  
  ssize_t w_num = _write( w_buf, sizeof( w_buf ));
  ssize_t rVal = -1;

  _LOG_VERB(( _id(), "w=", w_num, " ", _vtoa( w_buf, sizeof( w_buf ))));

  if( w_num != sizeof( w_buf )) {
    
    _LOG_WARN(( _id( "Unable to write to register" ),
                "w_num=", w_num, ", val=",
                _vtoa( w_buf, sizeof( w_buf )), errno2str()));

  } else 
    rVal = w_num;
        
  return rVal;
}


const std::string
i2c::_id( const char* m ) const noexcept {
  
  std::stringstream s;
  
  if( m )
    s << m;
  
  s << " (bus=" << quote( bus()) << ", addr=0x" << t2hex( uint8_t( addr()))
    << ", f=" << fd() << ") ";
  
  return s.str();
}

//  LocalWords:  Mutex mutex
  
