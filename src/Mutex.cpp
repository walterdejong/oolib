//
//	ooMutex.cpp	WJ112
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

#include "oo/Mutex.h"

namespace oo {

void Mutex::lock(void) {
	if (m_.get() == nullptr) {
		throw ReferenceError();
	}
	try {
		m_->lock();
	} catch(std::system_error) {
		throw OSError("failed to lock Mutex");
	}
	state = MutexLocked;
}

void Mutex::unlock(void) {
	if (m_.get() == nullptr) {
		throw ReferenceError();
	}
	if (state != MutexLocked) {
		return;
	}

	state = MutexUnlocked;
	try {
		m_->unlock();
	} catch(std::system_error) {
		throw OSError("failed to unlock Mutex");
	}
}

bool Mutex::trylock(void) {
	if (m_.get() == nullptr) {
		throw ReferenceError();
	}
	if (m_->try_lock()) {
		state = MutexLocked;
	} else {
		state = MutexUnlocked;
	}
	return state;
}

}	// namespace oo

// EOB
