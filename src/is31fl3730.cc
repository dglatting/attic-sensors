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
 * $Log: is31fl3730.cc,v $
 * Revision 1.15  2019/10/23 03:54:00  root
 * First pass at integrating other project changes.
 *
 * Revision 1.14  2019/10/15 22:38:05  root
 * Integrated new "templates" header from another project.
 *
 * Revision 1.13  2019/10/07 03:14:39  root
 * Minor const changes.
 *
 * Revision 1.12  2019/09/29 03:56:17  root
 * Various syntax and text updates.
 *
 * Revision 1.11  2019/09/28 02:19:06  root
 * Moved some static variables to inline.
 *
 * Revision 1.10  2019/09/27 07:41:57  root
 * Introduced "structured bindings." e=enum,i=int,f=float,b=bits,m=mask.
 *
 * Revision 1.9  2019/09/27 04:10:26  root
 * Cleaned up errno2str() calls.
 *
 * Revision 1.8  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.7  2019/09/22 06:28:35  root
 * Because I added addr(), I have to change the parameter names for
 * some functions. Duh.
 *
 * Revision 1.6  2019/09/21 09:10:30  root
 * Made use of utility functions.
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
 * Revision 1.2  2019/09/14 07:58:29  root
 * Work in progress.
 *
 * Revision 1.1  2019/09/12 04:42:11  root
 * Initial revision
 *
 */

extern "C" {

#include <assert.h>

}

#include <chrono>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include "is31fl3730.h"
#include "log.h"
#include "templates.h"
#include "util.h"


extern const std::vector< std::string > is31fl3730_ident {
  _IS31FL3730_H_ID, "$Id: is31fl3730.cc,v 1.15 2019/10/23 03:54:00 root Exp root $"
};


static const std::vector< uint8_t > zero_cols( IS31FL3720_MAX_COLS, 0 );

  
is31fl3730::is31fl3730( void )
  : i2c( default_addr ),
    myConfigReg( default_config ), myPwmReg( default_pwm ),
    myLightingEffectReg( default_lighting_effect ),
    myMatrix1ColumnRegisters( zero_cols ),
    myMatrix2ColumnRegisters( zero_cols ) {
  
  _doInit();
  _check();
}

  
is31fl3730::is31fl3730( const u_char ad )
  : i2c( ad ),
    myConfigReg( default_config ), myPwmReg( default_pwm ),
    myLightingEffectReg( default_lighting_effect ),
    myMatrix1ColumnRegisters( zero_cols ),
    myMatrix2ColumnRegisters( zero_cols ) {
  
  _doInit();
  _check();
}

  
is31fl3730::is31fl3730( const std::string& bus, const u_char ad )
  : i2c( bus, ad ),
    myConfigReg( default_config ), myPwmReg( default_pwm ),
    myLightingEffectReg( default_lighting_effect ),
    myMatrix1ColumnRegisters( zero_cols ),
    myMatrix2ColumnRegisters( zero_cols ) {
  
  _doInit();
  _check();
}


is31fl3730::~is31fl3730( void ) {}

  
bool
is31fl3730::_doInit( void ) noexcept {
  
  bool retVal = false;
  
  if( i2c::_doInit()) 
    retVal = reset();

  return retVal;
}


is31fl3730::is31fl3730( is31fl3730& ad )
  : i2c( ad ) {
  
  myConfigReg              = ad.myConfigReg;
  myPwmReg                 = ad.myPwmReg;
  myLightingEffectReg      = ad.myLightingEffectReg;
  myMatrix1ColumnRegisters = ad.myMatrix1ColumnRegisters;
  myMatrix2ColumnRegisters = ad.myMatrix2ColumnRegisters;
  
}


is31fl3730::is31fl3730( is31fl3730&& ad )
  : i2c( ad ) {

  myConfigReg              = ad.myConfigReg;
  myPwmReg                 = ad.myPwmReg;
  myLightingEffectReg      = ad.myLightingEffectReg;
  myMatrix1ColumnRegisters = ad.myMatrix1ColumnRegisters;
  myMatrix2ColumnRegisters = ad.myMatrix2ColumnRegisters;
  
  ad.myConfigReg         = default_config;
  ad.myPwmReg            = default_pwm;
  ad.myLightingEffectReg = default_lighting_effect;
  
  ad.myMatrix1ColumnRegisters = zero_cols;
  ad.myMatrix2ColumnRegisters = zero_cols;
  
}


