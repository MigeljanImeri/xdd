set(DEBUG_TESTS
    test_xdd_debug_init.sh
    )

foreach(TEST ${DEBUG_TESTS})
  configure_file("${TEST}" "${TEST}" COPYONLY)
  add_test(NAME "${TEST}"
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  set_tests_properties("${TEST}" PROPERTIES LABELS "debug")
  set_tests_properties("${TEST}" PROPERTIES SKIP_RETURN_CODE 255)
endforeach()
