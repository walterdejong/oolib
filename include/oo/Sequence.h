/*
	ooSequence.h	WJ112
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

#ifndef OOSEQUENCE_H_WJ112
#define OOSEQUENCE_H_WJ112

#include "oo/Sizeable.h"

#include <iterator>
#include <algorithm>
#include <numeric>

// this works on Sizables, but it really only makes sense for Sequences
#define foreach(x,y)	for(size_t x = 0; (x) < len(y); x++)

namespace oo {

// virtual base class for any 'sequence' types
// Sequence types are arrays and lists
// Sizeable means they have length and capacity

template <typename T>
class Sequence : public Sizeable {
public:
//	Sequence() : Sizeable() { }
	virtual ~Sequence() { }

	virtual T& operator[](int) = 0;
	virtual const T& operator[](int) const = 0;

//	iterators: auto typing is nice but it just doesn't work
//	virtual auto begin(void) -> decltype() const = 0;
//	virtual auto end(void) -> decltype() const = 0;

	// must define push_back() for std::back_inserter() to work
	virtual void push_back(const T&) = 0;
};

// map/filter/reduce functionality
// Note how the class T does not need to be a Sequence<T> ...
// Class T must however implement iterators and method push_back()

// apply function to every member of T a
// T is a collection like Array<T> or List<T>
template <class T, typename F>
T apply(F func, const T& a) {
	T ret;
	std::transform(a.cbegin(), a.cend(), std::back_inserter(ret), func);
	return ret;
}

// apply filter function to every member of U t (member type is T)
// return a new filtered collection like Array<T> or List<T>
template <class T, typename F>
T filter(F func, const T& a) {
	T ret;
	std::copy_if(a.cbegin(), a.cend(), std::back_inserter(ret), func);
	return ret;
}

// reduce array to a single value by applying the function to every element
template <typename F, class T>
auto reduce(F func, const T& a) -> decltype(
	func(std::declval<typename T::value_type>(), std::declval<typename T::value_type>())) {
	// declare local var init = 0;
	decltype(func(std::declval<typename T::value_type>(),
		std::declval<typename T::value_type>())) init{};
	return std::accumulate(a.cbegin(), a.cend(), init, func);
}

}	// namespace

#endif	// OOSEQUENCE_H_WJ112

// EOB
