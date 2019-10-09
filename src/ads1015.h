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
 * Wording in this document, other than the copyright, utilizes
 * meaning defined in BCP-14.  
 *
 *
 * Notes: 
 *  1, Threshold is unimplemented.
 *  2, The "comp" modes are untested.
 *
 *
 * $Log: ads1015.h,v $
 * Revision 1.21  2019/10/07 03:15:14  root
 * Minor const changes.
 *
 * Revision 1.20  2019/10/07 02:54:33  root
 * Converted types from u_short to uint16_t because I cannot guarantee
 * u_short as 16 bits, though it usually is.
 *
 * Revision 1.19  2019/09/29 03:56:17  root
 * Various syntax and text updates.
 *
 * Revision 1.18  2019/09/28 02:30:46  root
 * Changed some const decls to constexpr.
 *
 * Revision 1.17  2019/09/28 02:19:06  root
 * Moved some static variables to inline.
 *
 * Revision 1.16  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.15  2019/09/16 23:19:01  root
 * Harmonized (code-wise) with other objects.
 *
 * Revision 1.14  2019/09/15 05:08:34  root
 * Added comments and simplified code.
 *
 * Revision 1.13  2019/09/14 05:21:29  root
 * Removed erroneous enums from AINn. Fixed log messages. Added
 * ostream functions for enums.
 *
 * Revision 1.12  2019/09/07 07:09:14  root
 * Added forgotten defs. Added reset function, which does
 * nothing. Simplified log statements. Modified read/write to use base
 * class functions.
 *
 * Revision 1.11  2019/09/04 05:24:14  root
 * Moved the enums definition into the class, where they should have
 * been. Added more self test cases.
 *
 * Revision 1.10  2019/09/04 04:23:47  root
 * Added MIT copyright notice.
 * Revision 1.9 2019/08/31 08:18:49
 * root Fixed conversion related bugs.
 *
 * Revision 1.8  2019/08/31 05:59:47  root
 * "Max/min voltage is gone - it's the user's problem. Over voltage
 * flags are gone, too. Added additional data structure tests. Can
 * read voltages but more debugging needed. "Mux" is now private, as
 * it should be. Bit masks were inverted. :(
 *
 * Revision 1.7  2019/08/29 07:06:40  root
 * Got rid of the SMBUS code - it doesn't work.
 * Added more functions to the ads1015.
 * Code now freaking works.
 *
 * Revision 1.6  2019/08/28 07:57:21  root
 * Daily check-in. Things improving. Kernel trap gone.
 *
 * Revision 1.5  2019/08/28 04:59:38  root
 * Discovered I was using the wrong function to write to i2c which was
 * causing the kernel to trap.
 *
 * Revision 1.4  2019/08/27 06:38:57  root
 * Templatized simple functions.
 *
 * Revision 1.3  2019/08/27 02:41:34  root
 * Does anyone know why I named the dev file with a capital D? Stupid.
 *
 * Revision 1.2  2019/08/26 19:37:49  root
 * Saving work in-progress.
 *
 * Revision 1.1  2019/08/24 23:50:23  root
 * Initial revision
 *
 */

#ifndef __ADS1015_H__
#define __ADS1015_H__

extern "C" {

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
  
#include <linux/i2c-dev.h>
  
}

#include <iostream>
#include <memory>
#include <map>
#include <mutex>
#include <ostream>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>

#include "dev.h"
#include "log.h"
#include "util.h"


#define _ADS1015_H_ID "$Id: ads1015.h,v 1.21 2019/10/07 03:15:14 root Exp $"


namespace i2c {

  class ads1015 : public Dev {

  public:

    // The following definitions are from the Texas Instruments
    // ADS1013/4/5 document "Ultra-Small, Low-Power, 12-bit
    // Analog-to-Digital Converter with Internal Reference."
    // SBAS473C - MAY 2009 - REVISED OCTOBER 2009
    //
    // "_MASK" is simply a bit masking entry and should be ignored.
    
    enum class OS          { _NONE, BEGIN, CONVERSION_READY, _MASK };
    enum class AINp        { _NONE, AIN0, AIN1, AIN2, AIN3, _MASK };
    enum class AINn        { _NONE, AIN1, AIN3, GND, _MASK };
    enum class PGA_GAIN    { _NONE, FS_6144, FS_4096, FS_2048,
			     FS_1024, FS_512, FS_256, _MASK };
    enum class MODE        { _NONE, CONTINUOUS, SINGLE_SHOT, _MASK };
    enum class SAMPLE_RATE { _NONE, SR_128, SR_250, SR_490, SR_920,
                             SR_1600, SR_2400, SR_3300, _MASK };
    enum class COMP_MODE   { _NONE, TRADITIONAL, WINDOW, _MASK };
    enum class COMP_POL    { _NONE, ACTIVE_LOW, ACTIVE_HIGH, _MASK };
    enum class COMP_LAT    { _NONE, NON_LATCHING, LATCHING, _MASK };
    enum class COMP_QUE    { _NONE, ONE, TWO, FOUR, DISABLE, _MASK };

  private:

    // These data structures are to map the previous enums to
    // configuration register bits.
    //
    // Ok, what is going on here? Well, it's a little nuts but it is
    // also typed. Rather that parade bits around, strong typing
    // through enums is used. Yes, bits are simple and produce tighter
    // code and this code is rather expressive. However, this code is
    // more strongly typed. You lose something, you gain something.
    //
    // Shrug.
    
    static const std::map< OS,                 uint16_t > OS_MAP;
    static const std::map< std::tuple< AINp,
				       AINn >, uint16_t > MUX_MAP;
    static const std::map< PGA_GAIN,           uint16_t > PGA_GAIN_MAP;
    static const std::map< MODE,               uint16_t > MODE_MAP;
    static const std::map< SAMPLE_RATE,        uint16_t > SAMPLE_RATE_MAP;
    static const std::map< COMP_MODE,          uint16_t > COMP_MODE_MAP;
    static const std::map< COMP_POL,           uint16_t > COMP_POL_MAP;
    static const std::map< COMP_LAT,           uint16_t > COMP_LAT_MAP;
    static const std::map< COMP_QUE,           uint16_t > COMP_QUE_MAP;

    // Default configuration:
    //                        Begin conversion(s),
    //   MUX:                 AINp=AIN0, AINn=AIN1
    //   PGA Gain:            FS_2048
    //   Operational Mode:    Single shot/Power down
    //   Sample Rate:         1600 SPS
    //   Comparator mode:     Traditional
    //   Comparator polarity: Active low
    //   Latching comparator: non-latching
    //   Comparator queue:    Disable
    
    inline static constexpr uint16_t default_config = 0x8583;

    // Default i2c address of the converter.
    
    inline static constexpr int16_t default_addr = 0x49;

    // The addresses of the registers.

    inline static constexpr uint8_t       REG_CONV = uint8_t( 0b00 ),
                                           REG_CFG = uint8_t( 0b01 ),
                                     REG_LO_THRESH = uint8_t( 0b10 ),
                                     REG_HI_THRESH = uint8_t( 0b11 );

    // Mirrored configuration register.
    
    uint16_t myConfigReg;
    
    // Check certain data structures for consistency and assert() if
    // something doesn't make sense.
    
    void _check( void ) noexcept;

    // Write to the configuration register and read from it.
    
    int32_t _write_cfg( void ) const noexcept;
    int32_t _read_cfg(  void ) const noexcept;

    // Set/get the multiplexer. The multiplexer is used to select
    // which A/D value is read.

    const std::tuple< AINp, AINn> _mux( void ) const noexcept;
    const std::tuple< AINp, AINn> _mux( AINp p, AINn n );

    // This template is used in the simple functions that read the
    // configuration register and return a bit sequence mapped to an
    // enumerated type.
    
    template< typename E >
    inline
    E _reg_read_map_cfg( const std::map< E, uint16_t>& m ) const noexcept {
      
      E       e_type = E::_NONE;
      int32_t reg    = _read_cfg();
      
      if( reg < 0 ) {
	
	_LOG_WARN(( _id( "Failed to read config register." )));
	
      } else {
	
	const uint16_t mask = m.at( E::_MASK );

	for( const auto& i : m )
	  if(( i.first != E::_NONE ) && ( i.first != E::_MASK ))
	    if(( reg & mask ) == i.second ) {
	      e_type = i.first;
	      break;
	    }

	// Note: A E-type of none is always possible for bits
	//       indicating a process, such as whether a conversion
	//       is complete or not. In other cases, bits are fairly
	//       static where "none" is not a possibility. Therefore,
	//       this point of the code is slightly flawed because it
	//       doesn't know the difference between static and
	//       dynamic bits and therefore cannot test whether "none"
	//       is an impossibility.
	
      }
      
      return e_type;
    }

    // Similar to the last template function, this template is used as
    // the bulk of the simple functions that write an enumerated type
    // mapped to a bit sequence to the configuration register.
    
    template< typename E >
    inline
    int32_t _reg_write_map_cfg( const E& e, const std::map< E, uint16_t>& m ) {

      myConfigReg &= ~m.at( E::_MASK );

      for( const auto& i : m )
	if(( i.first != E::_NONE ) && ( i.first != E::_MASK ))
	    if( e == i.first ) {
	    myConfigReg |= i.second;
	    break;
	  }
      int32_t rVal = _write_cfg();

      if( rVal < 0 )
	_LOG_WARN(( _id( "Failed write to configuration register" ),
		    ", err=", rVal, errno2str( errno )));
      
      return rVal;
    }

    // The following templates are used in the _check() routine and
    // exist only when the code is compiled for debugging.

    template< typename E >
    inline
    void
    _check_masking( const std::map< E, uint16_t >& m ) {
#ifdef _DPG_DEBUG

      // The map size MUST always be greater than zero (i.e., there
      // MUST be something to map).
      
      assert( m.size());

      // Test:
      // 1, If I mask the bits against the mask's complement then
      //    there shouldn't be any extraneous bits set.
      // 2, If I mask the bits against the mask then the result MUST
      //    be the bits.
      
      const uint16_t mask = m.at( E::_MASK );
      for( const auto& i : m )
        if(( i.first != E::_NONE ) && ( i.first != E::_MASK )) {
          assert(( i.second & ~mask ) == 0 );
          assert(( i.second & mask ) == i.second );
        }

#endif
    }

    template< typename A, typename B >
    inline
    void _check_bit_overlap( const A& a, const B& b ) const {
#ifdef _DPG_DEBUG

      // Test:
      // 1, Two different function bit maps exist in the same register
      //    but the bits in each map MUST not overlap.

      for( auto& i : a )
        for( auto& j : b )
          assert(( i.second & j.second ) == 0 );
#endif
    }

    // This routine performs initialization of an objects and its
    // device in the constructor.
    
    virtual bool _doInit( void );

  public:

             ads1015( void );
    virtual ~ads1015( void );

    ads1015(                         const int16_t addr );
    ads1015( const std::string& bus, const int16_t addr );

    ads1015( const ads1015&  ad ) =  delete;
    ads1015(       ads1015&  ad );
    ads1015( const ads1015&& ad ) = delete;
    ads1015(       ads1015&& ad );

    ads1015& operator=( const ads1015&  ad ) = delete;
    ads1015& operator=(       ads1015&  ad );
    ads1015& operator=( const ads1015&& ad ) = delete;
    ads1015& operator=(       ads1015&& ad );

    bool operator==( const ads1015& d ) const noexcept;
    bool operator!=( const ads1015& d ) const noexcept;

    int reset( void );
    
    // Set/get the operational status.  Only the BEGIN bit can be set
    // and only the READY bit can be read.

    const OS os( void ) const noexcept;
    const OS os( const OS o );

    // Set/get the programable gain amplifier.
    
    const PGA_GAIN gain( void ) const noexcept;
    const PGA_GAIN gain( const PGA_GAIN g );

    const int i_gain( void ) const noexcept; // Alternative to using an enum
    const int i_gain( const int g );
    
    // Set/get the operating mode (continuous or single-shot).
    
    const MODE mode( void ) const noexcept;
    const MODE mode( const MODE m );

    // Set/get the sample rate.
    
    const SAMPLE_RATE rate( void ) const noexcept;
    const SAMPLE_RATE rate( const SAMPLE_RATE r );

    const int i_rate( void ) const noexcept; // Alternative to using an enum
    const int i_rate( const int r );

    // Set/get the comparator mode, polarity, latch, and queue.

    const COMP_MODE comp_mode( void ) const noexcept;
    const COMP_MODE comp_mode( const COMP_MODE m );

    const COMP_POL comp_pol( void ) const noexcept;
    const COMP_POL comp_pol( const COMP_POL p );

    const COMP_LAT comp_lat( void ) const noexcept;
    const COMP_LAT comp_lat( const COMP_LAT l );

    const COMP_QUE comp_que( void ) const noexcept;
    const COMP_QUE comp_que( const COMP_QUE q );

    // Get a signed floating point value of a conversion register. How
    // the analog inputs are connected are unknown. Therefore, both
    // differential and single input multiplex inputs are
    // supported. That said, often in I/IoT the inputs are single
    // ended.
    
    enum class CREG { DIFF01, // Differential between AIN0 and AIN1
		      DIFF03, // Differential between AIN0 and AIN3
		      DIFF13, // Differential between AIN1 and AIN3
		      DIFF23, // Differential between AIN2 and AIN3
		      CHAN0,  // Single ended, AIN0 (aka "Channel 0")
                      CHAN1,  // Single ended, AIN1 (aka "Channel 1")
		      CHAN2,  // Single ended, AIN2 (aka "Channel 2")
		      CHAN3   // Single ended, AIN3 (aka "Channel 3")
    };

    const float operator[]( const CREG& r );

  };
  
  
}  

// Helpful output operators.

std::ostream& operator<<( std::ostream& os, i2c::ads1015::OS o );
std::ostream& operator<<( std::ostream& os, i2c::ads1015::PGA_GAIN g );
std::ostream& operator<<( std::ostream& os, i2c::ads1015::MODE m );
std::ostream& operator<<( std::ostream& os, i2c::ads1015::SAMPLE_RATE r );
std::ostream& operator<<( std::ostream& os, i2c::ads1015::COMP_MODE m );
std::ostream& operator<<( std::ostream& os, i2c::ads1015::COMP_POL p );
std::ostream& operator<<( std::ostream& os, i2c::ads1015::COMP_LAT l );
std::ostream& operator<<( std::ostream& os, i2c::ads1015::COMP_QUE q );

std::ostream& operator<<( std::ostream& os,
			  std::tuple< i2c::ads1015::AINp,
			              i2c::ads1015::AINn >& m );


#endif


//  LocalWords:  enums Ok IoT AINn
