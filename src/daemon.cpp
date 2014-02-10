//
//	oodaemon.cpp	WJ112
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

#include "oo/daemon.h"
#include "oo/Error.h"
#include "oo/Argv.h"

#include <cstdlib>
#include <cstdio>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

namespace oo {

void daemonize(bool goto_background) {
	if (getppid() == 1) {
		// if started from init, skip the most part
		goto_background = false;

		// close all file descriptors
		for(int fd = ::getdtablesize()-1; fd >= 0; fd--)
			::close(fd);

		// reopen stdin, stdout, stderr as /dev/null
		int fd = ::open("/dev/null", O_RDWR);
		if (fd >= 0) {
			::dup(fd);
			::dup(fd);
		}
	}

	if (goto_background) {
		::signal(SIGTTOU, SIG_IGN);
		::signal(SIGTTIN, SIG_IGN);
		::signal(SIGTSTP, SIG_IGN);

		pid_t pid = ::fork();
		if (pid == (pid_t)-1) {
			throw OSError("failed to fork");
		}
		if (pid > 0) {		// parent exits
			std::exit(0);
		}
		::setsid();

		// lose controlling tty
		int fd = ::open("/dev/tty", O_WRONLY);
		if (fd >= 0) {
			::ioctl(fd, TIOCNOTTY, nullptr);
			::close(fd);
		}

		::setpgrp();

		// fork again
		pid = ::fork();
		if (pid == (pid_t)-1) {
			throw OSError("failed to fork");
		}
		if (pid > 0) {		// parent exits
			std::exit(0);
		}
		// close all file descriptors
		for(int fd = ::getdtablesize()-1; fd >= 0; fd--) {
			::close(fd);
		}
		// reopen stdin, stdout, stderr as /dev/null
		if ((fd = ::open("/dev/null", O_RDWR)) >= 0) {
			::dup(fd);
			::dup(fd);
		}
	}

	::chdir("/");
	::umask(022);
}

void drop_privs(uid_t uid, gid_t gid) {
	if (::getuid() == (uid_t)0) {		// running as root
		if (::setgid(gid) != 0) {
			throw OSError("failed to drop group privileges");
		}
		// change the uid
		// use panic() rather than throw ... you can't catch+ignore panic()
		if (::setuid(uid) != 0) {
			panic("failed to drop privileges");
		}
	}
}

void drop_privs(const char *username) {
	if (username == nullptr) {
		throw ReferenceError();
	}

	struct passwd *pw = ::getpwnam(username);
	if (pw == nullptr) {
		char buf[256];

		std::snprintf(buf, sizeof(buf),
			"failed to drop privileges: no such user '%s'", username);

		panic(buf);
	}
	drop_privs(pw->pw_uid, pw->pw_gid);
}

void write_pidfile(void) {
	// Argv must have been initialized
	if (Argv.prognam() == nullptr) {
		return;
	}

	// note: use buffer and fopen: still easier than stringstream and fstream

	char pidfile[1024];
	::snprintf(pidfile, sizeof(pidfile), "/var/run/%s.pid", Argv.prognam());

	FILE *f = std::fopen(pidfile, "w+");
	if (f != nullptr) {
		std::fprintf(f, "%u\n", (unsigned int)getpid());
		std::fclose(f);

		std::atexit(remove_pidfile);
	}
}

void remove_pidfile(void) {
	// Argv must have been initialized
	if (Argv.prognam() == nullptr) {
		return;
	}

	char pidfile[1024];
	std::snprintf(pidfile, sizeof(pidfile), "/var/run/%s.pid", Argv.prognam());

	::unlink(pidfile);
}

}	// namespace

// EOB
