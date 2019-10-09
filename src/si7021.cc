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
 * $Log: si7021.cc,v $
 * Revision 1.11  2019/10/07 03:19:45  root
 * Minor const changes.
 *
 * Revision 1.10  2019/09/29 03:56:17  root
 * Various syntax and text updates.
 *
 * Revision 1.9  2019/09/28 02:19:06  root
 * Moved some static variables to inline.
 *
 * Revision 1.8  2019/09/27 07:41:57  root
 * Introduced "structured bindings." e=enum,i=int,f=float,b=bits,m=mask.
 *
 * Revision 1.7  2019/09/27 04:10:26  root
 * Cleaned up errno2str() calls.
 *
 * Revision 1.6  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.5  2019/09/21 09:11:19  root
 * Made use of utility functions.
 *
 * Revision 1.4  2019/09/14 05:24:12  root
 * Added some assert statements in _check().
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
 *
 */

extern "C" {

#include <assert.h>

}

#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include "si7021.h"
#include "log.h"
#include "util.h"


extern const std::vector< std::string > si7021_ident {
  _Si7021_H_ID, "$Id: si7021.cc,v 1.11 2019/10/07 03:19:45 root Exp $"
};


namespace i2c {

  // Save me some redundant typing (and some memory).

  static const std::map< float, int > heater_xlate {
    {  3.09, 0b0000 }, {  9.18, 0b0001 }, { 15.24, 0b0010 },
    { 27.39, 0b0100 }, { 51.69, 0b1000 }, { 94.20, 0b1111 }
  };

  
  si7021::si7021( void )
    : Dev( default_addr ) {
        
    _check();
    _doInit();
  }

  
  si7021::si7021( const u_char addr )
    : Dev( addr ) {
        
    _doInit();
  }

  
  si7021::si7021( const std::string& bus, const u_char addr )
    : Dev( bus, addr ) {
            
    _doInit();
  }

  
  si7021::~si7021( void ) {}

  
  bool
  si7021::_doInit( void ) {
    
    bool retVal = false;
    
    if( Dev::doInit()) {


    }

    return retVal;
  }

  
  void
  si7021::_check( void ) noexcept {

#ifdef _DPG_DEBUG
    uint8_t ctrl_reg = 0x00, heater_reg = 0x00;

    assert( _read_control_reg( ctrl_reg ) == 1 );
    assert( _read_heater_control_reg( heater_reg ) == 1 );

    assert( reset() == 0 );

    heater( false );
    assert( heater() == false );
    heater( true );
    assert( heater() == true );

    for( const auto& [f,b] : heater_xlate ) {
      assert( f == heater_level( f  ));
      assert( b == heater_level( b ));

      heater_level( b );
      assert( f == f_heater_level());

      heater_level( f );
      assert( b == heater_level());

    }

    assert( sn().size() == 8 );
    assert( firmware() != 0x00 );
    assert( f_firmware() != -1.0 );

    for( const auto& i :
	   { std::make_tuple< int, int > ( 0, 0 ),
	     std::make_tuple< int, int > ( 0, 1 ),
	     std::make_tuple< int, int > ( 1, 0 ),
	     std::make_tuple< int, int > ( 1, 1 )}) {

      std::tuple< int, int > r = resolution( std::get<0>( i ),
					     std::get<1>( i ));
      assert( i == r );
      assert( i == resolution());

      static constexpr std::tuple<int, int> one   { 12, 14 },
	                                    two   {  8, 12 },
	                                    three { 10, 13 },
	                                    four  { 11, 11 };
      
      if(( std::get<0>(i) == 0 ) && ( std::get<1>(i) == 0 ))
	assert( b_resolution() == one );
      if(( std::get<0>(i) == 0 ) && ( std::get<1>(i) == 1 ))
        assert( b_resolution() == two );
      if(( std::get<0>(i) == 1 ) && ( std::get<1>(i) == 0 ))
        assert( b_resolution() == three );
      if(( std::get<0>(i) == 1 ) && ( std::get<1>(i) == 1 ))
        assert( b_resolution() == four );

    }

    assert( status());
    
    _write_control_reg( ctrl_reg );
    _write_heater_control_reg( heater_reg );
    
    _LOG_VERB(( "Data structures bit map tests passed" ));
#endif
  }


