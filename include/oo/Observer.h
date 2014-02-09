//
//	ooObserver.h	WJ112
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

#ifndef OOOBSERVER_H_WJ112
#define OOOBSERVER_H_WJ112

#include "oo/Dict.h"

#include <stdlib.h>

namespace oo {

class Observer {
public:
	virtual void notify(const char *) = 0;
};


class NotificationCenter {
public:
	~NotificationCenter() { all_observers = None; }

	void add_observer(Observer&, const char *);
	void remove_observer(Observer&, const char * = nullptr);
	void notify(const char *);

private:
	// dict[event] => array of Observers
	Dict< Array<Observer *> > all_observers;
};

extern NotificationCenter defaultNotificationCenter;

void add_observer(Observer&, const char *);
void remove_observer(Observer&, const char * = nullptr);
void notify(const char *);

}	// namespace oo

#endif	// OOOBSERVER_H_WJ112

// EOB
