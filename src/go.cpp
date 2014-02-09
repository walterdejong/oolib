/*
	oogo.cpp	WJ112
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

#include "oo/go.h"
#include "oo/Error.h"
#include "oo/Mutex.h"

#include <cstdlib>
#include <cassert>
#include <map>
#include <cerrno>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace oo {

/*
	tid_mgr maps thread ids to simple integers
	so std::thread::id's like 0x1290a8ff become thread #3
	get the current thread "tid" via gettid()

	Mind that this is OK until you fork() ... which copies eternal_tid
	and you get duplicate tids in the system
	You may use jump_tid() to prevent tid clashes
*/
static std::vector<std::thread> thread_mgr;
static std::mutex thread_mgr_lock;
static std::atomic<unsigned int> eternal_tid;
static std::map<std::thread::id, unsigned int> tid_mgr;
static std::mutex tid_mgr_lock;

static bool initialize(void);

static bool initialized = initialize();

static bool initialize(void) {
	// called from main thread
	tid_mgr[std::this_thread::get_id()] = 0;
	eternal_tid = 1;
	return true;
}

static void assign_tid(void) {
	std::lock_guard<std::mutex> guard(tid_mgr_lock);
	tid_mgr[std::this_thread::get_id()] = eternal_tid++;
}

// run a bound function in a thread
// launched by go_trampoline()
// This takes a copy of func (on purpose; it prevents crashes)
static void go_thread_main(std::function<void()> func, std::atomic<int> *start) {
	assign_tid();

	(*start)++;

	func();
}

// launch function as thread
// invoked by go(func, arg1, arg2, ...)
void go_trampoline__(const std::function<void()>& func) {
	std::lock_guard<std::mutex> guard(thread_mgr_lock);

	std::atomic<int> start;
	start = 0;

	try {
		// keep the thread as variable in the thread_mgr array
		thread_mgr.push_back(std::thread(go_thread_main, func, &start));
	} catch(std::system_error) {
		throw OSError("failed to start thread");
	}

	// spinlock ... eek
	// how to improve this, without destroying locks before the thread ever
	// got to them, etc. ?
	while(start == 0);
}

// join all threads, and cleanup the thread manager
void join(void) {
	std::lock_guard<std::mutex> guard_thrd(thread_mgr_lock);

	for(auto& t : thread_mgr) {
		t.join();
	}

	thread_mgr.clear();

	std::lock_guard<std::mutex> guard_tid(tid_mgr_lock);
	tid_mgr.clear();
}

// get current thread id
unsigned int gettid(void) {
	unsigned int tid = (unsigned int)-1;

	std::lock_guard<std::mutex> guard(tid_mgr_lock);

	std::thread::id my_id = std::this_thread::get_id();
	if (tid_mgr.find(my_id) != tid_mgr.end()) {
		tid = tid_mgr[my_id];
	}
	return tid;
}

void child_trampoline__(const std::function<void()>& func) {
	int status;

	switch(::fork()) {
		case (pid_t)-1:
			throw OSError("failed to fork off a child process");
			break;

		case (pid_t)0:
			// child process; this is a forked copy
			assign_tid();
			func();
			std::exit(0);	// terminate child process
			break;

		default:
			assign_tid();
			if (::waitpid(-1, &status, WNOHANG) == (pid_t)-1 && errno != ECHILD) {
				throw OSError("failed waitpid() call, something wrong with child process");
			}
	}
}

void wait(void) {
	int status;

	for(;;) {
		if (::waitpid(-1, &status, 0) == (pid_t)-1) {
			if (errno == ECHILD)	// no more child processes
				break;

			throw OSError("failed waitpid() call, something wrong with child process");
		}
	}
}

void jump_tid(unsigned int n) {
	assert(n > 1);

	n--;
	if (!n) {
		return;
	}
	eternal_tid += n;
}

}	// namespace

// EOB
