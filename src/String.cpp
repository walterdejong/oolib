/*
	ooString.cpp	WJ112
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

#include "oo/String.h"
#include "oo/print.h"

#include <cctype>

namespace oo {

const String kStringStripDefaultCharSet = String(" \t\r\n\v\f");
const size_t kSmallestString = 15;


String::String(const String& s) {
	s_len = s.s_len;
	s_cap = s.s_cap;
	s_data = new char[s_cap];
	std::memcpy(s_data, s.s_data, s_cap);
}

String::String(const std::string& s) {
	s_len = s.length();
	s_cap = s_len + 1;
	s_data = new char[s_cap];
	std::memcpy(s_data, s.c_str(), s_cap);
}

String::String(const char *s) {
	if (s == nullptr) {
		s_len = 0;
		s_cap = kSmallestString;
		s_data = new char[s_cap];
		*s_data = 0;
	} else {
		s_len = std::strlen(s);
		s_cap = s_len + 1;
		s_data = new char[s_cap];
		std::memcpy(s_data, s, s_cap);
	}
}

String::String(const char *s, size_t n) {
	if (s == nullptr) {
		s_len = 0;
		s_cap = kSmallestString;
		s_data = new char[s_cap];
		*s_data = 0;
	} else {
		s_cap = n + 1;
		s_data = new char[s_cap];
		std::strncpy(s_data, s, n);
		s_data[n] = 0;
		s_len = std::strlen(s_data);
	}
}

String::String(const rune *r) {
	if (r == nullptr) {
		s_len = 0;
		s_cap = kSmallestString;
		s_data = new char[s_cap];
		*s_data = 0;
	} else {
		s_len = s_cap = 0;
		s_data = nullptr;
		grow(utf8_encoded_len(r) + 1);
		utf8_encode(r, s_data, s_cap);
		s_len = std::strlen(s_data);
	}
}

String::String(rune code) {
	rune r[2] = { code, 0 };

	s_len = s_cap = 0;
	s_data = nullptr;
	grow(utf8_encoded_len(r) + 1);
	utf8_encode(r, s_data, s_cap);
	s_len = std::strlen(s_data);
}

void String::grow(size_t n) {
	if (n <= s_cap) {
		return;
	}

	// round up to next multiple of 16
	// this helps performance of strings that repeatedly grow
	// eg. by using operator+=()
	n += 15;
	n &= ~15;

	char *new_data = new char[n];
	new_data[0] = 0;

	if (s_data != nullptr) {
		std::memcpy(new_data, s_data, s_len + 1);
		delete [] s_data;
	}
	s_data = new_data;
	s_cap = n;
}

rune String::operator[](int idx) const {
	if (!s_len) {
		throw IndexError();
	}
	if (idx < 0) {
		idx += s_len;
		if (idx < 0) {
			throw IndexError();
		}
	}
	if ((size_t)idx > s_len) {
		throw IndexError();
	}
	if ((size_t)idx == s_len) {
		return 0;
	}
	return s_data[idx] & 0xff;
}

String& String::operator+=(const String& s) {
	if (!s.len()) {
		return *this;
	}

	grow(s_len + s.s_len + 1);
	std::memcpy(s_data + s_len, s.s_data, s.s_len + 1);
	s_len += s.s_len;
	return *this;
}

String& String::operator+=(rune code) {
	if (!code) {
		return *this;
	}

	size_t n = utf8_encoded_len(code);
	grow(s_len + n + 1);

	rune r[2] = { code, 0 };
	utf8_encode(r, s_data + s_len, n + 1);
	s_len += n;
	return *this;
}

String& String::operator*=(int n) {
	if (n <= 1) {
		return *this;
	}
	if (!s_len) {
		return *this;
	}

	grow(s_len * n + 1);

	size_t data_len = s_len;

	while(n > 1) {
		std::memcpy(s_data + data_len, s_data, s_len);
		data_len += s_len;
		n--;
	}
	s_len = data_len;
	s_data[s_len] = 0;
	return *this;
}

// default arguments start=0, end=0
int String::find(rune r, int start, int end) const {
	if (r >= 0x80) {
		String s(r);
		return find(s, start, end);
	}

	if (start < 0) {
		start += s_len;

		if (start < 0) {
			return -1;
		}
	}
	if (end <= 0) {
		end += s_len;

		if (end <= 0) {
			return -1;
		}
	}
	if ((size_t)end > s_len) {
		end = s_len;
	}
	if ((size_t)start >= s_len || start >= end) {
		return -1;
	}

	for(int pos = start; (size_t)pos <= end; pos++) {
		if (s_data[pos] == r) {
			return pos;
		}
	}
	return -1;
}

// default arguments start=0, end=0
int String::rfind(rune r, int start, int end) const {
	if (r >= 0x80) {
		String s(r);
		return rfind(s, start, end);
	}

	if (start < 0) {
		start += s_len;

		if (start < 0) {
			return -1;
		}
	}
	if (end <= 0) {
		end += s_len;

		if (end <= 0) {
			return -1;
		}
	}
	if ((size_t)end > s_len) {
		end = s_len;
	}
	if ((size_t)start >= s_len || start >= end) {
		return -1;
	}

	for(int pos = end-1; pos >= start; pos--) {
		if (s_data[pos] == r) {
			return pos;
		}
	}
	return -1;
}

// default arguments start=0, end=0
int String::find(const String& s, int start, int end) const {
	if (start < 0) {
		start += s_len;

		if (start < 0) {
			return -1;
		}
	}

	// end must be at least at 'distance' of length of search string
	end -= s.len();

	if (end <= 0) {
		end += s_len;

		if (end <= 0) {
			return -1;
		}
	}
	if ((size_t)end > s_len) {
		end = len();
	}
	if ((size_t)start >= s_len || start > end) {
		return -1;
	}

	for(int pos = start; pos <= end; pos++) {
		if (!std::strncmp(s_data + pos, s.s_data, s.s_len)) {
			return pos;
		}
	}
	return -1;
}

// default arguments start=0, end=0
int String::rfind(const String& s, int start, int end) const {
	if (start < 0) {
		start += s_len;

		if (start < 0) {
			return -1;
		}
	}

	// end must be at least at 'distance' of length of search string
	end -= s.len();

	if (end <= 0) {
		end += s_len;

		if (end <= 0) {
			return -1;
		}
	}
	if ((size_t)end > s_len) {
		end = len();
	}
	if ((size_t)start >= s_len || start > end) {
		return -1;
	}

	for(int pos = end; pos >= start; pos--) {
		if (!std::strncmp(s_data + pos, s.s_data, s.s_len)) {
			return pos;
		}
	}
	return -1;
}

// default argument is a default strippin charset
String String::strip(const String& charset) const {
	if (!s_len) {
		return String(*this);
	}

	// lstrip
	char *p = s_data;
	while(*p) {
		if (charset.find(*p) == -1) {
			break;
		}
		p++;
	}
	if (!*p) {
		return String();
	}

	String s(p);

	// rstrip
	int l = s.len() - 1;
	while(l >= 0) {
		if (charset.find(s.s_data[l]) == -1) {
			break;
		}
		s.s_data[l--] = 0;
		s.s_len--;
	}
	if (!s) {
		return String();
	}
	return s;
}

String String::lstrip(const String& charset) const {
	if (!s_len) {
		return String(*this);
	}

	// lstrip
	char *p = s_data;
	while(*p) {
		if (charset.find(*p) == -1) {
			break;
		}
		p++;
	}
	if (!*p) {
		return String();
	}
	return String(p);
}

String String::rstrip(const String& charset) const {
	String s(*this);

	// rstrip
	int l = s.len() - 1;
	while(l >= 0) {
		if (charset.find(s.s_data[l]) == -1) {
			break;
		}
		s.s_data[l--] = 0;
		s.s_len--;
	}
	if (!s) {
		return String();
	}
	return s;
}

String String::upper(void) const {
	String s(*this);

	char *p = s.s_data;
	while(*p) {
		if (*p >= 'a' && *p <= 'z') {
			*p -= ' ';
		}
		p++;
	}
	return s;
}

String String::lower(void) const {
	String s(*this);

	char *p = s.s_data;
	while(*p) {
		if (*p >= 'A' && *p <= 'Z') {
			*p += ' ';
		}
		p++;
	}
	return s;
}

String String::capitalize(void) const {
	String s(*this);

	char *p = s.s_data;
	if (*p >= 'a' && *p <= 'z') {
		*p -= ' ';
	}
	return s;
}

String String::capwords(void) const {
	String s(*this);

	char *p = s.s_data;
	bool cap_next = true;

	while(*p) {
		if (cap_next) {
			if (std::isalnum(*p)) {
				*p = std::toupper(*p);
				cap_next = false;
			}
		} else {
			if (!std::isalnum(*p)) {
				cap_next = true;
			}
		}
		p++;
	}
	return s;
}

String String::slice(int idx1, int idx2) const {
	if (idx1 < 0) {
		idx1 += len();

		if (idx1 < 0)
			idx1 = 0;
	}
	if ((size_t)idx1 >= len()) {
		return String();
	}
	if (idx2 < 0) {
		idx2 += len();

		if (idx2 < 0) {
			idx2 = 0;
		}
	}
	if ((size_t)idx2 > len()) {
		idx2 = len();
	}
	if (idx1 >= idx2) {
		return String();
	}

	return String(s_data + idx1, idx2 - idx1);
}

// default argument n == -1 (replace all)
String String::replace(const String& sub, const String& repl, int n) const {
	if (!n) {
		return *this;
	}

	std::string s = s_data;

	size_t pos = 0;

	while(true) {
		pos = s.find(sub.s_data, pos);
		if (pos == std::string::npos) {
			break;
		}
		s.replace(pos, sub.len(), repl.s_data);
		pos += repl.len();

		if (n > 0) {
			n--;
			if (!n) {
				break;
			}
		}
	}
	return String(s);
}

// default argument sep = ' '
Array<String> String::split(rune sep) const {
	if (!s_len) {
		return Array<String>();
	}

	const String search(sep);

	Array<String> a;

	char *p = s_data;
	int pos, start = 0;
	while(true) {
		pos = find(search, start);
		if (pos == -1) {
			a.append(String(p));
			break;
		}

		a.append(String(p, pos - start));

		start = pos + search.len();
		p = s_data + start;
	}
	return a;
}

// default argument sep = ' '
String String::join(const Array<String>& a, rune sep) const {
	if (sep >= 0x80) {
		return join(a, String(sep));
	}

	if (!a.len()) {
		return String();
	}

	// first calculate total size
	size_t total = 0;
	for(size_t i = 0; i < a.len(); i++) {
		total += a[i].len() + 1;
	}
	total--;

	// make the joined string
	String s;
	s.grow(total + 1);

	size_t l;

	if (a[0].s_data != nullptr) {
		std::strcpy(s.s_data, a[0].s_data);
		l = a[0].len();
	} else {
		s.s_data[0] = 0;
		l = 0;
	}

	for(size_t i = 1; i < a.len(); i++) {
		s.s_data[l++] = sep;
		std::strcpy(s.s_data + l, a[i].s_data);
		l += a[i].len();
	}
	s.s_data[l] = 0;
	s.s_len = l;
	return s;
}

String String::join(const Array<String>& a, const String& sep) const {
	if (!a.len()) {
		return String();
	}

	// first calculate total size
	const String add(sep);

	size_t total = 0;
	for(size_t i = 0; i < a.len(); i++) {
		total += a[i].len() + add.len();
	}
	total -= add.len();

	// make the joined string
	String s;
	s.grow(total + 1);

	if (a[0].s_data != nullptr) {
		std::strcpy(s.s_data, a[0].s_data);
	} else {
		s.s_data[0] = 0;
	}
	for(size_t i = 1; i < a.len(); i++) {
		std::strcat(s.s_data, add.s_data);

		if (a[i].s_data != nullptr) {
			std::strcat(s.s_data, a[i].s_data);
		}
	}
	s.s_len = std::strlen(s.s_data);
	return s;
}

void String::utf8_encode(const rune *r, char *s, size_t n) {
	if (r == nullptr || s == nullptr) {
		throw ReferenceError();
	}
	if (!n) {
		throw ValueError();
	}

	rune code;

	while(*r && n > 0) {
		code = *r;

		if (code < 0x80) {
			if (n <= 1) {
				throw StringEncodingError("String::utf8_encode(): buffer too small");
			}
			*s++ = code;
			n--;
		} else {
			if (code < 0x800) {
				if (n <= 2) {
					throw StringEncodingError("String::utf8_encode(): buffer too small");
				}
				*s++ = ((code & 0x7c0) >> 6) | 0xc0;
				*s++ = (code & 0x3f) | 0x80;
				n -= 2;
			} else {
				if ((code >= 0x800 && code < 0xd800) || (code >= 0xe000 && code < 0x10000)) {
					if (n <= 3) {
						throw StringEncodingError("String::utf8_encode(): buffer too small");
					}
					*s++ = ((code & 0xf000) >> 12) | 0xe0;
					*s++ = ((code & 0xfc0) >> 6) | 0x80;
					*s++ = (code & 0x3f) | 0x80;
					n -= 3;
				} else {
					if (code > 0x10000 && code <= 0x10ffff) {
						if (n <= 4) {
							throw StringEncodingError("String::utf8_encode(): buffer too small");
						}
						*s++ = ((code & 0x1c0000) >> 18) | 0xf0;
						*s++ = ((code & 0x3f000) >> 12) | 0x80;
						*s++ = ((code & 0xfc0) >> 6) | 0x80;
						*s++ = (code & 0x3f) | 0x80;
						n -= 4;
					} else {
						throw StringEncodingError();
					}
				}
			}
		}
		r++;
	}
	*s = 0;
}

size_t String::utf8_encoded_len(rune code) {
	if (code < 0x80) {
		return 1;
	}
	if (code >= 0x80 && code < 0x800) {
		return 2;
	}
	if (code >= 0x800 && code < 0xd800) {
		return 3;
	}
	if (code >= 0xe000 && code < 0x10000) {
		return 3;
	}
	if (code > 0x10000 && code <= 0x10ffff) {
		return 4;
	}
	throw StringEncodingError();
	return 4;	// not reached
}

size_t String::utf8_encoded_len(const rune *r) {
	if (r == nullptr) {
		throw ReferenceError();
	}

	size_t l = 0;

	while(*r) {
		l += utf8_encoded_len(*r);
		r++;
	}
	return l;
}

String sprint(const char *fmt, ...) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}
	if (!*fmt) {
		return String();
	}

	va_list ap;
	va_start(ap, fmt);

	String s = vsprint(fmt, ap);

	va_end(ap);
	return s;
}

String vsprint(const char *fmt, va_list ap) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}
	if (!*fmt) {
		return String();
	}

	std::stringstream ss;

	vssprint(ss, fmt, ap);

	return String(ss.str());
}

}	// namespace

// EOB
