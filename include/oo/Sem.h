//
//	ooSem.h	WJ112
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

#ifndef OOSEM_H_WJ112
#define OOSEM_H_WJ112

#include "oo/Base.h"
#include "oo/Error.h"
#include "oo/String.h"

#include <sstream>
#include <ostream>
#include <memory>

#include <semaphore.h>

namespace oo {

class Sem : public Base {
public:
	Sem() : Base(), name_(), sem_(std::shared_ptr<sem_t>()) { }

	Sem(const String&, int);

	Sem(const Sem& s) : Base(), name_(), sem_(s.sem_) { }

	Sem(Sem&& s) : Sem() {
		swap(*this, s);
	}

	virtual ~Sem() {
		close();
		unlink();
	}

	Sem& operator=(Sem s) {
		swap(*this, s);
		return *this;
	}

	static void swap(Sem& a, Sem& b) {
		std::swap(a.name_, b.name_);
		std::swap(a.sem_, b.sem_);
	}

	virtual std::string repr(void) const {
		if (name_.empty()) {
			return "<Sem>";
		}
		std::stringstream ss;
		ss << "<Sem: \"" << name_ << "\">";
		return ss.str();
	}

	void clear(void) { close(); }

	bool operator!(void) const { return (sem_.get() == nullptr); }

	void close(void) {
		if (sem_.get() != nullptr) {
			::sem_close(sem_.get());
			sem_.reset();
		}
	}

	void unlink(void) {
		if (!name_.empty()) {
			// silently ignore any unlink errors
			::sem_unlink(name_.c_str());
		}
	}

	void wait(void) {
		if (::sem_wait(sem_.get()) != 0) {
			throw OSError("semaphore wait failed");
		}
	}

	void post(void) {
		if (::sem_post(sem_.get()) != 0) {
			throw OSError("semaphore post failed");
		}
	}

	bool trywait(void);

	// also known as ...
	void signal(void) { post(); }

	void P(void) { wait(); }
	void V(void) { post(); }

	void acquire(void) { wait(); }
	void release(void) { post(); }

	void down(void) { wait(); }
	void up(void) { post(); }

	String name(void) const { return name_; }

private:
	class SemDeleter {
	public:
		void operator()(sem_t *s) const {
			if (s != nullptr) {
				::sem_close(s);
			}
		}
	};

	String name_;
	std::shared_ptr<sem_t> sem_;

	friend std::ostream& operator<<(std::ostream&, const Sem&);
};

// used for printing
inline std::ostream& operator<<(std::ostream& os, const Sem& sem) {
	os << sem.str();
	return os;
}

}	// namespace

#endif	// OOSEM_H_WJ112

// EOB
