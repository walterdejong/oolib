### oolib
Copyright 2014 by Walter de Jong <walter@heiho.net>

oolib is a library that aims to make developing in C++ easier by providing
some standard things like a String, an Array, a Dict (dictionary).
Although internally oolib uses STL and the C++ std library, you should be
able to do without and just use oolib.

The interface is based upon Python's and I borrowed some feats of Go (golang)
like go() and a channel type.


oolib is still very much beta software and not very well documented. If you
want to play around with it, check out the test codes.


oolib is licensed under the BSD 2-clause license, also known as the
Simplified BSD License.


### Building & Installing

* oolib depends on autoconf
* oolib depends on libtool
* oolib depends on C++11
* oolib depends on pcre (install `pcre`, `libpcre` or `libpcre-devel`)

```bash
# prepare configuration
$ autoreconf -vfi
# configuring
$ ./configure
# linux: ./configure LIBS=-lrt
# use clang: ./configure CXX=clang++
# installing
$ sudo make install
# uninstalling
$ sudo make uninstall
```

### Running Tests

*NOTE: requires oolib to be installed*

```bash
# build test
$ cd test
$ make testXXX
# run test
$ ./testXXX
```


### Using oolib with your project

This example shows how to compile and link your program with oolib using g++/clang.

Assuming you have oolib installed in: `/usr/local/lib`, and the header files are contained in: `/usr/local/include`. To target an alternate location change -I and -L flags accordingly.

**main.cpp** example 'file'

```cpp
#include <oolib>
using namespace oo;
int main(){
  print("I'm using oolib, awesome!");
  return 0;
}
```

**Compiling**

* Replace clang++ with g++ to use that compiler instead.

```bash
# compile with c++11, error reporting all, dwarf2 debug info (optional)
$ clang++ -Wall -std=c++11 -gdwarf-2 -fPIC -c main.cpp
```

**Linking**

```bash
# link with the installed oolib
$ clang++ main.o -o main -loo
```

**Execute**

```bash
# add execution permission
$ chmod +x ./main
# execute
$ ./main
# expected output:
I'm using oolib, awesome!
```

