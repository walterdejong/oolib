//
//	ooObserver.cpp	WJ112
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

#include "oo/Observer.h"

#include <stdlib.h>
#include <assert.h>

namespace oo {

// global var
NotificationCenter defaultNotificationCenter;


// register observer for a specific event
void NotificationCenter::add_observer(Observer& o, const char *event) {
	assert(event != nullptr);

	String key = event;
	Array<Observer *> a;

	if (this->all_observers.has_key(key)) {
		a = this->all_observers[key];
	}
	a.append(&o);
	this->all_observers[key] = a;
}

// default argument: event = nullptr
void NotificationCenter::remove_observer(Observer& o, const char *event) {
	String key;
	Array<Observer *> a;

	if (event == nullptr) {
		// remove all occurrences of o

		Array<String> keys = this->all_observers.keys();

		foreach(i, keys) {
			key = keys[i];
			a = this->all_observers[key];
			if (a.remove(&o)) {
				this->all_observers[key] = a;
			}
		}
	} else {
		// remove o only for event

		key = event;

		if (this->all_observers.has_key(key)) {
			a = this->all_observers[key];
			if (a.remove(&o)) {
				this->all_observers[key] = a;
			}
		}
	}
}

void NotificationCenter::notify(const char *event) {
	assert(event != nullptr);

	String key = event;

	if (this->all_observers.has_key(key)) {
		Array<Observer *> a = this->all_observers[key];

		// call notify event in all registered observers
		foreach(i, a) {
			a[i]->notify(event);
		}
	}
}

void add_observer(Observer& o, const char *event) {
	defaultNotificationCenter.add_observer(o, event);
}

// default argument: event = nullptr
void remove_observer(Observer& o, const char *event) {
	defaultNotificationCenter.remove_observer(o, event);
}

void notify(const char *event) {
	defaultNotificationCenter.notify(event);
}

}	// namespace

// EOB