is31fl3730&
is31fl3730::operator=( is31fl3730& ad ) {
  
  i2c::operator=( ad );
  
  myConfigReg              = ad.myConfigReg;
  myPwmReg                 = ad.myPwmReg;
  myLightingEffectReg      = ad.myLightingEffectReg;
  myMatrix1ColumnRegisters = ad.myMatrix1ColumnRegisters;
  myMatrix2ColumnRegisters = ad.myMatrix2ColumnRegisters;
  
  return *this;
}


is31fl3730&
is31fl3730::operator=( is31fl3730&& ad ) {
  
  i2c::operator=( ad );

  myConfigReg         = ad.myConfigReg;
  myPwmReg            = ad.myPwmReg;
  myLightingEffectReg = ad.myLightingEffectReg;
  
  ad.myConfigReg         = default_config;
  ad.myPwmReg            = default_pwm;
  ad.myLightingEffectReg = default_lighting_effect;
  
  ad.myMatrix1ColumnRegisters = zero_cols;
  ad.myMatrix2ColumnRegisters = zero_cols;
  
  return *this;
}


bool
is31fl3730::operator==( const is31fl3730& d ) const noexcept {
  
  bool retVal = false;
  
  if( i2c::operator==( d ))
    if( myConfigReg == d.myConfigReg )
      if( myPwmReg == d.myPwmReg )
	if( myLightingEffectReg == d.myLightingEffectReg )
	  if( myMatrix1ColumnRegisters == d.myMatrix1ColumnRegisters )
	    if( myMatrix2ColumnRegisters == d.myMatrix2ColumnRegisters )
	      retVal = true;
  
  return retVal;
}


bool
is31fl3730::operator!=( const is31fl3730& d ) const noexcept {
  
  return !operator==( d );
}
  

void
is31fl3730::_check( void ) noexcept {
  
#ifdef _DPG_DEBUG
  assert( zero_cols.size() == IS31FL3720_MAX_COLS );

  assert( audio_xlate_f.size() == audio_xlate_r.size());
  for( const auto& [k,v] : audio_xlate_f ) {
    assert( ag2i( k ) == v );
    assert( i2ag( v ) == k );
    assert( i2ag( ag2i( k )) == k  );
    assert( ag2i( i2ag( v )) == v );
  }
  
  assert( current_xlate_f.size() == current_xlate_r.size());
  for( const auto& [k,v] : current_xlate_f ) {
    assert( rc2i( k ) == v );
    assert( i2rc( v ) == k );
    assert( i2rc( rc2i( k )) == k );
    assert( rc2i( i2rc( v )) == v );
  }
  
  // Always true in the constructor.
  
  assert( myMatrix1ColumnRegisters.size() == IS31FL3720_MAX_COLS );
  assert( myMatrix2ColumnRegisters.size() == IS31FL3720_MAX_COLS );
  
  _check_masking< SOFTWARE_SHUTDOWN >( SSD_MAP         );
  _check_masking< DISPLAY_MODE      >( DISPLAY_MAP     );
  _check_masking< AUDIO_MODE        >( AUDIO_MODE_MAP  );
  _check_masking< MATRIX_MODE       >( MATRIX_MAP      );
  _check_masking< AUDIO_GAIN_MODE   >( AUDIO_GAIN_MAP  );
  _check_masking< ROW_CURRENT_MODE  >( ROW_CURRENT_MAP );
  
  // Configuration register.
  
  _check_bit_overlap< decltype( SSD_MAP        ),
		      decltype( DISPLAY_MAP    )>( SSD_MAP, DISPLAY_MAP );
  _check_bit_overlap< decltype( SSD_MAP        ),
		      decltype( AUDIO_MODE_MAP )>( SSD_MAP, AUDIO_MODE_MAP );
  _check_bit_overlap< decltype( SSD_MAP        ),
		      decltype( MATRIX_MAP     )>( SSD_MAP, MATRIX_MAP );
  
  _check_bit_overlap< decltype( DISPLAY_MAP    ),
		      decltype( AUDIO_MODE_MAP )>( DISPLAY_MAP,
						   AUDIO_MODE_MAP );
  _check_bit_overlap< decltype( DISPLAY_MAP    ),
		      decltype( AUDIO_MODE_MAP )>( DISPLAY_MAP,
						   AUDIO_MODE_MAP );
  _check_bit_overlap< decltype( DISPLAY_MAP    ),
		      decltype( MATRIX_MAP     )>( DISPLAY_MAP,
						   MATRIX_MAP );
  
  _check_bit_overlap< decltype( AUDIO_MODE_MAP ),
		      decltype( MATRIX_MAP     )>( AUDIO_MODE_MAP,
						   MATRIX_MAP );
  
  // Lighting effect register.
  
  _check_bit_overlap< decltype( AUDIO_GAIN_MAP ),
		      decltype( ROW_CURRENT_MAP )>( AUDIO_GAIN_MAP,
						    ROW_CURRENT_MAP );
  
  _LOG_VERB(( "Data structures bit map tests passed" ));
#endif
}


