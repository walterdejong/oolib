/*
	testString.cpp	WJ112
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

int main(void) {
	String s1 = "Hello, world";
	print("s1: %q  len: %zu  cap: %zu", &s1, len(s1), cap(s1));

	String s2 = "こんにちは ワールド";
	print("s2: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));

	s1 = s2;
	print("s1 = s2;");
	print("s1: %q  len: %zu  cap: %zu", &s1, len(s1), cap(s1));
	print("s1[-1]: '%r'", s1[-1]);
	print("s1[3]:  '%r'", s1[3]);

	s1 = s2;
	print("s1: %q  len: %zu  cap: %zu", &s1, len(s1), cap(s1));

	s1 += " en doei!!!";
	print("s1: %q  len: %zu  cap: %zu", &s1, len(s1), cap(s1));

	s1 = s2 + " en tot ziens" + '!' + '!';
	print("s1: %q  len: %zu  cap: %zu", &s1, len(s1), cap(s1));

	s1 += '!';
	s1 += '!';
	s1 += '!';
	print("s1: %q  len: %zu  cap: %zu", &s1, len(s1), cap(s1));

	s1 = String("abc") * 10;
	print("s1: %q  len: %zu  cap: %zu", &s1, len(s1), cap(s1));

	print("s1 == 'abc': %d", (s1 == "abc") ? 1 : 0);
	print("s1 == 'abcabcabcabcabcabcabcabcabcabc': %d", (s1 == "abcabcabcabcabcabcabcabcabcabc") ? 1 : 0);

	print("find(c): %d", s1.find('c'));
	print("find(c, -3): %d", s1.find('c', -3));
	print("find(c, -3, 20): %d", s1.find('c', -3, 20));
	print("find(c, 4): %d", s1.find('c', 4));
	print("find(Z): %d", s1.find('Z'));

	print("rfind(c): %d", s1.rfind('c'));
	print("rfind(c, -3): %d", s1.rfind('c', -3));
	print("rfind(c, -3, 20): %d", s1.rfind('c', -3, 20));
	print("rfind(c, 4): %d", s1.rfind('c', 4));
	print("rfind(Z): %d", s1.rfind('Z'));

	print("find(cab): %d", s1.find("cab"));
	print("find(cab, 3): %d", s1.find("cab", 3));
	print("find(cab, -4): %d", s1.find("cab", -4));
	print("find(cab, -4, 28): %d", s1.find("cab", -4, 28));
	print("find(cab, -4, 29): %d", s1.find("cab", -4, 29));

	print("rfind(cab): %d", s1.rfind("cab"));
	print("rfind(cab, 3): %d", s1.rfind("cab", 3));
	print("rfind(cab, -4): %d", s1.rfind("cab", -4));
	print("rfind(cab, -4, 28): %d", s1.rfind("cab", -4, 28));
	print("rfind(cab, -4, 29): %d", s1.rfind("cab", -4, 29));

	s1 = "\t  strip testje ...	";
	s2 = s1.lstrip();
	print("s2: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));

	s2 = s1.rstrip();
	print("s2: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));

	s2 = s1.strip();
	print("s2: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));

	s1 = "the quick brown 狐 jumps over the lazy 犬";
	s2 = s1.upper();
	print("s2: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));
	s2 = s1.capwords();
	print("s2: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));

	s2 = s1.slice(4, 9);
	print("s2: slice: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));
	s2 = s1.slice(-6, len(s1));
	print("s2: slice: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));
	s2 = s1.slice(-100, 200);
	print("s2: slice: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));
	s2 = s1.slice(100, 200);
	print("s2: slice: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));

	s2 = s1.replace("the ", "a ");
	print("s2: replace: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));

	print("split():");
	s1 = "the quick brown 狐 jumps over the lazy 犬";
	Array<String> a = s1.split();
	foreach(i, a)
		print("a[%zu]: %q", i, &a[i]);

	s2 = s1.join(a, '.');
	print("s2: join: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));

	s2 = s1.join(a, " + ");
	print("s2: join: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));

	s2 = sprint("%v %v jumps %d times over %v", &a[2], &a[3], 10, &a[-1]);
	print("s2: sprint: %q  len: %zu  cap: %zu", &s2, len(s2), cap(s2));

	int n = convert<int>(std::string("1234"));
	print("n == %d", n);
	print("type conversion: %s", (n == 1234) ? "OK" : "FAIL");

	s1 = "abc";
	s2 = s1;
	print("eq: %s", (s1 == s2) ? "OK" : "FAIL");

	s2 += "d";
	print("ne: %s", (s1 != s2) ? "OK" : "FAIL");
	print("lt: %s", (s1 < s2) ? "OK" : "FAIL");
	print("gt: %s", (s2 > s1) ? "OK" : "FAIL");

//	s2 = None;
//	print("s2: assignment to None: %v  len: %zu  cap: %zu", &s2, len(s2), cap(s2));
	del(s2);
	print("s2.empty(): %s", s2.empty() ? "OK" : "FAIL");
	print("operator!(): %s", (!s2) ? "OK" : "FAIL");
//	print("compare to None: %s", (s2 == None) ? "OK" : "FAIL");
	print("compare to empty: %s", (s2 == String()) ? "OK" : "FAIL");

	String line = "";
	print("empty string: \"%v\"", &line);
//	line = None;
	del(line);
	print("null string: \"%v\"", &line);

	return 0;
}

// EOB
