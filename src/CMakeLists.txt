# place binaries here instead of src
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include_directories(
  base bx client common compat fs net public xnet xni
  "${CMAKE_CURRENT_BINARY_DIR}/compat" # oonfig.h
)

# copied from configure.ac
# removed __INTEL__, _FILE_OFFSET_BITS=64, LINUX, and SG_IO
add_compile_definitions(_GNU_SOURCE)            # should probably move this to individual files that need it
add_compile_definitions(PACKAGE_STRING="${PACKAGE_STRING}")
add_compile_definitions(PACKAGE_VERSION="${PACKAGE_VERSION}")

add_subdirectory(base)
add_subdirectory(bx)
add_subdirectory(client)
add_subdirectory(common)
add_subdirectory(compat)
add_subdirectory(fs)
add_subdirectory(net)
add_subdirectory(public)
add_subdirectory(xnet)
add_subdirectory(xni)

add_library(xdd-lib
  $<TARGET_OBJECTS:base>
  $<TARGET_OBJECTS:bx>
  $<TARGET_OBJECTS:client>
  $<TARGET_OBJECTS:common>
  # $<TARGET_OBJECTS:compat>
  $<TARGET_OBJECTS:fs>
  $<TARGET_OBJECTS:net>
  $<TARGET_OBJECTS:public>
  $<TARGET_OBJECTS:xnet>
  $<TARGET_OBJECTS:xni>
)

target_link_libraries(xdd-lib Threads::Threads)
set_target_properties(xdd-lib PROPERTIES OUTPUT_NAME "xdd")

if (IBVERBS AND IBV_GET_DEVICE_LIST AND WITH_VERBS)
  target_link_libraries(xdd-lib ibverbs)
endif()

if (NUMA_FOUND AND WITH_NUMA)
  target_link_libraries(xdd-lib ${NUMA_LDFLAGS})
endif()

install(TARGETS xdd-lib LIBRARY)

add_executable(xdd $<TARGET_OBJECTS:xdd-client>)
target_link_libraries(xdd PUBLIC xdd-lib)

install(TARGETS xdd RUNTIME)
