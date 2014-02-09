/*
	ooFile.cpp	WJ112
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

#include "oo/File.h"
#include "oo/Error.h"
#include "oo/print.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>

// no fdopen() in std::
#include <stdio.h>

namespace oo {

File Stdin(::stdin), Stdout(::stdout), Stderr(::stderr);

String File::readline(void) {
	if (w_.get() == nullptr) {
		throw IOError("read line from a closed file");
	}

	char buf[1024];

	if (fgets(buf, sizeof(buf), w_.get()) == nullptr) {
		if (feof(w_.get())) {
			return String();
		}
		throw IOError();
	}
	return String(buf);
}

Array<String> File::readlines(void) {
	if (w_.get() == nullptr) {
		throw IOError("read lines from a closed file");
	}

	Array<String> a;
	String s;

	while(true) {
		s = this->readline();
		if (!s) {
			break;
		}
		a.append(s);
	}
	return a;
}

size_t File::read(void *buf, size_t n) {
	assert(buf != nullptr);

	if (w_.get() == nullptr) {
		throw IOError("read from a closed file");
	}

	size_t bytes_read = std::fread(buf, 1, n, w_.get());

	if (bytes_read < n && std::ferror(w_.get())) {
		throw IOError("read failed");
	}
	return bytes_read;
}

void File::write(const String& s) {
	if (this->isNone()) {
		if (this == &Stdout || this == &Stderr) {
			// it's OK to close stdout/stderr and still write to it
			return;
		}
		throw IOError("write to a closed file");
	}

	std::string str = s.str();

	size_t bytes_written = std::fwrite(str.c_str(), 1, str.size(), w_.get());

	if (bytes_written < str.size() && std::ferror(w_.get())) {
		throw IOError("write failed");
	}
}

void File::writelines(const Array<String>& a) {
	for(size_t i = 0; i < a.len(); i++) {
		this->write(a[i]);
	}
}

void File::write(void *buf, size_t n) {
	assert(buf != nullptr);

	if (this->isNone()) {
		if (this == &Stdout || this == &Stderr) {
			// it's OK to close stdout/stderr and still write to it
			return;
		}
		throw IOError("write to a closed file");
	}

	size_t bytes_written = std::fwrite(buf, 1, n, w_.get());

	if (bytes_written < n && std::ferror(w_.get())) {
		throw IOError("write failed");
	}
}

void File::seek(long offset, int whence) {
	if (w_.get() == nullptr) {
		throw IOError("seek on closed file");
	}
	if (std::fseek(w_.get(), offset, whence) == -1) {
		throw IOError("seek failed");
	}
}

long File::tell(void) const {
	if (w_.get() == nullptr) {
		throw IOError("tell on closed file");
	}

	long pos = std::ftell(w_.get());

	if (pos == -1L) {
		throw IOError("tell failed");
	}
	return pos;
}

bool File::mktemp(void) {
	const char *tmpdir = std::getenv("TMPDIR");
	if (tmpdir == nullptr) {
		tmpdir = "/tmp";
	}

	char *tmpname = new char[std::strlen(tmpdir) + 16];
	std::sprintf(tmpname, "%s/temp.XXXXXX", tmpdir);
	int fd = ::mkstemp(tmpname);
	if (fd == -1) {
		delete [] tmpname;
		return false;
	}

	name_ = tmpname;
	delete [] tmpname;
	mode_ = "w+b";

	return this->open(fd, mode_);
}

void File::truncate(off_t l) {
	assert(!name_.empty());

	if (::truncate(name_.c_str(), l) == -1) {
		throw IOError("truncate failed");
	}
}

struct stat File::stat(void) {
	assert(!name_.empty());

	struct stat statbuf;

	if (::stat(name_.c_str(), &statbuf) == -1) {
		throw IOError("stat failed");
	}
	return statbuf;
}

struct stat File::lstat(void) {
	assert(!name_.empty());

	struct stat statbuf;

	if (::lstat(name_.c_str(), &statbuf) == -1) {
		throw IOError("stat failed");
	}
	return statbuf;
}

// factory funs

File open(const String& a_filename, const String& a_mode) {
	assert(!a_filename.empty());
	assert(!a_mode.empty());

	File f;
	f.open(a_filename, a_mode);
	return f;
}

File open(int fd, const String& a_mode) {
	assert(fd >= 0);
	assert(!a_mode.empty());

	File f;
	f.open(fd, a_mode);
	return f;
}

File popen(const String& cmd, const String& a_mode) {
	assert(!cmd.empty());
	assert(!a_mode.empty());

	File f;
	f.popen(cmd, a_mode);
	return f;
}

// default argument unlink_file=true
File tempfile(bool unlink_file) {
	File f;

	f.mktemp();

	if (unlink_file) {
		f.unlink();
	}
	return f;
}

void fprint(File& f, const char *fmt, ...) {
	assert(fmt != nullptr);

	if (!*fmt) {
		return;
	}
	std::va_list ap;
	va_start(ap, fmt);

	vfprint(f, fmt, ap);

	va_end(ap);
}

void vfprint(File& f, const char *fmt, std::va_list ap) {
	assert(fmt != nullptr);

	std::stringstream ss;
	vssprint(ss, fmt, ap);
	f.write(ss.str());
}

}	// namespace

// EOB
