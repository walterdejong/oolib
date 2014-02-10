/*
	oodir.cpp	WJ112
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

#include "oo/dir.h"

#include <cstdlib>
#include <cerrno>

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

namespace oo {

// returns 0 on success or negative errno on error
int listdir(const String& path, Array<String>& a) {
	if (path.empty()) {
		throw ValueError();
	}

	const char *cpath = path.str().c_str();

	struct stat statbuf;

	if (::stat(cpath, &statbuf) == -1) {
		return (errno > 0) ? -errno : errno;
	}

	int have_access = ::access(cpath, R_OK|X_OK);
	if (have_access == -1) {
		return (errno > 0) ? -errno : errno;
	}

	DIR *dir_p = ::opendir(cpath);
	if (dir_p == nullptr) {
		return (errno > 0) ? -errno : errno;
	}

	struct dirent *entry;

	while((entry = readdir(dir_p)) != nullptr) {
		// skip "." and ".." dirs
		if (entry->d_name[0] == '.') {
			if (!entry->d_name[1]) {
				continue;
			}
			if (entry->d_name[1] == '.' && !entry->d_name[2]) {
				continue;
			}
		}
		a.append(entry->d_name);
	}
	closedir(dir_p);
	return 0;
}

int treewalk(const String& path, void (*visit)(const String&, Array<String>&),
	int (*onError)(const String&, int), bool followlinks) {

	if (path.empty()) {
		throw ValueError();
	}
	if (visit == nullptr) {
		throw ReferenceError();
	}

	Array<String> entries;

	int err = listdir(path, entries);
	if (err != 0) {
		return err;
	}

	visit(path, entries);

	String fullpath, entry;
	struct stat statbuf;

	while(len(entries) > 0) {
		entry = entries.pop(0);

		fullpath = path + "/" + entry;

		// if we can't access a single path element, ignore the error and continue the walk
		if (::stat(fullpath.str().c_str(), &statbuf) == -1) {
			if (onError == nullptr) {
				continue;
			}

			err = onError(fullpath, errno);
			if (!err) {
				continue;
			}
			return err;
		}
		if (statbuf.st_mode & S_IFDIR) {
			if ((statbuf.st_mode & S_IFLNK) && !followlinks) {
				continue;
			}
			err = treewalk(fullpath, visit, onError, followlinks);
			if (err != 0) {
				return err;
			}
		}
	}
	return 0;
}

bool exists(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	return (::lstat(path, &statbuf) == 0);
}

bool isfile(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		return false;
	}
	return ((statbuf.st_mode & S_IFREG) == S_IFREG);
}

bool isdir(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		return false;
	}
	return ((statbuf.st_mode & S_IFDIR) == S_IFDIR);
}

bool islink(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::lstat(path, &statbuf) == -1) {
		return false;
	}
	return ((statbuf.st_mode & S_IFLNK) == S_IFLNK);
}

bool isfifo(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		return false;
	}
	return ((statbuf.st_mode & S_IFIFO) == S_IFIFO);
}

bool isblockdev(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		return false;
	}
	return ((statbuf.st_mode & S_IFBLK) == S_IFBLK);
}

bool ischardev(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		return false;
	}
	return ((statbuf.st_mode & S_IFCHR) == S_IFCHR);
}

bool isdev(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		return false;
	}
	return ((statbuf.st_mode & (S_IFCHR|S_IFBLK)) != 0);
}

bool issock(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		return false;
	}
	return ((statbuf.st_mode & S_IFSOCK) == S_IFSOCK);
}

off_t filesize(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		throw OSError("no such file or directory");
	}
	return statbuf.st_size;
}

mode_t file_mode(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		throw OSError("no such file or directory");
	}
	return statbuf.st_mode;
}

uid_t file_owner(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		throw OSError("no such file or directory");
	}
	return statbuf.st_uid;
}

gid_t file_group(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::stat(path, &statbuf) == -1) {
		throw OSError("no such file or directory");
	}
	return statbuf.st_gid;
}

struct stat file_stat(const char *path) {
	if (path == nullptr) {
		throw ReferenceError();
	}

	struct stat statbuf;

	if (::lstat(path, &statbuf) == -1) {
		throw OSError("no such file or directory");
	}
	return statbuf;
}

}	// namespace

// EOB
