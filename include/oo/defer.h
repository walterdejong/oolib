/*
	oodefer.h	WJ114

	This code is by Uriel Corfa, published on Korfuri's blog
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

#ifndef OODEFER_H_WJ114
#define OODEFER_H_WJ114

#include <functional>

namespace oo {

class DeferredAction {
public:
	~DeferredAction() {
		// destructor does the deferred function call
		if (f_ != nullptr) {
			f_();
		}
	}

	DeferredAction(DeferredAction&& a) : f_(std::move(a.f_)) { }

	// do not define these
	DeferredAction(const DeferredAction&) = delete;
	DeferredAction& operator=(const DeferredAction&) = delete;
	DeferredAction& operator=(DeferredAction&&) = delete;

private:
	// private constructor, binds function+args to f_
	template <typename... Tpack>
	DeferredAction(Tpack&&... p) :
		f_(std::bind(std::forward<Tpack>(p)...)) { }

	std::function<void()> f_;

	template <typename... Tpack>
	friend DeferredAction defer(Tpack&&... p);
};

// defer() creates a DeferredAction "finalizer-functor"
template <typename... Tpack>
DeferredAction defer(Tpack&&... p) {
	return DeferredAction(std::forward<Tpack>(p)...);
}

}	// namespace oo

#endif	// OODEFER_H_WJ114

// EOB
