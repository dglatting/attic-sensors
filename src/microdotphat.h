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
 * The following functions are untested:
 *  * The copy and move constructors,
 *  * The logical operators == and !=, and
 *  * scroll(), scroll_to(), scroll_vertical(), and scroll_horizontal(),
 * The following functions are unimplemented:
 *  * set_rotate180(),
 *  * set_mirror(), and
 *  * draw_tiny(),
 *
 *
 * $Log: microdotphat.h,v $
 * Revision 1.11  2019/10/23 03:54:00  root
 * First pass at integrating other project changes.
 *
 * Revision 1.10  2019/09/29 03:56:17  root
 * Various syntax and text updates.
 *
 * Revision 1.9  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.8  2019/09/25 03:56:42  root
 * Noted the tiny numbers font is unimplemented. Changed the return
 * type on the write_*() functions. Change the kerning default to
 * false.
 *
 * Revision 1.7  2019/09/24 07:57:25  root
 * Several of the scroll functions are now implemented. Function
 * return values changed where the functions returns were
 * meaningless. write_char() now implemented. Simplifications.
 *
 * Revision 1.6  2019/09/23 06:30:19  root
 * Added set/get row but as private functions. set_col() now returns
 * get_col() verses void. Added several private low level manipulation
 * routines.
 *
 * Revision 1.5  2019/09/23 01:47:28  root
 * Decimal point now works.
 *
 * Revision 1.4  2019/09/22 06:24:04  root
 * Added and renamed some functions. Added contextual text.
 *
 * Revision 1.3  2019/09/21 09:12:03  root
 * Added utility functions.
 *
 * Revision 1.2  2019/09/18 06:09:14  root
 * Start of Micro Dot pHAT debugging.
 *
 * Revision 1.1  2019/09/17 01:58:33  root
 * Initial revision
 *
 */

#ifndef __MICRODOTPHAT_H__
#define __MICRODOTPHAT_H__

extern "C" {

#include <assert.h>
#include <stdlib.h>
  
}

#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <vector>

#include "is31fl3730.h"
#include "log.h"
#include "util.h"


#define _MICRODOTPHAT_H_ID "$Id: microdotphat.h,v 1.11 2019/10/23 03:54:00 root Exp root $"


class MicroDotpHAT;
std::ostream& operator<<( std::ostream& os, const MicroDotpHAT& pHAT );


// This class is a c++ mimic of the Pimoroni Python code for the
// Pimoroni Micro Dot pHAT.
//
// The pHAT uses three is31fl3730 i2c matrix chips with two displays
// each to form a six display unit. The displays are a 5x7 LED
// matrix plus a decimal point.
//
// The display's are organized as follows from left to right:
//
//  Num:       3 3  2 2  1 1
//  Matrix:    2 1  2 1  2 1
//  i2c Addr:  0x63 0x62 0x61
//
// Thus:
//  1, There are six display (left to right)
//  2, There are three arrays (two displays in each array)
//  3, There are two matrices in each array (the left display and
//     right display).
// 
// This bit order in the two matrix *are different.* They are:
//  MATRIX 2                    MATRIX 1
//  Col  1 2 3 4 5              Row  8 7 6 5 4 3 2 1
//        00011111  Row 1                   01111111  Col 1
//        00011111  Row 2                   01111111  Col 2
//        00011111  Row 3                   01111111  Col 3
//        00011111  Row 4                   01111111  Col 4
//        00011111  Row 5                   01111111  Col 5
//        00011111  Row 6                   00000000
//        0X011111  Row 7                   0X000000
//                                          00000000
//  X = decimal point
//
// There are five columns and seven rows of lighted LEDs per
// digit. *However*, to simplify code, *seven* columns are allocated
// per digit.

class MicroDotpHAT {

#define NUM_MICRO_DOT_PHAT_DIGITS 6
#define NUM_MICRO_DOT_PHAT_COLS 7
#define NUM_MICRO_DOT_PHAT_ROWS 7
#define NUM_VISIBLE_MICRO_DOT_PHAT_COLS 5
  
private:

  // The left, middle, and right display pairs.
  
  std::shared_ptr< is31fl3730 > myLeft, myMiddle, myRight;

  // Whether to mirror (i.e., flip left/right) and/or rotate (i.e.,
  // flip top/bottom) the display.
  
  bool myMirror, myRotate;

  // Initialize the display objects.
  
  void _doInit( void ) noexcept;

  // Self check.
  
  void _check( void ) noexcept;
  
public:

           MicroDotpHAT( void );
  virtual ~MicroDotpHAT( void );

