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
 * $Log: is31fl3730.h,v $
 * Revision 1.10  2019/09/29 03:56:17  root
 * Various syntax and text updates.
 *
 * Revision 1.9  2019/09/28 02:30:46  root
 * Changed some const decls to constexpr.
 *
 * Revision 1.8  2019/09/28 02:19:06  root
 * Moved some static variables to inline.
 *
 * Revision 1.7  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.6  2019/09/22 06:27:25  root
 * Because I added addr(), I have to change the parameter names for
 * some functions. Duh.
 *
 * Revision 1.5  2019/09/21 08:56:43  root
 * Added an ostream utility function.
 *
 * Revision 1.4  2019/09/16 23:17:43  root
 * Works but comments need a little work.
 *
 * Revision 1.3  2019/09/15 05:08:34  root
 * Added comments and simplified code.
 *
 * Revision 1.2 2019/09/14 07:58:29 root Work
 * in progress.
 *
 * Revision 1.1  2019/09/11 05:54:05  root
 * Initial revision
 *
 */

#ifndef __IS31FL3730_H__
#define __IS31FL3730_H__

extern "C" {

#include <stdlib.h>
  
}

#include <ostream>
#include <string>
#include <vector>

#include "dev.h"
#include "log.h"
#include "util.h"


#define _IS31FL3730_H_ID "$Id: is31fl3730.h,v 1.10 2019/09/29 03:56:17 root Exp $"


namespace i2c { class is31fl3730; }
std::ostream& operator<<( std::ostream& os, const i2c::is31fl3730& i );       


namespace i2c {

  // This class is based on the document:
  //  Audio Modulated Matrix LED Driver
  //  IS31FL3730, ISSI May 2018
  //
  // Every class derived from Dev is a little awkward. The major
  // difference of this subclass is the device is a write-only
  // device. Therefore, there is little recourse to know whether the
  // device is there or functioning other than open() and write()
  // failures.
    
  class is31fl3730 : public Dev {

  private:

    // Various register and other defaults that are defined in the
    // spec.
    
    inline static constexpr u_char  default_addr            = 0x60;
    inline static constexpr uint8_t default_config          = 0x00;
    inline static constexpr uint8_t default_pwm             = 0x80;
    inline static constexpr uint8_t default_lighting_effect = 0x00;
    
    // The device is a write-only device. Here is where I cache
    // various registers.
        
    uint8_t myConfigReg;
    uint8_t myPwmReg;
    uint8_t myLightingEffectReg;

    // The data registers of each matrix display. They are fixed
    // length of 11.
    //
    // The way this works is each bit in a byte represents a row bit
    // and each index into the vector represents a column. Therefore,
    // the maximum is 8 rows by 11 columns (8x11). This code DOES NOT
    // manage row/column content, rather it just spits it to the
    // device.

#define IS31FL3720_MAX_COLS 11
    
    std::vector< uint8_t > myMatrix1ColumnRegisters,
                           myMatrix2ColumnRegisters;
    
    // Check certain data structures for consistency and assert() if
    // something doesn't make sense. The body of this function is
    // enabled/disabled at compile-time and ugly.
    //
    // This routine MAY modify the object's register but restores them
    // when finished, if possible.
    
    void _check( void ) noexcept;

    // This routine performs initialization of an instance and its
    // device in the constructors.
    
    virtual bool _doInit( void );

    // Write to the configuration, PWM, and lighting effect registers.
    
    int32_t _write_cfg( void ) const noexcept;
    int32_t _write_pwm( void ) const noexcept;
    int32_t _write_le( void ) const noexcept;

    // Write out the matrix data starting at addr.
    
    int32_t _write_matrix( const uint8_t ad,
			   const std::vector< uint8_t >& regs ) const noexcept;
    
  public:

    // Be careful with the copy constructors and some operators - they
    // may not be implemented. They should be implemented, but I may
    // have forgotten to write that code.
    //
    // As noted in class Dev, copy constructors MAY implicitly modify
    // the instance being copied because of the presence of a
    // std::shared_ptr.
    
             is31fl3730( void );
    virtual ~is31fl3730( void );

    is31fl3730(                         const u_char ad );
    is31fl3730( const std::string& bus, const u_char ad );

