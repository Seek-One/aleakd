# libaleakd
Another leak detection library

aleakd use is a simple libray that override the malloc and free function to help to find some memory leaks problems.

The library is able to manage a per-thread counter of allocation.

Because it use a static allocation list. You have to configure the number of thread you want to check and the maximum number of allocation per thread you want to manage.
See the config.h file.

Build
--------

cmake -DCMAKE_BUILD_TYPE=Debug .
make

Usage
--------

LD_PRELOAD=./aleak.so myprogram
