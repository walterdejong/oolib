//
//	testNotify.cpp	WJ112
//	
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


const char *Event1 = "This is event #1";
const char *Event2 = "This is event #2";


class MyObserver : public Observer {
public:
	MyObserver(String s) : name(s) { }

	void notify(const char *event) {
		print("observer %v: %s", &name, event);

		if (event == Event1)
			print("%v: Whee!", &name);

		else if (event == Event2)
			print("%v: Whoo!", &name);
	}

private:
	String name;
};


int main(int argc, char *argv[]) {
	MyObserver o("o1");
	add_observer(o, Event1);
	add_observer(o, Event2);

	MyObserver o2("o2");
	add_observer(o2, Event2);

	notify(Event1);
	notify(Event2);

	remove_observer(o, Event1);

	notify(Event1);	// not seen; there are no observers
	notify(Event2);

	remove_observer(o);

	notify(Event1);	// not seen; there are no observers
	notify(Event2);	// not seen; there are no observers

	return 0;
}

// EOB
