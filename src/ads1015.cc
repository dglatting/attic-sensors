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
 * $Log: ads1015.cc,v $
 * Revision 1.28  2019/10/20 04:35:48  root
 * Renamed reset() per superclass change.
 *
 * Revision 1.27  2019/10/19 01:03:11  root
 * First perversion to new code structure.
 *
 * Revision 1.26  2019/10/15 22:38:05  root
 * Integrated new "templates" header from another project.
 *
 * Revision 1.25  2019/10/07 03:42:36  root
 * Shortened the function quoteStr().
 *
 * Revision 1.24  2019/10/07 03:15:14  root
 * Minor const changes.
 *
 * Revision 1.23  2019/10/07 02:54:33  root
 * Converted types from u_short to uint16_t because I cannot guarantee
 * u_short as 16 bits, though it usually is.
 *
 * Revision 1.22  2019/10/01 04:48:39  root
 * A little const-iness.
 *
 * Revision 1.21  2019/09/29 03:56:17  root
 * Various syntax and text updates.
 *
 * Revision 1.20  2019/09/28 02:19:06  root
 * Moved some static variables to inline.
 *
 * Revision 1.19  2019/09/27 07:41:57  root
 * Introduced "structured bindings." e=enum,i=int,f=float,b=bits,m=mask.
 *
 * Revision 1.18  2019/09/27 04:10:26  root
 * Cleaned up errno2str() calls.
 *
 * Revision 1.17  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.16  2019/09/16 23:19:01  root
 * Harmonized (code-wise) with other objects.
 *
 * Revision 1.15  2019/09/15 05:08:34  root
 * Added comments and simplified code.
 *
 * Revision 1.14  2019/09/14 05:22:57  root
 * Changed some decls. Fixed log messages. Added ostream functions for
 * enums.
 *
 * Revision 1.13  2019/09/07 07:09:14  root
 * Added forgotten defs. Added reset function, which does
 * nothing. Simplified log statements. Modified read/write to use base
 * class functions.
 *
 * Revision 1.12  2019/09/04 05:24:14  root
 * Moved the enums definition into the class, where they should have
 * been. Added more self test cases.
 *
 * Revision 1.11  2019/09/04 04:23:47  root
 * Added MIT copyright notice.
 *
 * Revision 1.10  2019/09/01 06:44:02  root
 * In my rewrite of sample conversion I forgot to properly adjust
 * where the sign bit was located.
 *
 * Revision 1.9  2019/08/31 08:18:49  root
 * Fixed conversion related bugs.
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
 * Revision 1.3  2019/08/27 01:01:34  root
 * Adding additional utilities to source.
 *
 * Revision 1.2  2019/08/26 19:37:49  root
 * Saving work in-progress.
 *
 * Revision 1.1  2019/08/24 23:50:23  root
 * Initial revision
 *
 */

extern "C" {

#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
  
#include <linux/i2c-dev.h>
  
}

#include <chrono>
#include <iostream>
#include <memory>
#include <map>
#include <mutex>
#include <string>
#include <sstream>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "ads1015.h"
#include "log.h"
#include "templates.h"
#include "util.h"


extern const std::vector< std::string > ads1015_ident {
  _ADS1015_H_ID, "$Id: ads1015.cc,v 1.28 2019/10/20 04:35:48 root Exp $"
};


ads1015::ads1015( void )
  : i2c( default_addr ),
    myConfigReg( default_config ) {
  
  _doInit();
  _check();
}

  
ads1015::ads1015( const int16_t addr )
  : i2c( addr ),
    myConfigReg( default_config ) {
  
  _doInit();
  _check();
}


ads1015::ads1015( const std::string& bus, const int16_t addr )
  : i2c( bus, addr ),
    myConfigReg( default_config ) {
  
  _doInit();
  _check();
}


ads1015::~ads1015( void ) {}


ads1015&
ads1015::operator=( ads1015& ad ) {
  
  i2c::operator=( ad );
  
  myConfigReg = ad.myConfigReg;
  
  return *this;
}


ads1015&
ads1015::operator=( ads1015&& ad ) {
  
  i2c::operator=( ad );
  
  myConfigReg = ad.myConfigReg;
  
  ad.myConfigReg = 0;
  
  return *this;
}


