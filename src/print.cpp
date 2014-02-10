//
//	ooprint.cpp	WJ112
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

#include "oo/print.h"
#include "oo/Base.h"
#include "oo/Error.h"
#include "oo/String.h"
#include "oo/Mutex.h"

#include <iostream>
#include <iomanip>

namespace oo {

// the printer lock ensures screen output of multiple threads doesn't get messed up
static Mutex printer_lock;


void vssprint(std::stringstream& s, const char *fmt, std::va_list ap) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}
	if (!*fmt) {
		return;
	}

	int width;
	bool long_argument, size_argument;
	
	while(*fmt) {
		if (*fmt == '%') {
			// parse the format string
			fmt++;
			
			width = 0;
			long_argument = false;
			size_argument = false;
			
			// by default, right justified
			s << std::setiosflags(std::ios::right) << std::resetiosflags(std::ios::left)
				<< std::setfill(' ')
				<< std::dec
				<< std::setiosflags(std::ios::fixed) << std::setprecision(6);

			// justify left
			if (*fmt == '-') {
				fmt++;
				s << std::setiosflags(std::ios::left) << std::resetiosflags(std::ios::right);
			}
			// leading zero's
			while(*fmt == '0') {
				fmt++;
				s << std::setfill('0');
			}
			
			// width specifier
			while(*fmt >= '0' && *fmt <= '9') {
				width *= 10;
				width += (*fmt - '0');
				fmt++;
			}

			if (*fmt == '*') {		// variable width
				width = va_arg(ap, int);
				if (width < 0) {
					width = -width;
					s << std::setiosflags(std::ios::left) << std::resetiosflags(std::ios::right);
				}
				fmt++;
			}

			// floating point precision specifier
			if (*fmt == '.') {
				int precision = 0;
				bool precision_ok = false;
				
				fmt++;
				if (*fmt == '*') {
					precision = va_arg(ap, int);
					if (precision < 0)
						precision = -precision;

					fmt++;
					precision_ok = true;
				} else {
					while(*fmt == '0') {    // skip leading zeroes
						precision = 0;
						fmt++;
						precision_ok = true;
					}
					if (*fmt >= '1' && *fmt <= '9') {
						precision = *fmt - '0';
						fmt++;
						precision_ok = true;
					}
					while(*fmt >= '0' && *fmt <= '9') {
						precision *= 10;
						precision += (*fmt - '0');
						fmt++;
						precision_ok = true;
					}
				}

				// else error in format string
				if (!precision_ok) {
					throw RuntimeError("format string error (in precision field)");
				}
				
				// set precision
				s << std::setiosflags(std::ios::fixed) << std::setprecision(precision);
			}
			
			// 'long' argument
			if (*fmt == 'l') {
				fmt++;
				long_argument = true;
			}

			// 'size_t' argument
			if (*fmt == 'z') {
				fmt++;
				size_argument = true;
			}

			if (width > 0) {
				s << std::setw(width);
			}

			// format specifier
			switch(*fmt) {
				// print integer value
				case 'd':
					if (long_argument) {
						s << va_arg(ap, long);
					} else {
						if (size_argument) {
							s << va_arg(ap, ssize_t);
						} else {
							s << va_arg(ap, int);
						}
					}
					break;
					
				case 'x':
					s << std::hex;
					
					if (long_argument) {
						s << va_arg(ap, long);
					} else {
						if (size_argument) {
							s << va_arg(ap, size_t);
						} else {
							s << va_arg(ap, int);
						}
					}
					break;
					
				case 'X':
					s << std::uppercase << std::hex;
					
					if (long_argument) {
						s << va_arg(ap, long);
					} else {
						if (size_argument) {
							s << va_arg(ap, size_t);
						} else {
							s << va_arg(ap, int);
						}
					}
					s << std::nouppercase;
					break;
					
				case 'o':
					s << std::oct;
					
					if (long_argument) {
						s << va_arg(ap, long);
					} else {
						if (size_argument) {
							s << va_arg(ap, size_t);
						} else {
							s << va_arg(ap, int);
						}
					}
					break;
					
				case 'u':
					if (long_argument) {
						s << va_arg(ap, unsigned long);
					} else {
						if (size_argument) {
							s << va_arg(ap, size_t);
						} else {
							s << va_arg(ap, unsigned int);
						}
					}
					break;
					
				// print floating point values
				case 'e':
				case 'E':
				case 'f':
				case 'F':
				case 'g':
				case 'G':
				case 'a':
				case 'A':
					// note: this is nowhere near as versatile as printf()
					s << va_arg(ap, double);
					break;
					
				case 'C':
					long_argument = true;
					// fall through
					
				case 'c':
					if (long_argument) {
						throw RuntimeError("format string error (wchar_t is considered a broken type)");
					} else {
						s << (char)va_arg(ap, int);
					}
					break;
					
				case 'S':
					long_argument = true;
					// fall through
					
				case 's':
					if (long_argument) {
						throw RuntimeError("format string error (wchar_t* is considered a broken type)");
					} else {
						char *cs = va_arg(ap, char *);
						if (cs == nullptr) {
							s << "(nullptr)";
						} else {
							s << cs;
						}
					}
					break;
					
				// print an object's repr()
				case 'q':
					{
						Base *q = va_arg(ap, Base *);
						s << q->repr();
					}
					break;

				// print an object's str()
				case 'v':
					{
						Base *v = va_arg(ap, Base *);
						s << v->str();
					}
					break;

				// print pointer value
				case 'p':
					{
						long p = (long)va_arg(ap, void *);

						if (!p) {
							s << "(nullptr)";
						} else {
							s << std::hex << std::showbase << p << std::noshowbase;
						}
					}
					break;
				
				// boolean value
				case 't':
					s << ((bool)va_arg(ap, int) ? "true" : "false");
					break;

				// unicode value
				case 'U':
					s << "U+" << std::setfill('0') << std::setw(4) << std::hex << std::noshowbase
						<< (rune)va_arg(ap, rune);
					break;

				// rune
				case 'r':
					{
						String rs((rune)va_arg(ap, rune));
						s << rs.str();
					}
					break;

				// the percent sign
				case '%':
					s << '%';
					break;

				default:
					throw RuntimeError("format string error (unsupported modifier)");
			}
		} else {
			s << *fmt;
		}
		if (*fmt) {
			fmt++;
		}
	}
}