  MicroDotpHAT( const MicroDotpHAT& ) = delete;
  MicroDotpHAT(       MicroDotpHAT& );

  MicroDotpHAT( const MicroDotpHAT&& ) = delete;
  MicroDotpHAT(       MicroDotpHAT&& );

  const MicroDotpHAT& operator=( const MicroDotpHAT& ) = delete;
  MicroDotpHAT& operator=(       MicroDotpHAT& );

  bool operator==( const MicroDotpHAT& ) const noexcept;
  bool operator!=( const MicroDotpHAT& ) const noexcept;

  // This is the number of *total* rows and columns in the display
  // (i.e., not *each* matrix). This is useful when treating the
  // display as a n-by-m matrix of dots, excluding the gaps, of
  // course.
  
  const int num_rows( void )     const noexcept;
  const int num_cols( void )     const noexcept;

  // The number of digits and the number of rows/columns *per digit*.
  
  const int num_digits( void             )   const noexcept;
  const int num_cols_per_digit(     void ) const noexcept;
  const int num_rows_per_digit(     void ) const noexcept;
  const int num_vis_cols_per_digit( void ) const noexcept;
  
  // These are implementation forms of the Python function
  // interface. For functions that return a value to indicate success
  // or failure, success is indicated when they return 0 however the
  // ability to detect fault is suspect.
  //
  // Some things to understand:
  //  1, Digits are left to right: 0..5.
  //  2, As a matrix of pixels of x= 0..41, // column (7x6), five
  //                                        // pixel/display
  //                              y= 0..5   // row
  //  3, The top left of display 0 is pixel position (0,0)
  //  4, There is a two pixel gap between characters, which is
  //     impacted by kerning when writing strings.
  
  void clear( void ) noexcept;

  // These functions ARE NOT supported (yet?).
  
  void draw_tiny( int display, const std::string& text ) noexcept;
  void draw_tiny( int display, const char*        text ) noexcept;

  // Fill the display, either lighted or dark.
  // Note: This DOES NOT effect the decimal point.
  
  void fill( bool ) noexcept;

  // Scroll the display, which could be vertically (x=0), horizontally
  // (y=0), or diagonally. You can scroll positively
  // (i.e., { x,y | x,y > 0}) or negatively (i.e., {x,y | x,y < 0}).
  // Positive numbers result in scrolling down or right whereas negative
  // numbers are up or left.
  
  void scroll( int x = 0, int y = 0 ) noexcept;

  void scroll_to( int x = 0, int y = 0 ) noexcept;

  // Scroll x pixels horizontally.
  // Note: 1, Positive amount - scroll right to left circularly wrapping
  //          columns.
  //       2, Negative amount - scroll left to right circularly
  //          wrapping columns.

  void scroll_horizontal( int amount = 1 ) noexcept;

  // Scroll y pixels vertically.
  // Note: 1, Positive amount - scroll bottom to top circularly
  //          wrapping rows.
  //       2, Negative amount - scroll top to bottom circularly
  //          wrapping rows.
  
  void scroll_vertical( int amount = 1 ) noexcept;

  // Set/get the brightness level. The range is 0..128.
  
  uint8_t set_brightness( int brightness ) noexcept;
  uint8_t get_brightness( void           ) const noexcept;

  // Set the column (i.e., 0..41 to the last 7 bits of v).
  // Note: The Pimoroni implementation has a set_row() function but it
  //       isn't mentioned in the documentation. Therefore, there is
  //       no set_row() function implemented here, partially because I
  //       don't know what it would mean to set a row. That said, as a
  //       private function it makes sense for scrolling.

  const uint8_t set_col( int x, uint8_t v ) noexcept;
  const uint8_t get_col( int x ) const noexcept;

  // Set the decimal point in matrix 0..5 on or off.
  
  bool set_decimal( int digit, bool on_off ) noexcept;
  bool get_decimal( int digit ) const noexcept;
  
  // Set/get mirroring (i.e., flip left/right).
  
  bool set_mirror( bool )       noexcept;
  bool get_mirror( void ) const noexcept;

  // Set/get rotating (i.e., flip top/bottom).
  // Note: The decimal point cannot be flipped - it is physical.
    
  bool set_rotate180( bool )       noexcept;
  bool get_rotate180( void ) const noexcept;
  
  // Set the pixel x,y on or off.
  
  bool set_pixel( int x, int y, bool on_off ) noexcept;
  bool get_pixel( int x, int y              ) const noexcept;
  
  int32_t show( void ) noexcept;

  // Note that Japanese IS NOT supported (yet?).
  
  void write_char( char c, int x, int y ) noexcept;