int
ads1015::reset( void ) {
  
  return i2c::_reset();
}

  
bool
ads1015::_doInit( void ) noexcept {
  
  bool retVal = false;
  
  if( i2c::_doInit()) {
    
    // Write out the configuration register.
    
    if( _write_cfg() >= 0 )
      retVal = true;
    
  }
  
  return retVal;
}

  
void
ads1015::_check( void ) noexcept {

#ifdef _DPG_DEBUG
  _check_masking< OS_x          >( OS_MAP          );
  _check_masking< PGA_GAIN_x    >( PGA_GAIN_MAP    );
  _check_masking< MODE_x        >( MODE_MAP        );
  _check_masking< SAMPLE_RATE_x >( SAMPLE_RATE_MAP );
  _check_masking< COMP_MODE_x   >( COMP_MODE_MAP   );
  _check_masking< COMP_POL_x    >( COMP_POL_MAP    );
  _check_masking< COMP_LAT_x    >( COMP_LAT_MAP    );
  _check_masking< COMP_QUE_x    >( COMP_QUE_MAP    );

  { assert( MUX_MAP.size());
    
    constexpr std::tuple< AINp_x, AINn_x > mask_tuple =
      std::make_tuple< AINp_x, AINn_x >( AINp_x::_MASK, AINn_x::_MASK );
    const uint16_t mask = MUX_MAP.at( mask_tuple );
    for( const auto& [k,m] : MUX_MAP )
      if( k != mask_tuple ) {
	assert(( m & ~mask ) == 0 );
	assert(( m &  mask ) == m );
      }
  }
  
  _check_bit_overlap< decltype( OS_MAP          ),
		      decltype( MUX_MAP         )>( OS_MAP,
						    MUX_MAP );
  _check_bit_overlap< decltype( OS_MAP          ),
		      decltype( PGA_GAIN_MAP    )>( OS_MAP,
						    PGA_GAIN_MAP );
  _check_bit_overlap< decltype( OS_MAP          ),
		      decltype( MODE_MAP        )>( OS_MAP,
						    MODE_MAP );
  _check_bit_overlap< decltype( OS_MAP          ),
		      decltype( SAMPLE_RATE_MAP )>( OS_MAP,
						    SAMPLE_RATE_MAP );
  _check_bit_overlap< decltype( OS_MAP          ),
		      decltype( COMP_MODE_MAP   )>( OS_MAP,
						    COMP_MODE_MAP );
  _check_bit_overlap< decltype( OS_MAP          ),
		      decltype( COMP_POL_MAP    )>( OS_MAP,
						    COMP_POL_MAP );
  _check_bit_overlap< decltype( OS_MAP          ),
		      decltype( COMP_LAT_MAP    )>( OS_MAP,
						    COMP_LAT_MAP );
  _check_bit_overlap< decltype( OS_MAP          ),
		      decltype( COMP_QUE_MAP    )>( OS_MAP,
						    COMP_QUE_MAP );
  
  _check_bit_overlap< decltype( MUX_MAP         ),
		      decltype( PGA_GAIN_MAP    )>( MUX_MAP,
						    PGA_GAIN_MAP );
  _check_bit_overlap< decltype( MUX_MAP         ),
		      decltype( MODE_MAP        )>( MUX_MAP, 
						    MODE_MAP );
  _check_bit_overlap< decltype( MUX_MAP         ),
		      decltype( SAMPLE_RATE_MAP )>( MUX_MAP,
						    SAMPLE_RATE_MAP );
  _check_bit_overlap< decltype( MUX_MAP         ),
		      decltype( COMP_MODE_MAP   )>( MUX_MAP,
						    COMP_MODE_MAP );
  _check_bit_overlap< decltype( MUX_MAP         ),
		      decltype( COMP_POL_MAP    )>( MUX_MAP,
						    COMP_POL_MAP );
  _check_bit_overlap< decltype( MUX_MAP         ),
		      decltype( COMP_LAT_MAP    )>( MUX_MAP,
						    COMP_LAT_MAP );
  _check_bit_overlap< decltype( MUX_MAP         ),
		      decltype( COMP_QUE_MAP    )>( MUX_MAP,
						    COMP_QUE_MAP );
  
  _check_bit_overlap< decltype( PGA_GAIN_MAP    ),
		      decltype( MODE_MAP        )>( PGA_GAIN_MAP,
						    MODE_MAP );
  _check_bit_overlap< decltype( PGA_GAIN_MAP    ),
		      decltype( SAMPLE_RATE_MAP )>( PGA_GAIN_MAP,
						    SAMPLE_RATE_MAP );
  _check_bit_overlap< decltype( PGA_GAIN_MAP    ),
		      decltype( COMP_MODE_MAP   )>( PGA_GAIN_MAP,
						    COMP_MODE_MAP );
  _check_bit_overlap< decltype( PGA_GAIN_MAP    ),
		      decltype( COMP_POL_MAP    )>( PGA_GAIN_MAP,
						    COMP_POL_MAP );
  _check_bit_overlap< decltype( PGA_GAIN_MAP    ),
		      decltype( COMP_LAT_MAP    )>( PGA_GAIN_MAP,
						    COMP_LAT_MAP );
  _check_bit_overlap< decltype( PGA_GAIN_MAP    ),
		      decltype( COMP_QUE_MAP    )>( PGA_GAIN_MAP,
						    COMP_QUE_MAP );
  
  _check_bit_overlap< decltype( MODE_MAP        ),
		      decltype( SAMPLE_RATE_MAP )>( MODE_MAP,
						    SAMPLE_RATE_MAP );    
  _check_bit_overlap< decltype( MODE_MAP        ),
		      decltype( COMP_MODE_MAP   )>( MODE_MAP,
						    COMP_MODE_MAP );
  _check_bit_overlap< decltype( MODE_MAP        ),
		      decltype( COMP_POL_MAP    )>( MODE_MAP,
						    COMP_POL_MAP );
  _check_bit_overlap< decltype( MODE_MAP        ),
		      decltype( COMP_LAT_MAP    )>( MODE_MAP,
						    COMP_LAT_MAP );
  _check_bit_overlap< decltype( MODE_MAP        ),
		      decltype( COMP_QUE_MAP    )>( MODE_MAP,
						    COMP_QUE_MAP );
  
  _check_bit_overlap< decltype( SAMPLE_RATE_MAP ),
		      decltype( COMP_MODE_MAP   )>( SAMPLE_RATE_MAP,
						    COMP_MODE_MAP );
  _check_bit_overlap< decltype( SAMPLE_RATE_MAP ),
		      decltype( COMP_POL_MAP    )>( SAMPLE_RATE_MAP,
						    COMP_POL_MAP );
  _check_bit_overlap< decltype( SAMPLE_RATE_MAP ),
		      decltype( COMP_LAT_MAP    )>( SAMPLE_RATE_MAP,
						    COMP_LAT_MAP );
  _check_bit_overlap< decltype( SAMPLE_RATE_MAP ),
		      decltype( COMP_QUE_MAP    )>( SAMPLE_RATE_MAP,
						    COMP_QUE_MAP );
  
  _check_bit_overlap< decltype( COMP_MODE_MAP   ),
		      decltype( COMP_POL_MAP    )>( COMP_MODE_MAP,
						    COMP_POL_MAP );
  _check_bit_overlap< decltype( COMP_MODE_MAP   ),
		      decltype( COMP_LAT_MAP    )>( COMP_MODE_MAP,
						    COMP_LAT_MAP );
  _check_bit_overlap< decltype( COMP_MODE_MAP   ),
		      decltype( COMP_QUE_MAP    )>( COMP_MODE_MAP,
						    COMP_QUE_MAP );
  
  _check_bit_overlap< decltype( COMP_POL_MAP    ),
		      decltype( COMP_LAT_MAP    )>( COMP_POL_MAP,
						    COMP_LAT_MAP );
  _check_bit_overlap< decltype( COMP_POL_MAP    ),
		      decltype( COMP_QUE_MAP    )>( COMP_POL_MAP,
						    COMP_QUE_MAP );
  
  _check_bit_overlap< decltype( COMP_LAT_MAP    ),
		      decltype( COMP_QUE_MAP    )>( COMP_LAT_MAP,
						    COMP_QUE_MAP );
  
  assert( gain_xlate_f.size() == gain_xlate_r.size());
  for( const auto& i : gain_xlate_f )
    assert( i.first == gain_xlate_r.at( i.second ));
  for( const auto& i : gain_xlate_r )
    assert( i.first == gain_xlate_f.at( i.second ));

  assert( rate_xlate_f.size() == rate_xlate_r.size());
  for( const auto& i : rate_xlate_f )
    assert( i.first == rate_xlate_r.at( i.second ));
  for( const auto& i : rate_xlate_r )
    assert( i.first == rate_xlate_f.at( i.second ));
  
  decltype( myConfigReg ) cfg = myConfigReg;
 
  for( const auto& [e,i] : gain_xlate_f ) {
    gain( static_cast< PGA_GAIN >( e ));
    assert( i_gain() == i );
    i_gain( i );
    assert( gain() == static_cast< PGA_GAIN >( e ));
  }
  
  for( const auto& [e,i] : rate_xlate_f ) {
    rate( static_cast< SAMPLE_RATE >( e ));
    assert( i_rate() == i );
    i_rate( i );
    assert( rate() == static_cast< SAMPLE_RATE >( e ));
  }    
  
  myConfigReg = cfg;
  _write_cfg();
  
  _LOG_VERB(( "Data structures bit map tests passed" ));
#endif
}


