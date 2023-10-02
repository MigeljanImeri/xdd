#!/usr/bin/env bash
#
# Source the test configuration environment
#
# Description: outputs the elapsed time since the start of run
# Verify heartbeat elapsed time options works
#
# Get absolute path to script
SCRIPT=${BASH_SOURCE[0]}
SCRIPTPATH=$(dirname "${SCRIPT}")

# Source the test configuration environment
source "${SCRIPTPATH}"/../test_config
source "${SCRIPTPATH}"/../common.sh 

# Pre-test set-up
initialize_test
test_file="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}/test"
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"

test_file="${test_dir}/data1"
touch "${test_file}"

data_loc="${test_dir}/data2.T0000.csv"
delay_time=3
passes=3
theoretical_seconds="$((delay_time * passes))"

"${XDDTEST_XDD_EXE}" -target "${test_file}" -op write -passes "${passes}" -reqsize 1 -numreqs 1 -startdelay "${delay_time}" -hb elapsed -hb output "${test_dir}/data2" -hb lf

actual_seconds=$(wc -l < "${data_loc}")
error="$((actual_seconds-theoretical_seconds))"
abs_error="${error#-}"
percent_error=$(echo "scale=2 ; ${abs_error} / ${theoretical_seconds} * 100" | bc)
error_bound=20

pass_count=0
if (( $(echo "${percent_error} < ${error_bound}" | bc -l) )); then
  pass_count=1
fi

if [[ "${pass_count}" -eq "1" ]]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test 1 "${percent_error} >= ${error_bound} which means there exists an issue with -heartbeat (-hb) elapsed output"
fi
