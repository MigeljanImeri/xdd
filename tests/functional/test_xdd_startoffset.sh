#!/usr/bin/env bash
#
# Acceptance test for XDD
#
# Description - starts block transfer at n number of blocks from block 0
#
# Validate -startoffset by comparing the size of a file starting from block 0 to its size starting from the nth block
#
# Get absolute path to script
SCRIPT=${BASH_SOURCE[0]}
SCRIPTPATH=$(dirname "${SCRIPT}")

# Source the test configuration environment
source "${SCRIPTPATH}"/../test_config
source "${SCRIPTPATH}"/../common.sh 

# Pre-test set-up
initialize_test
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"

test_file="${test_dir}/data1"
touch "${test_file}"

# Numreqs and startoffset have a ratio to each other
block_size=1024
start_offset=5
num_reqs=1
"${XDDTEST_XDD_EXE}" -target "${test_file}" -op write -blocksize "${block_size}" -numreqs "${num_reqs}" -startoffset "${start_offset}"

# Determine file requested transfer size and actual size
actual_size=$(stat -c '%s' "${test_file}")
calc_actual_size=$((actual_size/block_size))
expected_size=$((start_offset+num_reqs))

# Verify results
if [[ "${calc_actual_size}" -eq "${expected_size}" ]]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test 1 "File size ${calc_actual_size} != ${expected_size} when doing -startoffset ${start_offset} -blocksize ${block_size} -numreqs $num_reqs"
fi
