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
 * $Log: templates.h,v $
 * Revision 1.2  2019/10/20 04:39:37  root
 * Minor text change.
 *
 * Revision 1.1  2019/10/15 22:12:17  root
 * Initial revision
 *
 */

#ifndef __TEMPLATES_H__
#define __TEMPLATES_H__

extern "C" {

#include <sys/types.h>
  
}

#include <functional>
#include <map>
#include <memory>


#define _TEMPLATES_H_ID "$Id: templates.h,v 1.2 2019/10/20 04:39:37 root Exp $"


//***************************************************************************

// The templates in this section are used in the _check() routines and
// exist only when the code is compiled for debugging.


// enum class A { aa, bb, cc, ... }
// enum class B { aa, bb, cc, ... }
//
//  const std::map< A, uint16_t > A_MAP {
//    { A::aa,    0b0010000000000000 },
//    { A:bb,     0b0110000000000000 },
//    { A:cc,     0b1010000000000000 },
//    ...
//    { A::_MASK, 0b1110000000000000 }
//  };
//  const std::map< B, uint16_t > A_MAP {
//    { B::aa,    0b0000000000100000 },
//    { B:bb,     0b0000000000010000 },                                      
//    { B:cc,     0b0000000000001000 },
//    ...
//    { B::_MASK, 0b0000000000111000 }
//  };

// The declarations above are used to represent bit maps in a single
// register. The _check_masking() routine goes through a bit map and
// tests whether there are any bit settings outside the mask.

template< typename K, typename V >
inline
void
_check_masking( const std::map< K, V >& m ) {
#ifdef _DPG_DEBUG
  
  // The map size MUST always be greater than zero (i.e., there MUST
  // be something to map).
  
  assert( m.size());
  
  // Test:
  // 1, If I mask the bits against the mask's complement then
  //    there shouldn't be any extraneous bits set.
  // 2, If I mask the bits against the mask then the result MUST
  //    be the bits.
  
  const V mask = m.at( K::_MASK );
  for( const auto& [tag,value] : m )
    if(( tag != K::_NONE ) && ( tag != K::_MASK )) {
	assert(( value & ~mask ) == 0 );
	assert(( value & mask ) == value );
      }
  
#endif
}

// The declarations above are used to represent bit maps in a single
// register. The _check_bit_overlap() routine goes through both bit
// maps and tests whether there are any bit overlaps (i.e., two
// different maps that manipulate the same bit(s)).

template< typename A, typename B >
inline
void _check_bit_overlap( const A& a, const B& b ) {
#ifdef _DPG_DEBUG
  
  // Test:
  // 1, Two different function bit maps exist in the same register
  //    but the bits in each map MUST not overlap.
  
  for( auto& i : a )
    for( auto& j : b )
      assert(( i.second & j.second ) == 0 );

#endif
}

//***************************************************************************


#endif


