/*
	ooSet.h	WJ114
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

#ifndef OOSET_H_WJ114
#define OOSET_H_WJ114

#include "oo/Base.h"
#include "oo/None.h"
#include "oo/Sizeable.h"
#include "oo/Error.h"
#include "oo/String.h"
#include "oo/Array.h"

#include <set>
#include <ostream>
#include <sstream>

namespace oo {

template <typename T>
class Set : public Base, public Sizeable {
public:
	typedef T value_type;

	Set() : Base(), Sizeable(), s_(std::set<T>()) { }
	Set(const NoneObject& none) : Base(), Sizeable(), s_(std::set<T>()) { }

	Set(const Set& s) : Base(), Sizeable(), s_(std::set<T>(s.s_)) { }

	Set(std::initializer_list<T> a) : Set() {
		for (auto it = a.begin(); it != a.end(); ++it) {
			s_.insert(*it);
		}
	}

	Set(Set<T>&& s) : Set() {
		swap(*this, s);
	}

//	~Set() { }

	Set<T>& operator=(Set<T> s) {
		swap(*this, s);
		return *this;
	}

	static void swap(Set<T>& a, Set<T>& b) {
		std::swap(a.s_, b.s_);
	}

	// these assign and compare to None
	using Base::operator=;
	using Base::operator!;
	using Base::operator==;
	using Base::operator!=;

	std::string repr(void) const;

	bool isNone(void) const { return (s_.size() == 0); }
	void setNone(void) { clear(); }

	void clear(void) { s_.clear(); }

	size_t len(void) const { return s_.size(); }
	size_t cap(void) const { return len(); }

	bool operator==(const Set<T>& s) const;
	bool operator<=(const Set<T>& s) const { return issubset(s); }
	// 'proper' subset
	bool operator<(const Set<T>& s) const { return (issubset(s) && (!(operator==(s)))); }
	bool operator>=(const Set<T>& s) const { return issuperset(s); }
	// 'proper' superset
	bool operator>(const Set<T>& s) const { return (issuperset(s) && (!(operator==(s)))); }

	bool issubset(const Set<T>&) const;
	bool issuperset(const Set<T>&) const;
	bool isdisjoint(const Set<T>&) const;

	Set<T>& operator|=(const Set<T>& s) {
		update_union(s);
		return *this;
	}

	Set<T> operator|(const Set<T> s) {
		return Set<T>(*this) |= s;
	}

	void update_union(const Set<T>&);

	Set<T>& operator&=(const Set<T>& s) {
		update_intersection(s);
		return *this;
	}

	Set<T> operator&(const Set<T> s) {
		return Set<T>(*this) &= s;
	}

	void update_intersection(const Set<T>&);

	Set<T>& operator-=(const Set<T>& s) {
		update_difference(s);
		return *this;
	}

	Set<T> operator-(const Set<T> s) {
		return Set<T>(*this) -= s;
	}

	void update_difference(const Set<T>&);

	Set<T>& operator^=(const Set<T>& s) {
		update_symmetric_difference(s);
		return *this;
	}

	Set<T> operator^(const Set<T> s) {
		return Set<T>(*this) ^= s;
	}

	void update_symmetric_difference(const Set<T>&);

	void add(const T& a) { s_.insert(a); }
	bool ismember(const T& a) const { return s_.find(a) != s_.end(); }
	bool has_item(const T& a) const { return ismember(a); }
	bool del(const T& a) { return s_.erase(a) == 1; }
	bool remove(const T& a) { return del(a); }
	T pop(void);	// throws IndexError

	// convert to array
	Array<T> items(void) const;

	// this enables range-based for-loops
	typename std::set<T>::iterator begin(void) { return s_.begin(); }
	typename std::set<T>::iterator end(void) { return s_.end(); }
	typename std::set<T>::const_iterator cbegin(void) const { return s_.cbegin(); }
	typename std::set<T>::const_iterator cend(void) const { return s_.cend(); }

private:
	std::set<T> s_;

	template <typename U>
	friend std::ostream& operator<<(std::ostream&, const Set<U>&);
};


// templated Set methods

template <typename T>
std::string Set<T>::repr(void) const {
	std::stringstream ss;
	ss << "{";

	for(typename std::set<T>::const_iterator it = s_.cbegin(); it != s_.cend(); ) {
		ss << *it;
		++it;
		if (it != s_.cend()) {
			ss << ", ";
		}
	}
	ss << "}";
	return ss.str();
}

// compare two sets
template <typename T>
bool Set<T>::operator==(const Set<T>& d) const {
	if (len() != d.len()) {
		return false;
	}

	typename std::set<T>::const_iterator it2;

	for(typename std::set<T>::const_iterator it = s_.cbegin(); it != s_.cend(); ++it) {
		it2 = d.s_.find(*it);
		if (it2 == d.s_.end()) {
			return false;
		}
	}
	return true;
}

template <typename T>
bool Set<T>::issubset(const Set<T>& other) const {
	// returns True when every element is in other
	if (len() > other.len()) {
		return false;
	}

	for(typename std::set<T>::const_iterator it = s_.cbegin(); it != s_.cend(); ++it) {
		if (!other.ismember(*it)) {
			return false;
		}
	}
	return true;
}

template <typename T>
bool Set<T>::issuperset(const Set<T>& other) const {
	// returns True when every element of other is in this set
	if (other.len() > len()) {
		return false;
	}

	for(typename std::set<T>::const_iterator it = other.cbegin(); it != other.cend(); ++it) {
		if (!ismember(*it)) {
			return false;
		}
	}
	return true;
}

template <typename T>
bool Set<T>::isdisjoint(const Set<T>& other) const {
	// return True if this set has no elements in common with other
	for(typename std::set<T>::const_iterator it = s_.cbegin(); it != s_.cend(); ++it) {
		if (other.ismember(*it)) {
			return false;
		}
	}
	return true;
}

template <typename T>
void Set<T>::update_union(const Set<T>& other) {
	// add other to this set
	for(typename std::set<T>::const_iterator it = other.cbegin(); it != other.cend(); ++it) {
		s_.insert(*it);
	}
}

template <typename T>
void Set<T>::update_intersection(const Set<T>& other) {
	// only keep items that are both in this set and in other
	for(typename std::set<T>::iterator it = s_.begin(); it != s_.end(); ) {
		if (!other.ismember(*it)) {
			it = s_.erase(it);
		} else {
			++it;
		}
	}
}

template <typename T>
void Set<T>::update_difference(const Set<T>& other) {
	// remove items that are in other
	for(typename std::set<T>::const_iterator it = other.cbegin(); it != other.cend(); ++it) {
		s_.erase(*it);
	}
}

template <typename T>
void Set<T>::update_symmetric_difference(const Set<T>& other) {
	// keep items that are in either set, but not both
	Set<T> tmp = (*this | other) - (*this & other);
	s_ = tmp.s_;
}

template <typename T>
Array<T> Set<T>::items(void) const {
	Array<T> a(len());

	for(typename std::set<T>::const_iterator it = s_.cbegin(); it != s_.cend(); ++it) {
		a.append(*it);
	}
	return a;
}

template <typename T>
T Set<T>::pop(void) {
	if (!len()) {
		throw IndexError();
	}

	typename std::set<T>::iterator it = s_.begin();
	T tmp = *it;
	s_.erase(it);
	return tmp;
}

// used for printing
template <typename T>
std::ostream& operator<<(std::ostream& os, const Set<T>& d) {
	os << d.str();
	return os;
}

}	// namespace

#endif	// OOSET_H_WJ114

// EOB
