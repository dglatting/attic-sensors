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
 * This is an expressive utility to develop C++ code for various i2c
 * devices but with specific purposes.
 *
 * I have a Raspberry PI 3B with two ads1015 A/D converters (one is on
 * a Pimoroni Enviro pHAT), a six character display (a Pimoroni Micro
 * Dot pHAT), and a temperature/humidity sensor (an Adafruit
 * Si7021). Attached to the A/Ds are six volatile gas sensors: a MQ2,
 * MQ3, MQ4, MQ6, MQ7, and a MQ9 from sparkfun. The remaining two A/D
 * inputs are connected to GND and Vdd.
 *
 * The goals of this project include:
 *  1) Have fun,
 *  2) Learn i2c interaction in C++ vs Python,
 *  3, Explore C++17,
 *  4) Learn of any gasses in my attic, and 
 *  5) Map those sensors to Munin.
 * 
 * I plan to write certain code for fun rather than function or
 * performance. This includes displaying sensor data on a six
 * character LED dot matrix display, which no one will see because the
 * Raspberry PI and i2c devices will be in my attic. I am also looking
 * at compensating gas sensor readings against temperature and
 * humidity because my attic is an attic - it gets hot and cold and it
 * is vented.
 *
 * This code is multi-threaded because that makes sense to me, and
 * it's fun. There will be an architecture document in the git.
 *
 *
 * $Log: main.cc,v $
 * Revision 1.30  2019/10/07 03:42:36  root
 * Shortened the function quoteStr().
 *
 * Revision 1.29  2019/10/07 00:14:21  root
 * Mostly textual clean-up but some minor code changes.
 *
 * Revision 1.28  2019/10/06 08:36:34  root
 * The Munin thread now behaves for exit.
 *
 * Revision 1.27  2019/10/06 06:38:56  root
 * Added output of the Vcc A/D pin and simplified some code.
 *
 * Revision 1.26  2019/10/06 06:05:55  root
 * Added PID store code to support start/stop scripts. There remains
 * one flaw in that code to be fixed (i.e., Munin thread join()).
 *
 * Revision 1.25  2019/10/02 05:37:56  root
 * Changed main loop delay from one to five seconds.
 *
 * Revision 1.24  2019/10/01 03:58:18  root
 * Slope adjustment of two sensors.
 *
 * Revision 1.23  2019/09/30 20:37:53  root
 * I MAY have the sensor slopes correct, now. Moved the sensor math
 * into a template. Turned the t/h heater off - it adds about 3deg to
 * the temp reading.
 *
 * Revision 1.22  2019/09/30 09:27:24  root
 * Beginings of calculating PPM and mg/L.
 *
 * Revision 1.21  2019/09/29 04:39:37  root
 * Moved some code around.
 *
 * Revision 1.20  2019/09/29 03:56:59  root
 * Moved path to sensor pipe and added chmod() to the pipe.
 *
 * Revision 1.19  2019/09/27 07:41:57  root
 * Introduced "structured bindings." e=enum,i=int,f=float,b=bits,m=mask.
 *
 * Revision 1.18  2019/09/27 04:07:56  root
 * Wrote/tested the Munin thread. Moved some code around. deleted
 * bit_shift_left() as redundant.
 *
 * Revision 1.17  2019/09/26 04:48:02  root
 * Removed __ signed/unsigned integer decls.
 *
 * Revision 1.16  2019/09/25 04:01:25  root
 * The screen saver now auto switches after one hour. Display of
 * sensors now implemented.
 *
 * Revision 1.15  2019/09/24 08:00:24  root
 * Started to add code to display values.
 *
 * Revision 1.14  2019/09/23 06:29:36  root
 * When flipping the display saver, my "fix" was wrong.
 *
 * Revision 1.13  2019/09/23 01:47:39  root
 * Moved the display saver to a thread.
 *
 * Revision 1.12  2019/09/18 06:09:14  root
 * Start of Micro Dot pHAT debugging.
 *
 * Revision 1.11  2019/09/16 23:18:06  root
 * Updated to interace with more devices.
 *
 * Revision 1.10  2019/09/07 07:05:06  root
 * Started to add code for Si7021. Unfinished.
 *
 * Revision 1.9  2019/09/04 04:23:47  root
 * Added MIT copyright notice.
 *
 * Revision 1.8  2019/09/02 08:50:28  root
 * Converted to multiple threads.
 *
 * Revision 1.7  2019/09/01 04:37:46  root
 * *** empty log message ***
 *
 * Revision 1.6  2019/08/29 07:06:40  root
 * Got rid of the SMBUS code - it doesn't work.
 * Added more functions to the ads1015.
 * Code now freaking works.
 *
 * Revision 1.5  2019/08/28 07:57:21  root
 * Daily check-in. Things improving. Kernel trap gone.
 *
 * Revision 1.4  2019/08/27 07:09:13  root
 * It crashes the kernel, so that's nice.
 *
 * Revision 1.3  2019/08/27 02:41:34  root
 * Does anyone know why I named the dev file with a capital D? Stupid.
 *
 * Revision 1.2  2019/08/27 01:01:34  root
 * Adding additional utilities to source.
 *
 * Revision 1.1  2019/08/26 19:37:49  root
 * Initial revision
 *
 */

