/*
	testFile.cpp	WJ112
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

#include <unistd.h>

using namespace oo;

int main(void) {
	File f("testFile.cpp");

	if (!f.open()) {
		print("failed to open file");
		return -1;
	}

	String s;

	while(true) {
		s = f.readline();
		if (!s)
			break;

		printn("> %v", &s);
	}
	print("f: %v", &f);

	struct stat statbuf = f.stat();

	print("file size: %lu", (unsigned long)statbuf.st_size);

	print("fileno: %d", f.fileno());

	f.close();

	f = tempfile(false);
	print("tempfile: %s", f.name().c_str());
	f.write("dag!!\n");

	f = None;	// this also closes the file
	if (!f)
		print("f equals None");

	print("test reading from a command pipe: /bin/ls -l");

	f = popen("/bin/ls -l");

	print("f: %v", &f);
	print("Stderr: %v", &Stderr);

	while(true) {
		s = f.readline();
		if (!s)
			break;

//		fprint(Stderr, "%v", &s);
		printn("%v", &s);
	}
	f = None;	// also closes the pipe
//	print("f == %v", &f);

	Stderr = None;	// close stderr
	fprint(Stderr, "bye bye!!\n");	// no error when writing to closed stderr
	return 0;
}

// EOB
