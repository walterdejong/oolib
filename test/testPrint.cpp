/*
	testPrint.cpp	WJ112
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
	printn("This is a test ...");
	print(" OK");

	print("Number %d !!", 10);
	print("On '%10d' lots of width", 10);
	print("On '%*d' lots of width", 10, 20);
	print("On '%*d' lots of width", -10, 30);

	print("| %-10s | %10s |", "aa", "bb");

	print("floating point: %04.4f", 10.123456f);

	print("hex: %x     HEX: %X    octal: %o", 0x12ac, 0x12ac, 0755);

	size_t n = 3;
	print("size_t: %zu", n);

	String s = "the quick brown 狐 jumps over the lazy 犬";
	print("String s: [%q] [%v]", &s, &s);

	Array<int> a;
	a.push(1);
	a.push(2);
	a.push(3);
	print("Array a: %v", &a);

	Array<String> b;
	b.push("hello, world");
	b.push("こんにちは ワールド");
	b.push(s);
	print("Array b: %v", &b);

	Dict<int> d;
	d["aap"] = 1;
	d["noot"] = 2;
	d["mies"] = 3;
	print("Dict d: %v", &d);

	print("boolean: %t, %t", true, false);
	print("unicode 'A': %U", 'A');
	print("unicode U+3069: %U", 0x3061);
	print("unicode 'A': %r", 'A');
	print("unicode U+3069: %r", 0x3061);

	print("the percent sign: %%");
	return 0;
}

// EOB
