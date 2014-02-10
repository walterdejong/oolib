//
//	ooSem.cpp	WJ112
//
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

#include "oo/Sem.h"

#include <cstdlib>
#include <cerrno>

#include <fcntl.h>

namespace oo {

Sem::Sem(const String& name, int value) {
	if (name.empty()) {
		throw ValueError();
	}

	// create semaphore
	// OSX does not support unnamed semaphores, so use a named semaphore
	// note that a named semaphore must be unlinked to destroy it
	// For properly defined POSIX behaviour, start names with a slash

	if (name.c_str()[0] != '/') {
		name_ = "/";
		name_ += name;
	} else {
		name_ = name;
	}

	sem_t *s;
	if ((s = ::sem_open(name_.c_str(), O_CREAT, (mode_t)0600, value)) == SEM_FAILED) {
		throw OSError("failed to initialize semaphore");
	}

	sem_ = std::shared_ptr<sem_t>(s, SemDeleter());
}

bool Sem::trywait(void) {
	if (::sem_trywait(sem_.get()) != 0) {
		if (errno == EAGAIN) {
			return false;
		}
		throw OSError("semaphore trywait failed");
	}
	return true;
}

}	// namespace

// EOB
