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
 * $Log: microdotphat.cc,v $
 * Revision 1.9  2019/09/29 03:56:17  root
 * Various syntax and text updates.
 *
 * Revision 1.8  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.7  2019/09/25 03:58:20  root
 * Added data structure for little numbers but the code in
 * unimplemented, and likely to remain so. Clear() no longer updates
 * the hardware - a show() is now required. All of the write_string()
 * functions are implemented.
 *
 * Revision 1.6  2019/09/24 07:59:37  root
 * Several of the scroll functions are now implemented. Function
 * return values changed where the functions returns were
 * meaningless. write_char() now implemented. Simplifications.
 *
 * Revision 1.5  2019/09/23 06:32:24  root
 * Added set/get row but as private functions. set_col() now returns
 * get_col() verses void. Added several private low level manipulation
 * routines. Simplified code based on new low level
 * functions. Expanded and enhanced _check().
 *
 * Revision 1.4  2019/09/23 01:47:28  root
 * Decimal point now works.
 *
 * Revision 1.3  2019/09/22 06:25:08  root
 * Okay, I screwed up - the origin of two matrices are different and
 * they are transposed. Nice.
 *
 * Revision 1.2  2019/09/21 09:13:38  root
 * Added utility functions. set_col()/set_pixel() seem to work now.
 *
 * Revision 1.1  2019/09/18 06:09:14  root
 * Initial revision
 *
 */

extern "C" {

#include <assert.h>

}

#include <chrono>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include "microdotphat.h"
#include "log.h"
#include "util.h"


extern const std::vector< std::string > microdotphat_ident {
  _MICRODOTPHAT_H_ID, "$Id: microdotphat.cc,v 1.9 2019/09/29 03:56:17 root Exp $"
};


// Save me some redundant typing (and some memory).

