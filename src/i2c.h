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
 * $Log: i2c.h,v $
 * Revision 1.16  2019/10/22 05:47:21  root
 * Minor text adjustments.
 *
 * Revision 1.15  2019/10/20 04:38:43  root
 * Added a static_assert.
 *
 * Revision 1.14  2019/10/20 02:57:32  root
 * Deleted doInit() which was previously renamed but the old decl not
 * deleted. Moved _reset() to protected.
 *
 * Revision 1.13  2019/10/19 05:36:07  root
 * Moved the read/write reg routines into i2c.
 *
 * Revision 1.12  2019/10/15 07:41:23  root
 * Substantial improvements.
 *
 * Revision 1.11  2019/10/15 04:49:19  root
 * Deleted name space "i2c" and renamed class "Dev" to class "i2c" -
 * Dev didn't fit into any model when I started work on a SPI base
 * class.
 *
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

#ifndef __I2C_H__
#define __I2C_H__

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
#include <vector>

#include "log.h"
#include "util.h"


#define _I2C_H_ID "$Id: i2c.h,v 1.16 2019/10/22 05:47:21 root Exp $"


// This class is a base class for i2c devices that describes a few
// basic but critical functions.

class i2c {

private:
  
protected:
  
  // The file descriptor to the bus is shared and closed when the
  // final instance is destroyed. The use of a shared pointer negates
  // the use of const copy operations.
    
  std::shared_ptr< int > myFD;
    
  // The address of the devce on the bus and the bus. The address is
  // signed such that a negative number might indicate an error. The
  // bus is just a memo.
  
  int16_t     myAddr;
  std::string myBus;
  
  // _doInit() is explicitly called in constructors but whether the
  // device is already initialized or needs to be initialized is
  // device dependent. The base class merely checks whether the file
  // descriptor, address, and bus are set, implies a _find() was
  // previously run. 
  //
  // The base class doesn't know whether a device is actually on a bus
  // - no I/O is performed because the base class doesn't know if
  // reading from the device is a good idea or whether the device is
  // write-only. Therefore, it is expected that subclasses will do
  // something device specific.

  virtual bool _doInit( void ) noexcept;

  // Reset the device on the bus, if possible. This function may be a
  // NO-OP in subclasses. What does the return value mean? I dunno -
  // depends on the device but 0 means ok.
  //
  // This is meant to be an explicit function rather than an implicit
  // one, such as through a constructor, to avoid unintentional
  // resets.
  
  int _reset( void );

  // Search the i2c bus range and addresses on the bus to find the
  // address. Return true if found.
  
  virtual bool _find( const std::string& bus, int16_t addr ) noexcept;
  virtual bool _find(                         int16_t addr ) noexcept;
  virtual bool _find( void                                 ) noexcept;
  
  // Check certain data structures for consistency and assert() if
  // something doesn't make sense. In the base class that simply means
  // whether the device's bus and address is set as well as whether
  // the device is open()d. It is expected that subclasses will
  // perform more specific checking.

  virtual void _check( void ) noexcept;
  
  // Write length bytes out to the device or read length bytes from
  // device. Return the number of bytes read or written.
    
  size_t _write( const uint8_t* b, const size_t l ) const noexcept;
  size_t _write( const std::vector< uint8_t >& b  ) const noexcept;
  
  size_t _read( uint8_t* b, const size_t l                ) const noexcept;
  size_t _read( std::vector< uint8_t >& b, const size_t l ) const noexcept;

  // Utility implementations of _read()/_write() that read and write
  // an 8-bit value to a register. These are fairly common, simple
  // functions.
  //
  // _read_reg(): Write to a register and read back an 8-bit
  //              value. Returns the number of bytes read, which
  //              should always be 1.
  // _write_reg(): Write to a register followed with an 8-bit
  //               value. returns the number of bytes written, which
  //               should always be 2.
  
  ssize_t _read_reg(  const uint8_t reg,       uint8_t* buf ) const noexcept;
  ssize_t _write_reg( const uint8_t reg, const uint8_t* buf ) const noexcept;

