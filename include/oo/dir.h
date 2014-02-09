/*
	oodir.h	WJ112
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

#ifndef OODIR_H_WJ112
#define OODIR_H_WJ112

#include "oo/String.h"

#include <sys/stat.h>

namespace oo {

// list directory contents
// returns 0 on success, or a negative error code on failure
// upon success, Array<String> has directory listing
int listdir(const String&, Array<String>&);

int treewalk(const String& path, void (*visit)(const String&, Array<String>&),
	int (*onError)(const String&, int) = nullptr, bool followlinks = false);

bool exists(const char *);
bool isfile(const char *);
bool isdir(const char *);
bool islink(const char *);
bool isfifo(const char *);
bool isblockdev(const char *);
bool ischardev(const char *);
bool isdev(const char *);
bool issock(const char *);

off_t filesize(const char *);
mode_t file_mode(const char *);
uid_t file_owner(const char *);
gid_t file_group(const char *);
struct stat file_stat(const char *);

}	// namespace

#endif	// OODIR_H_WJ112

// EOB