extern "C" {

#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
  
#include <linux/i2c-dev.h>
  
}

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include "ads1015.h"
#include "si7021.h"
#include "microdotphat.h"
#include "log.h"
#include "opts.h"
#include "dev.h"


static const std::string main_ident = "$Id: main.cc,v 1.30 2019/10/07 03:42:36 root Exp root $";


// Since this code is multi-threaded the i2c bus must locked for each
// transaction.

std::mutex i2c_bus;


// Where the PID file is cached.

static const std::string pid_file {
  "/var/run/sensors.pid"
};


// Where the Munin pipe is located.

static const std::string pipe_path {
  "/var/run/sensors"
};


// The gas sensor readings in PPM or mg/L and an associated condition
// variable that is signaled when updated.
//
// The sensor atomics are updated approximately once a second. It
// isn't expected that missing one or two updates due to a busy thread
// is of any concern.


std::atomic< float >  MQ2ppm(0), MQ3mgl(0), MQ4ppm(0),
                      MQ6ppm(0), MQ7ppm(0), MQ9ppm(0),
                      Vdd( 0 );
std::condition_variable MQcv;


// These are the two ADS1015 A/D converters. They are connected as
// follows:
//
// ad1, input 0 - MQ2
// ad1, input 1 - MQ4
// ad1, input 2 - MQ6
// ad1, input 3 - MQ3
// ad2, input 0 - MQ7
// ad2, input 1 - MQ9
// ad2, input 2 - GND
// ad2, input 3 - Vdd

i2c::ads1015 ad1, ad2( 0x48 );


// This is the temperature/humidity sensor.

i2c::si7021 th;


// Do this when the program exists. It's just a little house
// keeping. It may be called multiple times.
//
// This MUST be a simple routine because on exit some objects may be
// deconstructed.

void
clean_up( void ) {

  _LOG_VERB(( "Executing a clean exit." ));
  
  ::unlink( pid_file.c_str());
  ::unlink( pipe_path.c_str());
  
}


// When this atomic is set, threads should try to cleanly exit.
  
std::atomic< bool > doExit { false };


// Signal handling.

void
exit_signal_handler( int sig ) {

  switch( sig ) {

    // These signals indicate the process is to politely exit.
    
  case SIGHUP:  [[fallthrough]];
  case SIGQUIT: [[fallthrough]];
  case SIGKILL: [[fallthrough]];
  case SIGTERM:

    doExit.store( true );
    MQcv.notify_all();
    
    _LOG_INFO(( "Signal=", sig, " indicating clean exit" ));

    break;
    
    // WTF?

  default:

    _LOG_WARN(( "Unexptected signal=", sig, " ignored" ));

    break;
    
  }
}