    is31fl3730( const is31fl3730&  ad ) =  delete;
    is31fl3730(       is31fl3730&  ad );
    is31fl3730( const is31fl3730&& ad ) = delete;
    is31fl3730(       is31fl3730&& ad );

    is31fl3730& operator=( const is31fl3730&  ad ) = delete;
    is31fl3730& operator=(       is31fl3730&  ad );
    is31fl3730& operator=( const is31fl3730&& ad ) = delete;
    is31fl3730& operator=(       is31fl3730&& ad );

    bool operator==( const is31fl3730& d ) const noexcept;
    bool operator!=( const is31fl3730& d ) const noexcept;

    int reset( void );

    // The configuration register.

    // Turn the display on/off. This is the "shutdown" bit.
    
    const bool display_on( void ) const noexcept;
    const bool display_on( const bool ) noexcept;

    // Set the display mode, which is one of the two supported matrix
    // displays or both of them. These are the "display mode" bits.
    
    enum class DISPLAY : int { MATRIX1 = 1, MATRIX2 = 2, BOTH = 3 };

    const DISPLAY display_mode( void ) const noexcept;
    const DISPLAY display_mode( const DISPLAY ) noexcept;

    // Set the matrix mode selection, which is the row/column
    // arrangement. These are the "matrix mode" bits.
    
    enum class MATRIX : int { ADM_8x8 = 1, ADM_7x9 = 2,
			      ADM_6x10 = 3, ADM_5x11 = 4 };

    const MATRIX matrix_mode( void ) const noexcept;
    const MATRIX matrix_mode( const MATRIX ) noexcept;

    // Set whether the display intensity is modulated by the audio
    // input. The default is the intensity is controlled by the
    // "current" setting in the lighting effect register. This is the
    // "audio input enable" bit.
    
    const bool audio_enable( void ) const noexcept;
    const bool audio_enable( const bool ) noexcept;

    // The PWM register.

    // There are two PWM settings. The first is "128" and the second
    // 0..127. The PWM setting is from 0..128. Anything above 128 is
    // reinterpreted as 128.
    
    const uint8_t pwm( void ) const noexcept;
    const uint8_t pwm128( const bool ) noexcept;
    const uint8_t pwm( const uint8_t ) noexcept;

    // The lighting effect register.

    // Set the audio input gain which, of course, is only meaningful
    // if audio modulation is enabled. These are the "audio input
    // gain" bits of the lighting effect register.
    //
    // Provision is made to read/write the gain in terms of an integer
    // in addition to an enumerated type to simplify interfaces such
    // as a dial.
    
    enum class AUDIO_GAIN : int {
      dB_0 = 1, dB_3 = 2, dB_6 = 3, dB_9 = 4, dB_12 = 5,
      dB_15 = 6, dB_18 = 7, dB_minus_6 = 8
    };

    const AUDIO_GAIN audio_gain( void ) const noexcept;
    const AUDIO_GAIN audio_gain( const AUDIO_GAIN ) noexcept;

    const int   i_audio_gain( void ) const noexcept;
    const int   i_audio_gain( const int ) noexcept;

    // Set the full row current for each row output.
    
    enum class ROW_CURRENT : int { mA_40, mA_45, mA_75, mA_5, mA_10, mA_35 };

    const ROW_CURRENT row_current( void ) const noexcept;
    const ROW_CURRENT row_current( const ROW_CURRENT ) noexcept;

    const int         i_row_current( void ) const noexcept;
    const int         i_row_current( const int ) noexcept;

    // Manipulate the data in a matrix display's registers and update
    // the display.
    //
    // The code returns a set of registers to be manipulated based on
    // which matrix display is indicated but the code does not know
    // anything about register content or array size. That said, you
    // can change the matrix size but it may get reset to the default
    // size, such as in copy constructors and move assignments.
    
    enum class MATRIX_REG { MATRIX1, MATRIX2 };
    
    const std::vector< uint8_t >& matrix( const MATRIX_REG ) const noexcept;
          std::vector< uint8_t >& matrix( const MATRIX_REG )       noexcept;

    // Update a particular matrix or update both.
    
