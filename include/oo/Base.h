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
#include <ostream>

namespace oo {

class NoneObject;

class Base {
public:
//	Base() { }
	virtual ~Base() { }

	// these are reasonable default methods, but you should really override them
	// to have them make sense

	virtual std::string repr(void) const = 0;
	virtual std::string str(void) const {
		if (isNone()) {
			return "(None)";
		}
		return repr();
	}

	/*
		remember to use these operators in derived classes ... like:

		 using Base::operator=;
		 using Base::operator!;
		 using Base::operator==;
		 using Base::operator!=;

		override isNone() and clear() to adjust functionality
	*/
	virtual bool isNone(void) const = 0;
	bool operator!(void) const { return isNone(); }
	bool operator==(const NoneObject&) const { return isNone(); }
	bool operator!=(const NoneObject&) const { return !isNone(); }

	virtual void setNone(void) = 0;
	Base& operator=(const NoneObject&) { setNone(); return *this; }
};

/*
	You'd think that this would automatically work for all derived classes,
	but it doesn't

inline std::ostream& operator<<(std::ostream& os, const Base& t) {
	os << t.str();
	return os;
}
*/

}	// namespace

#endif	// OOBASE_H_WJ112

// EOB