// Because the sensor output line is used in multiple places, build it
// in this one place.

const std::string
sensor_line( void ) {

  std::stringstream              ss;
  std::unique_lock< std::mutex > lck( i2c_bus );

  ss << std::fixed << std::setprecision(2)
     << "t=" << roundz( th.t(), 2 ) << " "
     << "h=" << roundz( th.h(), 2 ) << "  "
     << std::fixed << std::setprecision(3)
     << "MQ2=" << MQ2ppm.load() << " "
     << "MQ3=" << MQ3mgl.load() << " "
     << "MQ4=" << MQ4ppm.load() << " "
     << "MQ6=" << MQ6ppm.load() << " "
     << "MQ7=" << MQ7ppm.load() << " "
     << "MQ9=" << MQ9ppm.load() << " "
     << "Vdd=" << Vdd.load();

    return ss.str();
}


// This template is used in a thread responsible for updating the MQ
// sensor atomics. That thread smooths the A/D values through a
// running average to minimize power supply spikes. Specifically, some
// power supplies are noisy pieces of crap and which gets reflected in
// sampling. This template approximates the sensor value against a
// slope in the sensor's log-log plot.

template< typename F >
F
sensor( const F X0, const F F0,
	const F X1, const F F1,
	const F volts ) {

  const F slope = std::log10( F1 / F0 ) / std::log10( X1 / X0 );

  return roundz(( F0 * std::pow(( volts / X0 ), slope )), 3 );
}


