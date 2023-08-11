configure_file(common.sh common.sh)

# original tests depended on xdd being installed
option(TEST_INSTALLED "Use installed xdd for tests" ON)
if (TEST_INSTALLED)
  set(XDD_EXEC "$<TARGET_FILE:xdd>")
else()
  set(XDD_EXEC "${CMAKE_BINARY_DIR}/bin/xdd")
endif()

# certain tests rely on the time command being installed
message(CHECK_START "Looking for /usr/bin/time")
find_program(Time_EXECUTABLE NAMES time)
if (NOT Time_EXECUTABLE)
  message(CHECK_FAIL "not found: ctest will skip tests test_xdd_{passdelay/startdelay}")
else()
  message(CHECK_PASS "found")
endif() 

# configurable output directory
set(TESTDIR "${CMAKE_BINARY_DIR}/test-dir" CACHE PATH "Where to place test files")
add_custom_target(mk-test-dir ALL
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${TESTDIR}")

# not configurable
set(TESTDIR_TESTS "${TESTDIR}/tests")
add_custom_target(mk-test-tests ALL
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${TESTDIR_TESTS}"
  DEPENDS mk-test-dir)
set(TESTDIR_LOGS "${TESTDIR}/logs")
add_custom_target(mk-test-logs ALL
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${TESTDIR_LOGS}"
  DEPENDS mk-test-dir)

# generate config file
file(GENERATE
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_config"
  CONTENT "XDDTEST_XDD_EXE=${XDD_EXEC}
XDDTEST_LOCAL_MOUNT=${TESTDIR}/tests
XDDTEST_OUTPUT_DIR=${TESTDIR}/logs
TIME_CMD=${Time_EXECUTABLE}"
)

add_subdirectory(functional)
add_subdirectory(debug)