    const int32_t update( const MATRIX_REG ) const noexcept;
    const int32_t update( void             ) const noexcept;

  private:

    // This is my second attempt at using enumerations for strong
    // function types while masking private elements, such as "_NONE."
    //
    // I'm explicitly declaring the base type as integer, which is the
    // default anyway, then perform cast equivalence between public
    // (above) and private enumerations - basically, compiler
    // hackery. Enumerations in C++ are very useful but also very
    // limited thereby conjuring all kinds of hacky tricks (peruse the
    // web and you'll see for yourself). A bit disappointing of the
    // data type, IMO.

    enum class SOFTWARE_SHUTDOWN { _NONE, SHUTDOWN, _MASK };
    static const std::map< SOFTWARE_SHUTDOWN, uint8_t > SSD_MAP;

    enum class DISPLAY_MODE : int {
      _NONE = -1, _MASK = -2,
      MATRIX1 = int( DISPLAY::MATRIX1 ),
      MATRIX2 = int( DISPLAY::MATRIX2 ),
      BOTH    = int( DISPLAY::BOTH )
    };
    static const std::map< DISPLAY_MODE, uint8_t > DISPLAY_MAP;             

    enum class MATRIX_MODE : int {
      _NONE = -1, _MASK = -2,
      ADM_8x8  = int( MATRIX::ADM_8x8 ),
      ADM_7x9  = int( MATRIX::ADM_7x9 ),
      ADM_6x10 = int( MATRIX::ADM_6x10 ),
      ADM_5x11 = int( MATRIX::ADM_5x11 ),
    };
    static const std::map< MATRIX_MODE, uint8_t > MATRIX_MAP;

    enum class AUDIO_MODE { _NONE, ENABLE, _MASK };
    static const std::map< AUDIO_MODE, uint8_t > AUDIO_MODE_MAP;

    enum class AUDIO_GAIN_MODE : int {
      _NONE = -1, _MASK = -2,
      dB_0  = int( AUDIO_GAIN::dB_0  ),
      dB_3  = int( AUDIO_GAIN::dB_3  ),
      dB_6  = int( AUDIO_GAIN::dB_6  ),
      dB_9  = int( AUDIO_GAIN::dB_9  ),
      dB_12 = int( AUDIO_GAIN::dB_12 ),
      dB_15 = int( AUDIO_GAIN::dB_15 ),
      dB_18 = int( AUDIO_GAIN::dB_18 ),
      dB_minus_6 = int( AUDIO_GAIN::dB_minus_6 )
    };
    static const std::map< AUDIO_GAIN_MODE, uint8_t > AUDIO_GAIN_MAP;

    enum class ROW_CURRENT_MODE : int {
      _NONE = -1, _MASK = -2,
      mA_40 = int( ROW_CURRENT::mA_40 ),
      mA_45 = int( ROW_CURRENT::mA_45 ),
      mA_75 = int( ROW_CURRENT::mA_75 ),
      mA_5  = int( ROW_CURRENT::mA_5  ),
      mA_10 = int( ROW_CURRENT::mA_10 ),
      mA_35 = int( ROW_CURRENT::mA_35 )
    };
    static const std::map< ROW_CURRENT_MODE, uint8_t > ROW_CURRENT_MAP;

    // The following two templates are used to read and write the
    // configuration and lighting effect registers to the device. The
    // goal, as part of the reasoning of templates, is to create
    // common code and let the compiler's optimizer clean things up as
    // specific to the template's usage.

    template< typename E >
    inline
    bool
    _read_bit_cfg( const std::map< E, uint8_t >& m, const E& b ) const noexcept {

      return ( myConfigReg & m.at( b )) ? true : false;
    }

    template< typename E >
    inline
    int32_t 
    _write_bit_cfg( const std::map< E, uint8_t >& m, const E& b, bool e ) {

      myConfigReg &= ~m.at( E::_MASK );

      if( e )
	myConfigReg |= m.at( b );

      int32_t rVal = _write_cfg();

      if( rVal < 0 )
        _LOG_WARN(( _id( "Unable to write to configuration register" ),
                    ", err=", rVal, errno2str( errno )));

      return rVal;
    }