int32_t
ads1015::_write_cfg( void ) const noexcept {
  
  const uint8_t w_buf[] = { REG_CFG,
			    uint8_t(( myConfigReg & 0xff00 ) >> 8 ),
			    uint8_t(  myConfigReg & 0x00ff )
  };
  int32_t rVal = sizeof( w_buf );
    
  size_t w_num = _write( w_buf, sizeof( w_buf ));
  
  if( w_num != sizeof( w_buf )) {
    
    _LOG_WARN(( _id( "Failure to write to configuration register" ),
		"w_num=", w_num, ", val=",
		_vtoa( w_buf, sizeof( w_buf )), errno2str()));
    rVal = -1;
    
  } 
  
  return rVal;
}


int32_t
ads1015::_read_cfg(  void ) const noexcept {
  
  constexpr uint8_t w_buf[] = { REG_CFG };
            uint8_t r_buf[] = { 0x00, 0x00 };
  int32_t  rVal = -1;

  ssize_t w_num = _write( w_buf, sizeof( w_buf ));
  ssize_t r_num = _read(  r_buf, sizeof( r_buf ));
  
  if( w_num != sizeof( w_buf )) {
    
    _LOG_WARN(( _id( "Failure to set configuration register" ),
		"w_num=", w_num, "val=", _vtoa( w_buf, sizeof( w_buf )),
		errno2str()));
    
  } else
    if( r_num != sizeof( r_buf )) {
      
      _LOG_WARN(( _id( "Failure to read configuration register" ),
		  "r_num=", r_num, errno2str()));
      
    } else {
      
      // Convert the read data to a return value.  Note that the sign
      // is zero thereby indicating no error.
      
      for( int i = 0; i < r_num; ++i ) {
	rVal <<= 8;
	rVal |= r_buf[ i ];
      }
      rVal &= 0xffff;
      
    }
  
  return rVal;
}


