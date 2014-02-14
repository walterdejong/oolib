/*
	ooArray.h	WJ112
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

#ifndef OOARRAY_H_WJ112
#define OOARRAY_H_WJ112

#include "oo/Base.h"
#include "oo/Sequence.h"
#include "oo/Error.h"
#include "oo/compare.h"

#include <cstdlib>
#include <vector>
#include <list>
#include <algorithm>
#include <ostream>
#include <sstream>

namespace oo {

// Array is an array, implemented with std::vector

template <typename T>
class List;

template <typename T>
class Array : public Base, public Sequence<T>, eq_less_comparable<Array<T> > {
public:
	typedef T value_type;
	typedef typename std::vector<T>::iterator iterator_type;
	typedef typename std::vector<T>::const_iterator const_iterator_type;

	Array() : Base(), Sequence<T>(), v_(std::vector<T>()) { }

	Array(size_t n) : Base(), Sequence<T>(), v_(std::vector<T>(n)) { }

	Array(const Array<T>& a) : Base(), Sequence<T>(), v_(a.v_) { }

	Array(Array<T>&& a) : Array() {
		swap(*this, a);
	}

	Array(const List<T>&);	// convert List to Array

	Array(std::initializer_list<T> a) : Array() {
		v_.reserve(a.size());
		for (auto it = a.begin(); it != a.end(); ++it) {
			v_.push_back(*it);
		}
	}

//	~Array() { }

	Array<T>& operator=(Array<T> a) {
		swap(*this, a);
		return *this;
	}

	static void swap(Array<T>& a, Array<T>& b) {
		std::swap(a.v_, b.v_);
	}

	std::string repr(void) const;

	// the swap trick frees all memory in the vector
	static void force_free(std::vector<T>& v) {
		std::vector<T> tmp;
		v.swap(tmp);
		// stack unwinds, deleting tmp
	}

	void clear(void) {
		force_free(v_);
		v_.clear();
	}

	size_t len(void) const { return v_.size(); }
	size_t cap(void) const { return v_.capacity(); }

	void grow(size_t n) {
		// round up to next multiple of 4
		// this helps performance of arrays that repeatedly grow
		// eg. by using operator+=()
		n = n + 4 - (n % 4);
		v_.reserve(n);
	}

	bool operator!(void) const { return this->empty(); }

	T& operator[](int idx);
	const T& operator[](int idx) const;

	Array<T>& operator+=(const Array<T>& t) {
		grow(v_.size() + t.v_.size());
		v_.insert(v_.end(), t.v_.begin(), t.v_.end());
		return *this;
	}

	Array<T> operator+(const Array<T>& a) const {
		Array<T> t = *this;
		t += a;
		return t;
	}

	// requires that T implements operator==()
	bool operator==(const Array<T>& t) const {
		if (len() != t.len()) {
			return false;
		}
		return std::equal(v_.begin(), v_.end(), t.v_.first());
	}

	bool operator<(const Array<T>& a) const { return len() < a.len(); }

	void append(const T& t) { grow(len() + 1); v_.push_back(t); }
	void push(const T& t) { append(t); }
	T pop(int idx=-1);
	void insert(int idx, const T&);
	bool remove(const T&);
	void extend(const Array<T>& a) { operator+=(a); }

	// requires that T implements operator==()
	int find(const T&) const;
	int rfind(const T&) const;

	// requires that T implements operator==() and operator<()
	void sort(void) { std::sort(v_.begin(), v_.end()); }
	bool bsearch(const T& t) const { return std::binary_search(v_.begin(), v_.end(), t); }

	void shuffle(void) { std::random_shuffle(v_.begin(), v_.end()); }
	void rotate(int);

	void unique(void) {
		typename std::vector<T>::iterator it = std::unique(v_.begin(), v_.end());
		v_.resize(it - v_.begin());
	}

	unsigned int count(const T& t) const { return (unsigned int)std::count(v_.begin(), v_.end(), t); }
	void fill(const T& t) { std::fill(v_.begin(), v_.end(), t); }

	Array<T> slice(int, int) const;

	// this enables range-based for-loops
	iterator_type begin(void) { return v_.begin(); }
	iterator_type end(void) { return v_.end(); }
	const_iterator_type cbegin(void) const { return v_.cbegin(); }
	const_iterator_type cend(void) const { return v_.cend(); }

	void push_back(const T& item) { v_.push_back(item); }

private:
	std::vector<T> v_;

	template <typename U>
	friend std::ostream& operator<<(std::ostream&, const Array<U>&);
};

// templated array methods are here ...

// convert List to Array
template <typename T>
Array<T>::Array(const List<T>& a) : Base(), Sequence<T>(), v_(std::vector<T>()) {
	v_.reserve(a.len());
	for(typename std::list<T>::const_iterator it = a.cbegin(); it != a.cend(); ++it) {
		v_.push_back(*it);
	}
}

template <typename T>
std::string Array<T>::repr(void) const {
	std::stringstream ss;
	ss << '[';

	for(typename std::vector<T>::const_iterator it = v_.cbegin(); it < v_.cend(); ) {
/*
	I wish this were possible, but g++ won't compile it when T is not a class
	Well, too bad ...

		if ((p = dynamic_cast<const Base *>(&(*it))) != nullptr)
			ss << p->repr();
		else
*/
		ss << *it;

		++it;
		if (it < v_.cend()) {
			ss << ", ";
		}
	}
	ss << ']';
	return ss.str();
}

