/*
	ooChan.h	WJ112
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

#ifndef OOCHAN_H_WJ112
#define OOCHAN_H_WJ112

#include "oo/Base.h"
#include "oo/Sizeable.h"
#include "oo/Error.h"

#include <cstdlib>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <vector>

namespace oo {

/*
	the channel class
	A channel is a queue of items that is shared between threads.
	Since threads already use a shared memory model, it makes no sense
	to dump large objects into a channel. They are good for communications,
	synchronisation signalling, and you can pass object pointers through
	channels.

	Mind that only the read() and write() methods do proper locking

	You can't close a channel, as there is never a need to, but you can
	call clear() on it to free up the buffer. This does not do locking,
	you have to make sure yourself that no other thread is using the
	channel at that moment. You can't use a channel anymore after it
	has been cleared (the program will segv). However you can grow() it
	and start using it again.
	Anyway, a channel is not like a	file or socket that you should open
	and close all the time.
*/
template <typename T>
class Chan : public Base, public Sizeable {
public:
	typedef T value_type;

	Chan(size_t n=1) : Base(), Sizeable(),
		mx_(), not_empty_(), not_full_(), buffer() {
		if (n <= 0) {
			throw ValueError();
		}
		buffer.reserve(n);
	}

	virtual ~Chan() { clear(); }

	std::string repr(void) const { return "<Chan>"; }

	// the swap trick frees all memory in the vector
	static void force_free(std::vector<T>& v) {
		std::vector<T> tmp;
		v.swap(tmp);
		// stack unwinds, deleting tmp
	}

	void clear(void) {
		// warning: it clears whether the mutex is locked or not
		force_free(buffer);
		buffer.clear();
	}

	size_t len(void) const { return buffer.size(); }
	size_t cap(void) const { return buffer.capacity(); }

	bool operator!(void) const { return empty(); }

	void grow(size_t n) {
		buffer.reserve(n);
	}

	void write(const T&);
	void read(T&);

	void put(const T& t) { this->write(t); }
	T get(void) {
		T t;
		this->read(t);
		return t;
	}

private:
	std::mutex mx_;
	std::condition_variable not_empty_, not_full_;
	std::vector<T> buffer;

	template <typename U>
	friend std::ostream& operator<<(std::ostream&, const Chan<U>&);
};

// templated methods

template <typename T>
void Chan<T>::write(const T& t) {
	std::unique_lock<std::mutex> lk(mx_);

	// wait until not full
	try {
		not_full_.wait(lk, [this](){ return this->len() < this->cap(); });
	} catch(std::system_error) {
		throw OSError("wait on channel failed");
	}

	// we have a lock and we have room, put the item
	buffer.push_back(t);

	// wake up an other thread
	try {
		not_empty_.notify_one();
	} catch(std::system_error) {
		throw OSError("channel signal failed");
	}
}

template <typename T>
void Chan<T>::read(T& t) {
	std::unique_lock<std::mutex> lk(mx_);

	// wait until not empty
	try {
		not_empty_.wait(lk, [this](){ return this->len() != 0; });
	} catch(std::system_error) {
		throw OSError("wait on channel failed");
	}

	// get an item
	typename std::vector<T>::iterator it = buffer.begin();
	t = *it;
	buffer.erase(it);

	// wake up an other thread
	try {
		not_full_.notify_one();
	} catch(std::system_error) {
		throw OSError("channel signal failed");
	}
}

// used for printing
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Chan<T>& c) {
	os << c.str();
	return os;
}

}	// namespace

#endif	// OOCHAN_H_WJ112

// EOB
