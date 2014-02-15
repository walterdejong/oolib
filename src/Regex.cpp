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

std::string Regex::repr(void) const {
	if (pattern_.empty()) {
		return "<Regex>";
	}
	std::stringstream ss;
	ss << "<Regex: " << pattern_.repr() << ">";
	return ss.str();
}

void Regex::precompile(void) {
	if (re_.get() != nullptr) {
		// already compiled, and cached
		return;
	}
	if (pattern_.empty()) {
		throw ValueError();
	}

	const char *errmsg = nullptr;
	int erroffset = 0;
	// oo::String are UTF-8 strings, so use PCRE_UTF8
	pcre *compiled = pcre_compile(pattern_.c_str(), PCRE_UTF8, &errmsg, &erroffset, nullptr);

	if (compiled == nullptr) {
		throw ValueError(errmsg);
	}

	re_ = std::shared_ptr<pcre>(compiled, PcreDeleter());
}

void Regex::compile(void) {
	this->precompile();

	if (study_.get() != nullptr) {
		// already studied, and cached
		return;
	}

	const char *errmsg = nullptr;

	pcre_extra *extra = pcre_study(re_.get(), 0, &errmsg);
	if (extra == nullptr) {
		throw ValueError(errmsg);
	}

	study_ = std::shared_ptr<pcre_extra>(extra, PcreStudyDeleter());
}

Array<String> Regex::search(const String& s, int options) {
	this->precompile();

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

	int matches = pcre_exec(re_.get(), study_.get(), subject, std::strlen(subject), 0, options, ovector, capcount * 3);
	if (matches == PCRE_ERROR_NOMATCH) {
		return arr;
	}
	if (matches <= 0) {
		throw ValueError("Regex::search(): error in pcre_exec()");
	}

	// put the results in array

	const char **results = nullptr;
	if (pcre_get_substring_list(subject, ovector, matches, &results) < 0) {
		throw ValueError("Regex::search() failed to extract results");
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
	this->precompile();

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
		int matches = pcre_exec(re_.get(), study_.get(), subject, subject_len, offset, options, ovector, capcount * 3);
		if (matches == PCRE_ERROR_NOMATCH) {
			return arr;
		}
		if (matches <= 0) {
			throw ValueError("Regex::findall(): error in pcre_exec()");
		}

		// put the results in array

		const char **results = nullptr;
		if (pcre_get_substring_list(subject, ovector, matches, &results) < 0) {
			throw ValueError("Regex::findall() failed to extract results");
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

}	// namespace oo

// EOB