int
is31fl3730::reset ( void ) {
  
  int rVal = std::numeric_limits< int >::min();
  
  if( i2c::_reset() == 0 ) {

    constexpr uint8_t w_buf[] = { 0xff };
    
    if( _write( w_buf, sizeof( w_buf )) != sizeof( w_buf )) {
      
      _LOG_WARN(( _id( "Unable to RESET device" ), errno2str()));
      
    } else {
      
      myConfigReg         = default_config;
      myPwmReg            = default_pwm;
      myLightingEffectReg = default_lighting_effect;
      
      myMatrix1ColumnRegisters = zero_cols;
      myMatrix2ColumnRegisters = zero_cols;
      
      rVal = 0;
      
    }
    
    // The spec doesn't say if a delay is necessary, so delay for an
    // arbitrary number of milliseconds.
    
    std::this_thread::sleep_for( std::chrono::milliseconds( 2 ));
    
  }
    
  return rVal;
}


int32_t 
is31fl3730::_write_cfg( void ) const noexcept {

  const uint8_t w_buf[] = { 0x00, myConfigReg };

  int32_t  rVal = std::numeric_limits< int32_t >::min();
  ssize_t w_num = _write( w_buf, sizeof( w_buf ));
  
  if( w_num != sizeof( w_buf )) {
    
    _LOG_WARN(( _id( "Unable to write to config register" ),
		", w_num=", w_num, ", vals: ",
		_vtoa( w_buf, sizeof( w_buf )), errno2str()));
    
  } else
    rVal = int32_t( w_num );
  
  _LOG_VERB(( "fd=", fd(), " addr=0x", t2hex( addr()), " ",
	      "w=", w_num, " ", _vtoa( w_buf, sizeof( w_buf ))));
  
  return rVal;
}


int32_t
is31fl3730::_write_pwm( void ) const noexcept {
  
  const u_char w_buf[] = { 0x19, myPwmReg };
  
  int32_t  rVal = std::numeric_limits< int32_t >::min();
  ssize_t w_num = _write( w_buf, sizeof( w_buf ));
  
  if( w_num != sizeof( w_buf )) {
    
    _LOG_WARN(( _id( "Unable to write to PWM register" ),
		", w_num=", w_num, ", vals: ",
		_vtoa( w_buf, sizeof( w_buf )), errno2str()));
    
  } else
    rVal = int32_t( w_num );
  
  _LOG_VERB(( "fd=", fd(), " addr=0x", t2hex( addr()), " ",
	      "w=", w_num, " ", _vtoa( w_buf, sizeof( w_buf ))));
  
  return rVal;
}
  

