/*
	testList.cpp	WJ112
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

int mul10(const int &n) {
	return n * 10;
}

bool val4(const int &n) {
	return (n == 4);
}

int reducefunc(const int& a, const int& b) {
	return a+b;
}

int main(void) {
	Array<int> t(8);
	for(size_t i = 0; i < len(t); i++)
		t[i] = i;
	print("TD t[] %d/%d %v", len(t), cap(t), &t);

	List<int> a = t;	// convert array to list

	print("len(a): %zu   cap(a): %zu", len(a), cap(a));
	print("a: %v", &a);

	List<int> l2 = { 0, 1, 2, 3, 4, 5, 6, 7 };	// initializer list
	print("len(l2): %zu   cap(l2): %zu", len(l2), cap(l2));
	print("l2: %v", &l2);

	print("\nb = a.slice(2, 5)");
	List<int> b = a.slice(2, 5);

	print("b: %v", &b);

	print("b = a;");
	b = a;

	print();
	print("b: %v", &b);

	print();
	int n;
	for(int i = 0; i < 4; i++) {
		n = 2 * i;
		a.append(n);
//		printf("a[i]: %d\n", a[i]);
	}
	print("len(a): %zu   cap(a): %zu", len(a), cap(a));

	print("a: %v", &a);
	print("len(a): %zu   cap(a): %zu", len(a), cap(a));

	n = 4;
	print("\nfind(4): %d", a.find(n));
	print("rfind(4): %d", a.rfind(n));

	print("a.insert(4, 18)");
	n = 18;
	a.insert(4, n);
	print("a: %v", &a);
	print("len(a): %zu   cap(a): %zu", len(a), cap(a));

	print("\na.sort();");
	a.sort();
	print("a: %v", &a);
	print("len(a): %zu   cap(a): %zu", len(a), cap(a));
	
	print("\na.bsearch();");
	n = 6;
	if (a.bsearch(n))
		print("bsearch(): true");
	else
		print("FAIL; bsearch(): false");

	// removes one value of '6' (there is another one in it)
	print("a.remove(6)");
	n = 6;
	a.remove(n);
	print("a: %v", &a);
	print("len(a): %zu   cap(a): %zu", len(a), cap(a));

	n = 4;
	print("\na contains %d values of 4", a.count(n));

	print("\na.rotate(-3)");
	a.rotate(-3);
	print("a: %v", &a);
	printf("len(a): %zu   cap(a): %zu", len(a), cap(a));

	print("\nb = apply(mul10, a)");
	b = apply(mul10, a);
	print("b: %v", &b);
	print("len(b): %zu   cap(b): %zu", len(b), cap(b));

	print("b = apply(lambda, a)");
	b = apply([](const int& x){ return 10 * x; }, a);
	print("b: %v", &b);
	print("len(b): %zu   cap(b): %zu", len(b), cap(b));

	print("\nb = filter(val4, a)");
	b = filter(val4, a);
	print("b: %v", &b);
	print("len(b): %zu   cap(b): %zu", len(b), cap(b));

	print("filter(lambda, 4)");
	b = filter([](const int& x) -> int { return x == 4; }, a);
	print("b: %v", &b);
	print("len(b): %zu   cap(b): %zu", len(b), cap(b));

	print("\nreduce(b) == %d", reduce(reducefunc, b));
	print("lambda reduce(b) == %d", reduce([](const int& x, const int& y) -> int { return x + y; }, b));

	print("\nb = a + b");
	b = a + b;
	print("b: %v", &b);
	print("len(b): %zu   cap(b): %zu", len(b), cap(b));

	print("\n");
	while(len(b) > 5)
		print("b.pop(5): %d", b.pop(5));

	print("len(b): %zu   cap(b): %zu", len(b), cap(b));

	print();
	print("a: %v", &a);
	print("len(a): %zu   cap(a): %zu", len(a), cap(a));

	print("\na.sort(); a.unique()");
	a.sort();
	a.unique();
	print("a: %v", &a);
	print("len(a): %zu   cap(a): %zu", len(a), cap(a));

	print("\nforeach test:");
	a[0] = 10;
	a[1] = 20;
	a[2] = 30;
	foreach(i, a) {
		print(" a[%d]: %d", i, a[i]);
	}
	print();

	print("C++11 range-based for test:");
	printn(" ");
	for(int v: a)
		printn("%d ", v);
	print();
	print();

	a = None;
	print("assignment to None: len(a): %zu   cap(a): %zu", len(a), cap(a));
	print("a.empty(): %s", a.empty() ? "OK" : "FAIL");

	if (!a)
		print("operator!(): OK");
	else
		print("FAIL: operator!();");

	if (a == None)
		print("compare to None: OK");
	else
		print("FAIL: compare to None");

	return 0;
}

// EOB
