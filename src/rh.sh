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
# $Log: rh.sh,v $
# Revision 1.2  2019/09/28 08:46:32  root
# Low end index in awk was set wrong.
#
# Revision 1.1  2019/09/28 07:47:22  root
# Initial revision
#

. "$MUNIN_LIBDIR/plugins/plugin.sh"

if [ "$1" = "config" ]; then
    echo 'graph_title Relative Humidity'
    echo 'graph_vlabel %'
    echo 'graph_category Attic Sensors'
    echo 'graph_args --base 1000 -l 0 --upper-limit 100'
    echo 'h.label Relative Humidity'
    exit 0
fi


/bin/nc -U /var/run/sensors | 
    awk '{ for( i=1; i<=NF; ++i ) { 
             if( tolower($i) ~ /^h=.+/ ) { 
 	       print $i; 
	     }
	   }
         }' |
    sed -e 's/=/.value /'


exit 0

