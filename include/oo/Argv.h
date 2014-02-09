/*
	ooArgv.h	WJ112
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

#ifndef OOARGV_H_WJ112
#define OOARGV_H_WJ112

#include "oo/Base.h"
#include "oo/Sizeable.h"
#include "oo/Array.h"

#include <cstdlib>
#include <string>

namespace oo {

// Argv is a singleton of ArgvObject
// You should call Argv.init(argc, argv) asap in main()

// note that Argv is not an Array<String> ... it's a wrapper that keeps char *argv[]
// it works more like a 'module'

// Argv can do command-line option parsing with Argv.getopt()
// It uses an array of long options, declared using Options
// and it can print usage info for those declared options
// see testArgv.cpp for an example

typedef enum {
	GetOptNextWord = 0,
	GetOptNextChar,
	GetOptRestArgs,
	GetOptError
} GetOptStateNum;

typedef struct {
	int code;
	const char *longopt, *arg, *desc;
} Options;

typedef struct {
	GetOptStateNum state;
	int idx, subidx, optopt;
	bool opterr;
	const char *optarg;
	Array<const char*> args;
} GetOptState;

class ArgvObject : public Base, public Sizeable {
public:
	static ArgvObject& instance(void);

	~ArgvObject() {
		if (s_prognam != nullptr)
			delete [] s_prognam;
	}

	// these assign and compare to None
	using Base::operator=;
	using Base::operator!;
	using Base::operator==;
	using Base::operator!=;

	std::string repr(void) const { return "<Argv>"; }
	std::string str(void) const;

	bool isNone(void) const { return (s_argc == 0); }
	void setNone(void) { clear(); }

	void clear(void) {
		s_argc = 0;
		s_argv = nullptr;

		if (s_prognam != nullptr) {
			delete [] s_prognam;
			s_prognam = nullptr;
		}

		optreset();
	}

	size_t len(void) const { return (size_t)s_argc; }

	void init(int, char **);

	const char *operator[](int) const;

	const char *prognam(void) const { return s_prognam; }
	const char *command(void) const { if (s_argv != nullptr) return s_argv[0]; return nullptr; }

	// this getopt() allows option arguments to occur after non-option arguments
	// (unlike UNIX getopt())
	int getopt(const Options *);
	void usage(const Options *);

	int optind(void) { return s_opt.idx; }
	int optidx(void) { return s_opt.idx; }
	int optopt(void) { return s_opt.optopt; }
	const char *optarg(void) { return s_opt.optarg; }
	const Array<const char *>& args(void) { return s_opt.args; }
	void opterr(bool yesno) { s_opt.opterr = yesno; }
	void optreset(void);

private:
	ArgvObject() : Base(), Sizeable(), s_argc(0), s_argv(nullptr), s_prognam(nullptr) { optreset(); s_opt.opterr = true; }

	int s_argc;
	char **s_argv;		// not const, but I promise it is treated as const
	char *s_prognam;

	GetOptState s_opt;
};

extern ArgvObject Argv;

}	// namespace

#endif	// OOARGV_H_WJ112

// EOB