int32_t
is31fl3730::_write_le( void ) const noexcept {
  
  const u_char w_buf[] = { 0x0d, myLightingEffectReg };
  
  int32_t  rVal = std::numeric_limits< int32_t >::min();
  ssize_t w_num = _write( w_buf, sizeof( w_buf ));
  
  if( w_num != sizeof( w_buf )) {
    
    _LOG_WARN(( _id( "Unable to write to lighting effect register" ),
		", w_num=", w_num, ", vals: ",
		_vtoa( w_buf, sizeof( w_buf )), errno2str()));
    
  } else
    rVal = int32_t( w_num );
  
  _LOG_VERB(( "fd=", fd(), " addr=0x", t2hex( addr()), " ",
	      "w=", w_num, " ", _vtoa( w_buf, sizeof( w_buf ))));
  
  return rVal;
}


int32_t
is31fl3730::_write_matrix( const uint8_t ad,
			   const std::vector< uint8_t >& regs ) const noexcept {

  assert( regs.size() <= IS31FL3720_MAX_COLS );
  
  const     size_t  wbuf1_size = 1 + regs.size();
  constexpr uint8_t   w_buf2[] = { 0x0c, 0x00 };
  
  int32_t rVal = std::numeric_limits< int32_t >::min();
  
  std::unique_ptr< uint8_t[] > w_buf1( new uint8_t[ wbuf1_size ]);
  assert( w_buf1.get());
  
  // Set up the buffer content.
  
  w_buf1.get()[ 0 ] = ad;
  for( size_t i = 0; i < regs.size(); ++i )
    w_buf1.get()[ i + 1 ] = regs[ i ];
  
  // Send out the matrix data and tell the display to update.
  
  ssize_t w_num1 = _write( w_buf1.get(), wbuf1_size );
  ssize_t w_num2 = _write( w_buf2, sizeof( w_buf2 ));
  
  if( w_num1 != ssize_t( wbuf1_size )) {
    
    _LOG_WARN(( _id( "Unable to write to matrix register" ),
		", w_num=", w_num1, ", vals: ",
		_vtoa( w_buf1.get(), wbuf1_size ), errno2str()));
    
  } else
    if( w_num2 != sizeof( w_buf2 )) {
      
      _LOG_WARN(( _id( "Unable to update matrix" ),
                  ", w_num=", w_num2, ", vals: ",
		  _vtoa( w_buf2, sizeof( w_buf2 )), errno2str()));
      
    } else
      rVal = int32_t( w_num1 );
  
  _LOG_VERB(( "fd=", fd(), " addr=0x", t2hex( addr()), " ",
	      "w=", w_num1, " ", _vtoa( w_buf1.get(), wbuf1_size )));
  _LOG_VERB(( "fd=", fd(), " addr=0x", t2hex( addr()), " ",
	      "w=", w_num2, " ", _vtoa( w_buf2, sizeof( w_buf2 ))));
  
  return rVal;
}

  
const bool
is31fl3730::display_on( void ) const noexcept {
  
  return !_read_bit_cfg< SOFTWARE_SHUTDOWN >
    ( SSD_MAP, SOFTWARE_SHUTDOWN::SHUTDOWN );
}


const bool
is31fl3730::display_on( const bool f ) noexcept {
  
  (void)_write_bit_cfg< SOFTWARE_SHUTDOWN >
    ( SSD_MAP, SOFTWARE_SHUTDOWN::SHUTDOWN, !f );
  
  return display_on();
}


const is31fl3730::DISPLAY
is31fl3730::display_mode( void ) const noexcept {
  
  return static_cast< DISPLAY >
    ( _read_map_cfg< DISPLAY_MODE >( DISPLAY_MAP ));
}

const is31fl3730::DISPLAY
is31fl3730::display_mode( const DISPLAY m ) noexcept {
  
  (void)_write_map_cfg< DISPLAY_MODE >
    ( static_cast< DISPLAY_MODE >( m ), DISPLAY_MAP );
  
  return display_mode();
}


const is31fl3730::MATRIX
  is31fl3730::matrix_mode( void ) const noexcept {
  
  return static_cast< MATRIX >
    ( _read_map_cfg< MATRIX_MODE >( MATRIX_MAP ));
}


const is31fl3730::MATRIX
is31fl3730::matrix_mode( const MATRIX m ) noexcept {
  
  (void)_write_map_cfg< MATRIX_MODE >
    ( static_cast< MATRIX_MODE >( m ), MATRIX_MAP );
  
  return matrix_mode();
}