void
MQx_update_sensor_thread( void ) {

  // "Sensor map"
  //
  // It's a little complex but the goal is to simplify the code in
  // terms of line count. By creating a complex data structure I can
  // eliminate a bunch of if/else/else code.
  //
  // The index into the map is a sensor identifier - an enumerated
  // type. The values are the sensor's name, running accumulator, N
  // prior samples, and A/D information.
  
  enum class QUE { MQ2, MQ3, MQ4, MQ6, MQ7, MQ9, Vdd };
  
  std::map
    < QUE, std::tuple
      < std::string,         // Printable name
	float,               // Accumulator
	std::queue< float >, // Samples queue
	i2c::ads1015::CREG,  // Which register to find the sample
	int                  // Which A/D (1 or 2)
	>
      > sensor_map;
#define SENSOR_NAME(x) std::get<0>(x)
#define SENSOR_ACC(x)  std::get<1>(x)
#define SENSOR_QUE(x)  std::get<2>(x)
#define SENSOR_REG(x)  std::get<3>(x)
#define SENSOR_AD(x)   std::get<4>(x)
  
#define SMOOTH_LEN 32

  // Initialize the sensor map.
  
  sensor_map[ QUE::MQ2 ] = { "MQ2", 0.0, std::queue< float >(),
                             i2c::ads1015::CREG::CHAN0, 1 };
  sensor_map[ QUE::MQ3 ] = { "MQ3", 0.0, std::queue< float >(),
			     i2c::ads1015::CREG::CHAN1, 1 };
  sensor_map[ QUE::MQ4 ] = { "MQ4", 0.0, std::queue< float >(),
			     i2c::ads1015::CREG::CHAN2, 1 };
  sensor_map[ QUE::MQ6 ] = { "MQ6", 0.0, std::queue< float >(),
			     i2c::ads1015::CREG::CHAN3, 1 };
  sensor_map[ QUE::MQ7 ] = { "MQ7", 0.0, std::queue< float >(),
			     i2c::ads1015::CREG::CHAN0, 2 };
  sensor_map[ QUE::MQ9 ] = { "MQ9", 0.0, std::queue< float >(),
			     i2c::ads1015::CREG::CHAN1, 2 };
  sensor_map[ QUE::Vdd ] = { "Vdd", 0.0, std::queue< float >(),
                             i2c::ads1015::CREG::CHAN3, 2 };

  // Run the loop every second.
  
  constexpr std::chrono::duration loop_duration = std::chrono::seconds( 1 );

  while( doExit.load() == false ) {

    _LOG_VERB(( "Awake" ));

    // Start of loop time point.
    
    const std::chrono::time_point<std::chrono::system_clock>
      start_tick = std::chrono::system_clock::now();

    for( auto& [id,m] : sensor_map ) {
      
      std::unique_lock< std::mutex > lck( i2c_bus );

      float samp = 0.0;

      // Get the sample.                                                      

      if( SENSOR_AD( m ) == 1 ) 
	samp = ad1[ SENSOR_REG( m )];
      else
	if( SENSOR_AD( m ) == 2 )
	  samp = ad2[ SENSOR_REG( m )];
	else
	  _LOG_ABORT(( "Impossible state" ));
	  
      // Be nice to other threads wanting the i2c bus.
      
      lck.unlock();
      
      // Add the sample to the queue and accumulator.
      
      SENSOR_QUE( m ).push( samp );
      SENSOR_ACC( m ) += samp;
      
      // Time to process samples?
      
      if( SENSOR_QUE( m ).size() > SMOOTH_LEN ) {
	
	// Remove the oldest sample from the running average.
	
	SENSOR_ACC( m ) -= SENSOR_QUE( m ).front();
	
	// Delete the oldest sample from the queue.
	  
	SENSOR_QUE( m ).pop();

	// Calculate the voltage on the A/D input.

	const float volts = ( SENSOR_ACC( m ) / float( SMOOTH_LEN ));

	// Now, update the global values.
	
	if( id == QUE::MQ2 ) {

	  constexpr float
              F0 =    0.8, F1 =   1.7,
	      X0 = 1000.0, X1 = 200.0;

	  MQ2ppm.store( sensor<float>( X0, F0, X1, F1, volts ));

	  _LOG_VERB(( "X0=", X0, ",X1=", X1,
		      ",F0=", F0 , ",F1=", F1,
		      ",MQ2 ppm=", MQ2ppm.load()));
	    
	} else
	  if( id == QUE::MQ3 ) {

	    constexpr float
	      F0 = 2.25, F1 = 0.53,
	      X0 = 0.10, X1 = 1.0;
	    
	    MQ3mgl.store( sensor<float>( X0, F0, X1, F1, volts ));

	    _LOG_VERB(( "X0=", X0, ",X1=", X1,
			",F0=", F0 , ",F1=", F1,
			",MQ3 mg/L=", MQ3mgl.load()));

	  } else
	    if( id == QUE::MQ4 ) {

	      constexpr float
		F0 =    1.0, F1 =   1.8,
		X0 = 1000.0, X1 = 200.0;

	      MQ4ppm.store( sensor<float>( X0, F0, X1, F1, volts ));

	      _LOG_VERB(( "X0=", X0, ",X1=", X1,
			  ",F0=", F0 , ",F1=", F1,
			  ",MQ4 ppm=", MQ4ppm.load()));

	    } else
	      if( id == QUE::MQ6 ) {

		constexpr float
		  F0 =    1.0, F1 =   2.1,
		  X0 = 1000.0,  X1 = 200.0;
		
		MQ6ppm.store( sensor<float>( X0, F0, X1, F1, volts ));

		_LOG_VERB(( "X0=", X0, ",X1=", X1,
			    ",F0=", F0 , ",F1=", F1,
			    ",MQ9 ppm=", MQ6ppm.load()));
		
	      } else
		if( id == QUE::MQ7 ) {

		  constexpr float
		    F0 =    0.225, F1 =   1.8,
		    X0 = 1000.0,   X1 = 36.0;

		  MQ7ppm.store( sensor<float>( X0, F0, X1, F1, volts ));

		  _LOG_VERB(( "X0=", X0, ",X1=", X1,
			      ",F0=", F0 , ",F1=", F1,
			      ",MQ7 ppm=", MQ9ppm.load()));

		} else
		  if( id == QUE::MQ9 ) {

		    constexpr float
		      F0 =    1.00, F1 =   2.1,
		      X0 = 1000.0,  X1 = 200.0;

		    MQ9ppm.store( sensor<float>( X0, F0, X1, F1, volts ));

		    _LOG_VERB(( "X0=", X0, ",X1=", X1,
				",F0=", F0 , ",F1=", F1,
				",MQ9 ppm=", MQ9ppm.load()));
		    
		  } else
		    if( id == QUE::Vdd ) {

		      Vdd.store( volts );
		      
		    } else
		      _LOG_ABORT(( "Impossible state" ));
	
      }
    }
    
    // The sensors have been update. Let anyone who wants to know,
    // know.
    
    MQcv.notify_all();

    // End of loop time point.
    
    const std::chrono::time_point<std::chrono::system_clock>
      stop_tick = std::chrono::system_clock::now();

    // Calculate the difference between the stop and start times,
    // subtract that from one second, and then delay for that amount
    // of time.
    
    auto slp = loop_duration - ( stop_tick - start_tick );

    if( slp < std::chrono::seconds( 0 ))
      slp = loop_duration;

    _LOG_VERB(( "tid=", _tid(), " sleeping for ",
		std::chrono::duration_cast
		<std::chrono::milliseconds>( slp ).count(), "ms" ));
    
    std::this_thread::sleep_for( slp );

  }

  _LOG_VERB(( "exiting" ));
}