  int
  si7021::reset ( void ) {

    int rVal = -1;
    
    if(  Dev::reset() == 0 ) {

      static const uint8_t w_buf[] = { 0xfe };
      
      if( _write( w_buf, sizeof( w_buf )) != sizeof( w_buf )) {
	
	_LOG_WARN(( _id( "Unable to RESET device" ), errno2str()));
	
      } else
	rVal = 0;

      std::this_thread::sleep_for( std::chrono::milliseconds( 15 ));
      
    }
    
    return rVal;
  }

  const std::vector< uint8_t >
  si7021::sn( void ) const noexcept {

    constexpr uint8_t w_buf1[] = { 0xfa, 0x0f },
                      w_buf2[] = { 0xfc, 0xc9 };
              uint8_t  r_buf[] = { 0x00, 0x00, 0x00, 0x00 };

    std::vector< uint8_t > rVal;

    ssize_t w_num, r_num;

    // Lets get the first part.
    
    w_num = _write( w_buf1, sizeof( w_buf1 ));
    r_num = _read(  r_buf,  sizeof( r_buf ));

    _LOG_VERB(( _id( "" ),
		"w=", w_num, " ", _vtoa( w_buf1, sizeof( w_buf1 ))));

    if( w_num != sizeof( w_buf1 )) {
      
      _LOG_WARN(( _id( "Failure to write request for part 1 of SN" ),
                  ", w_num=", w_num, ", vals: ",
		  _vtoa( w_buf1, sizeof( w_buf1 )), errno2str()));

    } else 
      if( r_num != sizeof( r_buf )) {

        _LOG_WARN(( _id( "Failure to read part 1 of SN" ),
		    ", r_num=", r_num, errno2str()));
      
      } else {

	// Got the first four bytes.
	
	for( size_t i = 0; i < sizeof( r_buf ); ++i )
	  rVal.push_back( r_buf[i]);
	
	// Lets get the second part.
	
	w_num = _write( w_buf2, sizeof( w_buf2 ));
	r_num = _read(  r_buf,  sizeof( r_buf ));

	if( w_num != sizeof( w_buf2 )) {
	  
	  _LOG_WARN(( _id( "Failure to write request for part 2 of SN" ),
		      ", w_num=", w_num, ", vals: ",
		      _vtoa( w_buf2, sizeof( w_buf2 )), errno2str()));

	} else
	  if( r_num != sizeof( r_buf )) {
	    
	    _LOG_WARN(( _id( "Failure to read part 2 of SN" ),
			", r_num=", r_num, errno2str()));
	    
	  } else {

	    // Got the second four bytes.
	    
	    for( size_t i = 0; i < sizeof( r_buf ); ++i )
	      rVal.push_back( r_buf[i]);

	  }
      }

    // If there was an error then empty the return buffer.
    
    if( rVal.size() != 8 )
      rVal.clear();
    else
      assert( rVal.size() == 8 );

    _LOG_VERB(( _id( "" ), "len=", rVal.size(), ", vals: ", _vtoa( rVal )));
    
    return rVal;
  }


  const std::string
  si7021::s_sn( void ) const noexcept {

    const std::vector< uint8_t > serial = sn();
          std::stringstream   s;

    if( serial.size()) {

      if(( serial[0] == 0x00 ) || ( serial[0] == 0xff ))
	s << "Engineering sample";
      else
	if( serial[0] == 0x0d )
	  s << "Si7013";
	else
	  if( serial[0] == 0x14 )
	    s << "Si7020";
	  else
	    if( serial[0] == 0x15 )
	      s << "Si7021";
	    else
	      s<< "Unknown=0x" << t2hex( serial[0]);
      
    }

    return s.str();
  }