const bool
is31fl3730::audio_enable( void ) const noexcept {
  
  return _read_bit_cfg< AUDIO_MODE >( AUDIO_MODE_MAP, AUDIO_MODE::ENABLE );
}


const bool
is31fl3730::audio_enable( const bool f ) noexcept {
  
  (void)_write_bit_cfg< AUDIO_MODE >
    ( AUDIO_MODE_MAP, AUDIO_MODE::ENABLE, f );
  
  return audio_enable();
}


const uint8_t
is31fl3730::pwm128( const bool f ) noexcept {
  
  if( myPwmReg & 0b10000000 ) {
    if( f )
      myPwmReg = 0b10000000;
    else
      myPwmReg &= ~0b10000000;
  } else
    if( f )
      myPwmReg = 0b10000000;
  
  (void)_write_pwm();
  
  return pwm();
}


const uint8_t
is31fl3730::pwm( const uint8_t p ) noexcept {
  
  myPwmReg = ( p & ~0b10000000 );
  
  (void)_write_pwm();
  
  return pwm();
}


const is31fl3730::AUDIO_GAIN
is31fl3730::audio_gain( void ) const noexcept {
  
  return static_cast< AUDIO_GAIN >
    ( _read_map_le( AUDIO_GAIN_MAP ));
}


const is31fl3730::AUDIO_GAIN
is31fl3730::audio_gain( const AUDIO_GAIN g ) noexcept {
  
  (void)_write_map_le< AUDIO_GAIN_MODE >
    ( static_cast< AUDIO_GAIN_MODE >( g ), AUDIO_GAIN_MAP );
  
  return audio_gain();
}


const is31fl3730::ROW_CURRENT
is31fl3730::row_current( void ) const noexcept {
  
  return static_cast< ROW_CURRENT >
    ( _read_map_le( ROW_CURRENT_MAP ));
}


const is31fl3730::ROW_CURRENT
is31fl3730::row_current( const ROW_CURRENT c ) noexcept {
  
  (void)_write_map_le< ROW_CURRENT_MODE >
    ( static_cast< ROW_CURRENT_MODE >( c ), ROW_CURRENT_MAP );
  
  return row_current();
}


const
std::vector< uint8_t >&
is31fl3730::matrix( const MATRIX_REG r ) const noexcept {
  
  if( r == MATRIX_REG::MATRIX1 )
    return myMatrix1ColumnRegisters;
  else
    if( r == MATRIX_REG::MATRIX2 )
      return myMatrix2ColumnRegisters;
    else
      _LOG_ERR(( _id( "Invalid matrix display selected" )));
  
  return zero_cols;
}


std::vector< uint8_t >&
is31fl3730::matrix( const MATRIX_REG r ) noexcept {
  
  static std::vector< uint8_t > fake( IS31FL3720_MAX_COLS, 0 );
  
  if( r == MATRIX_REG::MATRIX1 )
    return myMatrix1ColumnRegisters;
  else
    if( r == MATRIX_REG::MATRIX2 )
      return myMatrix2ColumnRegisters;
    else
      _LOG_ERR(( _id( "Invalid matrix display selected" )));
  
  return fake;
}


const int32_t
is31fl3730::update( void ) const noexcept {
  
  int rVal = std::numeric_limits< int32_t >::min();
  
  if(( rVal = update( MATRIX_REG::MATRIX1 )) >= 0 )
    rVal = update( MATRIX_REG::MATRIX2 );
  
  return rVal;
}


const int32_t
is31fl3730::update( const MATRIX_REG r ) const noexcept {
  
  int rVal = std::numeric_limits< int32_t >::min();
  
  if( r == MATRIX_REG::MATRIX1 )
    rVal = _write_matrix( 0x01, myMatrix1ColumnRegisters );
  else
    if( r == MATRIX_REG::MATRIX2 )
      rVal = _write_matrix( 0x0e, myMatrix2ColumnRegisters );
    else
      _LOG_ERR(( _id( "How did I get here?" )));
  
  return rVal;
}


