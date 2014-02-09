#
#	Makefile	WJ112
#
#	for oolib
#
#	- on Linux, use LD_LIBRARY_PATH
#	- OS X, use DYLD_LIBRARY_PATH
#
#	Like so:
#	$ DYLD_LIBRARY_PATH=../lib ./testxxx
#
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

CXX=clang++
#CXX=g++

# oolib requires C++11
CXX_STANDARD=-std=c++11

INCLUDE=../include

CXXFLAGS=-Wall $(CXX_STANDARD) -gdwarf-2 -fPIC -I$(INCLUDE)
#CXXFLAGS=-Wall $(CXX_STANDARD) -O3 -fPIC -I$(INCLUDE)

LFLAGS=-L../lib
LIBS=-loo
# Linux sometimes requires librt for the sem_xxx() functions
#LIBS=-loo -lrt

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

CXXFILES=$(wildcard *.cpp)

TARGETS=testNone testError testString testArray testList testDict testPrint \
	testFile testGo testDefer testMutex testChan testCond testSem \
	testRef testDir testArgv testSock testDaemon testObserver testSet \
	testFunctor

all: .depend $(TARGETS)

include .depend

testNone: testNone.o
	$(CXX) $(LFLAGS) testNone.o -o testNone $(LIBS)

testError: testError.o
	$(CXX) $(LFLAGS) testError.o -o testError $(LIBS)

testString: testString.o
	$(CXX) $(LFLAGS) testString.o -o testString $(LIBS)

testArray: testArray.o
	$(CXX) $(LFLAGS) testArray.o -o testArray $(LIBS)

testList: testList.o
	$(CXX) $(LFLAGS) testList.o -o testList $(LIBS)

testDict: testDict.o
	$(CXX) $(LFLAGS) testDict.o -o testDict $(LIBS)

testPrint: testPrint.o
	$(CXX) $(LFLAGS) testPrint.o -o testPrint $(LIBS)

testFile: testFile.o
	$(CXX) $(LFLAGS) testFile.o -o testFile $(LIBS)

testGo: testGo.o
	$(CXX) $(LFLAGS) testGo.o -o testGo $(LIBS)

testMutex: testMutex.o
	$(CXX) $(LFLAGS) testMutex.o -o testMutex $(LIBS)

testChan: testChan.o
	$(CXX) $(LFLAGS) testChan.o -o testChan $(LIBS)

testCond: testCond.o
	$(CXX) $(LFLAGS) testCond.o -o testCond $(LIBS)

testSem: testSem.o
	$(CXX) $(LFLAGS) testSem.o -o testSem $(LIBS)

testRef: testRef.o
	$(CXX) $(LFLAGS) testRef.o -o testRef $(LIBS)

testDir: testDir.o
	$(CXX) $(LFLAGS) testDir.o -o testDir $(LIBS)

testArgv: testArgv.o
	$(CXX) $(LFLAGS) testArgv.o -o testArgv $(LIBS)

testSock: testSock.o
	$(CXX) $(LFLAGS) testSock.o -o testSock $(LIBS)

testDaemon: testDaemon.o
	$(CXX) $(LFLAGS) testDaemon.o -o testDaemon $(LIBS)

testObserver: testObserver.o
	$(CXX) $(LFLAGS) testObserver.o -o testObserver $(LIBS)

testSet: testSet.o
	$(CXX) $(LFLAGS) testSet.o -o testSet $(LIBS)

testDefer: testDefer.o
	$(CXX) $(LFLAGS) testDefer.o -o testDefer $(LIBS)

testFunctor: testFunctor.o
	$(CXX) $(LFLAGS) testFunctor.o -o testFunctor $(LIBS)

dep .depend:
	$(CXX) $(CXX_STANDARD) -I$(INCLUDE) -M *.cpp >.depend

clean:
	rm -f *.o *~ core $(TARGETS)

# EOB
