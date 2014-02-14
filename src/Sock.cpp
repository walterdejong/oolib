//
//	ooSocket.c	WJ112
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

#include "oo/Sock.h"
#include "oo/print.h"

#include <cerrno>
#include <cstring>

#include <netdb.h>
#include <sys/types.h>

namespace oo {

int Sock::getprotobyname(const char *name) {
	if (name == nullptr) {
		name = "tcp";
	}

	struct protoent *proto = ::getprotobyname(name);
	if (proto == nullptr) {
		return -1;
	}
	return proto->p_proto;
}

int Sock::getservbyname(const char *name, const char *proto) {
	if (name == nullptr) {
		throw ReferenceError();
	}

	if (proto == nullptr) {
		proto = "tcp";
	}

	struct servent *serv = ::getservbyname(name, proto);
	if (!serv) {
		// maybe it's just a numeric string
		int port;
		try {
			port = convert<int>(name);
		} catch(ValueError err) {
			return -1;
		}
		if (port <= 0) {	// invalid port number
			return -1;		// should we throw ValueError instead?
		}
		return port;
	}
	return ntohs(serv->s_port);
}

String Sock::getservbyport(int port, const char *proto) {
	if (proto == nullptr) {
		proto = "tcp";
	}

	struct servent *serv = ::getservbyport(htons(port), proto);
	if (!serv) {
		return String("");
	}
	return String(serv->s_proto);
}

bool Sock::listen(const char *serv) {
	int sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		throw IOError("failed to create socket");
	}

	// set option reuse bind address
	int on = 1;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1) {
		::close(sock);
		throw IOError("failed to set socket option");
	}

#ifdef SO_REUSEPORT
	int on2 = 1;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &on2, sizeof(int)) == -1) {
		::close(sock);
		throw IOError("failed to set socket option");
	}
#endif

	struct sockaddr_in sa;
	std::memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;

	int port = this->getservbyname(serv);
	if (port == -1) {
		::close(sock);
		throw IOError("failed to listen, service unknown");
	}
	sa.sin_port = htons(port);

	socklen_t sa_len = sizeof(struct sockaddr_in);

	if (::bind(sock, (struct sockaddr *)&sa, sa_len) == -1) {
		::close(sock);
		throw IOError("failed to bind socket");
	}
	if (::listen(sock, SOMAXCONN) == -1) {
		::close(sock);
		throw IOError("failed to listen on socket");
	}
	if (!f_.open(sock, "rw")) {
		::close(sock);
		throw IOError("failed to tie socket to a stream");
	}
	return true;
}

bool Sock::listen6(const char *serv) {
	int sock = ::socket(AF_INET6, SOCK_STREAM, 0);
	if (sock == -1) {
		throw IOError("failed to create socket");
	}

	// set option reuse bind address
	int on = 1;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1) {
		::close(sock);
		throw IOError("failed to set socket option");
	}

#ifdef SO_REUSEPORT
	int on2 = 1;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &on2, sizeof(int)) == -1) {
		::close(sock);
		throw IOError("failed to set socket option");
	}
#endif

	struct sockaddr_in6 sa;
	std::memset(&sa, 0, sizeof(struct sockaddr_in6));
	sa.sin6_family = AF_INET6;
	sa.sin6_addr = in6addr_any;

	int port = this->getservbyname(serv);
	if (port == -1) {
		::close(sock);
		throw IOError("failed to listen, service unknown");
	}
	sa.sin6_port = htons(port);

	socklen_t sa_len = sizeof(struct sockaddr_in6);

	if (::bind(sock, (struct sockaddr *)&sa, sa_len) == -1) {
		::close(sock);
		throw IOError("failed to bind socket");
	}
	if (::listen(sock, SOMAXCONN) == -1) {
		::close(sock);
		throw IOError("failed to listen on socket");
	}
	if (!f_.open(sock, "rw")) {
		::close(sock);
		throw IOError("failed to tie socket to a stream");
	}
	return true;
}

