/*
	testDict.cpp	WJ112
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
	Dict<int> d;

	d["aap"] = 1;
	d["noot"] = 2;
	d["mies"] = 3;

	print("len(d): %zu", len(d));

	if (d.has_key("aap"))
		print("d['aap']: %d", d["aap"]);
	else
		print("FAIL; no key \"aap\"");

	if (d.has_key("jet"))
		print("FAIL; d['jet']: %d", d["jet"]);	// this would actually create an empty value for "jet"
	else
		print("no key \"jet\"");

	Dict<int> d3 = d;
	if (d3 == d)
		print("d and d3 are equal");
	else
		print("FAIL; d and d3 are not seen as equal");

	d["aap"] *= 10;
	if (d3 == d)
		print("FAIL; d and d3 are still seen as equal");
	else
		print("OK; d and d3 are no longer equal");

	Dict<int> d2 = d;
	print("del('aap'): %d", (int)d.del("aap"));
	print("del('jet'): %d", (int)d.del("jet"));
	print("len(d): %zu", len(d));
	print("len(d2): %zu", len(d2));
	print("len(d3): %zu", len(d3));

	d2.clear();
	print("clear(): len(d2): %zu", len(d2));

	print();
	print("dict: %q", &d3);
	print();

	Array<String> keys = d3.keys();
	foreach(i, keys)
		print("keys[%u]: %q", i, &keys[i]);
	print();

	Array<int> values = d3.values();
	foreach(i, values)
		print("values[%u]: %d", i, values[i]);
	print();

	print("C++11 range-based for-loop");
	for(auto kv : d3)
		print(" %v: %d", &kv.first, kv.second);
	print();

	d = None;
	print("assignment to None: len(%zu)", len(d));
	print("d.empty(): %s", d.empty() ? "OK" : "FAIL");

	if (!d)
		print("operator!(): OK");
	else
		print("FAIL: operator!();");

	if (d == None)
		print("compare to None: OK");
	else
		print("FAIL: compare to None");

	return 0;
}

// EOB
