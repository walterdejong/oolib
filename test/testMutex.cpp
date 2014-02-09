/*
	testMutex.cpp	WJ112

	- note that this is just a test code ...
	  Normally you would not wait for threads like this (use join() instead)
	  and putting a mutex around an int is a bad idea (use std::atomic<int>)
*/
/*
 * Copyright (c) 2014, Walter de Jong
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "oolib"

#include <unistd.h>

using namespace oo;

int done = 0;
Mutex mutex;

void testfunc(void) {
	print("[%u] hello from thread!", gettid());

	mutex.lock();
	print("[%u] %v", gettid(), &mutex);
	done++;
	print("[%u] %d", gettid(), done);
	mutex.unlock();
}

int main(void) {
	for(int i = 0; i < 6; i++) {
		go(testfunc);
	}

	// busy lock ... bad coding style (use channels or cv's instead)
	// this is just a testing code
	while(true) {
		mutex.lock();

		if (done >= 6) {
			print("all done: %v %d", &mutex, done);
			mutex.unlock();
			break;
		}
		mutex.unlock();
	}
	print("ending, mutex is %v", &mutex);
	join();

	mutex = None;
	if (!mutex) {
		print("mutex is None; %v", &mutex);
	} else {
		print("FAIL: mutex is not None; %v", &mutex);
	}
	return 0;
}

// EOB
