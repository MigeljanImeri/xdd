configure_file(common.sh common.sh)

set(FUNCTIONAL
    test_xdd_createnewfiles2.sh
    test_xdd_heartbeat_byte.sh
    test_xdd_heartbeat_elapsed.sh
    test_xdd_heartbeat_lf.sh
    test_xdd_heartbeat_output.sh
    test_xdd_heartbeat_target.sh
    test_xdd_heartbeat_tod.sh
    test_xdd_lockstep1.sh
    test_xdd_lockstep2.sh
    test_xdd_passdelay.sh
    test_xdd_pretruncate.sh
    test_xdd_reopen.sh
    test_xdd_runtime.sh
    test_xdd_startdelay.sh
    test_xdd_startoffset.sh
    test_xdd_syncwrite.sh
    test_xdd_timelimit.sh
    )

# original tests depended on xdd being installed
option(TEST_INSTALLED "Use installed xdd for tests" ON)
if (TEST_INSTALLED)
  set(XDD_EXEC "$<TARGET_FILE:xdd>")
else()
  set(XDD_EXEC "${CMAKE_BINARY_DIR}/bin/xdd")
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
XDDTEST_OUTPUT_DIR=${TESTDIR}/logs"
)
foreach(TEST ${FUNCTIONAL})
  configure_file("${TEST}" "${TEST}" COPYONLY)
  add_test(NAME "${TEST}"
    COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/${TEST} 2> ${TESTDIR}/logs/${TEST}.log"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  set_tests_properties("${TEST}" PROPERTIES LABELS "functional")
  set_tests_properties("${TEST}" PROPERTIES SKIP_RETURN_CODE 255)
endforeach()
