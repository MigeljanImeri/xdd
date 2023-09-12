#!/usr/bin/env bash
#
# Acceptance test for XDD
#
# Description - waits n amount of seconds before starting each pass
# Validate -startdelay by checking if run time is greater or equal to n seconds times the amount of passes
#
# Source the test configuration environment
#
source ../test_config
source ../common.sh

# Pre-test set-up
initialize_test
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"

test_file="${test_dir}/data1"
touch "${test_file}"

# Determine correct and elapsed time
num_passes=2
start_delay=4
correct_time=$((num_passes*start_delay))
xdd_cmd="${XDDTEST_XDD_EXE} -target ${test_file} -op write -reqsize 1024 -numreqs 10 -passes ${num_passes} -startdelay ${start_delay}"
# shellcheck disable=SC2086
timed_pass_output="$(2>&1 ${TIME_CMD} -p ${xdd_cmd})"
elapsed_time=$(echo "${timed_pass_output}" | grep '^real' | awk '{print $2}')

# Truncate elapsed_time
elapsed_time=$(echo "${elapsed_time}" | cut -d '.' -f 1)

# Verify output
if [[ "${elapsed_time}" -ge "${correct_time}" ]]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test 1 "$elapsed_time < $correct_time using -startdelay $start_delay -passes $num_passes"
fi
