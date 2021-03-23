# aleakd
Another leak detector

ALeakD is leak detector and memory analyser. It's composed with a libary that override memory allocation/deallocation and thread creation, which send message to a graphical server application that records all operation of the program.

By this way, you can debug memory leak without closing your application and check which thread is given leaks.

Features
--------

- Communication mode: TCP, UDP, Named pipe (recommended).
- Only compatible with POSIX systems
- View real time per thread usage and memory peak
- Statistics for each: malloc, calloc, realloc, free, posix_memalign, aligned_alloc, memalign, valloc, pvalloc
- Use filter in the GUI to find a memory operation
- Colored view for each memory allocation to view directly the state (free or not free)

Build
--------

cmake -DCMAKE_BUILD_TYPE=Debug .
make

Usage
--------

Start the server:
./aleakd-server

Start the program you want to check:
LD_PRELOAD=./libaleak.so myprogram