const ads1015::OS
ads1015::os( void ) const noexcept {
  
  return static_cast< OS >
    ( _reg_read_map_cfg< OS_x >( OS_MAP ));
}


const ads1015::OS
ads1015::os( const OS o ) {
  
  if( o == OS::BEGIN ) {
    
    myConfigReg |= OS_MAP.at( OS_x::BEGIN );
    _write_cfg();
    myConfigReg &= ~OS_MAP.at( OS_x::BEGIN );
    
  }
  
  return os();
}


const ads1015::PGA_GAIN
ads1015::gain( void ) const noexcept {
  
  return static_cast< PGA_GAIN >
    ( _reg_read_map_cfg< PGA_GAIN_x >( PGA_GAIN_MAP ));
}


const ads1015::PGA_GAIN
ads1015::gain( const PGA_GAIN g ) {
  
  (void)_reg_write_map_cfg< PGA_GAIN_x >
    ( static_cast< PGA_GAIN_x >( g ), PGA_GAIN_MAP );
  
  return gain();
}


const int
ads1015::i_gain( void ) const noexcept {
  
  decltype( gain_xlate_f )::const_iterator
    it = gain_xlate_f.find( static_cast< PGA_GAIN_x >( gain()));

  assert( it != gain_xlate_f.end());

  return  it->second;
}