void
display_update_thread( void ) {

  MicroDotpHAT disp;

  disp.clear();
  disp.show();

  // These declerations are for the display saver.
  
  bool decimal_or_col    = false;  // Doing cols=0, decimal=1
  bool          saver_on = false;  // Do display (and power) saver.

  // When to start the screen saver.
  
            std::chrono::duration
	      saver_countdown = std::chrono::seconds( 4 * 60 * 60 );
  constexpr std::chrono::duration
              loop_duration = std::chrono::seconds( 2 );

  while( doExit.load() == false ) {

    _LOG_VERB(( "Awake" ));

    // Start of loop time point.
    
    const std::chrono::time_point<std::chrono::system_clock>
      start_tick = std::chrono::system_clock::now();

    disp.clear();
    disp.set_brightness( 64 );
    
    // Time to do the screen saver?

    if( saver_on ) {

      std::unique_lock< std::mutex > lck( i2c_bus );

      if( decimal_or_col == false ) {

	// Columns.

	static int     col = 0, last_col = disp.num_cols() - 1;
	static uint8_t flip = 0xaa;

	// Light the column.
	
	disp.set_col( col, flip );

	// Unlight the prior column.
	
	if( last_col >= 0 )
	  disp.set_col( last_col, 0 );

	// Bump to the next column and flip the pattern.

	last_col = col;
	col      = (( col +1 ) % disp.num_cols());
	flip     = ~flip;

	// If we wrapped, then do decimal points.
	
	if( col == 0 )
          decimal_or_col = !decimal_or_col;

      } else {

	// Decimal point.

	static int digit = 0;

	// Light a digit's decimal point.
	
	disp.set_decimal( digit, true );

	// Bump to the next digit for the next loop.
	
	digit = (( digit + 1 ) % disp.num_digits());

	// If we wrapped then do columns.
	
	if( digit == 0 )
	  decimal_or_col = !decimal_or_col;
	
      }
    } else {

      // Do non-screensaver work. Non-screensaver work is to loop
      // through the sensors.
      
      static int func = 0;

      std::unique_lock< std::mutex > lck( i2c_bus );
      std::stringstream ss;
      
      switch( func ) {

      case 0:

	ss << "t " << std::fixed << std::setprecision(1) << std::setw( 5 )
	   << roundz( th.t(), 1 );

	break;

      case 1:

	ss << "h " << std::fixed << std::setprecision(1) << std::setw( 5 )
	   << roundz( th.h(), 1 );

        break;

      case 2:

        ss << "1 " << std::fixed << std::setprecision(3) << std::setw( 5 )
	   << roundz( MQ2ppm.load(), 3 );

        break;

      case 3:

	ss << "3 " << std::fixed << std::setprecision(3) << std::setw( 5 )
	   << roundz( MQ3mgl.load(), 3 );

        break;

      case 4:

	ss << "4 " << std::fixed << std::setprecision(3) << std::setw( 5 ) 
	   << roundz( MQ4ppm.load(), 3 );

        break;

      case 5:

	ss << "6 " << std::fixed << std::setprecision(3) << std::setw( 5 ) 
	   << roundz( MQ6ppm.load(), 3 );

        break;

      case 6:

	ss << "7 " << std::fixed << std::setprecision(3) << std::setw( 5 )
	   << roundz( MQ7ppm.load(), 3 );

        break;
	
      case 7:

        ss << "9 " << std::fixed << std::setprecision(3) << std::setw( 5 )
	   << roundz( MQ9ppm.load(), 3 );

        break;

      default:
	_LOG_ABORT(( "Impossible state" ));
	
      }

      // Output the formatted string which will always have a decimal
      // point. The decimal point has to be suppressed in the string
      // and replaced with a hardware decimal point.
      
      std::string s( ss.str());

      size_t dp = s.find( '.' );
      
      disp.write_string( s.erase( dp, 1 ));
      disp.set_decimal( dp, true );
      disp.set_brightness( 128 );
      
      // Step to the next function(al) to display.
      
      func = (( func + 1 ) % 8 );
      
    }

    // Whatever happened in the display buffer, show() it.
    
    disp.show();
    
    // End of loop time point.
    
    const std::chrono::time_point<std::chrono::system_clock>
      stop_tick = std::chrono::system_clock::now();

    // Calculate the difference between the stop and start points,
    // subtract that from one second, and then delay for that amount
    // of time.
    
    auto slp = loop_duration - ( stop_tick - start_tick );

    if( slp < std::chrono::seconds( 0 ))
      slp = loop_duration;

    // Figure out when to turn on the screen saver, then do it.
    
    if( saver_on == false ) {
      if( saver_countdown <= std::chrono::seconds( 0 )) 
	saver_on = true;
      else	
	saver_countdown -= loop_duration;
    }
    
    
    _LOG_VERB(( "tid=", _tid(), " sleeping for ",
                std::chrono::duration_cast
                <std::chrono::milliseconds>( slp ).count(), "ms" ));
    
    std::this_thread::sleep_for( slp );

  }

  _LOG_VERB(( "exiting" ));
}


