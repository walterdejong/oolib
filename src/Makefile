#
#	Makefile	WJ112
#
#	for oolib
#
# Copyright (c) 2014, Walter de Jong
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

PREFIX=/usr/local
INSTALLDIR=$(PREFIX)

CXX=clang++
#CXX=g++

# oolib requires C++11
CXX_STANDARD=-std=c++11

# debug info or optimization flags
CXX_OPTIMIZE=-gdwarf-2
#CXX_OPTIMIZE=-O3

INCLUDE=../include

CXXFLAGS=-Wall $(CXX_STANDARD) $(CXX_OPTIMIZE) -fPIC -I$(INCLUDE)

LFLAGS=
LIBS=
AR=ar
INSTALL=install

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

CXXFILES=$(wildcard *.cpp)
HEADERS=$(wildcard $(INCLUDE)/oo/*.h)
OBJS=Error.o print.o String.o File.o Mutex.o Sem.o go.o dir.o Argv.o \
	Sock.o Observer.o signal.o daemon.o oolib.o

TARGETS=liboo.so liboo.a

.PHONY: test

all: .depend $(TARGETS)

include .depend

liboo.so: $(OBJS)
	$(CXX) -gdwarf-2 -shared -o liboo.so $(OBJS)

liboo.a: $(OBJS)
	$(AR) rcs liboo.a $(OBJS)

dep .depend:
	$(CXX) $(CXX_STANDARD) -I$(INCLUDE) -M *.cpp >.depend

clean:
	rm -f $(OBJS) *~ core $(TARGETS)

cleanall: clean
	make -C ../test clean

test: $(TARGETS)
	mkdir -p ../lib
	cp $(TARGETS) ../lib
	make -C ../test

install: $(TARGETS)
	$(INSTALL) -d -g 0 -m 0755 -o root $(INSTALLDIR)/lib
	$(INSTALL) -c -v -g 0 -m 0755 -o root liboo.so $(INSTALLDIR)/lib
	$(INSTALL) -c -v -g 0 -m 0644 -o root liboo.a $(INSTALLDIR)/lib
	$(INSTALL) -d -g 0 -m 0755 -o root $(INSTALLDIR)/include/oo
	$(INSTALL) -c -v -g 0 -m 0644 -o root $(HEADERS) $(INSTALLDIR)/include/oo
	$(INSTALL) -c -v -g 0 -m 0644 -o root $(INCLUDE)/oolib $(INSTALLDIR)/include

uninstall:
	( rm $(INSTALLDIR)/include/oo/*.h ; \
	rmdir $(INSTALLDIR)/include/oo ; \
	rm $(INSTALLDIR)/include/oolib ; \
	rm $(INSTALLDIR)/lib/liboo.so ; \
	rm $(INSTALLDIR)/lib/liboo.a )

# EOB