  // A general form of _read_reg() except the return value isn't one
  // 8-bit register but some length of bytes.
  
  ssize_t _read_reg(  const uint8_t reg,
		      uint8_t* buf, size_t len ) const noexcept;

  // This is a silly little routine used in debug statements and
  // exists to reduce typing errors.
    
  const std::string _id( const char* m ) const noexcept;
  const std::string _id( void          ) const noexcept;

  // These are templates common across devices where a register is an
  // 8-bit entity contained within one 8-bit register. For wider or
  // partitioned registers, specialization SHOULD be implemented in
  // subclasses.
  //
  // These templates work off a map where the key is an enumeration
  // holding two predefined values in addition to specific
  // enumeration/bit maps. The enumeration and maps look like:
  //
  // enum class FILTER_x : int {
  //   _NONE = -1, _MASK = -2,
  //   OFF = 1, x2, x4, x8
  // }
  // static const std::map< FILTER_x, uint8_t > FILTER_MAP {
  //   { FILTER_x::OFF,   0b00000000 },
  //   { FILTER_x::x2,    0b00000100 },
  //   { FILTER_x::x4,    0b00001000 },
  //   { FILTER_x::x8,    0b00001100 },
  //   { FILTER_x::x16,   0b00010000 },
  //   { FILTER_x::_MASK, 0b00011100 }
  // }
  //
  // The enumeration maps to a register bit map where the enumeration
  // is used throughout the code rather than more numerous, and
  // therefore error prone, bit masking against bit sequences.
  
  template< typename K, typename V >
  inline
  K _read_map_8bit( const uint8_t           reg,
		    const std::map< K, V >&   m ) const noexcept {

    static_assert( sizeof( V ) == sizeof( uint8_t ));
    
    // The map size MUST always be greater than zero (i.e., there MUST
    // be something to map).
  
    assert( m.size());
    
    // The default (failure) return type.
    
    K e_type = K::_NONE;

    // Returned register value from the device.
    
    V regVal;

    static_assert( sizeof( regVal ) == sizeof( uint8_t ));
    
    // Fetch the device register's content.
    
    if( _read_reg( reg, &regVal, sizeof( regVal )) == sizeof( regVal )) {

      // Cache the bit mask.

      const V mask = m.at( K::_MASK );

      // Find something (other than NONE and MASK) that matches the
      // bits.
    
      for( const auto& [key,val] : m )
        if(( key != K::_NONE ) && ( key != K::_MASK ))
          if(( regVal & mask ) == val ) {
            e_type = key;
            break;
          }
    } 
    
    // "NONE" indicates whatever was sought isn't in the map and
    // therefore is a failure condition, since functions are strongly
    // typed, it's a private enumeration, and shouldn't be a possible
    // value.
    
    assert( e_type != K::_NONE );
    
    return e_type;
  }

  // This template is the write() side of the read map.
  //
  // Note: Errors are effectivly ignored.
  
  template< typename K, typename V >
  inline
  K _write_map_8bit( const uint8_t           reg,
		     const K                   k,
		     const std::map< K, V >&   m ) const noexcept {

    static_assert( sizeof( V ) == sizeof( uint8_t ));

    // Read the current register value.

    V       buf[] { 0x00 };
    ssize_t r_num = _read_reg( reg, buf );

    if( r_num == sizeof( buf )) {

      // Set/clear bits.
      
      buf[0] &= ~m.at( K::_MASK );
      buf[0] |=  m.at( k );

      // Write it to the device.
      
      ssize_t w_num = _write_reg( reg, buf );
      
      if( w_num != ( sizeof( buf ) + 1 ))
        _LOG_ERR(( _id( "Register write error" ),
                   "w_num=", w_num, " w=", _vtoa( buf, sizeof( buf )),
                   errno2str()));
      
    } else
      _LOG_ERR(( _id( "Register read error" ),
                 "r_num=", r_num, " r=", _vtoa( buf, sizeof( buf )),
                 errno2str()));
    
    return _read_map_8bit< K, V >( reg, m );
  }