bool Sock::connect(const char *ipaddr, const char *serv) {
	if (ipaddr == nullptr) {
		throw ReferenceError();
	}
	if (serv == nullptr) {
		throw ReferenceError();
	}

	if (!this->isclosed()) {
		throw IOError("socket is already in use");
	}

	struct addrinfo hints, *res;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;	// IPv4, IPv6, or any other protocol
	hints.ai_socktype = SOCK_STREAM;

	if (::getaddrinfo(ipaddr, serv, &hints, &res) != 0) {
		throw IOError("failed to get address info");
	}

	int sock = -1;

	// try connecting, try out all protocols
	for(struct addrinfo *r = res; r != nullptr; r = r->ai_next) {
		if ((sock = ::socket(r->ai_family, r->ai_socktype, r->ai_protocol)) == -1) {
			continue;
		}
		if (::connect(sock, r->ai_addr, r->ai_addrlen) == -1) {
			::close(sock);
			sock = -1;
			continue;
		}
		break;	// successful connect
	}

	freeaddrinfo(res);

	if (sock == -1) {
		throw IOError("failed to connect to remote host");
	}

	// tie it to a stream
	if (!f_.open(sock, "w+")) {
		::close(sock);
		sock = -1;
		throw IOError("failed to open socket as a stream");
	}
	return true;
}

Sock Sock::accept(void) const {
	if (this->isclosed()) {
		throw IOError("accept() called on a non-listening socket");
	}

	Sock sock;

	struct sockaddr_storage addr;
	socklen_t addr_len = sizeof(struct sockaddr_storage);

	int sockfd;

	for(;;) {
		sockfd = ::accept(f_.fileno(), (struct sockaddr *)&addr, &addr_len);
		if (sockfd == -1) {
			if (errno == EINTR) {
				continue;
			}
			return Sock();
		}
		break;
	}
	if (!sock.f_.open(sockfd, "w+")) {
		return Sock();
	}
	return sock;
}

String Sock::remoteaddr(void) const {
	if (this->isclosed()) {
		throw IOError("can not get remote address of an unconnected socket");
	}

	struct sockaddr_storage addr;
	socklen_t addr_len = sizeof(struct sockaddr_storage);

	if (::getpeername(f_.fileno(), (struct sockaddr *)&addr, &addr_len) == -1) {
		throw IOError("failed to get remote address of socket");
	}

	char host[NI_MAXHOST];

	if (::getnameinfo((struct sockaddr *)&addr, addr_len, host, sizeof(host),
		nullptr, 0, NI_NUMERICHOST|NI_NUMERICSERV) == -1) {
		throw IOError("failed to get numeric address of remote host");
	}
	return String(host);
}

Sock listen(const char *serv) {
	Sock sock;

	if (!sock.listen(serv)) {
		return Sock();
	}
	return sock;
}

Sock listen6(const char *serv) {
	Sock sock;

	if (!sock.listen6(serv)) {
		return Sock();
	}
	return sock;
}

Sock connect(const char *ipaddr, const char *serv) {
	Sock sock;

	if (!sock.connect(ipaddr, serv)) {
		return Sock();
	}
	return sock;
}

String resolv(const String& ipaddr) {
	if (ipaddr.empty()) {
		throw ValueError();
	}

	struct addrinfo *res;

	if (::getaddrinfo(ipaddr.c_str(), nullptr, nullptr, &res) != 0) {
		// probably invalid IP address
		return ipaddr;
	}

	char host[NI_MAXHOST];

	for(struct addrinfo *r = res; r != nullptr; r = r->ai_next) {
		if (::getnameinfo(r->ai_addr, r->ai_addrlen, host, sizeof(host), nullptr, 0, 0) == 0) {
			freeaddrinfo(res);
			return String(host);
		}
	}

	// some kind of error
//	print("TD error: %s", ::gai_strerror(err));

	freeaddrinfo(res);
	return ipaddr;
}

void fprint(Sock& sock, const char *fmt, ...) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}
	if (!*fmt) {
		return;
	}

	std::va_list ap;
	va_start(ap, fmt);

	vfprint(sock, fmt, ap);

	va_end(ap);
}

void vfprint(Sock& sock, const char *fmt, std::va_list ap) {
	if (fmt == nullptr) {
		throw ReferenceError();
	}

	std::stringstream ss;
	vssprint(ss, fmt, ap);
	sock.write(ss.str());
}

}	// namespace

// EOB
