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
#include "oo/Dict.h"
#include "oo/Error.h"

#include <algorithm>
#include <memory>

#include <pcre.h>

namespace oo {

/*
	class Regex represents the regex; does the compiling of the regex
	class Match represents the match; gets results from the 'ovector'
*/

typedef struct {
	int start, end;
} MatchPos;

class Match : public Base {
public:
	Match() : Base(), ovector_(std::shared_ptr<int>()), ovecsize_(0), matches_(0),
		nametable_(std::shared_ptr<char>()), namecount_(0), namesize_(0), subject_() { }

	Match(const Match& m) : Base(), ovector_(m.ovector_), ovecsize_(m.ovecsize_), matches_(m.matches_),
		nametable_(m.nametable_), namecount_(m.namecount_), namesize_(m.namesize_), subject_(m.subject_) { }

	Match(Match&& m) {
		swap(*this, m);
	}

//	virtual ~Match() { }

	Match& operator=(Match copy) {
		swap(*this, copy);
		return *this;
	}

	static void swap(Match& a, Match& b) {
		std::swap(a.ovector_, b.ovector_);
		std::swap(a.ovecsize_, b.ovecsize_);
		std::swap(a.matches_, b.matches_);
		std::swap(a.nametable_, b.nametable_);
		std::swap(a.namecount_, b.namecount_);
		std::swap(a.namesize_, b.namesize_);
		std::swap(a.subject_, b.subject_);
	}

	std::string repr(void) const { return "<Match>"; }

	bool operator!(void) const {
		return matches_ <= 0;
	}

	Array<String> groups(void) const;
	Dict<String> groupdict(void) const;

	int lastindex(void) const {
		if (matches_ > 0) {
			return matches_ - 1;
		}
		return -1;
	}

	// Note: be careful with unicode strings;
	// Note: positions are byte positions, NOT character positions
	int start(int group=0) const;
	int end(int group=0) const;
	MatchPos span(int group=0) const;

	String subject(void) const { return subject_; }

private:
	std::shared_ptr<int> ovector_;
	int ovecsize_, matches_;

	std::shared_ptr<char> nametable_;
	int namecount_, namesize_;

	String subject_;

	void prepare_(const String&, const pcre *, const pcre_extra *);
	void exec_(const pcre *, const pcre_extra *);

	friend class Regex;
};

class Regex : public Base {
public:
	// some aliases for often-used PCRE "compile-time" options
	static const int IGNORECASE = PCRE_CASELESS;
	static const int DOTALL = PCRE_DOTALL;
	static const int MULTILINE = PCRE_MULTILINE;
	static const int UNICODE = PCRE_UCP;
	static const int VERBOSE = PCRE_EXTENDED;

	Regex() : Base(), pattern_(), options_(0), re_(std::shared_ptr<pcre>()), study_(std::shared_ptr<pcre_extra>()) { }

	Regex(const String& s) : Base(), pattern_(s), options_(0), re_(std::shared_ptr<pcre>()), study_(std::shared_ptr<pcre_extra>()) { }

	Regex(const Regex& r) : Base(), pattern_(r.pattern_), options_(0), re_(r.re_), study_(r.study_) { }

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
	std::string str(void) const { return pattern_.str(); }

	bool operator!(void) const { return pattern_.empty(); }

	void compile(int options=0);	// 'studies' the regex

	Match match(const String& s, int options=0) {
		return search(s, options|PCRE_ANCHORED);
	}

	Match search(const String&, int options=0);
	Array<Array<String> > findall(const String&, int options=0);
	String sub(const String&, const String&, int count=0, int options=0);
	Array<String> split(const String&, int count=0, int options=0);
	String escape(void) const;

	static const char *errmsg[];
	static const char *strerror(int);

private:
	struct PcreDeleter {
		void operator()(pcre *p) {
			if (p != nullptr) {
				pcre_free(p);
			}
		}
	};

	struct PcreStudyDeleter {
		void operator()(pcre_extra *p) {
			if (p != nullptr) {
				pcre_free_study(p);
			}
		}
	};

	void precompile(int options=0);

	String pattern_;
	int options_;
	std::shared_ptr<pcre> re_;			// compiled pattern
	std::shared_ptr<pcre_extra> study_;	// extra study data
};

};

#endif	// OOREGEX_H_WJ114

// EOB
