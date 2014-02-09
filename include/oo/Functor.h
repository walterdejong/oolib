/*
	ooFunctior.h	WJ114

	- Functor only accepts void functions (for now)
	- it does accept variable arguments
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

#ifndef OOFUNCTOR_H_WJ114
#define OOFUNCTOR_H_WJ114

#include "oo/Base.h"
#include "oo/None.h"
#include "oo/Error.h"

#include <functional>
#include <utility>

namespace oo {

class Functor : public Base {
public:
	Functor() : Base(), f_(nullptr) { }
	Functor(const NoneObject& none) : Base(), f_(nullptr) { }

	Functor(const Functor& f) : Base(), f_(f.f_) { }

	// varargs constructor binds the function+arguments to f_
	template <typename... Tpack>
	Functor(Tpack&&... p) : Base(), f_(std::bind(std::forward<Tpack>(p)...)) { }

	Functor(const std::function<void()>& f) : Base(), f_(f) { }

	Functor(Functor&& f) : Functor() {
		swap(*this, f);
	}

//	~Set() { }

	Functor& operator=(Functor f) {
		swap(*this, f);
		return *this;
	}

	Functor& operator=(std::function<void()> f) {
		std::swap(f_, f);
		return *this;
	}

	static void swap(Functor& a, Functor& b) {
		std::swap(a.f_, b.f_);
	}

	// these assign and compare to None
	using Base::operator=;
	using Base::operator!;
	using Base::operator==;
	using Base::operator!=;

	std::string repr(void) const { return "<Functor>"; }

	bool isNone(void) const { return f_ == nullptr; }
	void setNone(void) { f_ = nullptr; }

	void operator()(void) const {
		if (f_ == nullptr) {
			throw ReferenceError();
		}
		f_();
	}

	void call(void) const {	this->operator()(); }

	operator std::function<void()>() { return f_; }

	std::function<void()> function(void) const { return f_; }

private:
	std::function<void()> f_;

	friend std::ostream& operator<<(std::ostream&, const Functor&);
};

// used for printing
inline std::ostream& operator<<(std::ostream& os, const Functor& f) {
	os << f.str();
	return os;
}

}	// namespace

#endif	// OOFUNCTOR_H_WJ114

// EOB
