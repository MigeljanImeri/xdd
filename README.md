# XDD

## Dependencies
- Linux
- CMake 3.12+
- A C Compiler
- Python 3+
- libnuma (optional)
- libverbs (optional)

## Build
```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

By default, XDD is installed in `/usr/bin`. In order to change the
installation path, use `-DCMAKE_INSTALL_PREFIX=<new path>` during the
configure step.

### numactl
If CMake discovers libnuma, it will be enabled as a
feature in XDD. It can be turned off with `-DWITH_NUMA=OFF`.

### InfiniBand Verbs
If CMake discovers libibverbs, it will be enabled as a
feature in XDD. It can be turned off with `-DWITH_VERBS=OFF`.

### FlameGraphs
XDD will be built for use with
[FlameGraph](https://github.com/brendangregg/FlameGraph) by compiling
with `-fno-omit-frame-pointer`. This can be disabled with
`-DWITH_FLAMEGRAPH=Off`.

## Qdepth/Thread count advisory
In XDD, the number of threads is often governed by the queue depth option
or the thread count option.  The number of threads than may be created is
system dependent.  For example, on my laptop I can only create 916 threads
before the system returns a "resource busy or unavailable" error. It's
not just a simple memory limitation necessarily either (i.e. reducing the
pthread stack size doesn't automatically increase the number of threads you
can create). In general, the number of threads that can be initialized
by XDD is limited by the system rather than XDD.  This is mainly interesting
when using fork-thread type access patterns.
