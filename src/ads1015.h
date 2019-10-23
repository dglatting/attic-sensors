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
 * Revision 1.24  2019/10/19 04:10:44  root
 * Minor text change.
 *
 * Revision 1.23  2019/10/19 01:03:11  root
 * First perversion to new code structure.
 *
 * Revision 1.22  2019/10/15 22:38:05  root
 * Integrated new "templates" header from another project.
 *
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

#include "i2c.h"
#include "log.h"
#include "util.h"


#define _ADS1015_H_ID "$Id: ads1015.h,v 1.24 2019/10/19 04:10:44 root Exp $"


class ads1015 : public i2c {

private:

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
  
  // This routine performs initialization of an objects and its device
  // in the constructor.
  
  bool _doInit( void ) noexcept;
  
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

  enum class OS : int { BEGIN = 1, CONVERSION_READY };

  const OS os( void ) const noexcept;
  const OS os( const OS o );
  
  // Set/get the programable gain amplifier.

  enum class PGA_GAIN : int { FS_6144 = 1, FS_4096, FS_2048,
			                   FS_1024, FS_512, FS_256 };

  const PGA_GAIN gain( void ) const noexcept;
  const PGA_GAIN gain( const PGA_GAIN g );
  
  const int i_gain( void ) const noexcept; // Alternative to using an enum
  const int i_gain( const int g );
  
  // Set/get the operating mode (continuous or single-shot).

  enum class MODE : int { CONTINUOUS = 1, SINGLE_SHOT };

  const MODE mode( void ) const noexcept;
  const MODE mode( const MODE m );
  
  // Set/get the sample rate.

  enum class SAMPLE_RATE : int { SR_128 = 1, SR_250, SR_490, SR_920,
				             SR_1600, SR_2400, SR_3300 };

  const SAMPLE_RATE rate( void ) const noexcept;
  const SAMPLE_RATE rate( const SAMPLE_RATE r );
  
  const int i_rate( void ) const noexcept; // Alternative to using an enum
  const int i_rate( const int r );
  
  // Set/get the comparator mode, polarity, latch, and queue.

  enum class COMP_MODE : int { TRADITIONAL = 1, WINDOW };

  const COMP_MODE comp_mode( void ) const noexcept;
  const COMP_MODE comp_mode( const COMP_MODE m );

  enum class COMP_POL :int { ACTIVE_LOW = 1, ACTIVE_HIGH };

  const COMP_POL comp_pol( void ) const noexcept;
  const COMP_POL comp_pol( const COMP_POL p );

  enum class COMP_LAT : int { NON_LATCHING = 1, LATCHING };

  const COMP_LAT comp_lat( void ) const noexcept;
  const COMP_LAT comp_lat( const COMP_LAT l );

  enum class COMP_QUE : int { ONE = 1, TWO, FOUR, DISABLE };

  const COMP_QUE comp_que( void ) const noexcept;
  const COMP_QUE comp_que( const COMP_QUE q );
  
  // Get a signed floating point value of a conversion register. How
  // the analog inputs are connected are unknown. Therefore, both
  // differential and single input multiplex inputs are
  // supported. That said, often in I/IoT the inputs are single ended.
    
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

private:

  enum class OS_x : int {
    _NONE = -1, _MASK = -2,
               BEGIN = int( OS::BEGIN ),
    CONVERSION_READY = int( OS::CONVERSION_READY )
  };
  inline static const std::map< OS_x, uint16_t > OS_MAP {
    { OS_x::BEGIN,                   0b1000000000000000 },
    { OS_x::CONVERSION_READY,        0b1000000000000000 },
    { OS_x::_MASK,                   0b1000000000000000 }
  };

  enum class PGA_GAIN_x : int {
    _NONE = -1, _MASK = -2,
    FS_6144 = int( PGA_GAIN::FS_6144 ),
    FS_4096 = int( PGA_GAIN::FS_4096 ),
    FS_2048 = int( PGA_GAIN::FS_2048 ),
    FS_1024 = int( PGA_GAIN::FS_1024 ),
    FS_512  = int( PGA_GAIN::FS_512  ),
    FS_256  = int( PGA_GAIN::FS_256  )
  };
  inline static const std::map< PGA_GAIN_x, uint16_t > PGA_GAIN_MAP {
    { PGA_GAIN_x::FS_6144,          0b0000000000000000 },
    { PGA_GAIN_x::FS_4096,          0b0000001000000000 },
    { PGA_GAIN_x::FS_2048,          0b0000010000000000 }, /* DEFAULT */
    { PGA_GAIN_x::FS_1024,          0b0000011000000000 },
    { PGA_GAIN_x::FS_512,           0b0000100000000000 },
    { PGA_GAIN_x::FS_256,           0b0000101000000000 },
    { PGA_GAIN_x::_MASK,            0b0000111000000000 }
  };
  
