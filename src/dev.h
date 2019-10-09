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
 * $Log: dev.h,v $
 * Revision 1.10  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.9  2019/09/21 08:57:19  root
 * Deleted most virtual function specs. Added utility functions.
 *
 * Revision 1.8  2019/09/14 05:24:52  root
 * Changed decl for address.
 *
 * Revision 1.7  2019/09/07 07:05:49  root
 * Added a reset() function for subclassing. Moved read/write into a
 * common routine to simplify subclasses. Added silly helper function.
 *
 * Revision 1.6  2019/09/04 04:23:47  root
 * Added MIT copyright notice.
 *
 * Revision 1.5  2019/08/29 07:06:40  root
 * Got rid of the SMBUS code - it doesn't work.
 * Added more functions to the ads1015.
 * Code now freaking works.
 *
 * Revision 1.4  2019/08/28 07:57:21  root
 * Daily check-in. Things improving. Kernel trap gone.
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

#ifndef __I2CDEV_H__
#define __I2CDEV_H__

extern "C" {

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
  
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
  
}

#include <iostream>
#include <memory>
#include <map>
#include <mutex>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>


#define _I2CDEV_H_ID "$Id: dev.h,v 1.10 2019/09/26 04:48:02 root Exp $"


namespace i2c {
  
  // This class is a base class for i2c devices that describes a few
  // basic but critical functions.

  class Dev {

  private:
    
  protected:
    
    // The file descriptor to the bus is shared and closed when the
    // final instance is destroyed. The use of a shared pointer
    // negates the use of const copy operations.
    
    std::shared_ptr< int > myFD;
    
    // The address of the devce on the bus.
    
    int16_t myAddr;
    
    // Search the i2c bus range and addresses on the bus to find the
    // address. Return true if found.
    
    bool _find( const std::string& bus, int16_t addr );
    bool _find(                         int16_t addr );

    // Do a test read of the device to see if anyone is home.
    
    bool _test_read( void ) const noexcept;

    // Write length bytes out to the device or read length bytes from
    // device. Return the number of bytes read or written.
    
    size_t _write( const uint8_t* b, const size_t l ) const noexcept;
    size_t _write( const std::vector< uint8_t >& b ) const noexcept;

    size_t _read( uint8_t* b, const size_t l ) const noexcept;
    size_t _read( std::vector< uint8_t >& b, const size_t l ) const noexcept;

    // This is a silly little routine used in debug statements meant
    // to reduce typing errors.
    
    const std::string _id( const char* m ) const noexcept;
    
  public:
    
    Dev( void );
    
    Dev( const Dev&  d ) = delete;
    Dev(       Dev&  d );
    Dev( const Dev&& d ) = delete;
    Dev(       Dev&& d );
    
    Dev(                       int16_t addr = -1 );
    Dev( const std::string& s, int16_t addr = -1 );
    
    virtual ~Dev( void );
    
    Dev& operator=( const Dev&& d ) = delete;
    Dev& operator=(       Dev&& d ); 
    Dev& operator=( const Dev&  d ) = delete;
    Dev& operator=(       Dev&  d );
    
    
    bool operator==( const Dev& d ) const noexcept;
    bool operator!=( const Dev& d ) const noexcept;

    // doInit() is implicitly called in constructors but whether the
    // device is already initialized or needs to be initialized is
    // device dependent. The base class merely checks whether the
    // address and bus are set. If the address is set but the bus is
    // not set then doInit() will look for a bus and *assume* the
    // device is on the first bus found in /dev.
    //
    // The base class doesn't know whether a device is actually on a
    // bus - no I/O is performed because the base class doesn't know
    // if reading from the device is a good idea or whether the device
    // is write-only.
    
    bool doInit( void );

    // This method merely determines whether the device is initialized
    // to the best of its ability. It is meant to be overridden in a
    // subclass. Subclasses should first call the base class function.
    
    bool isInit( void ) const noexcept;

    // Reset the device on the bus, if possible. This function may be
    // a NO-OP in subclasses. What does the return value mean? I dunno
    // - depends on the device but 0 means ok.
    //
    // This is meant to be an explicit function rather than an
    // implicit one, such as through a constructor, to avoid
    // unintentional resets.
    
    int reset( void );

    // Return the address and file descriptor.
    
    int16_t addr( void ) const noexcept;
    int     fd(   void ) const noexcept;

  };

  inline
  Dev::Dev( Dev& d )
    : myAddr( -1 ) {

    operator=( d );

  }

  inline
  Dev::Dev( Dev&& d )
    : myAddr( -1 ) {
    
    operator=( d );
    
  }

  inline
  int16_t
  Dev::addr( void ) const noexcept {

    return myAddr;
  }
  
  inline
  int
  Dev::fd( void ) const noexcept {

    return myFD.get() ? *myFD : -1;

  }

  
}


#endif


//  LocalWords:  Mutex mutex subclasses
