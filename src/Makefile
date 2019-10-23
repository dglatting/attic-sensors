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
# $Log: Makefile,v $
# Revision 1.11  2019/10/23 03:54:00  root
# First pass at integrating other project changes.
#
# Revision 1.10  2019/10/12 22:01:50  root
# *** empty log message ***
#
# Revision 1.9  2019/10/08 23:55:26  root
# Changed install flags.
#
# Revision 1.8  2019/10/07 04:02:38  root
# Added to install: and added test compile options.
#
# Revision 1.7  2019/10/05 04:10:04  root
# Started install component.
#
# Revision 1.6  2019/09/25 05:59:58  root
# Removed some redundant decls.
#
# Revision 1.5  2019/09/17 03:48:04  root
# Added Micro Dot pHAT.
#
# Revision 1.4  2019/09/04 04:23:47  root
# Added MIT copyright notice.
#
# Revision 1.3  2019/08/28 07:57:21  root
# Daily check-in. Things improving. Kernel trap gone.
#
# Revision 1.2  2019/08/27 07:09:13  root
# It crashes the kernel, so that's nice.
#
# Revision 1.1  2019/08/27 01:01:27  root
# Initial revision
#

# OpenMP isn't used (the code is too small) but is enabled anyway.

OMP=    -fopenmp
INLINE= -finline-functions  
STDCPP= -std=c++17
DBG=    -g -fstack-protector -Wall -D_DPG_DEBUG=1 -D_DPG_DEBUG_VERBOSE=1 
CMP=    -O -march=native 

OPTS=   ${OMP} ${STDCPP} ${INLINE} ${CMP} ${DBG}  

INCLUDES= -I. 
LIBS=   

CXX=       c++
CXXFLAGS=  ${OPTS} ${DBG} ${INCLUDES} -pthread

SRCS=    i2c.o ads1015.cc si7021.cc is31fl3730.cc \
		microdotphat.cc main.cc log.cc opts.cc util.cc
OBJS=    $(patsubst %.cc, %.o, ${SRCS})
PLUGINS= th.sh mq.sh rh.sh

%.o : %.cc
	${CXX} ${CXXFLAGS} -c -o $@ $<
	size $@

all: a.out .depends

# This is just for testing and amusement.
clang:
	make a.out CXX="clang++" OMP="" \
		STDCPP="-std=c++17" LIBS="-L /usr/local/lib \
			-Wl,-rpath=/usr/local/lib ${LIBS} " 
gcc:
	make a.out CXX="c++" STDCPP="-std=c++17" \
		LIBS="-L /usr/local/lib -Wl,-rpath= \
			-Wl,-rpath=/usr/local/lib ${LIBS} "

# This is just for testing and amusement.
gcc-size:
	make a.out CXX="c++" STDCPP="-std=c++17" CMP="-Os -march=native" \
		LIBS="-L /usr/local/lib -Wl,-rpath= \
                        -Wl,-rpath=/usr/local/lib ${LIBS} "

a.out: ${OBJS} .depends
	${CXX} ${CXXFLAGS} -o $@ $(filter %.o, $^) ${LIBS}
	size $@

.depends: ${SRCS} Makefile
	${CXX} -MM ${CXXFLAGS} ${SRCS} > .depends

depends: .depends

clean: 
	-rm .depends *.o *~ a.out

${OBJS}: Makefile

install: a.out $(foreach p, $(PLUGINS), scripts/$p)
	-mkdir -p /etc/munin/pluguns
	$(foreach p, $(PLUGINS), cp scripts/$p /etc/munin/pluguns;)
	$(foreach p, $(PLUGINS), chmod 755 /etc/munin/pluguns/$p;)
	-mkdir -p /usr/local/sbin
	install -m 755 -s a.out /usr/local/sbin/attic-sensors
	install -m 664 attic-sensors.service /etc/systemd/system/

-include .depends




# LocalWords:  pHAT
