#configure_file(common.sh common.sh)

set(DEBUG_TESTS
    test_xdd_debug_init.sh
    )

# original tests depended on xdd being installed
option(TEST_INSTALLED "Use installed xdd for tests" ON)
if (TEST_INSTALLED)
  set(XDD_EXEC "$<TARGET_FILE:xdd>")
else()
  set(XDD_EXEC "${CMAKE_BINARY_DIR}/bin/xdd")
endif()

# configurable output directory
#set(TESTDIR "${CMAKE_BINARY_DIR}/test-dir" CACHE PATH "Where to place test files")
#add_custom_target(mk-test-dir ALL
#  COMMAND "${CMAKE_COMMAND}" -E make_directory "${TESTDIR}")

# not configurable
#set(TESTDIR_TESTS "${TESTDIR}/tests")
#add_custom_target(mk-test-tests ALL
#  COMMAND "${CMAKE_COMMAND}" -E make_directory "${TESTDIR_TESTS}"
#  DEPENDS mk-test-dir)
#set(TESTDIR_LOGS "${TESTDIR}/logs")
#add_custom_target(mk-test-logs ALL
#  COMMAND "${CMAKE_COMMAND}" -E make_directory "${TESTDIR_LOGS}"
#  DEPENDS mk-test-dir)

# generate config file
#file(GENERATE
#  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_config"
#  CONTENT "XDDTEST_XDD_EXE=${XDD_EXEC}
#XDDTEST_LOCAL_MOUNT=${TESTDIR}/tests
#XDDTEST_OUTPUT_DIR=${TESTDIR}/logs"
#)
foreach(TEST ${DEBUG_TESTS})
  configure_file("${TEST}" "${TEST}" COPYONLY)
  add_test(NAME "${TEST}"
    COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/${TEST} 2> ${TESTDIR}/logs/${TEST}.log"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  set_tests_properties("${TEST}" PROPERTIES LABELS "debug")
  set_tests_properties("${TEST}" PROPERTIES SKIP_RETURN_CODE 255)
endforeach()
