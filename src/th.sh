#!/bin/sh
# -*- sh -*-
#
# Copyright(c)  2019 Dennis Glatting
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#
# $Log: th.sh,v $
# Revision 1.5  2019/10/05 03:38:18  root
# Kept the =. Shouldn't.
#
# Revision 1.4  2019/10/05 03:32:23  root
# I couldn't take it any more. I'm an American. I need to see
# temperature in Fahrenheit.
#
# Revision 1.3  2019/10/01 04:18:09  root
# Spelled out the vertical label.
#
# Revision 1.2  2019/09/28 08:46:32  root
# Low end index in awk was set wrong.
#
# Revision 1.1  2019/09/28 07:51:38  root
# Initial revision
#

. "$MUNIN_LIBDIR/plugins/plugin.sh"

if [ "$1" = "config" ]; then
    echo 'graph_title Temperature'
    echo 'graph_vlabel Fahrenheit'
    echo 'graph_category Attic Sensors'
    echo 'graph_args --base 1000 -l 0'
    echo 't.label Temperature'
    exit 0
fi


/bin/nc -U /var/run/sensors | 
    awk '{ for( i=1; i<=NF; ++i ) { 
             if( tolower($i) ~ /^t=.+[:space:]*/ ) { 
               c = substr($i, 3);
	       f = (( c * 9.0 / 5.0 ) + 32.0 );
 	       printf( "t.value %f\n", f ); 
	     }
	   }
         }'

exit 0

