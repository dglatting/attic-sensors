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
 * $Log: si7021.h,v $
 * Revision 1.8  2019/10/23 03:54:00  root
 * First pass at integrating other project changes.
 *
 * Revision 1.7  2019/09/29 03:56:17  root
 * Various syntax and text updates.
 *
 * Revision 1.6  2019/09/28 02:30:46  root
 * Changed some const decls to constexpr.
 *
 * Revision 1.5  2019/09/28 02:19:06  root
 * Moved some static variables to inline.
 *
 * Revision 1.4  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.3  2019/09/09 08:26:07  root
 * Massive debug/writing code work. Still working on t/h.
 *
 * Revision 1.2  2019/09/07 07:12:34  root
 * Work in progress.
 *
 * Revision 1.1  2019/09/04 05:53:01  root
 * Initial revision
 *
 */

#ifndef __SI7021_H__
#define __SI7021_H__

extern "C" {

#include <stdlib.h>
  
}

#include <string>
#include <tuple>
#include <vector>

#include "i2c.h"
#include "log.h"
#include "util.h"


#define _Si7021_H_ID "$Id: si7021.h,v 1.8 2019/10/23 03:54:00 root Exp root $"


class si7021 : public i2c {

private:
  
  // The default address of the device.
  
  inline static constexpr u_char default_addr = 0x40;
  
  // Check certain data structures for consistency and assert() if
  // something doesn't make sense.
  
  void _check( void ) noexcept;

  // This routine performs initialization of an objects and its device
  // in the constructor.
    
  virtual bool _doInit( void ) noexcept;
  
  // Return the control registers. Returns the number of bytes read. A
  // returned value of less than zero indicates an error.
  
  const int32_t _read_control_reg( uint8_t& reg ) const noexcept;
  const int32_t _read_heater_control_reg( uint8_t& reg ) const noexcept;
  
  // Write to the control register and return the current register
  // value.
  
  const int32_t _write_control_reg( uint8_t& reg ) const noexcept;
  const int32_t _write_heater_control_reg( uint8_t& reg ) const noexcept;
  
public:
  
           si7021( void );
  virtual ~si7021( void );

  si7021(                         const u_char addr );
  si7021( const std::string& bus, const u_char addr );
  
  si7021( const si7021&  ad ) =  delete;
  si7021(       si7021&  ad );
  si7021( const si7021&& ad ) = delete;
  si7021(       si7021&& ad );
  
  si7021& operator=( const si7021&  ad ) = delete;
  si7021& operator=(       si7021&  ad );
  si7021& operator=( const si7021&& ad ) = delete;
  si7021& operator=(       si7021&& ad );
  
  bool operator==( const si7021& d ) const noexcept;
  bool operator!=( const si7021& d ) const noexcept;
  
  int reset( void );
  
  // Return the 64-bit serial number as bytes in the vector. The
  // returned length is 64/8=8 bytes. If the length of the returned
  // vector is zero then an error occurred.
    
  const std::vector< uint8_t >   sn( void ) const noexcept;
  const std::string            s_sn( void ) const noexcept;
  
  // Return the firmware revision. Return either the byte from the
  // device or the version as a float. As always, a negative number
  // indicates an error.
  
  const uint8_t firmware( void ) const noexcept;
  const float f_firmware( void ) const noexcept;

  // Set or query the device's A/D resolution as defined in the
  // following table. These functions return a negative number on
  // error.
  //             RH   Temp
  //    0,0  12-bit 14-bit
  //    0,1   8-bit 12-bit
  //    1,0  10-bit 13-bit /* DEFAULT */
  //    1,1  11-bit 11-bit
  
  const std::tuple< int, int > resolution( void           ) const noexcept;
  const std::tuple< int, int > resolution( const int rh,
					   const int temp ) const noexcept;

  // Return the resolution in A/D bit size.
  
  const std::tuple< int, int > b_resolution( void ) const noexcept;
  
  
  // Turn the heater on or off and set the heater's level. Setting any
  // level implies turning the heater on.
  //
  //  0: 3.09 mA
  //  1: 9.18 mA
  //  2: 15.24 mA
  //  4: 27.39 mA
  //  8: 51.69 mA
  // 15: 94.20 mA
  //
  // When testing or setting the heater, a failure (falsely?)
  // indicates an off indication.
  
  const bool  heater( void             ) const noexcept; // Is it on?
  const bool  heater( const bool state ) const noexcept; // Turn on/off
  
  // Heater level in 0..15. A negative value indicates an error.
  
  const int   heater_level( void      ) const noexcept;
  const int   heater_level( const int ) const noexcept;
  
  // Heater level in mA. A negative value indicates an error.
  
  const float f_heater_level( void        ) const noexcept;
  const float   heater_level( const float ) const noexcept;
  
  // Device status. TRUE=Ok, FALSE=Low voltage.
  
  const bool status( void ) const noexcept;
  
  // Read the temperature and humidity.
  //
  // Note: This code was developed on a Raspberry PI and so I chose to
  // use clock stretching to allow the device to indicate when
  // conversion is completed rather than try to be efficient with the
  // bus (and over complicate) by adaptive delay between write/read
  // against conversion width.
    
  const float t( void ) const noexcept,
              h( void ) const noexcept;

  // Convert Celsius to Fahrenheit.
  
  const float c_to_f( const float c ) const noexcept;
  
};

inline
const float
si7021::c_to_f( const float c ) const noexcept {
  
  return ( c * ( 9.0 / 5.0 )) + 32.0;
}


#endif


//  LocalWords:  enums Ok IoT
