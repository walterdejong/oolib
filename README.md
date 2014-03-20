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

* oolib depends on C++11
* oolib depends on pcre (install `pcre`, `libpcre` or `libpcre-devel`)

```bash
# build src
$ cd src/
$ sudo make install
# uninstalling
$ sudo make uninstall
```

### Running Tests

```bash
# build src and test
$ cd src/
$ make test
$ cd ../test
# run test
$ ./testXXX
```

