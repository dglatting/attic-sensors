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
# $Log: mq.sh,v $
# Revision 1.5  2019/10/01 04:18:35  root
# Changed the vertical level title.
#
# Revision 1.4  2019/10/01 03:59:34  root
# Added warning/critical levels for certain gases.
#
# Revision 1.3  2019/09/30 20:40:26  root
# Labeling changes.
#
# Revision 1.2  2019/09/28 08:46:32  root
# Low end index in awk was set wrong.
#
# Revision 1.1  2019/09/28 06:56:44  root
# Initial revision
#

. "$MUNIN_LIBDIR/plugins/plugin.sh"

if [ "$1" = "config" ]; then
    echo 'graph_title MQ Sensors'
    echo 'graph_vlabel PPM or mg/L'
    echo 'graph_category Attic Sensors'
    echo 'graph_args --base 1000 -l 0 --upper-limit 5'
    echo 'graph_order MQ2 MQ3 MQ4 MQ6 MQ7 MQ9'
    echo 'MQ2.label MQ2 Combustible Gas (PPM)'
    echo 'MQ3.label MQ3 Alcohol Vapor (mg/L)'
    # https://www.ncbi.nlm.nih.gov/books/NBK208285/
    echo 'MQ4.label MQ4 Methane (PPM)'
    echo 'MQ4.warning :5000'
    echo 'MQ6.label MQ6 Propane (PPM)'
    # https://www.ncbi.nlm.nih.gov/books/NBK220007/table/ttt00022/?report=objectonly
    echo 'MQ7.label MQ7 Carbon Monoxide (PPM)'
    echo 'MQ7.warning :420'
    echo 'MQ7.critical :1700'
    # https://www.ncbi.nlm.nih.gov/books/NBK201461/table/tab7_1/?report=objectonly
    echo 'MQ9.label MQ9 LPG (PPM) '
    echo 'MQ9.warning :17000'
    echo 'MQ9.critical :33000'

    exit 0
fi


/bin/nc -U /var/run/sensors | 
    awk '{ for( i=1; i<=NF; ++i ) { 
             if( toupper($i) ~ /^MQ[0-9]=.+/ ) { 
 	       print $i; 
	     }
	   }
         }' |
    sed -e 's/=/.value /'


exit 0

