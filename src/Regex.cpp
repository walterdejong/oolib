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

Array<String> Regex::search(const String& s, int options) {
	precompile(options);

	Array<String> arr;

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

	// execute the regex match

	int matches = pcre_exec(re_.get(), study_.get(), subject, std::strlen(subject), 0, 0, ovector, capcount * 3);
	if (matches == PCRE_ERROR_NOMATCH) {
		return arr;
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

	int n = 0;
	for(const char *p = results[0]; p != nullptr; n++, p = results[n]) {
		if (p == nullptr) {
			// PCRE results should never be nullptr
			throw ReferenceError();
		}
		arr.append(String(p));
	}
	pcre_free_substring_list(results);

	return arr;
}

Array<String> Regex::findall(const String& s, int options) {
	/*
		this func is the very same as search(), except that it loops
		to find all matches in the subject string
	*/
	precompile(options);

	Array<String> arr;

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
		int matches = pcre_exec(re_.get(), study_.get(), subject, subject_len, offset, 0, ovector, capcount * 3);
		if (matches == PCRE_ERROR_NOMATCH) {
			return arr;
		}
		if (matches <= 0) {
			throw RuntimeError(Regex::strerror(matches));
		}

		// put the results in array

		const char **results = nullptr;
		if (pcre_get_substring_list(subject, ovector, matches, &results) < 0) {
			throw RuntimeError("error extracting regex results");
		}

		arr.grow(arr.len() + matches);

		int n = (offset == 0) ? 0 : 1;
		for(const char *p = results[n]; p != nullptr; n++, p = results[n]) {
			if (p == nullptr) {
				// PCRE results should never be nullptr
				throw ReferenceError();
			}
			arr.append(String(p));
		}
		pcre_free_substring_list(results);

		offset = ovector[1];
	}
	return arr;
}

Dict<String> Regex::searchbyname(const String& s, int options) {
	precompile(options);

	Dict<String> d;

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

	// execute the regex match

	int matches = pcre_exec(re_.get(), study_.get(), subject, std::strlen(subject), 0, 0, ovector, capcount * 3);
	if (matches == PCRE_ERROR_NOMATCH) {
		return d;
	}
	if (matches <= 0) {
		throw RuntimeError(Regex::strerror(matches));
	}

	// put biggest match in d["$_"]
	const char *result = nullptr;
	if (pcre_get_substring(subject, ovector, matches, 0, &result) < 0) {
		throw RuntimeError("error extracting regex results");
	}
	d["$_"] = result;
	pcre_free_substring(result);
	result = nullptr;

	// let's go put all results in dict, by name

	int namecount = 0;
	pcre_fullinfo(re_.get(), study_.get(), PCRE_INFO_NAMECOUNT, &namecount);
	if (namecount <= 0) {
		// there are no names; return d as it is now
		return d;
	}

	int namesize = 0;
	pcre_fullinfo(re_.get(), study_.get(), PCRE_INFO_NAMEENTRYSIZE, &namesize);
	if (namesize <= 0) {
		throw RuntimeError("illegal value for regex name size");
	}

	char *nametable = nullptr;
	pcre_fullinfo(re_.get(), study_.get(), PCRE_INFO_NAMETABLE, &nametable);
	if (nametable == nullptr) {
		throw RuntimeError("error getting regex name table");
	}

	// put the results in dict
	// walk the name table, get the result number, put result in dict

	for(int i = 0; i < namecount; i++) {
		char *name = nametable + 2;
		int num = pcre_get_stringnumber(re_.get(), name);
		if (num < 0) {
			throw RuntimeError("error getting regex result number by name");
		}

		if (pcre_get_substring(subject, ovector, matches, num, &result) < 0) {
			throw RuntimeError("error extracting regex results");
		}
		d[name] = result;
		pcre_free_substring(result);
		result = nullptr;

		nametable += namesize;
	}
	return d;
}

}	// namespace oo

// EOB
