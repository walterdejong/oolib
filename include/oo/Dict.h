/*
	ooDict.h	WJ112
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

#ifndef OODICT_H_WJ112
#define OODICT_H_WJ112

#include "oo/Base.h"
#include "oo/Sizeable.h"
#include "oo/Error.h"
#include "oo/String.h"
#include "oo/Array.h"
#include "oo/compare.h"

#include <map>
#include <ostream>
#include <sstream>

namespace oo {

template <typename T>
class Dict : public Base, public Sizeable, eq_less_comparable<Dict<T> > {
public:
	typedef T value_type;

	Dict() : Base(), Sizeable(), m_(std::map<String, T>()) { }

	Dict(const Dict& d) : Base(), Sizeable(), m_(std::map<String, T>(d.m_)) { }

	Dict(Dict<T>&& d) : Base(), Sizeable(), m_(std::move(d.m_)) { }

//	~Dict() { }

	Dict<T>& operator=(const Dict<T>& d) {
		if (this == &d) {
			return *this;
		}
		m_ = d.m_;
		return *this;
	}

	Dict<T>& operator=(Dict<T>&& d) {
		m_ = std::move(d.m_);
		return *this;
	}

	std::string repr(void) const;

	void clear(void) {
		m_.clear();
	}

	size_t len(void) const { return m_.size(); }
	size_t cap(void) const { return len(); }

	bool operator!(void) const { return this->empty(); }

	//	this never throws KeyError (like it does in Python)
	//	instead, any non-existing keys will create a new (empty) item in the map
	T& operator[](const String& key) { return m_.operator[](key); }

	bool operator==(const Dict<T>& d) const;
	bool operator<(const Dict<T>& d) const { return len() < d.len(); }

	bool has_key(const String& s) const { return m_.find(s) != m_.end(); }
	bool del(const String& s) { return m_.erase(s) == 1; }

	Array<String> keys(void) const;
	Array<T> values(void) const;

	// this enables range-based for-loops
	typename std::map<String, T>::iterator begin(void) { return m_.begin(); }
	typename std::map<String, T>::iterator end(void) { return m_.end(); }
	typename std::map<String, T>::const_iterator cbegin(void) const { return m_.cbegin(); }
	typename std::map<String, T>::const_iterator cend(void) const { return m_.cend(); }

private:
	std::map<String, T> m_;

	template <typename U>
	friend std::ostream& operator<<(std::ostream&, const Dict<U>&);
};


// templated Dict methods

template <typename T>
std::string Dict<T>::repr(void) const {
	std::stringstream ss;
	ss << "{";

	for(typename std::map<String, T>::const_iterator it = m_.cbegin(); it != m_.cend(); ) {
		// it->first are Strings
		// it->second, we don't know the type
		ss << '"' << it->first << '"' << ": " << it->second;
		++it;
		if (it != m_.cend()) {
			ss << ", ";
		}
	}
	ss << "}";
	return ss.str();
}

// compare two maps
template <typename T>
bool Dict<T>::operator==(const Dict<T>& d) const {
	if (len() != d.len()) {
		return false;
	}

	typename std::map<String, T>::const_iterator it2;

	for(typename std::map<String, T>::const_iterator it = m_.cbegin(); it != m_.cend(); ++it) {
		it2 = d.m_.find(it->first);
		if (it2 == d.m_.cend()) {
			return false;
		}
		if (it->second != it2->second) {
			return false;
		}
	}
	return true;
}

template <typename T>
Array<String> Dict<T>::keys(void) const {
	Array<String> a(len());

	int i = 0;
	for(typename std::map<String, T>::const_iterator it = m_.cbegin(); it != m_.cend(); ++it) {
		a[i++] = it->first;
	}
	return a;
}

template <typename T>
Array<T> Dict<T>::values(void) const {
	Array<T> a(len());

	int i = 0;
	for(typename std::map<String, T>::const_iterator it = m_.cbegin(); it != m_.cend(); ++it) {
		a[i++] = it->second;
	}
	return a;
}

// used for printing
template <typename T>
std::ostream& operator<<(std::ostream& os, const Dict<T>& d) {
	os << d.str();
	return os;
}

}	// namespace

#endif	// OODICT_H_WJ112

// EOB
