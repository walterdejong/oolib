/*
	oogo.h	WJ112
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

#ifndef OOGO_H_WJ112
#define OOGO_H_WJ112

#include "oo/Functor.h"

#include <functional>
#include <utility>
#include <thread>

namespace oo {

// get current thread id
unsigned int gettid(void);

void go_trampoline__(const std::function<void()>&);

/*
	launch a thread
	Use join() to wait for termination of all threads
	Threads must be joined before the program ends or you might get
	weird SEGVs and the like upon termination
	Use either getpid() or gettid() in the child to get its id
*/
template <typename... Tpack>
void go(Tpack&&... args) {
	go_trampoline__(std::bind(std::forward<Tpack>(args)...));
}

// specialized templates for passing in a Functor
// const Functor& is only used for const functions or explicitly const Functors
// in most cases go(Functor&) is used
template <>
inline void go<const Functor&>(const Functor& f) {
	go_trampoline__(f.function());
}

template <>
inline void go<Functor&>(Functor& f) {
	go_trampoline__(f.function());
}

// join() is like wait(), it waits for all threads to end
void join(void);

void child_trampoline__(const std::function<void()>&);

/*
	fork off a child process
	Use wait() to wait for termination of all child processes
	Use either getpid() or gettid() in the child to get its id
	Mind that gettid() will produce duplicate tids if you spawn off threads
	or children from child processes ... (this happens because fork() copies
	the eternal_tid counter, it's not shared among processes)
	Use jump_tid() to work around that problem somewhat
*/
template<typename... Tpack>
void child(Tpack&&... args) {
	child_trampoline__(std::bind(std::forward<Tpack>(args)...));
}

void wait(void);

// increase eternal_tid by n
// use this to create room for more tids when you spawn from child processes
void jump_tid(unsigned int);

}	// namespace

#endif	// OOGO_H_WJ112

// EOB
