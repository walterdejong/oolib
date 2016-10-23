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
#include <sstream>

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
extern const int kIOError;
extern const int kRuntimeError;
extern const int kOSError;
extern const int kStringEncodingError;

class Error : public Base {
public:
	// how many constructors does it take?
	Error() : Base(), errcode(0), errname("Error"), errmsg("unknown error") { }

	Error(int c, const char *n, const char *m) : Base(), errcode(c), errname(n), errmsg(m) { }
	Error(int c, const std::string& n, const std::string& m) : Base(), errcode(c), errname(n), errmsg(m) { }

	Error(const Error& e) : Base(), errcode(e.errcode), errname(e.errname), errmsg(e.errmsg) { }

	Error(Error&& e) : Error() {
		errcode = e.errcode;
		errname = std::move(e.errname);
		errmsg = std::move(e.errmsg);
	}

	virtual ~Error() { }

	Error& operator=(const Error& e) {
		if (this == &e) {
			return *this;
		}
		errcode = e.errcode;
		errname = e.errname;
		errmsg = e.errmsg;
		return *this;
	}

	Error& operator=(Error&& e) {
		errcode = e.errcode;
		errname = std::move(e.errname);
		errmsg = std::move(e.errmsg);
		return *this;
	}

	std::string repr(void) const {
		std::stringstream ss;
		ss << "<" << errname << ": " << errcode << ", \"" << errmsg << "\">";
		return ss.str();
	}

	std::string str(void) const {
		return errname + ": " + errmsg;
	}

	void clear(void) {
		errcode = 0;
		errname = "";
		errmsg = "unknown error";
	}

	bool operator!(void) const { return (errcode == 0); }

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

#define oo_define_error(x,y)	\
	extern const int k##x ;		\
	class x : public Error {									\
	public:														\
		x() : Error(k##x, #x, (y)) { }							\
		x(int c, const char *m) : Error(c, #x, m) { }			\
		x(int c, const std::string& m) : Error(c, #x, m) { }	\
		x(const char *m) : Error(k##x, #x, m) { }				\
		x(const std::string& m) : Error(k##x, #x, m) { }		\
		x(const x& e) : Error(e) { }							\
		x(x&& e) : Error(e) { }									\
		x& operator=(const x& e) {								\
			Error::operator=(e);								\
			return *this;										\
		}														\
		x& operator=(x&& e) {									\
			Error::operator=(e);								\
			return *this;										\
		}														\
	}

oo_define_error(MemoryError, "out of memory");
oo_define_error(IndexError, "index out of range");
oo_define_error(ValueError, "invalid value");
oo_define_error(TypeError, "invalid type in operation");
oo_define_error(ZeroDivisionError, "division by zero");
oo_define_error(ReferenceError, "null pointer reference");
oo_define_error(IOError, "I/O error");
oo_define_error(RuntimeError, "runtime error");	// often used for other kind of errors
oo_define_error(OSError, "OS error");
oo_define_error(StringEncodingError, "string encoding error");

void panic(const Error&);
void panic(const std::string&);

}	// namespace

#endif	// OOERROR_H_WJ112

// EOB
