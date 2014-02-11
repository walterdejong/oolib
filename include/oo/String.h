/*
	ooString.h	WJ112
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

#ifndef OOSTRING_H_WJ112
#define OOSTRING_H_WJ112

#include "oo/Base.h"
#include "oo/Sizeable.h"
#include "oo/Error.h"
#include "oo/Array.h"
#include "oo/compare.h"
#include "oo/types.h"

#include <cstdarg>
#include <cstring>
#include <string>
#include <ostream>
#include <sstream>
#include <istream>

namespace oo {

class String;

extern const String kStringStripDefaultCharSet;


class String : public Base, public Sizeable, eq_less_comparable<String> {
public:
	String() : Base(), Sizeable(), s_len(0), s_cap(1) {
		s_data = new char[1];
		*s_data = 0;
	}

	String(const NoneObject&) : Base(), Sizeable(), s_len(0), s_cap(0), s_data(nullptr) { }

	String(const String&);
	String(const std::string&);
	String(const char *);
	String(const rune *);
	String(rune);

	String(String&& s) : String() {
		swap(*this, s);
	}

	~String() {
		if (s_data != nullptr) {
			delete [] s_data;
		}
	}

	String& operator=(String copy) {
		swap(*this, copy);
		return *this;
	}

	static void swap(String& a, String& b) {
		std::swap(a.s_len, b.s_len);
		std::swap(a.s_cap, b.s_cap);
		std::swap(a.s_data, b.s_data);
	}

	// these assign and compare to None
	using Base::operator=;
	using Base::operator!;
	using Base::operator==;
	using Base::operator!=;

	bool isNone(void) const { return (s_data == nullptr); }
	void setNone(void) { clear(); }

	void clear(void) {
		if (s_data != nullptr) {
			delete [] s_data;
			s_data = nullptr;
		}
		s_len = s_cap = 0;
	}

	std::string repr(void) const {
		if (isNone()) {
			return "(None)";
		}
		std::stringstream ss;
		ss << '"' << s_data << '"';
		return ss.str();
	}

	std::string str(void) const {
		if (isNone()) {
			return "(none)";
		}
		return std::string(s_data);
	}

	size_t len(void) const { return s_len; }
	size_t cap(void) const { return s_cap; }

	void grow(size_t);

	rune operator[](int) const;

	String& operator+=(const String&);

	String operator+(const String& s) const {
		return String(*this) += s;
	}

	String& operator+=(rune);

	String operator+(rune r) const {
		return String(*this) += r;
	}

	String& operator*=(int);

	String operator*(int n) const {
		return String(*this) *= n;
	}

	bool operator==(const String& s) const {
		if (isNone()) {
			return false;
		}
		if (s_len != s.s_len) {
			return false;
		}
		return (std::strcmp(s_data, s.s_data) == 0);
	}

	bool operator<(const String& s) const {
		if (isNone()) {
			throw ReferenceError();
		}
		return (std::strcmp(s_data, s.s_data) < 0);
	}

	int find(rune, int=0, int=0) const;
	int rfind(rune, int=0, int=0) const;

	int find(const String&, int=0, int=0) const;
	int rfind(const String&, int=0, int=0) const;

	String strip(const String& charset=kStringStripDefaultCharSet) const;
	String lstrip(const String& charset=kStringStripDefaultCharSet) const;
	String rstrip(const String& charset=kStringStripDefaultCharSet) const;

	// these only work correctly for English text ...
	String upper(void) const;
	String lower(void) const;
	String capitalize(void) const;
	String capwords(void) const;

	String slice(int, int) const;

	String substr(int start, size_t num = 0) const {
		if (num == 0) {
			num = s_len;
		}
		return slice(start, num);
	}

	String replace(const String&, const String&, int n=-1) const;

	Array<String> split(rune=' ') const;

	// this method should have been a static ... but this is how Python does it, too
	String join(const Array<String>&, rune=' ') const;
	String join(const Array<String>&, const String&) const;

	const char *c_str(void) const { return s_data; }

private:
	size_t s_len, s_cap;
	char *s_data;

	static size_t utf8_len(const char *);		// returns # of characters in utf-8 string (excluding nul terminator)
	static void utf8_decode(const char *, rune *, size_t);
	static void utf8_encode(const rune *, char *, size_t);
	static size_t utf8_encoded_size(rune);
	static size_t utf8_encoded_len(const rune *);

	friend std::ostream& operator<<(std::ostream&, const String&);
};

// used for printing
inline std::ostream& operator<<(std::ostream& os, const String& s) {
	os << s.str();
	return os;
}

// convert string to different type (like an int)
// thanks to stackoverflow.com
template <typename T>
T convert(const std::string& s) {
	std::istringstream iss(s);
	T v;

	iss >> std::ws >> v >> std::ws;

	if (!iss.eof()) {
		throw ValueError();
	}
	return v;
}

template <typename T>
inline T convert(const char *s) {
	if (s == nullptr) {
		throw ReferenceError();
	}
	return convert<T>(std::string(s));
}

template <typename T>
inline T convert(const String& s) {
	if (s.isNone()) {
		throw ReferenceError();
	}
	return convert<T>(s.str());
}

String sprint(const char *fmt, ...);
String vsprint(const char *fmt, va_list);

}	// namespace

#endif	// OOSTRING_H_WJ112

// EOB
