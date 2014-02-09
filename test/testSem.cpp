/*
	testSem.cpp	WJ112
	
	- multiple consumers, as shown on Wikipedia
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

const int kNumConsumers = 3;
const int kItemsPerConsumer = 2;
const int kFull = 4;	// play with this value

Sem mutex("mutex", 1);
Sem empty_count("empty_count", kFull);
Sem item_count("item_count", 0);

unsigned int nitems = 0;

void consumer(void) {
	int got_items = 0;
	unsigned int tid = gettid();

	print("[%u] consumer: starting", tid);

	while(got_items < kItemsPerConsumer) {
		print("[%u] consumer: waiting for item", tid);
		item_count.wait();

		mutex.wait();
		nitems--;
		got_items++;
		print("[%u] consumer: got item %d", tid, got_items);
		mutex.post();

		empty_count.post();
	}

	print("[%u] consumer: I'm happy, signing off", tid);
}

void producer(void) {
	int items_made = 0;

	print("(%u) producer: starting", gettid());

	empty_count.post();		// do not hang in empty_count.wait()

	while(items_made < kNumConsumers * kItemsPerConsumer) {
		print("producer: crafting item");
		items_made++;

		empty_count.wait();

		mutex.wait();
		print("producer: making item %d available", items_made);
		nitems++;
		mutex.post();

		item_count.post();
	}

	print("producer: all items made, signing off");
}

Sem get_mutex(void) {
	Sem m("testsem", 1);
	print("factory fun: returning binary semaphore %v", &m);
	return m;
}

int main(void) {
	mutex = get_mutex();

	// semaphore mutexes must start as "1"
	mutex.post();

	for(int i = 0; i < kNumConsumers; i++)
		go(consumer);

	go(producer);

	join();
	print("all done");

	print("sem: %v", &mutex);
	mutex = None;
	print("sem: %v", &mutex);
	if (!mutex)
		print("sem is None");
	else
		print("FAIL: your skill is not enough");
	return 0;
}

// EOB
