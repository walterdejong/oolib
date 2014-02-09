/*
	ooList.h	WJ112
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

#ifndef OOLIST_H_WJ112
#define OOLIST_H_WJ112

#include "oo/Base.h"
#include "oo/None.h"
#include "oo/Sequence.h"
#include "oo/Error.h"
#include "oo/compare.h"

#include <cstdlib>
#include <list>
#include <vector>
#include <algorithm>
#include <ostream>
#include <sstream>
#include <cassert>

namespace oo {

// List is backed by std::list, which is a double linked list
// You can use it in the same way as an Array though, except that you
// can't reserve initial space for a List; it has no capacity like Array

template <typename T>
class Array;

template <typename T>
class List : public Base, public Sequence<T>, eq_less_comparable<List<T> > {
public:
	typedef T value_type;
	typedef typename std::list<T>::iterator iterator_type;
	typedef typename std::list<T>::const_iterator const_iterator_type;

	List() : Base(), Sequence<T>(), l_(std::list<T>()) { }

	List(const NoneObject& none) : Base(), Sequence<T>(), l_(std::list<T>()) { }

	List(const List<T>& a) : Base(), Sequence<T>(), l_(a.l_) { }

	List(List<T>&& a) : List() {
		swap(*this, a);
	}

	List(const Array<T>&);	// convert array to list

	List(std::initializer_list<T> a) : List() {
		for (auto it = a.begin(); it != a.end(); it++) {
			l_.push_back(*it);
		}
	}

//	~List() { }

	List<T>& operator=(List<T> a) {
		swap(*this, a);
		return *this;
	}

	static void swap(List<T>& a, List<T>& b) {
		std::swap(a.l_, b.l_);
	}

	// these assign and compare to None
	using Base::operator=;
	using Base::operator!;
	using Base::operator==;
	using Base::operator!=;

	std::string repr(void) const;

	bool isNone(void) const { return (l_.size() == 0); }
	void setNone(void) { clear(); }

	void clear(void) { l_.clear(); }

	size_t len(void) const { return l_.size(); }

	T& operator[](int idx);
	const T& operator[](int idx) const;

	List<T>& operator+=(const List<T>& t) {
		l_.insert(l_.end(), t.l_.begin(), t.l_.end());
		return *this;
	}

	List<T> operator+(const List<T>& a) const {
		List<T> t = *this;
		t += a;
		return t;
	}

	// requires that T implements operator==()
	bool operator==(const List<T>& t) const {
		if (len() != t.len())
			return false;

		return std::equal(l_.begin(), l_.end(), t.l_.first());
	}

	bool operator<(const List<T>& a) const { return len() < a.len(); }

	void append(const T& t) { l_.push_back(t); }
	void push(const T& t) { append(t); }
	T pop(int idx=-1);
	void insert(int idx, const T&);
	bool remove(const T&);
	void extend(const List<T>& a) { operator+=(a); }

	// requires that T implements operator==()
	int find(const T&) const;
	int rfind(const T&) const;

	// requires that T implements operator==() and operator<()
	void sort(void) { l_.sort(); }
	bool bsearch(const T& t) const { return std::binary_search(l_.begin(), l_.end(), t); }

// std::random_shuffle() does not work with list::iterator
//	void shuffle(void) { std::random_shuffle(l_.begin(), l_.end()); }
	void rotate(int);
	void unique(void);

	unsigned int count(const T& t) const { return (unsigned int)std::count(l_.begin(), l_.end(), t); }
	void fill(const T& t) { std::fill(l_.begin(), l_.end(), t); }

	List<T> slice(int, int) const;

	// this enables range-based for-loops
	iterator_type begin(void) { return l_.begin(); }
	iterator_type end(void) { return l_.end(); }
	const_iterator_type cbegin(void) const { return l_.cbegin(); }
	const_iterator_type cend(void) const { return l_.cend(); }

	void push_back(const T& item) { l_.push_back(item); }

	template <typename U>
	friend std::ostream& operator<<(std::ostream&, const List<U>&);

private:
	std::list<T> l_;
};

// templated methods are here ...

// convert Array to List
template <typename T>
List<T>::List(const Array<T>& a) : Base(), Sequence<T>(), l_(std::list<T>()) {
	for(typename std::vector<T>::const_iterator it = a.cbegin(); it != a.cend(); ++it) {
		l_.push_back(*it);
	}
}

template <typename T>
std::string List<T>::repr(void) const {
	std::stringstream ss;
	ss << '(';

	for(typename std::list<T>::const_iterator it = l_.cbegin(); it != l_.cend(); ) {
/*
	I wish this were possible, but g++ won't compile it when T is not a class
	Well, too bad ...

		if ((p = dynamic_cast<const Base *>(&(*it))) != nullptr)
			os << p->repr();
		else
*/
		ss << *it;
		++it;
		if (it != l_.cend()) {
			ss << ", ";
		}
	}
	ss << ')';
	return ss.str();
}