  // These templates are very similar to the read/write map templates
  // except they work off a boolean setting.
  //
  // The problems with the prior templates include:
  //  1, The mask could be against a multi-bit field, and
  //  2, The way the _check() and other routines are written assume a
  //     zero field is a mapped value rather than one or more boolean
  //     values. Also, they have an assumption that a value within a
  //     field are unique values rather than a combination of values.
  //
  // Note: Errors are effectivly ignored.
  
  template< typename K, typename V >
  inline
  bool _read_bit_8bit( const uint8_t            reg,
		       const K&                 bitKey,
		       const std::map< K, V >&  m ) const noexcept {

    static_assert( sizeof( V ) == sizeof( uint8_t ));

    bool rVal = false;
    V    r_buf[] { 0x00 };
    
    if( _read_reg( reg, r_buf ) == sizeof( r_buf )) 
      if( r_buf[0] &  m.at( bitKey ))
        rVal = true;
    
    return rVal;
  }

  template< typename K, typename V >
  inline
  bool _write_bit_8bit( const uint8_t            reg,
			const K&                 bitKey,
			const bool               setting,
			const std::map< K, V >&  m ) const noexcept {

    static_assert( sizeof( V ) == sizeof( uint8_t ));
    
    V r_buf[] { 0x00 };

    if( _read_reg( reg, r_buf ) == sizeof( r_buf )) {

      // Set/clear the bit(s).
      
      r_buf[0] &= ~m.at( bitKey );
      if( setting )
        r_buf[0] |= m.at( bitKey );

      // Write it back to the device.
      
      ssize_t w_num = _write_reg( reg, r_buf );

      if( w_num != ( sizeof( r_buf ) + 1 ))
        _LOG_ERR(( _id( "Register write error" ),
                   "w_num=", w_num, " w=", _vtoa( r_buf, sizeof( r_buf )),
                   errno2str()));

    } else
      _LOG_ERR(( _id( "Register read error" ),
                 " r=", _vtoa( r_buf, sizeof( r_buf )),
                 errno2str()));
    
    return _read_bit_8bit< K, V >( reg, bitKey, m );
  }
    

public:
  
  i2c( void );
    
  i2c( const i2c&  d ) = delete;
  i2c(       i2c&  d );
  i2c( const i2c&& d ) = delete;
  i2c(       i2c&& d );
  
  i2c(                       int16_t addr = -1 );
  i2c( const std::string& s, int16_t addr = -1 );
  
  virtual ~i2c( void );
  
  i2c& operator=( const i2c&& d ) = delete;
  i2c& operator=(       i2c&& d ); 
  i2c& operator=( const i2c&  d ) = delete;
  i2c& operator=(       i2c&  d );
  
  bool operator==( const i2c& d ) const noexcept;
  bool operator!=( const i2c& d ) const noexcept;
  
  // Return the address, file descriptor, and bus.
  
        int16_t      addr( void ) const noexcept;
        int          fd(   void ) const noexcept;
  const std::string& bus( void  ) const noexcept;
  
};

inline
i2c::i2c( i2c& d )
  : myAddr( -1 ) {
  
  operator=( d );
  
}

inline
i2c::i2c( i2c&& d )
  : myAddr( -1 ) {
  
  operator=( d );
  
}

inline
int16_t
i2c::addr( void ) const noexcept {
  
  return myAddr;
}

inline
int
i2c::fd( void ) const noexcept {
  
  return myFD.get() ? *myFD : -1;
  
}

inline
const std::string&
i2c::bus( void ) const noexcept {

  return myBus;
}

inline
const std::string
i2c::_id( void ) const noexcept {

  return _id( "" );
}


inline
ssize_t
i2c::_read_reg(  const uint8_t reg, uint8_t* buf ) const noexcept {

  return _read_reg( reg, buf, sizeof( uint8_t ));
}


#endif


//  LocalWords:  Mutex mutex subclasses doInit
