//
//	ooSock.h	WJ112
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

#ifndef OOSOCK_H_WJ112
#define OOSOCK_H_WJ112

#include "oo/File.h"

#include <utility>

#include <unistd.h>
#include <sys/socket.h>

namespace oo {

/*
	Sock is a high-level abstraction for sockets
	It uses only blocking TCP SOCK_STREAM sockets: no UDP
	It supports both IPv4 and IPv6
	and you can use it much like Files: read(), write(), readline(), etc.

	If you need non-blocking sockets or other lower-level access,
	either mess around with Sock.fileno() or write your own altogether
*/

class Sock : public Base {
public:
	Sock() : Base(), f_() { }

	Sock(const Sock& s) : Base(), f_(s.f_) { }

	Sock(Sock&& s) : Base(), f_(std::move(s.f_)) { }

	virtual ~Sock() {
		clear();
	}

	Sock& operator=(const Sock& s) {
		if (this == &s) {
			return *this;
		}
		f_ = s.f_;
		return *this;
	}

	Sock& operator=(Sock&& s) {
		f_ = std::move(s.f_);
		return *this;
	}

	std::string repr(void) const { return "<Sock>"; }

	bool operator!(void) const { return f_.isclosed(); }

	void clear(void) {
		shutdown();
		close();
	}

	void shutdown(int how = SHUT_RDWR) {
		if (!f_.isclosed()) {
			f_.flush();
			::shutdown(f_.fileno(), how);
		}
	}

	void close(void) {
		if (!f_.isclosed()) {
			f_.flush();
			f_.close();
		}
	}

	bool isclosed(void) const { return f_.isclosed(); }

	static int getprotobyname(const char *name = nullptr);
	// note, these use real port numbers, not in network byte order
	static int getservbyname(const char *name, const char *proto = nullptr);
	static String getservbyport(int, const char *proto = nullptr);

	bool listen(const char *serv);
	bool listen6(const char *serv);
	bool connect(const char *ipaddr, const char *serv);
	Sock accept(void) const;

	String readline(void) { return f_.readline(); }
	Array<String> readlines(void) { return f_.readlines(); }
	size_t read(void *v, size_t n) { return f_.read(v, n); }
	void write(const String& s) { return f_.write(s); }
	void writelines(const Array<String>& a) { return f_.writelines(a); }
	void write(void *v, size_t n) { return f_.write(v, n); }

	int fileno(void) const { return f_.fileno(); }

	String remoteaddr(void) const;
	String getpeername(void) const { return remoteaddr(); }

private:
	File f_;
};

Sock listen(const char *serv);
Sock listen6(const char *serv);
Sock connect(const char *ipaddr, const char *serv);
String resolv(const String&);

void fprint(Sock&, const char *, ...);
void vfprint(Sock&, const char *, std::va_list);

}

#endif	// OOSOCK_H_WJ112

// EOB
