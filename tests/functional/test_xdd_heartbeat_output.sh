#!/usr/bin/env bash
#
# Description - sends all heartbeat updates to specified file
#
# Verify -hb output by checking if specified file exists
#
# Source test environment
source ../test_config
source ../common.sh

# Pre-test set-up
initialize_test
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"

test_file="${test_dir}/data1"
touch "${test_file}"

# -hb should output to file data2.T0000.csv
file_name=data2.T0000.csv

"${XDDTEST_XDD_EXE}" -target "${test_file}" -op write -reqsize 512 -numreqs 512 -hb 1 -hb output "${test_dir}/data2"

#determine whether file exists
f_file=$(find "${test_dir}" -name "${file_name}")
# shellcheck disable=SC2116
f_file=$(echo "${f_file##/*/}")

if [[ "${f_file}" == "${file_name}" ]]; then
    test_success=1
else
    test_success=0
fi


# Verify output
if [[ "${test_success}" -ge "1" ]]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test 1 "$f_file != $file_name so there is an issue with -heartbeat (-hb) output as the file $file_name does not exist"
fi
