/*
	ooRegex.h	WJ114
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

#ifndef OOREGEX_H_WJ114
#define OOREGEX_H_WJ114

#include "oo/Base.h"
#include "oo/String.h"
#include "oo/Error.h"

#include <algorithm>
#include <memory>

#include <pcre.h>

namespace oo {

class Regex : public Base {
public:
	Regex() : Base(), pattern_(), re_(std::shared_ptr<pcre>()), study_(std::shared_ptr<pcre_extra>()) { }

	Regex(const String& s) : Base(), pattern_(s), re_(std::shared_ptr<pcre>()), study_(std::shared_ptr<pcre_extra>()) { }

	Regex(const Regex& r) : Base(), pattern_(r.pattern_), re_(r.re_), study_(r.study_) { }

	Regex(Regex&& r) : Regex() {
		swap(*this, r);
	}

//	virtual ~Regex() { }

	Regex& operator=(Regex copy) {
		swap(*this, copy);
		return *this;
	}

	Regex& operator=(const String& s) {
		Regex r(s);
		swap(*this, r);
		return *this;
	}

	static void swap(Regex& a, Regex& b) {
		std::swap(a.pattern_, b.pattern_);
		std::swap(a.re_, b.re_);
		std::swap(a.study_, b.study_);
	}

	std::string repr(void) const;

	bool operator!(void) const { return pattern_.empty(); }

	void compile(void);		// 'studies' the regex

	Array<String> match(const String& s, int options=0) {
		return this->search(s, options | PCRE_ANCHORED);
	}

	Array<String> search(const String&, int options=0);

private:
	class PcreDeleter {
	public:
		void operator()(pcre *p) {
			if (p != nullptr) {
				pcre_free(p);
			}
		}
	};

	class PcreStudyDeleter {
	public:
		void operator()(pcre_extra *p) {
			if (p != nullptr) {
				pcre_free_study(p);
			}
		}
	};

	void precompile(void);

	String pattern_;
	std::shared_ptr<pcre> re_;			// compiled pattern
	std::shared_ptr<pcre_extra> study_;	// extra study data
};

};

#endif	// OOREGEX_H_WJ114

// EOB