void
munin_service_thread( void ) {

  bool ready = false;

  std::shared_ptr< int >
    sock( new int( -1 ), []( int* fd ) {
			   if( fd ) {
			     if( *fd >= 0 )
			       ::close( *fd );
			     delete fd;
			   }});
  assert( sock );
  
  if((  *sock = socket( AF_UNIX, SOCK_STREAM, 0 )) >= 0 ) {

    // Delete the old pipe, if it exists.
    
    ::unlink( pipe_path.c_str());

    // Make and bind to the new pipe.

#define LISTENQ 5
    
    struct sockaddr_un tAddr1;
    int                err;
    
    ::memset( &tAddr1, 0, sizeof( tAddr1 ));
    ::strcpy( tAddr1.sun_path, pipe_path.c_str());

    tAddr1.sun_family = AF_UNIX;
    
    if(( err = bind( *sock, (struct sockaddr *)&tAddr1, sizeof( tAddr1 ))))
      _LOG_ERR(( "Unable to bind() to Munin pipe, err=", err, errno2str()));
    else
      if(( err = listen( *sock, LISTENQ )))
	_LOG_ERR(( "Unable to start listen(), err=", err, errno2str()));
      else
	if(( err = ::chmod( pipe_path.c_str(), 0777 )))
	  _LOG_ERR(( "Unable to chmod() pipe, err=", err, errno2str()));
	else
	  ready = true;

  } else
    _LOG_ERR(( "Unable to create Munin socket(), err=", *sock, errno2str()));

  // If I was successful in creating the pipe then go with
  // it. Otherwise, there isn't much point to this thread.
  
  while( ready && ( doExit.load() == false )) {

    // Timeout every five seconds so that program termination can be
    // detected.
    
    struct timeval to { 5, 0 };
    fd_set         fds;
    
    FD_ZERO( &fds );
    FD_SET( *sock, &fds );

    // Wait for a connection or a timeout.
    
    if( ::select( FD_SETSIZE, &fds, nullptr, nullptr, &to ) > 0 ) {
      if( FD_ISSET( *sock, &fds )) {
	
	struct sockaddr_un tAddr2;

	::memset( &tAddr2, 0, sizeof( tAddr2 ));
      
	socklen_t tLen = sizeof( tAddr2 );
	int       fd = ::accept( *sock, (struct sockaddr *)&tAddr2, &tLen );

	_LOG_VERB(( "Awake" ));
    
	if( fd < 0 ) {
	  
	  // Some errors we don't care about, others we do.
	  
	  if( errno == EINTR )
	    continue;
	  else
	    _LOG_WARN(( "accept() err=", fd, errno2str()));

	} else {
	  
	  // Got a connection. Spit back the data and close.
	  
	  std::string s { sensor_line() };

	  s += "\n";
	  
	  if( int err; ( err = ::write( fd, s.c_str(), s.length())) < 0 )
	    _LOG_INFO(( "Failure to write Munin line ", quote( s ),
			", err=", err, errno2str()));
	  
	}    
	
	// If a connection was made, close it - the output is complete.
	
	if( fd >= 0 )
	  ::close( fd );
    
      }
    }
  }

  _LOG_VERB(( "exiting" ));
}