    // The following read/write templates are an interface between the
    // device's register, the cached register (remember that the
    // device is write-only), and private data structures.
    
    template< typename E >
    inline
    E _read_map( const std::map< E, uint8_t >& m,
		 const uint8_t& reg ) const noexcept {

      // The default (failure) return type.
      
      E e_type = E::_NONE;

      // Cache the mask.
      
      const uint8_t mask = m.at( E::_MASK );

      // Find something (other than NONE and MASK) that matches the
      // bits.

      for( const auto& i : m )
        if(( i.first != E::_NONE ) && ( i.first != E::_MASK ))
          if(( reg & mask ) == i.second ) {
            e_type = i.first;
            break;
          }

      // "NONE" indicates whatever was sought isn't in the map and is,
      // therefore, a failure condition, since functions are strongly
      // typed, it's a private enumeration, and shouldn't be a
      // possible passed value.
      
      assert( e_type != E::_NONE );

      return e_type;
    }

    template< typename E >
    inline
    E _read_map_cfg( const std::map< E, uint8_t >& m ) const noexcept {

      return _read_map< E >( m, myConfigReg );
    }

    template< typename E >
    inline
    int32_t _write_map_cfg( const E& e, const std::map< E, uint8_t >& m ) {

      // Turn the bits off.
      
      myConfigReg &= ~m.at( E::_MASK );

      // Based on the enumeration, set bits on.

      typename std::map< E, uint8_t >::const_iterator it = m.find( e );

      if( it != m.end()) 
	myConfigReg |= it->second;

      int32_t rVal = _write_cfg();

      if( rVal < 0 )
        _LOG_WARN(( _id( "Unable to write to configuration register" ),
		    ", err=", rVal, errno2str( errno )));
      
      return rVal;
    }

    template< typename E >
    inline
    E _read_map_le( const std::map< E, uint8_t >& m ) const noexcept {
      
      return _read_map< E >( m, myLightingEffectReg );
    }

    template< typename E >
    inline
    int32_t _write_map_le( const E& e, const std::map< E, uint8_t >& m ) {

      // Turn the bits off.

      myLightingEffectReg &= ~m.at( E::_MASK );

      // Based on the enumeration, set bits on.
      
      typename std::map< E, uint8_t >::const_iterator it = m.find( e );

      if( it != m.end())
        myLightingEffectReg |= it->second;

      int32_t rVal = _write_le();

      if( rVal < 0 )
        _LOG_WARN(( _id( "Unable to write to lighting effect register" ),
		    ", err=", rVal, errno2str( errno )));
      
      return rVal;
    }

    // The following templates are used in the _check() routine and
    // exist only when the code is compiled for debugging.
    
    template< typename E >
    inline
    void
    _check_masking( const std::map< E, uint8_t >& m ) {
#ifdef _DPG_DEBUG

      // The map size MUST always be greater than zero (i.e., there
      // MUST be something to map).
      
      assert( m.size());

      // Test:
      // 1, If I mask the bits against the mask's complement then
      //    there shouldn't be any extraneous bits set.
      // 2, If I mask the bits against the mask then the result MUST
      //    be the bits.
      
      const u_short mask = m.at( E::_MASK );
      for( const auto& i : m )
	if(( i.first != E::_NONE ) && ( i.first != E::_MASK )) {
	  assert(( i.second & ~mask ) == 0 );
          assert(( i.second & mask ) == i.second );
	}

#endif
    }

    template< typename A, typename B>
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

    // Utility friends.

    friend std::ostream& ::operator<<( std::ostream&, const is31fl3730& ); 

  };
  
  inline
  const uint8_t
  is31fl3730::pwm( void ) const noexcept {
    
    return myPwmReg;
  }
    

}


// Helpful output operators.

std::ostream& operator<<( std::ostream& os, i2c::is31fl3730::MATRIX m );
std::ostream& operator<<( std::ostream& os, i2c::is31fl3730::DISPLAY d );
std::ostream& operator<<( std::ostream& os, i2c::is31fl3730::AUDIO_GAIN g );
std::ostream& operator<<( std::ostream& os, i2c::is31fl3730::ROW_CURRENT c );


#endif


//  LocalWords:  enums Ok IoT PWM Dev BCP hackery