  enum class MODE_x : int {
    _NONE = -1, _MASK = -2,
     CONTINUOUS = int( MODE::CONTINUOUS  ),
    SINGLE_SHOT = int( MODE::SINGLE_SHOT )
  };
  inline static const std::map< MODE_x, uint16_t > MODE_MAP {
    { MODE_x::CONTINUOUS,           0b0000000000000000 },
    { MODE_x::SINGLE_SHOT,          0b0000000100000000 }, /* DEFAULT */
    { MODE_x::_MASK,                0b0000000100000000 }
  };

  enum class SAMPLE_RATE_x : int {
    _NONE = -1, _MASK = -2,
     SR_128 = int( SAMPLE_RATE::SR_128  ),
     SR_250 = int( SAMPLE_RATE::SR_250  ),
     SR_490 = int( SAMPLE_RATE::SR_490  ),
     SR_920 = int( SAMPLE_RATE::SR_920  ),
    SR_1600 = int( SAMPLE_RATE::SR_1600 ),
    SR_2400 = int( SAMPLE_RATE::SR_2400 ),
    SR_3300 = int( SAMPLE_RATE::SR_3300 )
  };
  inline static const std::map< SAMPLE_RATE_x, uint16_t > SAMPLE_RATE_MAP {
    { SAMPLE_RATE_x::SR_128,        0b0000000000000000 },
    { SAMPLE_RATE_x::SR_250,        0b0000000000100000 },
    { SAMPLE_RATE_x::SR_490,        0b0000000001000000 },
    { SAMPLE_RATE_x::SR_920,        0b0000000001100000 },
    { SAMPLE_RATE_x::SR_1600,       0b0000000010000000 }, /* DEFAULT */
    { SAMPLE_RATE_x::SR_2400,       0b0000000010100000 },
    { SAMPLE_RATE_x::SR_3300,       0b0000000011000000 },
    { SAMPLE_RATE_x::_MASK,         0b0000000011100000 }
  };

  enum class COMP_MODE_x : int {
    _NONE = -1, _MASK = -2,
    TRADITIONAL = int( COMP_MODE::TRADITIONAL ),
         WINDOW = int( COMP_MODE::WINDOW      )
  };
  inline static const std::map< COMP_MODE_x, uint16_t > COMP_MODE_MAP {
    { COMP_MODE_x::TRADITIONAL,     0b0000000000000000 }, /* DEFAULT */
    { COMP_MODE_x::WINDOW,          0b0000000000010000 },
    { COMP_MODE_x::_MASK,           0b0000000000010000 }
  };

  enum class COMP_POL_x :int {
    _NONE = -1, _MASK = -2,
    ACTIVE_LOW = int( ads1015::COMP_POL::ACTIVE_LOW  ),
    ACTIVE_HIGH = int( ads1015::COMP_POL::ACTIVE_HIGH )
  };
  inline static const std::map< COMP_POL_x, uint16_t > COMP_POL_MAP {
    { COMP_POL_x::ACTIVE_LOW,       0b0000000000000000 }, /* DEFAULT */
    { COMP_POL_x::ACTIVE_HIGH,      0b0000000000001000 },
    { COMP_POL_x::_MASK,            0b0000000000001000 }
  };

  enum class COMP_LAT_x : int {
    _NONE = -1, _MASK = -2,
    NON_LATCHING = int( COMP_LAT::NON_LATCHING ),
        LATCHING = int( COMP_LAT::LATCHING     )
  };
  inline static const std::map< COMP_LAT_x, uint16_t > COMP_LAT_MAP {
    { COMP_LAT_x::NON_LATCHING,     0b0000000000000000 }, /* DEFAULT */
    { COMP_LAT_x::LATCHING,         0b0000000000000100 },
    { COMP_LAT_x::_MASK,            0b0000000000000100 }
  };

  enum class COMP_QUE_x : int {
    _NONE = -1, _MASK = -2,
         ONE = int( COMP_QUE::ONE     ),
         TWO = int( COMP_QUE::TWO     ),
        FOUR = int( COMP_QUE::FOUR    ),
    DISABLE  = int( COMP_QUE::DISABLE )
  };
  inline static const std::map< COMP_QUE_x, uint16_t > COMP_QUE_MAP {
    { COMP_QUE_x::ONE,               0b0000000000000000 },
    { COMP_QUE_x::TWO,               0b0000000000000001 },
    { COMP_QUE_x::FOUR,              0b0000000000000010 },
    { COMP_QUE_x::DISABLE,           0b0000000000000011 }, /* DEFAULT */
    { COMP_QUE_x::_MASK,             0b0000000000000011 }
  };
 