  const uint8_t
  si7021::firmware( void ) const noexcept {

    constexpr uint8_t w_buf[] = { 0x84, 0xb8 };
              uint8_t r_buf[] = { 0x00 };
    
    ssize_t w_num = _write( w_buf, sizeof( w_buf ));
    ssize_t r_num = _read(  r_buf, sizeof( r_buf ));

    uint8_t rVal = 0; /* INVALID */

    if( w_num != sizeof( w_buf )) {

      _LOG_WARN(( _id( "Failure to ask for firmware revision" ),
		  ", w_num=", w_num, errno2str()));

    } else 
      if( r_num != sizeof( r_buf )) {
	
	_LOG_WARN(( _id( "Failure to fetch firmware revision" ),
		    ", r_num=", r_num,  errno2str()));
	
      } else
	rVal = r_buf[0];

    return rVal;
  }


  const float si7021::f_firmware( void ) const noexcept {

    float   rVal = -1.0;
    uint8_t fw = firmware();

    if( fw == 0xff )
      rVal = 1.0;
    if( fw == 0x20 )
      rVal = 2.0;

    return rVal;
  }
  

  const int32_t
  si7021::_read_control_reg( uint8_t& reg ) const noexcept {

    constexpr uint8_t w_buf[] = { 0xe7 };
              uint8_t r_buf[] = { 0x00 };

    int32_t rVal = -1;

    ssize_t w_num = _write( w_buf, sizeof( w_buf ));
    ssize_t r_num = _read(  r_buf, sizeof( r_buf ));

    if( w_num != sizeof( w_buf )) {

      _LOG_WARN(( _id( "Failure to ask for control register" ),
                  ", w_num=", w_num, ", vals: ",
		  _vtoa( w_buf, sizeof( w_buf )), errno2str()));

    } else
      if( r_num != sizeof( r_buf )) {

        _LOG_WARN(( _id( "Failure to read control register" ),
                    ", r_num=", r_num, errno2str()));

      } else {

	// Return the control register value.
	
	reg = r_buf[0];
	rVal = sizeof( r_buf );
	
      }

    return rVal;
  }


  const int32_t
  si7021::_read_heater_control_reg( uint8_t& reg ) const noexcept {

    constexpr uint8_t w_buf[] = { 0x11 };
              uint8_t r_buf[] = { 0x00 };

    int32_t rVal = -1;

    ssize_t w_num = _write( w_buf, sizeof( w_buf ));
    ssize_t r_num = _read(  r_buf, sizeof( r_buf ));

    if( w_num != sizeof( w_buf )) {

      _LOG_WARN(( _id( "Failure to write ask for heater control register" ),
                  ", w_num=", w_num, ", vals: ",
		  _vtoa( w_buf, sizeof( w_buf )), errno2str()));

    } else
      if( r_num != sizeof( r_buf )) {

        _LOG_WARN(( _id( "Failure to read heater control register" ),
                    ", r_num=", r_num, errno2str()));

      } else {

        // Return the heater control register value.

        reg = r_buf[0];
        rVal = sizeof( r_buf );

      }

    return rVal;
  }
  

  const int32_t
  si7021::_write_control_reg( uint8_t& reg ) const noexcept {

    int32_t rVal = -1;

    const uint8_t w_buf[] = { 0xe6, reg };

    ssize_t w_num = _write( w_buf, sizeof( w_buf ));

    if( w_num != sizeof( w_buf )) {

      _LOG_WARN(( _id( "Unable to write to control register" ),
		  ", w_num=", w_num, ", vals: ",
		  _vtoa( w_buf, sizeof( w_buf )), errno2str()));
      
    } else
      rVal = int32_t( w_num );
      
    return rVal;
  }

  
  const int32_t
  si7021::_write_heater_control_reg( uint8_t& reg ) const noexcept {

    int32_t rVal = -1;

    const uint8_t w_buf[] = { 0x51, reg };

    ssize_t w_num = _write( w_buf, sizeof( w_buf ));
    
    if( w_num != sizeof( w_buf )) {

      _LOG_WARN(( _id( "Unable to write to heater control register" ),
                  ", w_num=", w_num, ", vals: ",
		  _vtoa( w_buf, sizeof( w_buf )), errno2str()));
        
    } else
      rVal = int32_t( w_num );

    return rVal;
  }

  
  const bool
  si7021::heater( void ) const noexcept {

    uint8_t reg = 0x00;

    (void)_read_control_reg( reg );

    return ( reg & 0b00000100 ) ? true : false;
  }


