# aleakd
Another leak detector

ALeakD is leak detector and memory analyser. It's composed with a libary that override memory allocation/deallocation and thread creation, which send message to a graphical server application that records all operation of the program.

By this way, you can debug memory leak without closing your application and check which thread is given leaks.

License
-------

This program is licensed under the terms of the MIT License.

Requirements
------------

- CMake
- Qt version 5.11 or superior

Features
--------

- Communication mode: TCP, UDP, Named pipe (recommended).
- Only compatible with POSIX systems
- View real time per thread usage and memory peak
- Statistics for each: malloc, calloc, realloc, free, posix_memalign, aligned_alloc, memalign, valloc, pvalloc
- Use filter in the GUI to find a memory operation
- Colored view for each memory allocation to view directly the state (free or not free)
- Get backtrace for every memory operation
- View graphical memory usage

Build
--------

Install dependencies:
```
    apt-get install git cmake qtbase5-dev qttools5-dev qttools5-dev-tools libqt5charts5-dev
```

Edit the config.h file to modify some options.

Get the code:
```
    git clone https://github.com/Jet1oeil/aleakd.git && cd aleakd
```

Compile:
```
    cmake -DCMAKE_BUILD_TYPE=Debug .
    make
```

Run the server:
```
    ./aleakd-server
```

Run the the program to be analyzed:
```
    LD_PRELOAD=./libaleakd.so myprogram
```

Options
-------

You can use the following environnement vaiables:
```
ALEAKD_SERVER_HOST: host address to the aleakd server
ALEAKD_SERVER_PORT: port address to the aleakd server (default: 19000)
ALEAKD_BACKTRACE: enable backtrace message (default: 1). This can reduce the performance.
```


Screenshot
----------

![alt text](https://raw.githubusercontent.com/Jet1oeil/aleakd/master/doc/aleakd-screenshot-1.png)
![alt text](https://raw.githubusercontent.com/Jet1oeil/aleakd/master/doc/aleakd-screenshot-2.png)
![alt text](https://raw.githubusercontent.com/Jet1oeil/aleakd/master/doc/aleakd-screenshot-3.png)
![alt text](https://raw.githubusercontent.com/Jet1oeil/aleakd/master/doc/aleakd-screenshot-4.png)
