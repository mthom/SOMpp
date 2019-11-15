SOM++ - The Simple Object Machine implemented in C++
====================================================

Introduction
------------

SOM is a minimal Smalltalk dialect used to teach VM construction at the [Hasso
Plattner Institute][SOM]. It was originally built at the University of Århus
(Denmark) where it was also used for teaching.

Currently, implementations exist for Java (SOM), C (CSOM), C++ (SOM++), Python
(PySOM), RPython (RPySOM), the Truffle framework (TruffleSOM), and
Squeak/Pharo Smalltalk (AweSOM).

A simple SOM Hello World looks like:

```Smalltalk
Hello = (
  run = (
    'Hello World!' println.
  )
)
```

This repository contains the C++ implementation of SOM, including an
implementation of the SOM standard library and a number of examples. Please see
the [main project page][SOMst] for links to the VM implementations.

The code in this branch relies on the libjitbuilder.a.  Since this is a binary file
I have not included here.

To test you can download the Docker image available here:
https://ibm.app.box.com/v/oscon2016-omr-sompp

Then you need to load the image into Docker with the following command:
docker load -i oscon_omr_sompp.tgz

Run the docker image:
docker run --rm -it omr/sompp:latest /bin/bash

The SOM++ code can be found at:
/home/omr/SOMpp

SOM++ can be built with CMake:

    $ mkdir cmake_build
    $ cd cmake_build/
    $ cmake -DCMAKE_BUILD_TYPE=Debug ..
    $ make

Afterwards, the tests can be executed with:

    $ ./SOM++ -cp Smalltalk TestSuite/TestHarness.som
   
A simple Hello World program is executed with:

    $ ./SOM++ -cp Smalltalk Examples/Hello/Hello.som

**Note**: On Linux, the library search path needs to be adapted:

    $ export LD_LIBRARY_PATH=.

The debug version of CSOM can be built using the `debug` target:

    $ make debug

Information on previous authors are included in the AUTHORS file. This code is
distributed under the MIT License. Please see the LICENSE file for details.
Additional documentation, detailing for instance the object model and how to
implement primitives, is available in the `doc` folder. The omrglue folder
is dual licensed under EPL V1.0 and AL V2.0.

Advanced Compilation Settings
-----------------------------

SOM++ supports different garbage collectors, including a basic mark/sweep, and
a generational GC. Furthermore, it implements different variants for integer
handling.


Tagged integers:

    default: off
    option name: TAGGING
    example: make TAGGING=true

Integer caching:

    default: off
    option name: INT_CACHE
    example: make INT_CACHE=true

Build Status
------------

Thanks to Travis CI, all commits of this repository are tested.
The current build status is: [![Build Status](https://travis-ci.org/SOM-st/SOMpp.png?branch=master)](https://travis-ci.org/SOM-st/SOMpp/)

 [SOM]: http://www.hpi.uni-potsdam.de/hirschfeld/projects/som/
 [SOMst]: https://travis-ci.org/SOM-st/

Status of AOT/JIT Compilation
-----------------------------

This fork is an extension of Charlie Gracie's 2016 fork, which added a JIT
compiler written using [Eclipse OMR](https://github.com/eclipse/omr)'s JitBuilder
technology. It uses the JIT compiler to compile SOM++ code ahead-of-time, which it
persists and stores to a customized shared cache, derived from OpenJ9's shared
class cache. Subsequent runs of SOM++ load compiled code from the cache, when available.
Currently, the JIT compiler does not interoperate with the AOT compiler.
In the future, this may change.
