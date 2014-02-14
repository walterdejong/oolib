//
//	ooError.cpp	WJ112
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

#include "oo/Error.h"

#include <iostream>
#include <cstdlib>

namespace oo {

const int kMemoryError = -1;
const int kIndexError = -2;
const int kValueError = -3;
const int kTypeError = -4;
const int kZeroDivisionError = -5;
const int kReferenceError = -6;
const int kIOError = -7;
const int kRuntimeError = -8;
const int kOSError = -9;
const int kStringEncodingError = -10;

static bool initError(void);
static void terminate_handler(void);

static bool initialized = initError();

static bool initError(void) {
	std::set_terminate(terminate_handler);
	return true;
}

static void terminate_handler(void) {
	try {
		throw;
	} catch(Error& err) {
		std::cout << std::endl << err.str() << std::endl << std::endl;
	} catch(...) {
		std::cout << std::endl << "fatal error: uncaught exception" << std::endl << std::endl;
	}
	std::abort();
}

void panic(const Error& e) {
	std::cout << std::endl << "fatal error: " << e << std::endl << std::endl;
	std::abort();
}

void panic(const std::string& s) {
	std::cout << std::endl << "fatal error: " << s << std::endl << std::endl;
	std::abort();
}

}	// namespace

// EOB