  const bool
  si7021::heater( const bool state ) const noexcept {

    uint8_t reg = 0x00;

    if( _read_control_reg( reg ) > 0 ) {

      // Set or clear the heater bit.
      
      reg &= 0b11111011;
      if( state )
	reg |= 0b00000100;

      (void)_write_control_reg( reg );

    }
    
    return heater();
  }

  
  const int
  si7021::heater_level( void ) const noexcept {

    int     rVal = -1;
    uint8_t reg  = 0x00;

    if( _read_heater_control_reg( reg ) > 0 )
      rVal = reg & 0b0001111;

    return rVal;
  }


  const int
  si7021::heater_level( const int level ) const noexcept {

    assert(( level >= 0 ) && ( level <= 15 ));
    
    uint8_t reg  = 0x00;

    if( _read_heater_control_reg( reg ) > 0 ) {

      reg &= 0b11110000;
      reg |= uint8_t( level );

      (void)_write_heater_control_reg( reg );

    }

    return heater_level();
  }


  const float
  si7021::f_heater_level( void ) const noexcept {

    const int   level = heater_level();
          float rVal = -1.0;

    for( const auto& [f,b] : heater_xlate ) 
      if( level == b ) 
	rVal = f;

    return rVal;
  }


  const float
  si7021::heater_level( const float level ) const noexcept {

    float rVal = -1.0;
    
    decltype( heater_xlate )::const_iterator it = heater_xlate.find( level );
    if( it == heater_xlate.end()) {

      _LOG_ABORT(( _id( "Unknown level" ), "l=", level ));

    } else {

      if( heater_level( it->second ) >= 0 )
	rVal = f_heater_level();
      
    }
    
    return rVal;
  }


  const std::tuple< int, int >
  si7021::resolution( void ) const noexcept {

    std::tuple< int, int > rVal = { -1, -1 };
    uint8_t                reg  = 0x00;

    if( _read_control_reg( reg ) > 0 ) {

      std::get<0>( rVal ) = 0;
      std::get<1>( rVal ) = 0;

      if( reg & 0b10000000 )
	std::get<0>( rVal ) = 1;
      if( reg & 0b00000001 )
        std::get<1>( rVal ) = 1;

      _LOG_DEBUG(( _id( "A/D resolutions" ),
		   std::get<0>( rVal ), std::get<1>( rVal )));
      
    } else
      _LOG_WARN(( _id( "Unable to determine resolution" )));

    return rVal;
  }


  const std::tuple< int, int >
  si7021::resolution( const int rh, const int temp ) const noexcept {

    std::tuple< int, int > rVal = { -1, -1 };
    uint8_t                reg  = 0x00;

    if( _read_control_reg( reg ) > 0 ) {

      reg &= 0b01111110;

      if( rh )
	reg |= 0b10000000;
      if( temp )
	reg |= 0b00000001;

      if( _write_control_reg( reg ) > 0 )
	rVal = resolution();
      else
	_LOG_WARN(( _id( "Unable to set resolution" ), rh, temp ));

    }    

    return rVal;
  }


