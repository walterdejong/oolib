/*
	testCond.cpp	WJ112
	
	- before, oolib had a class Cond and this code would demo it
	- Cond was impossible to port to C++11, but this code now
	  demoes how to use std::condition_variable
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

#include <mutex>
#include <condition_variable>

using namespace std;
using namespace oo;

const int kNumConsumers = 3;
const int kItemsPerConsumer = 2;

mutex mx;
condition_variable cv;
unsigned int nitems = 0;

void consumer(void) {
	int got_items = 0;
	unsigned int tid = gettid();

	while(got_items < kItemsPerConsumer) {
		unique_lock<mutex> lk(mx);

		while(!nitems) {
			print("[%u] consumer: waiting for item", tid);
			cv.wait(lk);
		}

		nitems--;
		got_items++;
		print("[%u] consumer: got item: %d", tid, got_items);
	}
}

void producer(void) {
	for(int n = 0; n < kNumConsumers * kItemsPerConsumer; n++) {
		print("producer: crafting item");
//		usleep(100);

		unique_lock<mutex> lk(mx);

		nitems++;
		print("producer: making item %d available", nitems);

		cv.notify_one();
	}
}

int main(void) {
	for(int i = 0; i < kNumConsumers; i++)
		go(consumer);

	go(producer);

	join();
	print("all done");
	return 0;
}

// EOB
