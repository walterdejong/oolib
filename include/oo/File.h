/*
	ooFile.h	WJ112
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

#ifndef OOFILE_H_WJ112
#define OOFILE_H_WJ112

#include "oo/Base.h"
#include "oo/None.h"
#include "oo/String.h"
#include "oo/Error.h"

#include <cstdio>
#include <cstdarg>
#include <memory>

#include <unistd.h>
#include <sys/stat.h>

namespace oo {

class File : public Base {
public:
	File() : Base(), name_(), mode_(),
		w_(std::shared_ptr<FILE>()), is_pipe_(false) { }

	File(const String& name, const String& mode = "r") : Base(),
		name_(name), mode_(mode), w_(std::shared_ptr<FILE>()), is_pipe_(false) { }

	File(FILE *f, bool is_pipe = false) : Base(), name_(), mode_(), is_pipe_(is_pipe) {
		if (f == nullptr) {
			w_ = std::shared_ptr<FILE>();
		} else {
			w_ = is_pipe_ ? std::shared_ptr<FILE>(f, PipeDeleter())
				: std::shared_ptr<FILE>(f, FileDeleter());
		}
	}

	File(const File& f) : Base(), name_(f.name_), mode_(f.mode_),
		w_(f.w_), is_pipe_(f.is_pipe_) { }

	File(File&& f) : File() { swap(*this, f); }

	File(const NoneObject&) : Base(), name_(), mode_(),
		w_(std::shared_ptr<FILE>()), is_pipe_(false) { }

	~File() { this->close(); }

	// these assign and compare to None
	using Base::operator=;
	using Base::operator!;
	using Base::operator==;
	using Base::operator!=;

	File& operator=(File f) {
		swap(*this, f);
		return *this;
	}

	static void swap(File& a, File& b) {
		std::swap(a.name_, b.name_);
		std::swap(a.mode_, b.mode_);
		std::swap(a.w_, b.w_);
		std::swap(a.is_pipe_, b.is_pipe_);
	}

	std::string repr(void) const {
		if (!name_.empty()) {
			std::stringstream ss;
			ss << "<File " << this->fileno() << ":\"" << name_ << "\">";
			return ss.str();
		}
		return "<File>";
	}

	bool isNone(void) const { return (w_.get() == nullptr); }
	void setNone(void) { this->clear(); }

	void clear(void) {
		this->close();
		name_.clear();
		mode_.clear();
	}

	bool open(const String& name, const String& mode) {
		close();

		if (name.empty()) {
			throw ValueError();
		}
		if (mode.empty()) {
			throw ValueError();
		}
		name_ = name;
		mode_ = mode;
		w_ = std::shared_ptr<FILE>(std::fopen(name.c_str(), mode.c_str()), FileDeleter());
		is_pipe_ = false;
		return (w_.get() != nullptr);
	}

	bool open(void) { return open(name_, mode_); }

	bool open(FILE *f, bool ispipe = false) {
		if (f == nullptr) {
			throw ReferenceError();
		}

		close();

		name_.clear();
		mode_.clear();
		is_pipe_ = ispipe;
		w_ = is_pipe_ ? std::shared_ptr<FILE>(f, PipeDeleter())
			: std::shared_ptr<FILE>(f, FileDeleter());
		return (w_.get() != nullptr);
	}

	bool open(int fd, const String& mode) {
		if (fd <= 0) {
			throw ValueError();
		}
		if (mode.empty()) {
			throw ValueError();
		}

		close();

		name_.clear();
		mode_ = mode;
		w_ = std::shared_ptr<FILE>(::fdopen(fd, mode.c_str()), FileDeleter());
		is_pipe_ = false;
		return (w_.get() != nullptr);
	}

	bool popen(const String& cmd, const String& mode) {
		if (cmd.empty()) {
			throw ValueError();
		}
		if (mode.empty()) {
			throw ValueError();
		}

		close();

		name_ = cmd;
		mode_ = mode;
		w_ = std::shared_ptr<FILE>(::popen(cmd.c_str(), mode.c_str()), PipeDeleter());
		is_pipe_ = (w_.get() != nullptr);

		return (w_.get() != nullptr);
	}

	void close(void) {
		this->flush();
		w_ = std::shared_ptr<FILE>();	// destroys previous w_
		is_pipe_ = false;
	}

	String readline(void);
	Array<String> readlines(void);
	size_t read(void *, size_t);
	void write(const String&);
	void writelines(const Array<String>&);
	void write(void *, size_t);
	void seek(long, int);
	long tell(void) const;

	void flush(void) {
		if (w_.get() != nullptr) {
			std::fflush(w_.get());
		}
	}

	bool mktemp(void);
	void truncate(off_t);
	struct stat stat(void);
	struct stat lstat(void);

	void unlink(void) {
		if (!name_.empty()) {
			::unlink(name_.c_str());
		}
	}

	String name(void) const { return name_; }
	String mode(void) const { return mode_; }
	FILE *stream(void) const { return w_.get(); }
	bool ispipe(void) const { return is_pipe_; }

	int fileno(void) const {
		if (w_.get() == nullptr) {
			return -1;
		}
		return ::fileno(w_.get());
	}

private:
	// two local classes act as delete-functors for the shared_ptr<FILE>
	class FileDeleter {
	public:
		void operator()(FILE *f) const {
			if (f != nullptr) {
				std::fclose(f);
			}
		}
	};

	class PipeDeleter {
	public:
		void operator()(FILE *p) const {
			if (p != nullptr) {
				::pclose(p);
			}
		}
	};

	String name_, mode_;
	std::shared_ptr<FILE> w_;
	bool is_pipe_;

	friend std::ostream& operator<<(std::ostream&, const File&);
};

inline std::ostream& operator<<(std::ostream& os, const File& f) {
	os << f.str();
	return os;
}

extern File Stdin, Stdout, Stderr;


// factory funs
File open(const String& filename, const String& mode = "r");
File open(int fd, const String& mode = "r");
File popen(const String& cmd, const String& mode = "r");
File tempfile(bool unlink_file=true);

void fprint(File&, const char *, ...);
void vfprint(File&, const char *, std::va_list);

}	// namespace

#endif	// OOFILE_H_WJ112

// EOB