  const std::tuple< int, int >
  si7021::b_resolution( void ) const noexcept {

    std::tuple< int, int > rVal = resolution();

    if(( std::get<0>( rVal ) != -1 ) && ( std::get<1>( rVal ) != -1 )) {

      if(( std::get<0>( rVal ) == 0 ) && ( std::get<1>( rVal ) == 0 ))
	rVal = std::make_tuple< int, int >( 12, 14 );
      if(( std::get<0>( rVal ) == 0 ) && ( std::get<1>( rVal ) == 1 ))
	rVal = std::make_tuple< int, int >(  8, 12 );
      if(( std::get<0>( rVal ) == 1 ) && ( std::get<1>( rVal ) == 0 ))
        rVal = std::make_tuple< int, int >( 10, 13 );
      if(( std::get<0>( rVal ) == 1 ) && ( std::get<1>( rVal ) == 1 ))
        rVal = std::make_tuple< int, int >( 11, 11 );

    }

    return rVal;
  }


  const bool
  si7021::status( void ) const noexcept {

    bool    rVal = false;
    uint8_t reg  = 0x00;
    
    if( _read_control_reg( reg ) > 0 ) {
      
      if(( reg & 0b01000000 ) == 0 )
	rVal = true;
      
    } else
      _LOG_WARN(( _id( "Unable to determine device status" )));
    
    _LOG_VERB(( _id ( "" ), "status=", rVal ));

    return rVal;
  }


  const float
  si7021::t( void ) const noexcept {

    constexpr uint8_t  w_buf[] = { 0xe3 };
              uint8_t  r_buf[] = { 0x00, 0x00, 0x00 };
              float       rVal = -1;

    ssize_t w_num = _write( w_buf, sizeof( w_buf ));
    ssize_t r_num = _read(  r_buf, sizeof( r_buf ));

    if( w_num != sizeof( w_buf )) {

      _LOG_WARN(( _id( "Unable to write temperature command" ),
		  ", w_num=", w_num, ", vals: ",
                  _vtoa( w_buf, sizeof( w_buf )), errno2str()));

    } else
      if( r_num != sizeof( r_buf )) {

        _LOG_WARN(( _id( "Failure to read temperature" ),
                    ", r_num=", r_num, errno2str()));

      } else {

	rVal = float( u_int( r_buf[0] << 8 ) + u_int( r_buf[1]));
	rVal *= ( 175.72 / 65536.0 );
	rVal -= 46.85;

      }

    _LOG_VERB(( _id( "" ), 
		", w_len=", sizeof( w_buf ),
		", w: ", _vtoa( w_buf, sizeof( w_buf )),
		", r_len=", sizeof( r_buf ),
		", r: ", _vtoa( r_buf, sizeof( r_buf )),
		", t=", rVal ));

    return rVal;
  }
  

  const float
  si7021::h( void ) const noexcept {

    constexpr uint8_t  w_buf[] = { 0xe5 };
              uint8_t  r_buf[] = { 0x00, 0x00, 0x00 };
              float       rVal = -1;

    ssize_t w_num = _write( w_buf, sizeof( w_buf ));
    ssize_t r_num = _read(  r_buf, sizeof( r_buf ));

    if( w_num != sizeof( w_buf )) {

      _LOG_WARN(( _id( "Unable to write humidity command" ),
                  ", w_num=", w_num, ", vals: ",
                  _vtoa( w_buf, sizeof( w_buf )), errno2str()));
      
    } else
      if( r_num != sizeof( r_buf )) {
	
        _LOG_WARN(( _id( "Failure to read humidity" ),
                    ", r_num=", r_num, errno2str()));

      } else {

	rVal = float(( int( r_buf[0]) << 8 ) | int( r_buf[1]));
	rVal *= ( 125.0 / 65536.0 );
	rVal -= 6.0;

      }

    _LOG_VERB(( _id( "" ),
                ", w_len=", sizeof( w_buf ),
                ", w: ", _vtoa( w_buf, sizeof( w_buf )),
                ", r_len=", sizeof( r_buf ),
                ", r: ", _vtoa( r_buf, sizeof( r_buf )),
                ", h=", rVal ));

    return rVal;
  }

	
}
