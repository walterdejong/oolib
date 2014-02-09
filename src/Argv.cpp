/*
	ooArgv.cpp	WJ112
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

#include "oo/Argv.h"
#include "oo/String.h"
#include "oo/Error.h"
#include "oo/print.h"

#include <sstream>
#include <cassert>
#include <cstring>

#include <libgen.h>

namespace oo {

ArgvObject Argv = ArgvObject::instance();

static ArgvObject *obj = nullptr;

ArgvObject& ArgvObject::instance(void) {
	if (obj == nullptr) {
		obj = new ArgvObject();
	}
	return *obj;
}

std::string ArgvObject::str(void) const {
	if (isNone()) {
		return "(None)";
	}

	std::stringstream ss;

	ss << s_argv[0];
	for(unsigned int n = 1; n < len(); n++) {
		ss << " " << s_argv[n];
	}
	return ss.str();
}

void ArgvObject::init(int argc, char **argv) {
	assert(argc > 0);
	assert(argv != nullptr);

	s_argc = argc;
	s_argv = argv;

	// set program
	// make a copy, because basename() may edit the string
	size_t len = std::strlen(argv[0]) + 1;
	char p[len];
	std::memcpy(p, argv[0], len);

	char *b = ::basename(p);
	if (b == nullptr) {
		return;
	}

	// copy basename to s_prognam
	len = std::strlen(b) + 1;
	s_prognam = new char[len];
	std::memcpy(s_prognam, b, len);

	optreset();
	s_opt.args.grow(s_argc);	// reserve plenty of space for arguments
}

const char *ArgvObject::operator[](int idx) const {
	if (idx < 0) {
		idx += len();

		if (idx < 0) {
			throw IndexError();
		}
	}
	if ((size_t)idx >= len()) {
		throw IndexError();
	}
	return s_argv[idx];
}

// this is a stateful getopt() parser
// state is kept in s_optidx
// call Argv.optreset() to reset state

int ArgvObject::getopt(const Options *options) {
	if (options == nullptr) {
		s_opt.state = GetOptRestArgs;
	}

	assert(s_argc > 0);
	assert(s_argv != nullptr);

	const char *arg, *option;
	char ch;
	int n, option_len;
	bool found, req_arg;

	while(s_opt.idx < s_argc) {
		switch(s_opt.state) {
			case GetOptNextWord:
				arg = s_argv[s_opt.idx];

				if (arg[0] != '-') {
					// it's an argument
					s_opt.args.append(arg);
					break;
				}

				if (!arg[1]) {
					// it's a "-" argument (stdin)
					s_opt.args.append(arg);
					break;
				}

				if (arg[1] != '-') {
					// it's a short option
					s_opt.state = GetOptNextChar;
					s_opt.idx--;		// rig this so it restarts this index
					s_opt.subidx = 1;
					break;
				}

				if (!arg[2]) {
					// it's a "--" argument; no more options
					s_opt.state = GetOptRestArgs;
					break;
				}

				option = arg + 2;

				if ((s_opt.optarg = std::strchr(option, '=')) != nullptr) {
					option_len = s_opt.optarg - option;	// eek ... pointer arithmetic
					s_opt.optarg++;
				} else {
					option_len = std::strlen(option);
				}
				found = false;
				req_arg = false;

				for(n = 0; !(options[n].code == 0 && options[n].longopt == nullptr
					&& options[n].arg == nullptr && options[n].desc == nullptr); n++) {
					if (options[n].longopt == nullptr) {
						continue;
					}
					// this odd compare is necessary because of possible '=' token plus argument
					if (!std::strncmp(option, options[n].longopt, option_len) && !options[n].longopt[option_len]) {
						found = true;
						s_opt.optopt = options[n].code;
						req_arg = (options[n].arg != nullptr);
						break;
					}
				}

				if (!found) {
					// unknown option at s_opt.idx
					// oh no ... option may still contain '=' token plus argument
					// ah well, it's OK
					if (s_opt.opterr) {
						printerr("unknown option '--%s'", option);
					}
					s_opt.state = GetOptError;
					s_opt.optopt = 0;
					return '?';
				}

				if (req_arg) {
					if (s_opt.optarg != nullptr) {
						s_opt.idx++;
						return s_opt.optopt;
					}
					if (s_opt.idx+1 >= s_argc) {
						if (s_opt.opterr) {
							printerr("option '--%s' requires an argument", options[n].longopt);
						}
						s_opt.state = GetOptError;
						s_opt.optopt = 0;
						return ':';
					}

					// treat next command-line arg as option argument
					s_opt.idx++;
					s_opt.optarg = s_argv[s_opt.idx++];
					return s_opt.optopt;
				}

				if (s_opt.optarg != nullptr) {
					if (s_opt.opterr) {
						printerr("option '--%s' takes no argument", options[n].longopt);
					}
					s_opt.state = GetOptError;
					s_opt.optopt = 0;
					return ':';
				}
				s_opt.idx++;
				return s_opt.optopt;

			case GetOptNextChar:
				// parse short option

				ch = s_argv[s_opt.idx][s_opt.subidx++];
				s_opt.optopt = ch;

				found = false;
				req_arg = false;

				for(n = 0; !(options[n].code == 0 && options[n].longopt == nullptr
					&& options[n].arg == nullptr && options[n].desc == nullptr); n++) {
					if (options[n].code == ch) {
						found = true;
						s_opt.optopt = options[n].code;
						req_arg = (options[n].arg != nullptr);
						break;
					}
				}
				if (!found) {
					// unknown option at s_opt.optopt
					if (s_opt.opterr) {
						printerr("unknown option '-%c'", ch);
					}
					s_opt.state = GetOptError;
					return '?';
				}

				if (req_arg) {
					if (s_argv[s_opt.idx][s_opt.subidx]) {
						// more option characters given, which is bad here
						if (s_opt.opterr) {
							printerr("option '-%c' requires an argument", ch);
						}
						s_opt.state = GetOptError;
						return ':';
					}

					if (s_opt.idx+1 >= s_argc) {
						if (s_opt.opterr) {
							printerr("option '-%c' requires an argument", ch);
						}
						s_opt.state = GetOptError;
						return ':';
					}

					// treat next command-line arg as option argument
					s_opt.idx++;
					s_opt.optarg = s_argv[s_opt.idx++];
					s_opt.state = GetOptNextWord;
					return s_opt.optopt;
				}

				s_opt.optarg = nullptr;

				if (!s_argv[s_opt.idx][s_opt.subidx]) {	// end of word, switch to next state
					s_opt.idx++;
					s_opt.state = GetOptNextWord;
				}
				return s_opt.optopt;

			case GetOptRestArgs:		// the rest is only arguments
				s_opt.args.append(s_argv[s_opt.idx]);
				break;

			case GetOptError:
				return -1;
		}
		s_opt.idx++;
	}
	return -1;
}

void ArgvObject::usage(const Options *options) {
	if (options == nullptr) {
		return;
	}
	print("options:");

	// first count the width of center column
	int width = 0;
	int w;

	for(int n = 0; !(options[n].code == 0 && options[n].longopt == nullptr
		&& options[n].arg == nullptr && options[n].desc == nullptr); n++) {
		if (options[n].longopt != nullptr) {
			w = std::strlen(options[n].longopt);
		} else {
			w = 0;
		}
		if (options[n].arg != nullptr) {
			w += std::strlen(options[n].arg) + 1;
		}
		if (w > width) {
			width = w;
		}
	}

	String line;

	// print the usage lines
	for(int n = 0; !(options[n].code == 0 && options[n].longopt == nullptr
		&& options[n].arg == nullptr && options[n].desc == nullptr); n++) {
		if (options[n].code >= '0' && options[n].code <= 'z') {
			printn(" -%c", options[n].code);

			if (options[n].longopt != nullptr) {
				printn(", ");
			} else {
				printn("  ");
			}
		} else {
			printn("     ");
		}
		if (options[n].longopt != nullptr) {
			line = "--";
			line += options[n].longopt;

			if (options[n].arg != nullptr) {
				line += '=';
				line += options[n].arg;
			}
		} else {
			if (options[n].arg != nullptr) {
				line = options[n].arg;
			} else {
				line = "";
			}
		}
		printn("%-*v", width + 6, &line);

		if (options[n].desc != nullptr) {
			print("%s", options[n].desc);
		} else {
			print();
		}
	}
	print();
}

void ArgvObject::optreset(void) {
	s_opt.state = GetOptNextWord;
	s_opt.idx = 1;
	s_opt.subidx = s_opt.optopt = 0;
	s_opt.optarg = nullptr;
	s_opt.args.clear();
}

}	// namespace

// EOB