const int
ads1015::i_gain( const int i_g ) {
  
  decltype( gain_xlate_r )::const_iterator
    it = gain_xlate_r.find( i_g );

  if( it != gain_xlate_r.end())
    gain( static_cast< PGA_GAIN >( it->second ));
  else
    _LOG_ERR(( _id( "I am lost to set gain" ), ", from ", quote( i_g )));
  
  return i_gain();
}

  
const ads1015::MODE
ads1015::mode( void ) const noexcept {
  
  return static_cast< MODE >
    ( _reg_read_map_cfg< MODE_x >( MODE_MAP ));
}


const ads1015::MODE
ads1015::mode( const MODE m ) {
  
  (void)_reg_write_map_cfg< MODE_x >
    ( static_cast< MODE_x >( m ), MODE_MAP );
  
  return mode();
}


const ads1015::SAMPLE_RATE
ads1015::rate( void ) const noexcept {
  
  return static_cast< SAMPLE_RATE>
    ( _reg_read_map_cfg< SAMPLE_RATE_x >( SAMPLE_RATE_MAP ));
}


const ads1015::SAMPLE_RATE
ads1015::rate( const SAMPLE_RATE r ) {
  
  (void)_reg_write_map_cfg< SAMPLE_RATE_x >
    ( static_cast< SAMPLE_RATE_x >( r ), SAMPLE_RATE_MAP );
  
  return rate();
}


const int
ads1015::i_rate( void ) const noexcept {
  
  decltype( rate_xlate_f )::const_iterator
    it = rate_xlate_f.find( static_cast< SAMPLE_RATE_x >( rate()));
  
  assert( it != rate_xlate_f.end());
  
  return it->second;
}


const int
ads1015::i_rate( const int i_r ) {


  decltype( rate_xlate_r )::const_iterator
    it = rate_xlate_r.find( i_r );

  if( it != rate_xlate_r.end())
    rate( static_cast< SAMPLE_RATE >( it->second ));
  else    
    _LOG_ERR(( _id( "I am lost to set sample rate" ), ", from ",
	       quote( i_r )));
    
  return i_rate();
}


const ads1015::COMP_MODE
ads1015::comp_mode( void ) const noexcept {
  
  return static_cast< COMP_MODE >
    ( _reg_read_map_cfg< COMP_MODE_x >( COMP_MODE_MAP ));
}


const ads1015::COMP_MODE
ads1015::comp_mode( const COMP_MODE m ) {
  
  (void)_reg_write_map_cfg< COMP_MODE_x >
    ( static_cast< COMP_MODE_x >( m ), COMP_MODE_MAP );
  
  return comp_mode();
}


const ads1015::COMP_POL
ads1015::comp_pol( void ) const noexcept {
  
  return static_cast< COMP_POL >
    ( _reg_read_map_cfg< COMP_POL_x >( COMP_POL_MAP ));
}


const ads1015::COMP_POL
ads1015::comp_pol( const COMP_POL p ) {
  
  (void)_reg_write_map_cfg< COMP_POL_x >
    ( static_cast< COMP_POL_x >( p ), COMP_POL_MAP );
  
  return comp_pol();
}


const ads1015::COMP_LAT
ads1015::comp_lat( void ) const noexcept {
  
  return static_cast < COMP_LAT >
    ( _reg_read_map_cfg< COMP_LAT_x >( COMP_LAT_MAP ));
}

const ads1015::COMP_LAT
ads1015::comp_lat( const COMP_LAT l ) {
  
  (void)_reg_write_map_cfg< COMP_LAT_x >
    ( static_cast< COMP_LAT_x >( l ), COMP_LAT_MAP );
  
  return comp_lat();
}


const ads1015::COMP_QUE
ads1015::comp_que( void ) const noexcept {
  
  return static_cast< COMP_QUE >
    ( _reg_read_map_cfg< COMP_QUE_x >( COMP_QUE_MAP ));
}


const ads1015::COMP_QUE
ads1015::comp_que( const COMP_QUE q ) {
  
  (void)_reg_write_map_cfg< COMP_QUE_x >
    ( static_cast< COMP_QUE_x >( q ), COMP_QUE_MAP );
  
  return comp_que();
}


