/*
	ooRef.h	WJ112
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

#ifndef OOREF_H_WJ112
#define OOREF_H_WJ112

#include "oo/Base.h"
#include "oo/Error.h"

#include <ostream>
#include <memory>
#include <utility>

namespace oo {

// Ref is the same as a shared_ptr, but it can be None

template <typename T>
class Ref : public Base {
public:
	Ref() : Base(), r_(), isnone_(true) { }

	// warning: this is NOT for arbitrary pointers ... only for "new T"'s
	Ref(T *p) : Base(), r_(p), isnone_(false) {
		if (p == nullptr) {
			throw ValueError();
		}
	}

	Ref(const Ref<T>& r) : Base(), r_(r.r_), isnone_(r.isnone_) { }

	Ref(Ref<T>&& r) : Ref() { swap(*this, r); }

	Ref(const NoneObject& none) : Base(), r_(), isnone_(true) { }

//	virtual ~Ref() { }

	Ref<T>& operator=(Ref<T> r) {
		swap(*this, r);
		return *this;
	}

	static void swap(Ref<T>& a, Ref<T>& b) {
		std::swap(a.r_, b.r_);
		std::swap(a.isnone_, b.isnone_);
	}

	// these assign and compare to None
	using Base::operator=;
	using Base::operator!;
	using Base::operator==;
	using Base::operator!=;

	bool isNone(void) const { return isnone_; }
	void setNone(void) {
		r_ = std::shared_ptr<T>();
		isnone_ = true;
	}

	virtual std::string repr(void) const { return "<Ref>"; }

	virtual std::string str(void) const {
		if (isnone_)
			return "(None)";

		return repr();
	}

	T& operator*(void) const {
		if (isnone_)
			throw ReferenceError();

		return *r_;
	}

	T *operator->(void) const {
		if (isnone_)
			throw ReferenceError();

		return r_.operator->();
	}

	T *get(void) const {
		if (isnone_)
			return nullptr;

		return r_.get();
	}

private:
	std::shared_ptr<T> r_;
	bool isnone_;

	template <typename U>
	friend std::ostream& operator<<(std::ostream&, const Ref<U>&);
};

// used for printing
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Ref<T>& r) {
	os << r.str();
	return os;
}

}	// namespace

#endif	// OOREF_H_WJ112

// EOB