int
main( int argc, char** argv ) {

  // Say hello.
  
  extern const std::vector< std::string >
    ads1015_ident, is31fl3730_ident, si7021_ident, i2c_ident,
    log_ident, microdotphat_ident, opts_ident, util_ident;

  std::cout << main_ident << std::endl;
  for( const auto& i : { ads1015_ident, is31fl3730_ident, si7021_ident,
			  i2c_ident, microdotphat_ident, log_ident,
			  opts_ident, util_ident })
    for( const std::string& j : i )
      std::cout <<  j << std::endl;

  // Parse the options.
  
  if( parse_opts( argc, argv ) == false )
    exit( -1 );
  
  // Initialize and start the A/D converters connected to the MQ
  // sensors.
  
  ad1.gain( i2c::ads1015::PGA_GAIN::FS_6144 );
  ad1.mode( i2c::ads1015::MODE::CONTINUOUS );
  ad1.rate( i2c::ads1015::SAMPLE_RATE::SR_3300 );
  ad1.os( i2c::ads1015::OS::BEGIN );
    
  ad2.gain( i2c::ads1015::PGA_GAIN::FS_6144 );
  ad2.mode( i2c::ads1015::MODE::CONTINUOUS );
  ad2.rate( i2c::ads1015::SAMPLE_RATE::SR_3300 );
  ad2.os( i2c::ads1015::OS::BEGIN );

  // Start the temperature and humidity sensor. The heater adds about
  // three degress to the sense, so turn it off.
  
  th.heater( false );

  // Whether to become a daemon. Default is TRUE.

  if( doDaemon ) {

    // The parent exits and the child becomes the session leader.
    
    if( fork())
      ::exit( 0 );
    ::setsid();

  }

  // Create a PID file.
  //
  // Test for pre-existence of a PID file. If it exists, then is there
  // a running process? Non-errors are a bad thing because they
  // progressively suggest a process is indeed running.
    
  struct stat sb;

  ::memset( &sb, 0, sizeof( sb ));
  if( int err; ( err= ::stat( pid_file.c_str(), &sb ) == 0 )) {

    // A PID file exists. Is there a running process?
    
    std::ifstream ifs {
      pid_file
    };

    if( ifs.good()) {
      
      std::stringstream ss {
        "/proc/"
      };
      pid_t pid;
      
      ifs >> pid;
      ss << pid;

      _LOG_VERB(( "Checking for proc ", quote( ss.str())));
      
      if(( err = ::stat( ss.str().c_str(), &sb )) == 0 ) {

	// A process with that PID is running. Assume it is a copy of
	// this program, rather than confirm it (it's logical to
	// assume a running instance of this program). Exit. The code
	// will probably crash (if it hasn't already) because the i2c
	// bus will be busy.

	_LOG_ABORT(( "Refusing to create PID file "
		     "(one exists and a process exists)" ));
          
      }
    }
    ifs.close();
  }
  
  // Cache the PID

  std::ofstream ofs {
    pid_file, std::ofstream::out | std::ofstream::trunc
  };
      
  if( ofs.good()) {

    // Open succeeded, write out PID.
    
    ofs << ::getpid() << std::endl;

    _LOG_VERB(( "Creating PID file ", quote( pid_file ),
		" with PID=", ::getpid()));
    
  } else {
    
    // Open did not succeed. Merely log an error.
    
    _LOG_ERR(( "Unable to open/create PID file ", quote( pid_file )));
    
  }
  ofs.close();

  if( int err; ( err = ::chmod( pid_file.c_str(), 0644 )))
    _LOG_ERR(( "Unable to chmod() PID file, err=", err, errno2str()));

  // Catch some signals in an attempt to nicely exit.

  ::signal( SIGHUP,  exit_signal_handler );
  ::signal( SIGQUIT, exit_signal_handler );
  ::signal( SIGKILL, exit_signal_handler );
  ::signal( SIGTERM, exit_signal_handler );

  // Everything is initialized, at least as far as main() is
  // concerned, so start the threads.
  
  std::thread ad_sensor_thread( MQx_update_sensor_thread );
  std::thread display_thread( display_update_thread );
  std::thread munin_thread( munin_service_thread );
  
  // Main loop.

  std::mutex junk;

  // The loop prints the sensors every sixty seconds but the loop runs
  // every five seconds.
  
  constexpr std::chrono::duration
              loop_print     = std::chrono::seconds( 60 ),
              sleep_duration = std::chrono::seconds(  5 );
            std::chrono::duration loop_timer { loop_print };
  
  while( doExit.load() == false ) {

    loop_timer -= sleep_duration;

    // If the loop timer has expired, there is something to do.
    
    if( loop_timer <= std::chrono::seconds( 0 )) {

      // Start of loop time point.
    
      const std::chrono::time_point<std::chrono::system_clock>
	start_tick = std::chrono::system_clock::now();

      // Wait for the A/D sensors.
    
      std::unique_lock<std::mutex> l( junk );

      MQcv.wait( l );
      
      // Get and output the temperature, relative humidity, and MQ
      // sensors.
      
      _LOG_INFO(( sensor_line()));
      
      // End of loop time point.
    
      const std::chrono::time_point<std::chrono::system_clock>
	stop_tick = std::chrono::system_clock::now();

      // Calculate the difference between the stop and start points
      // and subtract that from the loop timer.

      loop_timer -= std::chrono::duration_cast< std::chrono::seconds >
	( stop_tick - start_tick );

    }

    // Adjust the loop timer and sleep. This process creates a certain
    // amount of loop time instability but it keeps the loop fairly
    // honest.
    
    if( loop_timer <= std::chrono::seconds( 0 ))
      loop_timer += loop_print;
      
    std::this_thread::sleep_for( sleep_duration );
      
  }

  // The other threads should exit any time now. Wait for them.
  
  ad_sensor_thread.join();
  display_thread.join();
  munin_thread.join();

  // A pause for the cause.
  
  std::this_thread::sleep_for( std::chrono::seconds( 1 ));

  // Ensure a little house cleaning.
  
  clean_up();
  
  return 0;
}


//  LocalWords:  Mutex mutex Pimoroni Enviro pHAT Adafruit MQ GND Vdd
//  LocalWords:  sparkfun Munin stdout stdin PID