template <typename T>
T& Array<T>::operator[](int idx) {
	if (idx < 0) {
		idx += len();
		if (idx < 0) {
			throw IndexError();
		}
	}
	if ((size_t)idx >= len()) {
		throw IndexError();
	}
	return v_.operator[](idx);
}

template <typename T>
const T& Array<T>::operator[](int idx) const {
	if (idx < 0) {
		idx += len();
		if (idx < 0) {
			throw IndexError();
		}
	}
	if ((size_t)idx >= len()) {
		throw IndexError();
	}
	return v_.operator[](idx);
}

// default argument idx=-1
template <typename T>
T Array<T>::pop(int idx) {
	if (idx < 0) {
		idx += len();
		if (idx < 0) {
			throw IndexError();
		}
	}
	if ((size_t)idx > len()) {
		throw IndexError();
	}

	T t = v_.operator[](idx);
	v_.erase(v_.begin() + idx);
	return t;
}

template <typename T>
void Array<T>::insert(int idx, const T& t) {
	if (idx < 0) {
		idx += len();
		if (idx < 0) {
			throw IndexError();
		}
	} else {
		if ((size_t)idx > len()) {
			idx = len();
		}
	}
	grow(len() + 1);
	v_.insert(v_.begin() + idx, t);
}

template <typename T>
bool Array<T>::remove(const T& t) {
	typename std::vector<T>::iterator it = std::find(v_.begin(), v_.end(), t);

	if (it == v_.end())	{	// not found
		return false;
	}
	v_.erase(it);
	return true;
}

template <typename T>
int Array<T>::find(const T& t) const {
	typename std::vector<T>::const_iterator it = std::find(v_.cbegin(), v_.cend(), t);

	if (it == v_.cend()) {	// not found
		return -1;
	}
	return (it - v_.cbegin());
}

template <typename T>
int Array<T>::rfind(const T& t) const {
	int n = 1;
	for(typename std::vector<T>::const_reverse_iterator it = v_.rbegin(); it != v_.rend(); ++it) {
		if (*it == t) {
			return (len() - n);
		}
		n++;
	}
	return -1;
}

template <typename T>
void Array<T>::rotate(int n) {
	// if n < 0, rotate left
	// if n > 0, rotate right
	if (!n) {
		return;
	}
	// std::rotate() rotates left for positive values, so ...
	n = -n;
	if (n < 0) {
		n += len();
		if (n < 0) {
			throw ValueError();
		}
	} else {
		if ((size_t)n > len()) {
			throw ValueError();
		}
	}
	std::rotate(v_.begin(), v_.begin() + n, v_.end());
}

template <typename T>
Array<T> Array<T>::slice(int idx1, int idx2) const {
	if (idx1 < 0) {
		idx1 += len();
		if (idx1 < 0) {
			idx1 = 0;
		}
	}
	if ((size_t)idx1 >= len()) {
		return Array<T>();
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
		return Array<T>();
	}
	Array<T> a(idx2 - idx1);
	std::copy(v_.begin() + idx1, v_.begin() + idx2, a.v_.begin());
	return a;
}

// used for printing
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Array<T>& a) {
	os << a.str();
	return os;
}

}	// namespace

#endif	// OOARRAY_H_WJ112

// EOB
