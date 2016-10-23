//
//	ooMutex.h	WJ112
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

#ifndef OOMUTEX_H_WJ112
#define OOMUTEX_H_WJ112	1

#include "oo/Base.h"
#include "oo/Error.h"

#include <ostream>
#include <sstream>
#include <memory>
#include <mutex>

namespace oo {

typedef enum {
	MutexUnlocked = 0,
	MutexLocked
} MutexState;

// Mutex is based on std::mutex
// It's basically the same, except that Mutex has an islocked() method

class Mutex : public Base {
public:
	Mutex() : Base(), state(MutexUnlocked), m_(std::shared_ptr<std::mutex>(new std::mutex)) { }

	Mutex(const Mutex& m) : Base(), state(m.state), m_(m.m_) { }

	Mutex(Mutex&& m) : Base() {
		state = m.state;
		m_ = std::move(m.m_);
	}

	virtual ~Mutex() {
		if (m_.get() != nullptr) {
			unlock();
		}
	}

	Mutex& operator=(const Mutex& m) {
		if (this == &m) {
			return *this;
		}
		state = m.state;
		m_ = m.m_;
		return *this;
	}

	Mutex& operator=(Mutex&& m) {
		state = m.state;
		m_ = std::move(m.m_);
		return *this;
	}

	virtual std::string repr(void) const {
		if (islocked()) {
			return "<Mutex: locked>";
		}
		return "<Mutex>";
	}

	void clear(void) { unlock(); }

	bool operator!(void) const { return (state == MutexUnlocked); }

	virtual void lock(void);
	virtual void unlock(void);
	virtual bool trylock(void);

	virtual bool islocked(void) const { return (state == MutexLocked); }

private:
	MutexState state;
	std::shared_ptr<std::mutex> m_;

	friend std::ostream& operator<<(std::ostream&, const Mutex&);
};

// used for printing
inline std::ostream& operator<<(std::ostream& os, const Mutex& m) {
	os << m.str();
	return os;
}

}	// namespace oo

#endif	// OOMUTEX_H_WJ112

// EOB