static const std::vector< uint8_t > zero {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static constexpr i2c::is31fl3730::MATRIX_REG
  m1 = i2c::is31fl3730::MATRIX_REG::MATRIX1,
  m2 = i2c::is31fl3730::MATRIX_REG::MATRIX2;

// Lifted from Pimoroni. The following is a copy of their license on
// github.
//
// https://github.com/pimoroni/microdot-phat/blob/master/LICENSE
//
// MIT License
// 
// Copyright (c) 2017 Pimoroni Ltd.
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// The index into this map is an ASCII character, such as an 'e'. The
// character maps to a vector of columns with the leftmost column the
// zero index into the vector.

static const std::map< const u_int,
		       const std::vector< uint8_t >> _font {
  { ' ',  { 0x00, 0x00, 0x00, 0x00, 0x00 }},
  { '!',  { 0x00, 0x00, 0x5f, 0x00, 0x00 }},
  { '"',  { 0x00, 0x07, 0x00, 0x07, 0x00 }},
  { '#',  { 0x14, 0x7f, 0x14, 0x7f, 0x14 }},
  { '$',  { 0x24, 0x2a, 0x7f, 0x2a, 0x12 }},
  { '%',  { 0x23, 0x13, 0x08, 0x64, 0x62 }},
  { '&',  { 0x36, 0x49, 0x55, 0x22, 0x50 }},
  { '\'', { 0x00, 0x05, 0x03, 0x00, 0x00 }},
  { '(',  { 0x00, 0x1c, 0x22, 0x41, 0x00 }},
  { ')',  { 0x00, 0x41, 0x22, 0x1c, 0x00 }},
  { '*',  { 0x08, 0x2a, 0x1c, 0x2a, 0x08 }},
  { '+',  { 0x08, 0x08, 0x3e, 0x08, 0x08 }},
  { ',',  { 0x00, 0x50, 0x30, 0x00, 0x00 }},
  { '-',  { 0x08, 0x08, 0x08, 0x08, 0x08 }},
  { '.',  { 0x00, 0x60, 0x60, 0x00, 0x00 }},
  { '/',  { 0x20, 0x10, 0x08, 0x04, 0x02 }},
  { '0',  { 0x3e, 0x51, 0x49, 0x45, 0x3e }},
  { '1',  { 0x00, 0x42, 0x7f, 0x40, 0x00 }},
  { '2',  { 0x42, 0x61, 0x51, 0x49, 0x46 }},
  { '3',  { 0x21, 0x41, 0x45, 0x4b, 0x31 }},
  { '4',  { 0x18, 0x14, 0x12, 0x7f, 0x10 }},
  { '5',  { 0x27, 0x45, 0x45, 0x45, 0x39 }},
  { '6',  { 0x3c, 0x4a, 0x49, 0x49, 0x30 }},
  { '7',  { 0x01, 0x71, 0x09, 0x05, 0x03 }},
  { '8',  { 0x36, 0x49, 0x49, 0x49, 0x36 }},
  { '9',  { 0x06, 0x49, 0x49, 0x29, 0x1e }},
  { ':',  { 0x00, 0x36, 0x36, 0x00, 0x00 }},
  { ';',  { 0x00, 0x56, 0x36, 0x00, 0x00 }},
  { '<',  { 0x00, 0x08, 0x14, 0x22, 0x41 }},
  { '=',  { 0x14, 0x14, 0x14, 0x14, 0x14 }},
  { '>',  { 0x41, 0x22, 0x14, 0x08, 0x00 }},
  { '?',  { 0x02, 0x01, 0x51, 0x09, 0x06 }},
  { '@',  { 0x32, 0x49, 0x79, 0x41, 0x3e }},
  { 'A',  { 0x7e, 0x11, 0x11, 0x11, 0x7e }},
  { 'B',  { 0x7f, 0x49, 0x49, 0x49, 0x36 }},
  { 'C',  { 0x3e, 0x41, 0x41, 0x41, 0x22 }},
  { 'D',  { 0x7f, 0x41, 0x41, 0x22, 0x1c }},
  { 'E',  { 0x7f, 0x49, 0x49, 0x49, 0x41 }},
  { 'F',  { 0x7f, 0x09, 0x09, 0x01, 0x01 }},
  { 'G',  { 0x3e, 0x41, 0x41, 0x51, 0x32 }},
  { 'H',  { 0x7f, 0x08, 0x08, 0x08, 0x7f }},
  { 'I',  { 0x00, 0x41, 0x7f, 0x41, 0x00 }},
  { 'J',  { 0x20, 0x40, 0x41, 0x3f, 0x01 }},
  { 'K',  { 0x7f, 0x08, 0x14, 0x22, 0x41 }},
  { 'L',  { 0x7f, 0x40, 0x40, 0x40, 0x40 }},
  { 'M',  { 0x7f, 0x02, 0x04, 0x02, 0x7f }},
  { 'N',  { 0x7f, 0x04, 0x08, 0x10, 0x7f }},
  { 'O',  { 0x3e, 0x41, 0x41, 0x41, 0x3e }},
  { 'P',  { 0x7f, 0x09, 0x09, 0x09, 0x06 }},
  { 'Q',  { 0x3e, 0x41, 0x51, 0x21, 0x5e }},
  { 'R',  { 0x7f, 0x09, 0x19, 0x29, 0x46 }},
  { 'S',  { 0x46, 0x49, 0x49, 0x49, 0x31 }},
  { 'T',  { 0x01, 0x01, 0x7f, 0x01, 0x01 }},
  { 'U',  { 0x3f, 0x40, 0x40, 0x40, 0x3f }},
  { 'V',  { 0x1f, 0x20, 0x40, 0x20, 0x1f }},
  { 'W',  { 0x7f, 0x20, 0x18, 0x20, 0x7f }},
  { 'X',  { 0x63, 0x14, 0x08, 0x14, 0x63 }},
  { 'Y',  { 0x03, 0x04, 0x78, 0x04, 0x03 }},
  { 'Z',  { 0x61, 0x51, 0x49, 0x45, 0x43 }},
  { '[',  { 0x00, 0x00, 0x7f, 0x41, 0x41 }},
  { '\\', { 0x02, 0x04, 0x08, 0x10, 0x20 }},
  { ']',  { 0x41, 0x41, 0x7f, 0x00, 0x00 }},
  { '^',  { 0x04, 0x02, 0x01, 0x02, 0x04 }},
  { '_',  { 0x40, 0x40, 0x40, 0x40, 0x40 }},
  { '`',  { 0x00, 0x01, 0x02, 0x04, 0x00 }},
  { 'a',  { 0x20, 0x54, 0x54, 0x54, 0x78 }},
  { 'b',  { 0x7f, 0x48, 0x44, 0x44, 0x38 }},
  { 'c',  { 0x38, 0x44, 0x44, 0x44, 0x20 }},
  { 'd',  { 0x38, 0x44, 0x44, 0x48, 0x7f }},
  { 'e',  { 0x38, 0x54, 0x54, 0x54, 0x18 }},
  { 'f',  { 0x08, 0x7e, 0x09, 0x01, 0x02 }},
  { 'g',  { 0x08, 0x14, 0x54, 0x54, 0x3c }},
  { 'h',  { 0x7f, 0x08, 0x04, 0x04, 0x78 }},
  { 'i',  { 0x00, 0x44, 0x7d, 0x40, 0x00 }},
  { 'j',  { 0x20, 0x40, 0x44, 0x3d, 0x00 }},
  { 'k',  { 0x00, 0x7f, 0x10, 0x28, 0x44 }},
  { 'l',  { 0x00, 0x41, 0x7f, 0x40, 0x00 }},
  { 'm',  { 0x7c, 0x04, 0x18, 0x04, 0x78 }},
  { 'n',  { 0x7c, 0x08, 0x04, 0x04, 0x78 }},
  { 'o',  { 0x38, 0x44, 0x44, 0x44, 0x38 }},
  { 'p',  { 0x7c, 0x14, 0x14, 0x14, 0x08 }},
  { 'q',  { 0x08, 0x14, 0x14, 0x18, 0x7c }},
  { 'r',  { 0x7c, 0x08, 0x04, 0x04, 0x08 }},
  { 's',  { 0x48, 0x54, 0x54, 0x54, 0x20 }},
  { 't',  { 0x04, 0x3f, 0x44, 0x40, 0x20 }},
  { 'u',  { 0x3c, 0x40, 0x40, 0x20, 0x7c }},
  { 'v',  { 0x1c, 0x20, 0x40, 0x20, 0x1c }},
  { 'w',  { 0x3c, 0x40, 0x30, 0x40, 0x3c }},
  { 'x',  { 0x44, 0x28, 0x10, 0x28, 0x44 }},
  { 'y',  { 0x0c, 0x50, 0x50, 0x50, 0x3c }},
  { 'z',  { 0x44, 0x64, 0x54, 0x4c, 0x44 }},
  { '{',  { 0x00, 0x08, 0x36, 0x41, 0x00 }},
  { '|',  { 0x00, 0x00, 0x7f, 0x00, 0x00 }},
  { '}',  { 0x00, 0x41, 0x36, 0x08, 0x00 }},
  { '~',  { 0x08, 0x08, 0x2a, 0x1c, 0x08 }} 
};

// Tiny numbers are horizontal.

static const std::map< const u_int,
                       const std::vector< uint8_t >> _tiny_numbers {
  { '0', { 0b11111, 0b11111 }},
  { '1', { 0b11111          }},
  { '2', { 0b10111, 0b11101 }},
  { '3', { 0b10101, 0b11111 }},
  { '4', { 0b11110, 0b00011 }},
  { '5', { 0b11101, 0b10111 }},
  { '6', { 0b11111, 0b00111 }},
  { '7', { 0b10000, 0b11111 }},
  { '8', { 0b10101, 0b10101 }},
  { '9', { 0b11100, 0b11111 }}
};

#define DISPLAY_LEFT_ADDR 0x63
#define DISPLAY_MIDDLE_ADDR 0x62
#define DISPLAY_RIGHT_ADDR 0x61


// Access the elements within the POS data structure.

#define POS_DIGIT(x)  std::get<0>(x)
#define POS_ARRAY(x)  std::get<1>(x)
#define POS_MATRIX(x) std::get<2>(x)
#define POS_X(x)      std::get<3>(x)
#define POS_Y(x)      std::get<4>(x)


MicroDotpHAT::MicroDotpHAT( void )
  : myLeft(   new i2c::is31fl3730( DISPLAY_LEFT_ADDR   )),
    myMiddle( new i2c::is31fl3730( DISPLAY_MIDDLE_ADDR )),
    myRight(  new i2c::is31fl3730( DISPLAY_RIGHT_ADDR  )),
    myMirror( false ), myRotate( false ) {

  _doInit();
  _check();
}

    
MicroDotpHAT::~MicroDotpHAT( void ) {}


MicroDotpHAT::MicroDotpHAT( MicroDotpHAT& mdp )
  : myLeft( mdp.myLeft ), myMiddle( mdp.myMiddle ), myRight( mdp.myRight ),
    myMirror( mdp.myMirror ), myRotate( mdp.myRotate ) {

  _doInit();
}


MicroDotpHAT&
MicroDotpHAT::operator=( MicroDotpHAT& mdp ) {

  myLeft   = mdp.myLeft;
  myMiddle = mdp.myMiddle;
  myRight  = mdp.myRight;

  myMirror = mdp.myMirror;
  myRotate = mdp.myRotate;

  return *this;
}


bool
MicroDotpHAT::operator==( const MicroDotpHAT& mdp ) const noexcept {

  return ( myLeft == mdp.myLeft ) &&
    ( myMiddle == mdp.myMiddle ) &&
    ( myRight == mdp.myRight ) &&
    ( myMirror == mdp.myMirror ) &&
    ( myRotate == mdp.myRotate );
}


void
MicroDotpHAT::_doInit( void ) noexcept {

  assert( myLeft.get() && myMiddle.get() && myRight.get());

  for( auto& i : { myLeft.get(), myMiddle.get(), myRight.get() }) {
    
    i->pwm(          127                                 );
    i->row_current(  i2c::is31fl3730::ROW_CURRENT::mA_35 );
    i->display_mode( i2c::is31fl3730::DISPLAY::BOTH      );
    i->matrix_mode(  i2c::is31fl3730::MATRIX::ADM_8x8    );
    i->audio_enable( false                               );
    i->audio_gain(   i2c::is31fl3730::AUDIO_GAIN::dB_0   );

    i->matrix( m1 ) = zero;
    i->matrix( m2 ) = zero;
    
    i->display_on( true );

    i->update();
    
  }
}


void
MicroDotpHAT::_check( void ) noexcept {

#ifdef _DPG_DEBUG
  assert( myLeft.get() && myMiddle.get() && myRight.get());

  std::default_random_engine g;
  std::uniform_int_distribution< uint8_t >
    dist_cols( 0, std::pow( 2, num_cols_per_digit()) - 1 ),
    dist_rows( 0, std::pow( 2, num_rows_per_digit()) - 1 );

  for( int x = 0; x < num_cols(); ++x )
    for( int y = 0; y < num_rows(); ++y ) {

      assert( set_pixel( x, y, false ) == get_pixel( x, y ));
      assert( get_pixel( x, y ) == false );

      assert( set_pixel( x, y, true ) == get_pixel( x, y ));
      assert( get_pixel( x, y ) == true );

    }
  clear();
  
  for( int x = 0; x < num_cols(); ++x ) {

    const uint8_t v = dist_cols( g );

    assert( set_col( x, v ) == get_col( x ));
    assert( get_col( x ) == v );
    
  }
  clear();

  for( int x = 0; x < num_digits(); ++x ) {

    assert( set_decimal( x, false ) == get_decimal( x ));
    assert( get_decimal( x ) == false );

    assert( set_decimal( x, true ) == get_decimal( x ));
    assert( get_decimal( x ) == true );
    
  }
  clear();
  
  for( int i = 0; i < num_digits(); ++i )
    for( int j = 0; j < num_rows(); ++j ) {
      
      const uint8_t v = dist_rows( g );
      
      assert( _set_row( i, j, v ) == _get_row( i, j ));
      assert( _get_row( i, j ) == v );
      
    }
  clear();

  _LOG_VERB(( "Data structures tests passed" ));
#endif
  
}


void
MicroDotpHAT::clear( void ) noexcept {

  assert( myLeft.get() && myMiddle.get() && myRight.get());

  for( auto& i : { myLeft.get(), myMiddle.get(), myRight.get() }) {

    i->matrix( m1 ) = zero;
    i->matrix( m2 ) = zero;

  }

  for( int i = 0; i < num_digits(); ++i )
    set_decimal( i, false );

}


void
MicroDotpHAT::fill( bool on_off ) noexcept {

  assert( myLeft.get() && myMiddle.get() && myRight.get());

  for( int x = 0; x < num_cols(); ++x )
    for( int y = 0; y < num_rows(); ++y )
      set_pixel( x, y, on_off );

}


void
MicroDotpHAT::scroll( int x, int y ) noexcept {

  if( y )
    scroll_vertical( y );
  if( x )
    scroll_horizontal( x );
  
}


void
MicroDotpHAT::scroll_horizontal( int x ) noexcept {

  std::vector< uint8_t > cols;

  // Copy out the columns.
  
  for( int c = 0; c < num_cols(); ++c )
    cols.push_back( get_col( c ));

  // Manipulate the columns.
  
  for( int i = 0; i < ::abs( x ); ++i )
    if( x > 0 ) {
      cols.insert( cols.begin(), cols.back());
      cols.pop_back();
    } else {
      cols.push_back( cols.front());
      cols.erase( cols.begin());
    }
  
  // Write the columns back.
  
  for( int c = 0; c < num_cols(); ++c )
    set_col( c, cols[c]);
}


void
MicroDotpHAT::scroll_vertical( int y ) noexcept {

  for( int i = 0; i < ::abs( y ); ++i ) 
    for( int d = 0; d < num_digits(); ++d ) {

      // Make a copy of the rows for the digit.
      
      std::vector< uint8_t > rows;
      for( int r = 0; r < num_rows(); ++r )
	rows.push_back( _get_row( d, r ));

      // Manipulate the row.
      
      if( y > 0 ) {
	rows.push_back( rows.front());
	rows.erase( rows.begin());
      } else {
	rows.insert( rows.begin(), rows.back());
	rows.pop_back();
      }

      // Copy the scrolled row back into the digit.
      
      for( int r = 0; r < num_rows(); ++r )
	_set_row( d, r, rows[r]);
          
    }
}


uint8_t
MicroDotpHAT::set_brightness( int brightness ) noexcept {

  assert( myLeft.get() && myMiddle.get() && myRight.get());

  if( brightness >= 128 ) {
    myLeft->pwm128( true );
    myMiddle->pwm128( true );
    myRight->pwm128( true );
  } else {
    myLeft->pwm( brightness );
    myMiddle->pwm( brightness );
    myRight->pwm( brightness );
  }

  return get_brightness();
}


bool
MicroDotpHAT::set_pixel( int x, int y, bool on_off ) noexcept {

  const POS p = _calc_positions( x, y );

  if( _choose_matrix( p ) == m2 ) 
    _set_pixel_m2( x, y, on_off );
  else 
    _set_pixel_m1( x, y, on_off );
  
  return get_pixel( x, y );
}


const uint8_t
MicroDotpHAT::set_col( int x, uint8_t v ) noexcept {

  for( int y = 0; y < num_rows(); ++y ) 
    set_pixel( x, y, ( _bit_mask( y ) & v ));

  return get_col( x );
}


inline
bool
MicroDotpHAT::_get_pixel_m2( int x, int y ) const noexcept {

  const POS              p = _calc_positions( x, y );
        i2c::is31fl3730* d = _choose_display( p );

  return ( d->matrix( m2 )[ POS_X(p)] & _bit_col( POS_Y(p)));
}


inline
bool
MicroDotpHAT::_set_pixel_m2( int x, int y, bool on_off ) noexcept {

  const POS              p = _calc_positions( x, y );
        i2c::is31fl3730* d = _choose_display( p );

  const uint8_t bit = _bit_col( POS_Y(p));

  d->matrix( m2 )[ POS_X(p)] &= ~bit;
  if( on_off )
    d->matrix( m2 )[ POS_X(p)] |= bit;

  return _get_pixel_m2( x, y );
}


inline
bool
MicroDotpHAT::_get_pixel_m1( int x, int y ) const noexcept {

  const POS              p = _calc_positions( x, y );
        i2c::is31fl3730* d = _choose_display( p );

  return ( d->matrix( m1 )[ POS_Y(p)] & _bit_col( POS_X(p)));
}


inline
bool
MicroDotpHAT::_set_pixel_m1( int x, int y, bool on_off ) noexcept {

  const POS              p = _calc_positions( x, y );
        i2c::is31fl3730* d = _choose_display( p );

  const uint8_t bit = _bit_col( POS_X(p));

  d->matrix( m1 )[ POS_Y(p)] &= ~bit;
  if( on_off )
    d->matrix( m1 )[ POS_Y(p)] |= bit;

  return _get_pixel_m1( x, y );
}


const uint8_t
MicroDotpHAT::get_col( int x ) const noexcept {

  uint8_t rVal = 0x00;

  for( int y = 0; y < num_rows(); ++y ) 
    if( get_pixel( x, y ))
      rVal |= _bit_mask( y );

  return rVal;
}


bool
MicroDotpHAT::set_decimal( int digit, bool on_off ) noexcept {

  assert( digit < num_digits());

  const int x = digit * num_cols_per_digit();
  const POS p = _calc_positions( x, 0 );

  if( _choose_matrix( p ) == m2 ) 
    _set_decimal_m2( digit, on_off );
  else 
    _set_decimal_m1( digit, on_off );
	  
  return get_decimal( digit );
}


inline
bool
MicroDotpHAT::_set_decimal_m2( int digit, bool on_off ) noexcept {

  assert( digit < num_digits());

  const int              x = digit * num_cols_per_digit();
  const POS              p = _calc_positions( x, 0 );
        i2c::is31fl3730* d = _choose_display( p );

  d->matrix( m2 )[ 7 ] &= ~0x40;
  if( on_off )
    d->matrix( m2 )[ 7 ] |= 0x40;

  return _get_decimal_m2( digit );
}


inline
bool
MicroDotpHAT::_get_decimal_m2( int digit ) const noexcept {

  assert( digit < num_digits());

  const int              x = digit * num_cols_per_digit();
  const POS              p = _calc_positions( x, 0 );
        i2c::is31fl3730* d = _choose_display( p );

  return ( d->matrix( m2 )[ 7 ] & 0x40 );
}


inline
bool
MicroDotpHAT::_set_decimal_m1( int digit, bool on_off ) noexcept {

  assert( digit < num_digits());

  const int              x = digit * num_cols_per_digit();
  const POS              p = _calc_positions( x, 0 );
        i2c::is31fl3730* d = _choose_display( p );

  d->matrix( m1 )[ 6 ] &= ~0x80;
  if( on_off )
    d->matrix( m1 )[ 6 ] |= 0x80;

  return _get_decimal_m1( digit );
}


inline
bool
MicroDotpHAT::_get_decimal_m1( int digit ) const noexcept {

  assert( digit < num_digits());

  const int              x = digit * num_cols_per_digit();
  const POS              p = _calc_positions( x, 0 );
        i2c::is31fl3730* d = _choose_display( p );

  return ( d->matrix( m1 )[ 6 ] & 0x80 );
}


bool
MicroDotpHAT::get_decimal( int digit ) const noexcept {

  assert( digit < num_digits());

  const int x = digit * num_cols_per_digit();
  const POS p = _calc_positions( x, 0 );

  bool rVal = false;
  
  if( _choose_matrix( p ) == m2 ) 
    rVal = _get_decimal_m2( digit );
  else
    rVal = _get_decimal_m1( digit );
  
  return rVal;
}


bool
MicroDotpHAT::get_pixel( int x, int y ) const noexcept {

  const POS     p = _calc_positions( x, y );
        bool rVal = false;

  if( _choose_matrix( p ) == m2 ) 
    rVal = _get_pixel_m2( x, y );
  else
    rVal = _get_pixel_m1( x, y );
  
  return rVal;
}


int32_t
MicroDotpHAT::show( void ) noexcept {

  assert( myLeft.get() && myMiddle.get() && myRight.get());

  int32_t rVal = 0;
  
  for( auto& i : { myLeft.get(), myMiddle.get(), myRight.get() }) 
    if( int32_t tVal; ( tVal = i->update()) < 0 )
      rVal = tVal;

  return rVal;
}


void
MicroDotpHAT::write_char( char c, int x, int y ) noexcept {

  assert( _font.find( c ) != _font.end());

  decltype( _font )::const_iterator it = _font.find( c );

  for( size_t i = 0; i < it->second.size(); ++i ) 
    for( int j = 0; j < num_cols_per_digit(); ++j )
      if(( x + int( i )) < num_cols())
	set_pixel( x + i, y + j, ( it->second[i] & _bit_mask( j )));
  
}


void
MicroDotpHAT::write_string( const std::string& text,
			    int offset_x, int offset_y,
			    bool kerning ) noexcept {

  std::vector< uint8_t > v;

  for( size_t i = 0; i < text.size(); ++i )
    v.push_back( text[i]);  
  write_string( v, offset_x, offset_y, kerning );

}


void
MicroDotpHAT::write_string( const char* text,
			    int offset_x, int offset_y,
			    bool kerning ) noexcept {

  std::vector< uint8_t > v;

  for( const char* t = text; *t; t++ )
    v.push_back( *t );

  write_string( v, offset_x, offset_y, kerning );
  
}


void
MicroDotpHAT::write_string( const std::vector< uint8_t >& text,
			    int offset_x, int offset_y,
			    bool kerning ) noexcept {

  for( size_t i = 0; i < text.size(); ++i ) {

    write_char( text[i], offset_x, offset_y );

    offset_x += ( kerning ? num_vis_cols_per_digit() : num_cols_per_digit());

  }

}


inline
const MicroDotpHAT::POS
MicroDotpHAT::_calc_positions( const int x, const int y ) const noexcept {

  assert(( x >= 0 ) && ( y >= 0 ));
  assert( y < num_rows());
  assert( x < num_cols());
          
  POS rVal { x / num_cols_per_digit(), // Digit
 	     0,                        // Array
	     0,                        // Matrix
	     x % num_cols_per_digit(), // X within digit
	     y % num_rows_per_digit()  // Y within digit
  };

  POS_ARRAY( rVal )  = POS_DIGIT( rVal ) / 2;
  POS_MATRIX( rVal ) = POS_DIGIT( rVal ) % 2;
  
  _LOG_VERB(( "x=", x, ",y=", y, "  digit=", POS_DIGIT( rVal ),
	      ", array=", POS_ARRAY( rVal ), ", matrix=", POS_MATRIX( rVal ),
	      ", x=", POS_X( rVal ), ", y=", POS_Y( rVal )));
  
  return rVal;
}


inline
i2c::is31fl3730*
MicroDotpHAT::_choose_display( const POS& p ) const noexcept {

  if( POS_ARRAY( p ) == 0 )
    return myLeft.get();
  else
    if( POS_ARRAY( p ) == 1 )
      return myMiddle.get();
    else
      if( POS_ARRAY( p ) == 2 )
        return myRight.get();

  _LOG_ABORT(( "I can't get here. display=", POS_ARRAY( p )));

  return nullptr;
}


inline
i2c::is31fl3730::MATRIX_REG
MicroDotpHAT::_choose_matrix( const POS& p ) const noexcept {

  if( POS_MATRIX( p )) 
    return m1;
  else
    return m2;

}


inline
uint8_t
MicroDotpHAT::_bit_col( int s ) const noexcept {

  return _bit_shift_left( s );
}


inline
uint8_t
MicroDotpHAT::_bit_mask( int s ) const noexcept {

  return _bit_shift_left( s );
}


uint8_t
MicroDotpHAT::_get_row( const int digit, const int row ) const noexcept {

  assert( digit < num_digits());
  assert( row   < num_rows());
  
  const int x = digit * num_cols_per_digit();

  uint8_t rVal = 0;

  for( int i = num_cols_per_digit(); i > 0; --i )
    if( get_pixel( x + i - 1, row ))
      rVal |= _bit_mask( i - 1 );
  
  return rVal;
}


uint8_t
MicroDotpHAT::_set_row( const int digit,
			const int row, const uint8_t v ) noexcept {

  const int x = digit * num_cols_per_digit();

  for( int i = num_cols_per_digit(); i > 0; --i ) 
    set_pixel( x + i - 1, row, v & _bit_mask( i - 1 ));
  
  return _get_row( digit, row );
}


std::ostream&
operator<<( std::ostream& os, const MicroDotpHAT::POS& p ) {

  os << "digit="  << POS_DIGIT(p)  << ", "
     << "array="  << POS_ARRAY(p)  << ", "
     << "matrix=" << POS_MATRIX(p) << ", "
     << "(x,y)=(" << POS_X(p) << ',' << POS_Y(p) << ')';

  return os;
}


std::ostream&
operator<<( std::ostream& os, const MicroDotpHAT& pHAT ) {

  os << "rotate=" << pHAT.get_rotate180() << ", "
     << "mirror=" << pHAT.get_mirror()    << ", "
     << "left="   << *pHAT.myLeft         << ", "
     << "middle=" << *pHAT.myMiddle       << ", "
     << "right="  << *pHAT.myRight;
    
  return os;
}



//  LocalWords:  LEDs Pimoroni github
