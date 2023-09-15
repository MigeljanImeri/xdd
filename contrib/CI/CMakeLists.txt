# enable make shellcheck
find_program(SHELLCHECK shellcheck)
if (SHELLCHECK)
  message(STATUS "Shellcheck found. Run 'make shellcheck' to check for shell script issues")
  configure_file(shellcheck.sh shellcheck.sh COPYONLY)
  add_custom_target(shellcheck "${CMAKE_CURRENT_BINARY_DIR}/shellcheck.sh" "${CMAKE_BINARY_DIR}")
endif()
