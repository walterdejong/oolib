/*
	testSet.cpp	WJ114
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
	Set<int> a = { 1, 2, 3 };
	Set<int> b = { 2, 4, 6 };

	print("a: %v", &a);
	print("b: %v", &b);

	print("a ismember 3: %s", a.ismember(3) ? "OK" : "FAIL");

	Set<int> c;
	c = a | b;
	print("union: %v", &c);

	c = a & b;
	print("intersection: %v", &c);

	c = a - b;
	print("difference: %v", &c);

	c = a ^ b;
	print("symmetric difference: %v", &c);

	c = { 1, 2, 3 };
	print("eq: %s", (a == c) ? "OK" : "FAIL");
	c |= b;
	print("lt: %s", (a < c) ? "OK" : "FAIL");
	print("le: %s", (a <= c) ? "OK" : "FAIL");

	c = { 4, 5, 6 };
	print("disjoint: %s", a.isdisjoint(c) ? "OK" : "FAIL");

	a.remove(2);
	print("a.remove(2): %v", &a);

	while(len(b) > 0) {
		print("b.pop(): %d", b.pop());
	}

	del(a);
	print("del(a): %v len: %zu", &a, len(a));
	print("a.empty(): %s", a.empty() ? "OK" : "FAIL");
	print("operator!(): %s", (!a) ? "OK" : "FAIL");

	return 0;
}

// EOB
