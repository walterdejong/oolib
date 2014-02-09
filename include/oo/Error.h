//
//	ooError.h	WJ112
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

#ifndef OOERROR_H_WJ112
#define OOERROR_H_WJ112

#include "oo/Base.h"

#include <ostream>

namespace oo {

// error constants are in the range -1 .. ~ -20
// it's nice to start your own error codes at -100 or so
// although it doesn't really matter as each error is its own exception type

extern const int kMemoryError;
extern const int kIndexError;
extern const int kValueError;
extern const int kTypeError;
extern const int kZeroDivisionError;
extern const int kReferenceError;
extern const int kAssertionError;
extern const int kIOError;
extern const int kRuntimeError;
extern const int kOSError;

class Error : public Base {
public:
	// how many constructors does it take?
	Error() : Base(), errcode(0), errname("Error"), errmsg("unknown error") { }
	Error(const NoneObject& none) : Base(), errcode(0), errname(""), errmsg("unknown error") { }

	Error(int c, const char *n, const char *m) : Base(), errcode(c), errname(n), errmsg(m) { }
	Error(int c, const std::string& n, const std::string& m) : Base(), errcode(c), errname(n), errmsg(m) { }

	Error(const Error& e) : Base(), errcode(e.errcode), errname(e.errname), errmsg(e.errmsg) { }

	Error(Error&& e) : Error() {
		swap(*this, e);
	}

	virtual ~Error() { }

	Error& operator=(Error e) {
		swap(*this, e);
		return *this;
	}

	static void swap(Error& a, Error& b) {
		std::swap(a.errcode, b.errcode);
		std::swap(a.errname, b.errname);
		std::swap(a.errmsg, b.errmsg);
	}

	// these assign and compare to None
	using Base::operator=;
	using Base::operator!;
	using Base::operator==;
	using Base::operator!=;

	std::string repr(void) const;
	std::string str(void) const;

	bool isNone(void) const { return (errname.size() == 0); }
	void setNone(void) { clear(); }

	void clear(void) {
		errcode = 0;
		errname = "";
		errmsg = "unknown error";
	}

	int code(void) const { return errcode; }
	std::string name(void) const { return errname; }
	std::string message(void) const { return errmsg; }

protected:
	int errcode;
	std::string errname, errmsg;

	friend std::ostream& operator<<(std::ostream&, const Error&);
};

inline std::ostream& operator<<(std::ostream& os, const Error& e) {
	os << e.str();
	return os;
}

// this is a stanza for defining new Error types

#define define_error(x,y)		\
	extern const int k##x ;		\
	class x : public Error {									\
	public:														\
		x() : Error(k##x, #x, (y)) { }							\
		x(int c, const char *m) : Error(c, #x, m) { }			\
		x(int c, const std::string& m) : Error(c, #x, m) { }	\
		x(const char *m) : Error(k##x, #x, m) { }				\
		x(const std::string& m) : Error(k##x, #x, m) { }		\
		x(const x& e) : Error(e.errcode, #x, e.errmsg) { }		\
		x(x&& e) : x() { swap(*this, e); }						\
		x(const NoneObject& none) : Error(none) { }				\
		x& operator=(x e) {										\
			swap(*this, e);										\
			return *this;										\
		}														\
		using Base::operator=;		\
		using Base::operator!;		\
		using Base::operator==;		\
		using Base::operator!=;		\
	}

define_error(MemoryError, "out of memory");
define_error(IndexError, "index out of range");
define_error(ValueError, "invalid value");
define_error(TypeError, "invalid type in operation");
define_error(ZeroDivisionError, "division by zero");
define_error(ReferenceError, "null pointer reference");	// usually not thrown; use ValueError or assert() instead
define_error(AssertionError, "assertion failed");
define_error(IOError, "I/O error");
define_error(RuntimeError, "runtime error");			// often used for other kind of errors
define_error(OSError, "OS error");

void panic(const Error&);
void panic(const std::string&);

}	// namespace

#endif	// OOERROR_H_WJ112

// EOB
