//
//	Regex.cpp	WJ114
//	
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

#include "oo/Regex.h"

#include <sstream>

namespace oo {

const char *Regex::errmsg[] = {
	"no error",
	"no match",
	"null pointer reference",
	"bad or unknown option bit set",
	"bad magic number in compiled regex",
	"unknown opcode in compiled regex",
	"unknown node in compiled regex",
	"out of memory",
	"no such substring",
	"backtracking limit exceeded",
	"callout error",
	"invalid UTF-8 string encountered",
	"invalid UTF-16 string encountered",
	"invalid UTF-32 string encountered",
	"invalid UTF-8 start offset encountered",
	"invalid UTF-16 start offset encountered",
	"partial match",
	"bad partial match",
	"unexpected internal error",
	"illegal value for ovector size",
	"DFA UITEM",
	"DFA UCOND",
	"DFA UMLIMIT",
	"DFA WSSIZE",
	"DFA RECURSE",
	"recursion limit exceeded",
	"NULLWSLIMIT",
	"invalid combination of PCRE_NEWLINE_xxx options",
	"invalid value for offset parameter",
	"truncated UTF-8 string encountered",
	"truncated UTF-16 string encountered",
	"recursion loop detected",
	"JIT stacklimit exceeded",
	"bad mode: mixup of 8/16/32 bit code",
	"bad endianness",
	"DFA BADRESTART",
	"JIT: invalid option",
	"invalid value for length parameter",
	"requested field is not set",
};

const char *Regex::strerror(int errnum) {
	if (errnum < 0) {
		errnum = -errnum;
	}

	static char errbuf[128];
	std::strcpy(errbuf, "regex error: ");

	int max_err = sizeof(Regex::errmsg)/sizeof(char*);
	std::strcat(errbuf, (errnum >= max_err) ? "unknown error" : Regex::errmsg[errnum]);
	return errbuf;
}

std::string Regex::repr(void) const {
	if (pattern_.empty()) {
		return "<Regex>";
	}
	std::stringstream ss;
	ss << "<Regex: " << pattern_.repr() << ">";
	return ss.str();
}

void Regex::precompile(int options) {
	if (re_.get() != nullptr && options == options_) {
		// already compiled, and cached
		return;
	}
	if (pattern_.empty()) {
		throw ValueError();
	}

	// oo::String are UTF-8 strings, so use PCRE_UTF8
	options |= PCRE_UTF8;

	const char *errmsg = nullptr;
	int erroffset = 0;

	pcre *compiled = pcre_compile(pattern_.c_str(), options, &errmsg, &erroffset, nullptr);
	if (compiled == nullptr) {
		if (errmsg == nullptr) {
			throw ReferenceError();
		}

		char errbuf[std::strlen(errmsg) + 16];
		std::strcpy(errbuf, "regex error: ");
		std::strcat(errbuf, errmsg);
		throw RuntimeError(errbuf);
	}

	re_ = std::shared_ptr<pcre>(compiled, PcreDeleter());
	options_ = options;
}

void Regex::compile(int options) {
	precompile(options);

	if (study_.get() != nullptr) {
		// already studied, and cached
		return;
	}

	const char *errmsg = nullptr;

	pcre_extra *extra = pcre_study(re_.get(), 0, &errmsg);
	if (extra == nullptr) {
		if (errmsg == nullptr) {
			throw ReferenceError();
		}

		char errbuf[std::strlen(errmsg) + 16];
		std::strcpy(errbuf, "regex error: ");
		std::strcat(errbuf, errmsg);
		throw RuntimeError(errbuf);
	}

	study_ = std::shared_ptr<pcre_extra>(extra, PcreStudyDeleter());
}

Match Regex::search(const String& s, int options) {
	precompile(options);

	Match m;

	m.prepare_(s, re_.get(), study_.get());
	m.exec_(re_.get(), study_.get());

	return m;
}

Array<Array<String> > Regex::findall(const String& s, int options) {
	precompile(options);

	Array<Array<String> > out;

	int capcount = 0;
	if (pcre_fullinfo(re_.get(), study_.get(), PCRE_INFO_CAPTURECOUNT, &capcount) < 0) {
		throw ValueError("invalid regex");
	}
	capcount++;
	int ovector[capcount * 3];

	const char *subject = s.c_str();
	if (subject == nullptr) {
		throw ReferenceError();
	}

	int subject_len = std::strlen(subject);
	int offset = 0;

	// execute the regex match

	while(offset < subject_len) {
		Array<String> arr;

		int matches = pcre_exec(re_.get(), study_.get(), subject, subject_len, offset, 0, ovector, capcount * 3);
		if (matches == PCRE_ERROR_NOMATCH) {
			return out;
		}
		if (matches <= 0) {
			throw RuntimeError(Regex::strerror(matches));
		}

		// put the results in array

		const char **results = nullptr;
		if (pcre_get_substring_list(subject, ovector, matches, &results) < 0) {
			throw RuntimeError("error extracting regex results");
		}

		arr.grow(matches);

		// first match is entire string; skip it
		int n = 1;
		for(const char *p = results[n]; p != nullptr; n++, p = results[n]) {
			if (p == nullptr) {
				// PCRE results should never be nullptr
				throw ReferenceError();
			}
			arr.append(String(p));
		}
		pcre_free_substring_list(results);

		out.append(arr);

		offset = ovector[1];
	}
	return out;
}

String Regex::sub(const String& repl, const String& s, int count, int options) {
	precompile(options);

	String out;
	String search_str = s;
	Match m;
	MatchPos pos;

	int n = count;
	if (n <= 0) {
		n = 1;
	}
	for(int i = 0; i < n; i++) {
		m = search(search_str, options);
		if (!m) {
			break;
		}

		pos = m.span();
		out += search_str.slice(0, pos.start) + repl;
		search_str = search_str.slice(pos.end, search_str.len());

		if (count <= 0) {
			// never exit the loop
			n++;
		}
	}
	out += search_str;
	return out;
}

Array<String> Regex::split(const String& s, int count, int options) {
	precompile(options);

	Array<String> out;
	String search_str = s;
	Match m;
	MatchPos pos;

	int n = count;
	if (n <= 0) {
		n = 1;
	}
	for(int i = 0; i < n; i++) {
		m = search(search_str, options);
		if (!m) {
			break;
		}

		pos = m.span();
		if (pos.start == 0 && pos.end >= search_str.len()) {
			break;
		}
		out.append(search_str.slice(0, pos.start));
		search_str = search_str.slice(pos.end, search_str.len());

		// if regex has subgroups, include them in the resulting array
		if (m.lastindex() > 0) {
			out.extend(m.groups());
		}

		if (count <= 0) {
			// never exit the loop
			n++;
		}
	}
	out.append(search_str);
	return out;
}

void Match::prepare_(const String& subj, const pcre *re, const pcre_extra *sd) {
	// prepare for execution; set ovector, copy nametable

	if (pcre_fullinfo(re, sd, PCRE_INFO_CAPTURECOUNT, &ovecsize_) < 0) {
		throw ValueError("invalid regex");
	}

	ovecsize_++;
	ovector_ = std::shared_ptr<int>(new int[ovecsize_ * 3], std::default_delete<int[]>());

	namecount_ = 0;
	pcre_fullinfo(re, sd, PCRE_INFO_NAMECOUNT, &namecount_);
	if (namecount_ > 0) {
		namesize_ = 0;
		pcre_fullinfo(re, sd, PCRE_INFO_NAMEENTRYSIZE, &namesize_);
		if (namesize_ <= 0) {
			throw RuntimeError("illegal value for regex name size");
		}

		const char *nametable = nullptr;
		pcre_fullinfo(re, sd, PCRE_INFO_NAMETABLE, &nametable);
		if (nametable == nullptr) {
			throw RuntimeError("error getting regex name table");
		}

		// the nametable is bound to the regex
		// in order to preserve memory integrity, we must copy the nametable
		// otherwise Regex may go out of scope, and the Match object would have a problem

		nametable_ = std::shared_ptr<char>(new char[namecount_ * namesize_], std::default_delete<char[]>());
		std::memcpy(nametable_.get(), nametable, namecount_ * namesize_);
	}

	// copy the subject string
	subject_ = subj;
}

void Match::exec_(const pcre *re, const pcre_extra *sd) {
	// execute compiled regex, set number of matches in Match

	const char *subj = subject_.c_str();
	if (subj == nullptr) {
		throw ReferenceError();
	}

	matches_ = pcre_exec(re, sd, subj, std::strlen(subj), 0, 0, ovector_.get(), ovecsize_ * 3);
	if (matches_ == PCRE_ERROR_NOMATCH) {
		return;
	}
	if (matches_ < 0) {
		throw RuntimeError(Regex::strerror(matches_));
	}
}

Array<String> Match::groups(void) const {
	// put the result subgroups in array

	Array<String> arr;

	if (matches_ <= 0) {
		return arr;
	}

	const char *subj = subject_.c_str();
	if (subj == nullptr) {
		throw ReferenceError();
	}

	int *ovector = ovector_.get();
	if (ovector == nullptr) {
		throw ReferenceError();
	}

	const char **results = nullptr;
	if (pcre_get_substring_list(subj, ovector, matches_, &results) < 0) {
		throw RuntimeError("error extracting regex results");
	}

	arr.grow(matches_);

	// start at 1; result 0 is the entire string
	int n = 1;
	for(const char *p = results[n]; p != nullptr; n++, p = results[n]) {
		if (p == nullptr) {
			// PCRE results should never be nullptr
			throw ReferenceError();
		}
		arr.append(String(p));
	}
	pcre_free_substring_list(results);
	return arr;
}

Dict<String> Match::groupdict(void) const {
	// put results in dict

	Dict<String> d;

	if (matches_ <= 0) {
		return d;
	}

	const char *subj = subject_.c_str();
	if (subj == nullptr) {
		throw ReferenceError();
	}

	int *ovector = ovector_.get();
	if (ovector == nullptr) {
		throw ReferenceError();
	}

	// let's go put all results in dict, by name

	if (namesize_ <= 0) {
		throw RuntimeError("illegal value for regex name size");
	}

	char *nametable = nametable_.get();
	if (nametable == nullptr) {
		throw ReferenceError();
	}

	if (namecount_ <= 0) {
		// there are no names
		return d;
	}

	// walk the name table, get the result number, put result in dict

	const char *result = nullptr;

	for(int i = 0; i < namecount_; i++) {
		// first two bytes in the nametable represent the number
		// after that is the name
		char *name = nametable + 2;

		int num = (nametable[0] << 8) | nametable[1];
		if (num < 0 || num > ovecsize_) {
			throw RuntimeError("illegal value for regex group number");
		}

		if (pcre_get_substring(subj, ovector, matches_, num, &result) < 0) {
			throw RuntimeError("error extracting regex results");
		}
		d[name] = result;
		pcre_free_substring(result);
		result = nullptr;

		nametable += namesize_;
	}
	return d;
}

int Match::start(int group) const {
	if (group < 0 || group > matches_ - 1) {
		throw ValueError();
	}

	int *ovector = ovector_.get();
	if (ovector == nullptr) {
		throw ReferenceError();
	}

	return ovector[group * 2];
}

int Match::end(int group) const {
	if (group < 0 || group > matches_ - 1) {
		throw ValueError();
	}

	int *ovector = ovector_.get();
	if (ovector == nullptr) {
		throw ReferenceError();
	}

	return ovector[group * 2 + 1];
}

MatchPos Match::span(int group) const {
	if (group < 0 || group > matches_ - 1) {
		throw ValueError();
	}

	int *ovector = ovector_.get();
	if (ovector == nullptr) {
		throw ReferenceError();
	}

	MatchPos pos;
	pos.start = ovector[group * 2];
	pos.end = ovector[group * 2 + 1];
	return pos;
}

}	// namespace oo

// EOB
