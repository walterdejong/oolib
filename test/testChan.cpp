/*
	testChan.cpp	WJ112
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

using namespace oo;

Chan<int> chan(4);	// 4 is channel buffer depth

void thread_func(void) {
	unsigned int tid = gettid();
	print("[%u] I'm awake", tid);

	int n;
	for(;;) {
		n = chan.get();
		if (n == -1) {
			print("[%u] terminating", tid);
			break;
		}
		print("[%u] got value: %d", tid, n);
	}
	print("[%u] I'm gone", tid);
}

int main(void) {
	const int num_threads = 4;

	for(int i = 0; i < num_threads; i++) {
		go(thread_func);
	}
	for(int n = 0; n < num_threads * 3; n++) {
		print("[0] writing value: %d", n);
		chan.put(n);
	}
	for(int n = 0; n < num_threads; n++) {
		print("[0] writing terminate code");
		chan.put(-1);
	}
	// wait till all threads are done
	join();

	print("len() : %zu  cap(): %zu", len(chan), cap(chan));

	chan = None;
	if (chan == None)
		print("chan equals None");
	else
		print("chan != None : FAIL");

	return 0;
}

// EOB