  enum class AINp_x : int {
    _NONE = -1, _MASK = -2,
    AIN0 = 1, AIN1, AIN2, AIN3
  };
  enum class AINn_x : int {
    _NONE = -1, _MASK = -2,
    AIN1 = 1, AIN3, GND
  };
  inline static const
  std::map< std::tuple< AINp_x, AINn_x >, uint16_t > MUX_MAP {
    { { AINp_x::AIN0,  AINn_x::AIN1  }, 0b0000000000000000 }, /* DEFAULT */
    { { AINp_x::AIN0,  AINn_x::AIN3  }, 0b0001000000000000 },
    { { AINp_x::AIN1,  AINn_x::AIN3  }, 0b0010000000000000 },
    { { AINp_x::AIN2,  AINn_x::AIN3  }, 0b0011000000000000 },
    { { AINp_x::AIN0,  AINn_x::GND   }, 0b0100000000000000 },
    { { AINp_x::AIN1,  AINn_x::GND   }, 0b0101000000000000 },
    { { AINp_x::AIN2,  AINn_x::GND   }, 0b0110000000000000 },
    { { AINp_x::AIN3,  AINn_x::GND   }, 0b0111000000000000 },
    { { AINp_x::_MASK, AINn_x::_MASK }, 0b0111000000000000 }
  };

  inline static const std::map< PGA_GAIN_x, int > gain_xlate_f {
    { PGA_GAIN_x::FS_6144, 6144 },
    { PGA_GAIN_x::FS_4096, 4096 },
    { PGA_GAIN_x::FS_2048, 2048 },
    { PGA_GAIN_x::FS_1024, 1024 },
    { PGA_GAIN_x::FS_512,   512 },
    { PGA_GAIN_x::FS_256,   256 }
  };
  inline static const std::map< int, PGA_GAIN_x > gain_xlate_r {
    { 6144, PGA_GAIN_x::FS_6144 },
    { 4096, PGA_GAIN_x::FS_4096 },
    { 2048, PGA_GAIN_x::FS_2048 },
    { 1024, PGA_GAIN_x::FS_1024 },
    {  512, PGA_GAIN_x::FS_512  },
    {  256, PGA_GAIN_x::FS_256  }
  };

  inline static const std::map< SAMPLE_RATE_x, int > rate_xlate_f {
    { SAMPLE_RATE_x::SR_128,   128 },
    { SAMPLE_RATE_x::SR_250,   250 },
    { SAMPLE_RATE_x::SR_490,   490 },
    { SAMPLE_RATE_x::SR_920,   920 },
    { SAMPLE_RATE_x::SR_1600, 1600 },
    { SAMPLE_RATE_x::SR_2400, 2400 },
    { SAMPLE_RATE_x::SR_3300, 3300 }
  };
  inline static const std::map< int, SAMPLE_RATE_x > rate_xlate_r {
    {  128, SAMPLE_RATE_x::SR_128  },
    {  250, SAMPLE_RATE_x::SR_250  },
    {  490, SAMPLE_RATE_x::SR_490  },
    {  920, SAMPLE_RATE_x::SR_920  },
    { 1600, SAMPLE_RATE_x::SR_1600 },
    { 2400, SAMPLE_RATE_x::SR_2400 },
    { 3300, SAMPLE_RATE_x::SR_3300 }
  };

  // Set/get the multiplexer. The multiplexer is used to select which
  // A/D value is read.

  const std::tuple< AINp_x, AINn_x > _mux( void ) const noexcept;
  const std::tuple< AINp_x, AINn_x > _mux( AINp_x p, AINn_x n );

  // Helpful output operators.
  
  friend std::ostream& operator<<( std::ostream& os, OS o );
  friend std::ostream& operator<<( std::ostream& os, PGA_GAIN g );
  friend std::ostream& operator<<( std::ostream& os, MODE m );
  friend std::ostream& operator<<( std::ostream& os, SAMPLE_RATE r );
  friend std::ostream& operator<<( std::ostream& os, COMP_MODE m );
  friend std::ostream& operator<<( std::ostream& os, COMP_POL p );
  friend std::ostream& operator<<( std::ostream& os, COMP_LAT l );
  friend std::ostream& operator<<( std::ostream& os, COMP_QUE q );
  friend std::ostream& operator<<( std::ostream&,
				   const std::tuple< AINp_x, AINn_x >& );

};


std::ostream& operator<<( std::ostream& os, ads1015::OS o );
std::ostream& operator<<( std::ostream& os, ads1015::PGA_GAIN g );
std::ostream& operator<<( std::ostream& os, ads1015::MODE m );
std::ostream& operator<<( std::ostream& os, ads1015::SAMPLE_RATE r );
std::ostream& operator<<( std::ostream& os, ads1015::COMP_MODE m );
std::ostream& operator<<( std::ostream& os, ads1015::COMP_POL p );
std::ostream& operator<<( std::ostream& os, ads1015::COMP_LAT l );
std::ostream& operator<<( std::ostream& os, ads1015::COMP_QUE q );
std::ostream& operator<<( std::ostream& os,
                          const std::tuple< ads1015::AINp_x,
			                    ads1015::AINn_x >& m );


#endif


//  LocalWords:  enums Ok IoT AINn
