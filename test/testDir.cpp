/*
	testDir.cpp	WJ112
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

#include "oolib"

using namespace oo;

void visit_func(const String& path, Array<String>& entries) {
	String fullpath;

	for(size_t i = 0; i < len(entries);) {
		// skip dot-files and dot-dirs
		if (entries[i][0] == '.') {
			entries.pop(i);
			continue;
		}

		fullpath = path + "/" + entries[i];

		print("%v", &fullpath);

		i++;
	}
/*
	which is the same as:

	foreach(i, entries) {
		if (entries[i][0] == '.') {
			entries.pop(i);
			i--;			// rig this
			continue;
		}
		fullpath = path + "/" + entries[i];
		print("%v", &fullpath);
	}
*/
}

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		print("usage: listdir [directory]");
		return 1;
	}

	Array<String> a;
	
	if (listdir(argv[1], a) < 0) {
		perror("listdir");
		return -1;
	}

	print("%v", &a);

//	foreach(i, a)
//		print("%v", &a[i]);

	int err = treewalk(argv[1], visit_func);
	print("treewalk return code: %d", err);

	if (isdir("/etc"))
		print("/etc is a directory");
	else
		print("/etc is not a directory");

	if (islink("/etc"))
		print("/etc is a symbolic link");
	else
		print("/etc is not a symbolic link");

	print("filesize testDir.cpp: %lu bytes", (unsigned long)filesize("testDir.cpp"));

	return 0;
}

// EOB