const int
is31fl3730::rc2i( const ROW_CURRENT c ) const noexcept {

  decltype( current_xlate_f )::const_iterator it = current_xlate_f.find( c );

  assert( it != current_xlate_f.end());
  
  return it->second;
}


const is31fl3730::ROW_CURRENT
is31fl3730::i2rc( const int i ) const noexcept {

  decltype( current_xlate_r )::const_iterator it = current_xlate_r.find( i );

  assert( it != current_xlate_r.end());

  return it->second;
}


const int
is31fl3730::ag2i( const AUDIO_GAIN g ) const noexcept {

  decltype( audio_xlate_f )::const_iterator it = audio_xlate_f.find( g );

  assert( it != audio_xlate_f.end());
  
  return it->second;
}


const is31fl3730::AUDIO_GAIN
is31fl3730::i2ag( const int i ) const noexcept {

  decltype( audio_xlate_r )::const_iterator it = audio_xlate_r.find( i );

  assert( it != audio_xlate_r.end());

  return it->second;
}


std::ostream&
operator<<( std::ostream& os, const is31fl3730& i ) {

  os << "addr=0x" << t2Hex( i.addr())       << ", "
     << "fd=" << i.fd()                     << ", "               
     << "display_mode=" << i.display_mode() << ", "
     << "matrix_mode="  << i.matrix_mode()  << ", "
     << "audio_enable=" << i.audio_enable() << ", "
     << "audio_gain="   << i.audio_gain()   << ", "
     << "row_current="  << i.row_current()  << ", "
     << "pwm="          << u_int( i.pwm())  << ", "
     << "matrix1="
     << _vtoa( i.matrix(  is31fl3730::MATRIX_REG::MATRIX1 )) << ", "
     << "matrix2="
     << _vtoa( i.matrix(  is31fl3730::MATRIX_REG::MATRIX2 ));
    
  return os;
}


std::ostream&
operator<<( std::ostream& os, is31fl3730::MATRIX m ) {

  switch( m ) {
  case is31fl3730::MATRIX::ADM_8x8:  os << "8x8"; break;
  case is31fl3730::MATRIX::ADM_7x9:  os << "7x9"; break;
  case is31fl3730::MATRIX::ADM_6x10: os << "6x10"; break;
  case is31fl3730::MATRIX::ADM_5x11: os << "5x11"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


std::ostream&
operator<<( std::ostream& os, is31fl3730::DISPLAY d ) {

  switch( d ) { 
  case is31fl3730::DISPLAY::MATRIX1: os << "Matrix 1 only"; break;
  case is31fl3730::DISPLAY::MATRIX2: os << "Matrix 2 only"; break;
  case is31fl3730::DISPLAY::BOTH:    os << "Both"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


std::ostream&
operator<<( std::ostream& os, is31fl3730::AUDIO_GAIN g ) {

  switch( g ) {
  case is31fl3730::AUDIO_GAIN::dB_0:  os << "0dB"; break;
  case is31fl3730::AUDIO_GAIN::dB_3:  os << "3dB"; break;
  case is31fl3730::AUDIO_GAIN::dB_6:  os << "6dB"; break;
  case is31fl3730::AUDIO_GAIN::dB_9:  os << "9dB"; break;
  case is31fl3730::AUDIO_GAIN::dB_12: os << "12dB"; break;
  case is31fl3730::AUDIO_GAIN::dB_15: os << "15dB"; break;
  case is31fl3730::AUDIO_GAIN::dB_18: os << "18dB"; break;
  case is31fl3730::AUDIO_GAIN::dB_minus_6: os << "-6dB"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


std::ostream&
operator<<( std::ostream& os, is31fl3730::ROW_CURRENT c ) {

  switch( c ) {
  case is31fl3730::ROW_CURRENT::mA_40: os << "40mA"; break;
  case is31fl3730::ROW_CURRENT::mA_45: os << "45mA"; break;
  case is31fl3730::ROW_CURRENT::mA_75: os << "75mA"; break;
  case is31fl3730::ROW_CURRENT::mA_5:  os <<  "5mA"; break;
  case is31fl3730::ROW_CURRENT::mA_10: os << "10mA"; break;
  case is31fl3730::ROW_CURRENT::mA_35: os << "35mA"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