template <typename T>
T& List<T>::operator[](int idx) {
	if (idx < 0) {
		idx += len();
		if (idx < 0) {
			throw IndexError();
		}
	}
	if ((size_t)idx > len()) {
		throw IndexError();
	}

	// the list::iterator has no operator+, but we can do this:
	typename std::list<T>::iterator it = l_.begin();

	while(idx > 0) {
		++it;
		idx--;
	}
	return *it;
}

template <typename T>
const T& List<T>::operator[](int idx) const {
	if (idx < 0) {
		idx += len();
		if (idx < 0) {
			throw IndexError();
		}
	}
	if ((size_t)idx > len()) {
		throw IndexError();
	}

	// the list::iterator has no operator+, but we can do this:
	typename std::list<T>::const_iterator it = l_.cbegin();

	while(idx > 0) {
		++it;
		idx--;
	}
	return *it;
}

// default argument idx=-1
template <typename T>
T List<T>::pop(int idx) {
	if (idx < 0) {
		idx += len();
		if (idx < 0) {
			throw IndexError();
		}
	}
	if ((size_t)idx > len()) {
		throw IndexError();
	}

	// the list::iterator has no operator+, but we can do this:
	typename std::list<T>::iterator it = l_.begin();

	while(idx > 0) {
		++it;
		idx--;
	}
	T t = *it;
	l_.erase(it);
	return t;
}

template <typename T>
void List<T>::insert(int idx, const T& t) {
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

	// the list::iterator has no operator+, but we can do this:
	typename std::list<T>::iterator it = l_.begin();

	while(idx > 0) {
		++it;
		idx--;
	}
	l_.insert(it, t);
}

template <typename T>
bool List<T>::remove(const T& t) {
	typename std::list<T>::iterator it = std::find(l_.begin(), l_.end(), t);

	if (it == l_.end())	{ 	// not found
		return false;
	}
	l_.erase(it);
	return true;
}

template <typename T>
int List<T>::find(const T& t) const {
	// can't do simple arithmetic on list::iterator, so std::find() is useless
	// ah well
	int n = 0;
	for(typename std::list<T>::const_iterator it = l_.cbegin(); it != l_.cend(); ++it) {
		if (*it == t) {
			return n;
		}
		n++;
	}
	return n;
}

template <typename T>
int List<T>::rfind(const T& t) const {
	int n = 1;
	for(typename std::list<T>::const_reverse_iterator it = l_.rbegin(); it != l_.rend(); ++it) {
		if (*it == t) {
			return (len() - n);
		}
		n++;
	}
	return -1;
}

template <typename T>
void List<T>::rotate(int n) {
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

	// the list::iterator has no operator+, but we can do this:
	typename std::list<T>::iterator it = l_.begin();

	while(n > 0) {
		++it;
		n--;
	}
	std::rotate(l_.begin(), it, l_.end());
}

template <typename T>
void List<T>::unique(void) {
	typename std::list<T>::iterator it = std::unique(l_.begin(), l_.end());

	// can't do simple arithmetic on a list::iterator ...
	size_t n = 0;
	while(it != l_.begin()) {
		n++;
		--it;
	}
	l_.resize(n);
}

template <typename T>
List<T> List<T>::slice(int idx1, int idx2) const {
	if (idx1 < 0) {
		idx1 += len();
		if (idx1 < 0) {
			idx1 = 0;
		}
	}
	if ((size_t)idx1 >= len()) {
		return List<T>();
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
		return List<T>();
	}

	// copy the range between idx1 and idx2 to a new list
	// list::iterator does not have operator+, so we do it step by step
	List<T> a;
	typename std::list<T>::const_iterator it = l_.cbegin();
	int n = 0;

	while(n < idx1) {
		++it;
		n++;
	}
	while(n < idx2) {
		a.append(*it);
		++it;
		n++;
	}
	return a;
}

// used for printing
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const List<T>& a) {
	os << a.str();
	return os;
}

}	// namespace

#endif	// OOLIST_H_WJ112

// EOB
