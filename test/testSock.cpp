//
//	testSock.cpp	WJ112
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

#include "oolib"

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace oo;

Sock main_conn;

void serve_client(void) {
	if (!main_conn) {
		print("[%u] problem, main_conn is None!", gettid());
		return;
	}
	
	main_conn.write("hello to the server\r\n> ");

	String line = main_conn.readline();
	line = line.strip();

	fprint(main_conn, "got: %v\r\n", &line);

	main_conn.shutdown();
	main_conn.close();
}

void sigchild_handler(int sig) {
	int status;

	::waitpid(-1, &status, 0);	// reap child: clean up the zombies
}

void server_func(void) {
	// setup signal handler that cleans up zombie processes
	set_signal(SIGCHLD, sigchild_handler);

	Sock server = listen("1234");
	if (server == None) {
		print("error: listen() returned None");
		return;
	}
	print("server accepting connections at port 1234");
	print("Press Ctrl-C to terminate");
	for(;;) {
		main_conn = server.accept();
		if (!main_conn) {
			print("accept: error");
			exit(-1);
		}

		String fromaddr = main_conn.remoteaddr();
		String remotehost = resolv(fromaddr);
		print("new connection from [%v] [%v]", &fromaddr, &remotehost);

		child(serve_client);

		// parent closes child's fd
		main_conn.close();
	}
	oo::wait();
}


int main(void) {
	printn("connecting to www.google.com port www ... ");

	Sock sock = connect("www.google.com", "www");

	if (sock == None) {
		print("failed!");
		return 127;
	}

	print("OK");

	sock.write("GET /index.html HTTP/1.0\r\n"
		"User-Agent: testSock 0.1\r\n"
		"\r\n");

/*
	char buf[4096];
	int n;

	while((n = sock.read(buf, sizeof(buf))) > 0) {
		buf[n] = 0;
		print("> %s", buf);
	}
*/

	String line;
	while(true) {
		line = sock.readline();
		if (line == None)
			break;

		line = line.strip();
		if (!line)
			continue;

		print("> %v", &line);
	}

// socket also closes automatically when it destructs
//	sock.close();

	sock = None;
	if (sock != None)
		print("compare to None: FAIL");

	// now test server functions
	print();
	print();
	server_func();
	return 0;
}

// EOB