const std::tuple< ads1015::AINp_x, ads1015::AINn_x >
ads1015::_mux( void ) const noexcept {
  
            std::tuple< AINp_x, AINn_x >  rVal {
	      AINp_x::_NONE, AINn_x::_NONE
	    };
  constexpr std::tuple< AINp_x, AINn_x > _mask {
	      AINp_x::_MASK, AINn_x::_MASK
  };
  int32_t reg = _read_cfg();
  
  if( reg >= 0 ) {
      
    const uint16_t mask = MUX_MAP.at( _mask );
    
    for( const auto& [k,m] : MUX_MAP )
      if( k != _mask )
	if(( reg & mask ) == m ) {
	  std::get<0>( rVal ) = std::get<0>( k );
	  std::get<1>( rVal ) = std::get<1>( k );
	  break;
	}
    
  }
  
  return rVal;
}


const std::tuple< ads1015::AINp_x, ads1015::AINn_x >
ads1015::_mux( AINp_x p, AINn_x n ) {
  
  constexpr std::tuple< AINp_x, AINn_x > _mask {
    AINp_x::_MASK, AINn_x::_MASK
  };
  const std::tuple< AINp_x, AINn_x > _cmp  { p, n };
  
  myConfigReg &= ~MUX_MAP.at( _mask );
  
  for( const auto& [k,m] : MUX_MAP )
    if( k != _mask )
      if( _cmp == k ) {
	myConfigReg |= m;
	break;
      }
  int32_t reg = _write_cfg();
  
  if( reg < 0 )
    _LOG_WARN(( _id( "Unable to write to configuration register" ),
		errno2str()));
  
  return _mux();
}


const float
ads1015::operator[]( const CREG& r ) {
  
  int16_t rSamp = 0;
  float   rVal = 0.0;
  
  static const std::map
    < const CREG,
      const std::tuple< AINp_x, AINn_x >> conversion_mux_map {
	{ CREG::DIFF01, { AINp_x::AIN0, AINn_x::AIN1 }},
	{ CREG::DIFF03, { AINp_x::AIN0, AINn_x::AIN3 }},
	{ CREG::DIFF13, { AINp_x::AIN1, AINn_x::AIN3 }},
	{ CREG::DIFF23, { AINp_x::AIN2, AINn_x::AIN3 }},
	{ CREG::CHAN0,  { AINp_x::AIN0, AINn_x::GND  }},
	{ CREG::CHAN1,  { AINp_x::AIN1, AINn_x::GND  }},
	{ CREG::CHAN2,  { AINp_x::AIN2, AINn_x::GND  }},
	{ CREG::CHAN3,  { AINp_x::AIN3, AINn_x::GND  }}
  };
  
  _mux( std::get<0>( conversion_mux_map.at( r )),
	std::get<1>( conversion_mux_map.at( r )));
  os( OS::BEGIN );
  
  const int us_sleep_time =
    ( int(( 1.0 * 12.0 * 1000000.0 ) / float( i_rate())) + 1 );
  
  _LOG_VERB(( "thread=", _tid(), ", sleep=", us_sleep_time, "us",
	      ", rate=", i_rate()));
  
  std::this_thread::sleep_for( std::chrono::microseconds( us_sleep_time ));
  
  constexpr uint8_t w_buf[] = { REG_CONV };
            uint8_t r_buf[] = { 0x00, 0x00 };
    
  size_t w_num = _write( w_buf, sizeof( w_buf ));
  size_t r_num = _read(  r_buf, sizeof( r_buf ));

  if( w_num != sizeof( w_buf )) {
    
    _LOG_WARN(( _id( "Unable to select conversion register" ),
		"w_num=", w_num, "val=", _vtoa( w_buf, sizeof( w_buf )),
		errno2str()));
    
  } else {
    
    if( r_num != sizeof( r_buf )) {
      
      _LOG_WARN(( _id( "Failure to read conversion register" ),
		  "r_num=", r_num, errno2str()));
      
    } else {
      
      // I have a valid conversion.
      //
      // Now a bit of hokey-pokey.  
      
      rSamp   = ( r_buf[0] << 8 ) | r_buf[1];
      rSamp >>= 4;
      rSamp  &= 0x0fff;
      
      if( rSamp & 0b100000000000 )
	rSamp |= 0xf000;
      
      rVal  = ::roundf((( float( rSamp ) / 2047.0 ) * float( i_gain())));
      rVal /= 1000.0;
      
    }
  }
  
  return rVal;
}


