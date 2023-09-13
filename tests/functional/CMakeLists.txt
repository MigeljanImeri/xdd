set(FUNCTIONAL
    test_xdd_createnewfiles2.sh
    test_xdd_dryrun.sh
    test_xdd_heartbeat_byte.sh
    test_xdd_heartbeat_elapsed.sh
    test_xdd_heartbeat_lf.sh
    test_xdd_heartbeat_output.sh
    test_xdd_heartbeat_target.sh
    test_xdd_heartbeat_tod.sh
    test_xdd_lockstep1.sh
    test_xdd_lockstep2.sh
    test_xdd_pretruncate.sh
    test_xdd_reopen.sh
    test_xdd_runtime.sh
    test_xdd_startoffset.sh
    test_xdd_syncwrite.sh
    test_xdd_timelimit.sh
    )

if (Time_EXECUTABLE)
  list(APPEND FUNCTIONAL
    test_xdd_passdelay.sh
    test_xdd_startdelay.sh)
endif()

foreach(TEST ${FUNCTIONAL})
  configure_file("${TEST}" "${TEST}" COPYONLY)
  add_test(NAME "${TEST}"
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  set_tests_properties("${TEST}" PROPERTIES LABELS "functional")
  set_tests_properties("${TEST}" PROPERTIES SKIP_RETURN_CODE 255)
endforeach()
