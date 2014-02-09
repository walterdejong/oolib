/*
	ooSizeable.h	WJ112
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

#ifndef OOSIZEABLE_H_WJ112
#define OOSIZEABLE_H_WJ112

#include "oo/Base.h"

#include <cstdlib>

namespace oo {

// Sizeable is a virtual base class for objects that have length, capacity
// and they can grow

class Sizeable /* : eq_less_comparable<classname> */ {
public:
//	Sizeable() { }
	virtual ~Sizeable() { }

	virtual size_t len(void) const = 0;
	virtual size_t cap(void) const { return len(); }

	virtual bool empty(void) const { return this->len() == 0; }

	virtual void grow(size_t) { }

/*
	This here is nonsense because of the Sizeable type
	You should implement operators for Sizeables however

	virtual bool operator==(const Sizeable&) = 0;
	virtual bool operator<(const Sizeable&) = 0;
*/
};

// you can call len() and cap() on Sizeables
inline size_t len(const Sizeable& s) { return s.len(); }
inline size_t cap(const Sizeable& s) { return s.cap(); }

}	// namespace

#endif	// OOSIZEABLE_H_WJ112

// EOB