std::ostream&
operator<<( std::ostream& os, ads1015::OS o ) {

  switch( o ) {
  case ads1015::OS::CONVERSION_READY: os << "Ready"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}

std::ostream&
operator<<( std::ostream& os, ads1015::PGA_GAIN g ) {

  switch( g ) { 
  case ads1015::PGA_GAIN::FS_6144: os << "6.144v"; break;
  case ads1015::PGA_GAIN::FS_4096: os << "4.096v"; break;
  case ads1015::PGA_GAIN::FS_2048: os << "2.048v"; break;
  case ads1015::PGA_GAIN::FS_1024: os << "1.024v"; break;
  case ads1015::PGA_GAIN::FS_512:  os << "0.512v"; break;
  case ads1015::PGA_GAIN::FS_256:  os << "0.256"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


std::ostream&
operator<<( std::ostream& os, ads1015::MODE m ) {

  switch( m ) {
  case ads1015::MODE::SINGLE_SHOT: os << "Single shot"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


std::ostream&
operator<<( std::ostream& os, ads1015::SAMPLE_RATE r ) {

  switch( r ) {
  case ads1015::SAMPLE_RATE::SR_128:  os << "128sps"; break;
  case ads1015::SAMPLE_RATE::SR_250:  os << "250sps"; break;
  case ads1015::SAMPLE_RATE::SR_490:  os << "490sps"; break;
  case ads1015::SAMPLE_RATE::SR_920:  os << "920sps"; break;
  case ads1015::SAMPLE_RATE::SR_1600: os << "1600sps"; break;
  case ads1015::SAMPLE_RATE::SR_2400: os << "2400sps"; break;
  case ads1015::SAMPLE_RATE::SR_3300: os << "3300sps"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


std::ostream&
operator<<( std::ostream& os, ads1015::COMP_MODE m ) {

  switch( m ) {
  case ads1015::COMP_MODE::TRADITIONAL: os << "Traditional"; break;
  case ads1015::COMP_MODE::WINDOW:      os << "Window"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


std::ostream&
operator<<( std::ostream& os, ads1015::COMP_POL p ) {

  switch( p ) {
  case ads1015::COMP_POL::ACTIVE_LOW:  os << "Active low"; break;
  case ads1015::COMP_POL::ACTIVE_HIGH: os << "Active high"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


std::ostream&
operator<<( std::ostream& os, ads1015::COMP_LAT l ) {
  
  switch( l ) {
  case ads1015::COMP_LAT::NON_LATCHING:  os << "Non-latching"; break;
  case ads1015::COMP_LAT::LATCHING:      os << "Latching"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


std::ostream&
operator<<( std::ostream& os, ads1015::COMP_QUE q ) {

  switch( q ) {
  case ads1015::COMP_QUE::ONE:     os << "One conversion"; break;
  case ads1015::COMP_QUE::TWO:     os << "Two conversions"; break;
  case ads1015::COMP_QUE::FOUR:    os << "Four conversions"; break;
  case ads1015::COMP_QUE::DISABLE: os << "Disabled"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}


std::ostream& operator<<( std::ostream& os,
                          const std::tuple< ads1015::AINp_x,
			                    ads1015::AINn_x >& m ) {

  switch( std::get<0>( m )) {
  case ads1015::AINp_x::AIN0: os << "AIN0:"; break;
  case ads1015::AINp_x::AIN1: os << "AIN1:"; break;
  case ads1015::AINp_x::AIN2: os << "AIN2:"; break;
  case ads1015::AINp_x::AIN3: os << "AIN3:"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  switch( std::get<1>( m )) {
  case ads1015::AINn_x::AIN1: os << "AIN0"; break;
  case ads1015::AINn_x::AIN3: os << "AIN3"; break;
  case ads1015::AINn_x::GND:  os << "GNDS"; break;
  default: os.setstate(std::ios_base::failbit);
  }
  return os;
}





