#!/usr/bin/env bash
#
# Description - prints a new line after each heartbeat update
#
# Verifty -hb lf by checking if output is printed with new lines
#
# Get absolute path to script
SCRIPT=${BASH_SOURCE[0]}
SCRIPTPATH=$(dirname "${SCRIPT}")

# Source the test configuration environment
source "${SCRIPTPATH}"/../test_config
source "${SCRIPTPATH}"/../common.sh 

# pre-test set-up
initialize_test
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"
test_file="${test_dir}/data1"
touch "${test_file}"


run_time=6
lines="$((run_time-1))"
"${XDDTEST_XDD_EXE}" -target "${test_file}" -reqsize 1024 -numreqs 1024 -runtime "${run_time}" -hb lf -hb output "${test_dir}/data2"

# get number of lines printed
head "-${lines}" "${test_dir}/data2.T0000.csv" >> "${test_dir}/data3"
match=$(echo "${lines}" | cut -f 1 -d ' ')

# verify output
if [[ "${match}" -eq "${lines}" ]]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test 1 "$match != $lines so their is an issue -heartbeat (-hb) lf"
fi