void ssprint(std::stringstream& s, const char *fmt, ...) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}
	if (!*fmt) {
		return;
	}
	std::va_list ap;
	va_start(ap, fmt);
	
	vssprint(s, fmt, ap);
	
	va_end(ap);
}

// vprint to output stream
void vosprint(std::ostream& os, const char *fmt, std::va_list ap) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}
	if (!*fmt) {
		return;
	}
	std::stringstream s;

	vssprint(s, fmt, ap);

	printer_lock.lock();
	os << s.str() << std::endl;
	printer_lock.unlock();
}

// vprint to output stream without newline
void vosprintn(std::ostream& os, const char *fmt, std::va_list ap) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}
	if (!*fmt) {
		return;
	}
	std::stringstream s;

	vssprint(s, fmt, ap);

	printer_lock.lock();
	os << s.str();
	printer_lock.unlock();
}

void print(void) {
	printer_lock.lock();
	std::cout << std::endl;
	printer_lock.unlock();
}

void print(const char *fmt, ...) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}

	std::va_list ap;
	va_start(ap, fmt);

	vosprint(std::cout, fmt, ap);

	va_end(ap);
}

void printn(const char *fmt, ...) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}

	std::va_list ap;
	va_start(ap, fmt);

	vosprintn(std::cout, fmt, ap);

	va_end(ap);
}

void printerr(void) {
	printer_lock.lock();
	std::cerr << std::endl;
	printer_lock.unlock();
}

void printerr(const char *fmt, ...) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}

	std::va_list ap;
	va_start(ap, fmt);

	vosprint(std::cerr, fmt, ap);

	va_end(ap);
}

void printnerr(const char *fmt, ...) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}

	std::va_list ap;
	va_start(ap, fmt);

	vosprintn(std::cerr, fmt, ap);

	va_end(ap);
}

}	// namespace

// EOB
