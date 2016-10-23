/*
	ooBase.h	WJ112
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

#ifndef OOBASE_H_WJ112
#define OOBASE_H_WJ112

#include <string>

namespace oo {

class Base {
public:
	virtual std::string repr(void) const = 0;
	virtual std::string str(void) const { return repr(); }

	virtual bool operator!(void) const = 0;

//	friend std::ostream& operator<<(std::ostream&, const __classname__&);
};

/*
	You should always implement this
	Classes like Array and Dict depend on it

	Now wouldn't it be great if it were possible to write a specialized template
	for this, that would work for any class T derived from Base?

// used for printing
std::ostream& operator<<(std::ostream& os, const __classname__& t) {
	os << t.str();
	return os;
}
*/

/*
	an object is deleted by swapping it with an empty instance
*/
template <class T>
void del(T& a) {
	a = std::move(T());
}

/*
	after "deleting" an object with "del()", it should be "undefined"
	Mind that objects that are "empty()" will often respond as "undefined"
*/
inline bool undefined(const Base& a) {
	return (!a);
}

inline bool defined(const Base& a) {
	return !undefined(a);
}

}	// namespace

#endif	// OOBASE_H_WJ112

// EOB