  // Write a string (i.e., ASCII characters) to the display starting
  // at the *PIXEL* offsets, which are usually zero and therefore
  // starts from the left to the right.
  //
  // Kerning is character width packing, which makes sense when
  // scrolling but I changed the default to False for common use.
  
  void write_string( const std::string& text,
		     int offset_x = 0, int offset_y = 0,
		     bool kerning = false ) noexcept;
  void write_string( const char* text,
		     int offset_x = 0, int offset_y = 0,
		     bool kerning = false ) noexcept;
  void write_string( const std::vector< uint8_t >& text,
                     int offset_x = 0, int offset_y = 0,
                     bool kerning = false ) noexcept;

private:

  // The following is internal stuff.

  // Given an absolute (x,y), what does it mean?
  
  typedef
  std::tuple< int,  // digit  0..5 (left->right)
	      int,  // array  0..2 (left->right)
	      int,  // matrix 0..1 (left->right)
	      // The following are the (x,y) within a
	      // matrix. Specifically, y (the row) indexes into the
	      // matrix data and x indicates the bit within that row.
	      int,  // x of pixel 0..7 (left->right)
	      int   // y of pixel 0..7 (top->bottom)
   >  POS;

  const POS _calc_positions( const int x, const int y ) const noexcept;

  is31fl3730* _choose_display( const POS& ) const noexcept; 
  is31fl3730::MATRIX_REG _choose_matrix( const POS& ) const noexcept;

  // The bit mask is the traditional shifting of 0x01 from the right
  // to the left however col/row may be more specific to a matrix.
  
  uint8_t _bit_col(  int ) const noexcept;
  uint8_t _bit_mask( int ) const noexcept;

  // Because setting pixels in each matrix may be different, there are
  // two separate private sets of functions to manage bits.
  
  bool _set_pixel_m2( int x, int y, bool on_off )       noexcept,
       _get_pixel_m2( int x, int y )              const noexcept,

       _set_pixel_m1( int x, int y, bool on_off )       noexcept,
       _get_pixel_m1( int x, int y )              const noexcept,

       _set_decimal_m2( int digit, bool on_off )       noexcept,
       _get_decimal_m2( int digit )              const noexcept,

       _set_decimal_m1( int digit, bool on_off )       noexcept,
       _get_decimal_m1( int digit )              const noexcept;

  // Set/get a row in the indicated digit. Only the last five bits
  // (NUM_VISIBLE_MICRO_DOT_PHAT_COLS) are used. The remaining bits
  // are either don't care (set) or zero (get). This might cause some
  // confusion.
  
  uint8_t _set_row( const int digit,
		    const int row,
		    const uint8_t v )       noexcept,
          _get_row( const int digit,
		    const int row   ) const noexcept;
	     
  // Utility friends.

  friend std::ostream& ::operator<<( std::ostream&, const MicroDotpHAT& );
  friend std::ostream&   operator<<( std::ostream&, const MicroDotpHAT::POS& );

};


inline
bool
MicroDotpHAT::operator!=( const MicroDotpHAT& mdp ) const noexcept {

  return !operator==( mdp );
}


inline
const int
MicroDotpHAT::num_digits( void ) const noexcept {

  return NUM_MICRO_DOT_PHAT_DIGITS;
}


inline
const int
MicroDotpHAT::num_rows( void ) const noexcept {

  return NUM_MICRO_DOT_PHAT_ROWS;
}


inline
const int
MicroDotpHAT::num_cols( void ) const noexcept {

  return num_cols_per_digit() * num_digits();
}


inline
const int
MicroDotpHAT::num_cols_per_digit( void ) const noexcept {

  return NUM_MICRO_DOT_PHAT_COLS;
}


inline
const int
MicroDotpHAT::num_rows_per_digit( void ) const noexcept {

  return NUM_MICRO_DOT_PHAT_ROWS;
}


inline
const int
MicroDotpHAT::num_vis_cols_per_digit( void ) const noexcept {

  return NUM_VISIBLE_MICRO_DOT_PHAT_COLS;
}


inline
bool
MicroDotpHAT::get_mirror( void ) const noexcept {

  return myMirror;
}


inline
bool
MicroDotpHAT::get_rotate180( void ) const noexcept {

  return myRotate;
}


inline
uint8_t
MicroDotpHAT::get_brightness( void ) const noexcept {

  assert( myLeft.get());

  return myLeft->pwm();
}


// Helpful output operators.

std::ostream& operator<<( std::ostream& os, const MicroDotpHAT::POS& p );


#endif


//  LocalWords:   Pimoroni pHAT Kerning kerning
