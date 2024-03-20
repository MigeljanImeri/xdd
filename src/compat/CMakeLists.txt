set(COMPAT_SRC
  # xint_linux.h
  )

# add_library(compat OBJECT ${COMPAT_SRC} "${CONFIG_H}")

check_include_files(inttypes.h      HAVE_INTTYPES_H)
check_include_files(libgen.h        HAVE_LIBGEN_H)
check_include_files(linux/magic.h   HAVE_LINUX_MAGIC_H)
check_include_files(sys/mount.h     HAVE_SYS_MOUNT_H)
check_include_files(netinet/tcp.h   HAVE_NETINET_TCP_H)
check_include_files(gtest/gtest.h   HAVE_GTEST_GTEST_H)

list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
list(APPEND CMAKE_REQUIRED_LIBRARIES -pthread) # CMP0075, requires CMake 3.12

# functions
list(APPEND CMAKE_REQUIRED_FLAGS -Wno-pedantic)
check_symbol_exists(clock_gettime               "time.h"        HAVE_CLOCK_GETTIME)
check_symbol_exists(initstate                   "stdlib.h"      HAVE_INITSTATE)
check_symbol_exists(memset                      "string.h"      HAVE_MEMSET)
check_symbol_exists(nanosleep                   "time.h"        HAVE_NANOSLEEP)
check_symbol_exists(posix_memalign              "stdlib.h"      HAVE_POSIX_MEMALIGN)
check_symbol_exists(pread                       "unistd.h"      HAVE_PREAD)
check_symbol_exists(pthread_attr_setaffinity_np "pthread.h"     HAVE_PTHREAD_ATTR_SETAFFINITY_NP)
check_symbol_exists(pwrite                      "unistd.h"      HAVE_PWRITE)
check_symbol_exists(random                      "stdlib.h"      HAVE_RANDOM)
check_symbol_exists(rand                        "stdlib.h"      HAVE_RAND)
check_symbol_exists(sched_getcpu                "sched.h"       HAVE_SCHED_GETCPU)
check_symbol_exists(sched_setscheduler          "sched.h"       HAVE_SCHED_SETSCHEDULER)
list(REMOVE_ITEM CMAKE_REQUIRED_FLAGS -Wno-pedantic)

# features
list(APPEND CMAKE_REQUIRED_FLAGS -Wno-unused-variable)
check_symbol_exists(BLKGETSIZE64                "linux/fs.h"    HAVE_DECL_BLKGETSIZE64)
check_c_source_compiles("#include <sched.h>
int main() { cpu_set_t cpuset; return 0; }" HAVE_CPU_SET_T)
check_c_source_compiles("#include <pthread.h>
int main() { pthread_barrier_t pb; return 0; }" HAVE_PTHREAD_BARRIER_T)
check_c_source_compiles("#include <stddef.h>
int main() { size_t sz; return 0; }" HAVE_SIZE_T)
check_symbol_exists(TCP_CONGESTION              "netinet/tcp.h" HAVE_DECL_TPC_CONGESTION)
list(REMOVE_ITEM CMAKE_REQUIRED_FLAGS -Wno-unused-variable)

list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES -pthread)
list(REMOVE_ITEM CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)

# set up config.h
configure_file(config.h.in config.h)
