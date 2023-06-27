#!/usr/bin/env bash
#
# Acceptance test for XDD
#
# Description - waits n amount of seconds after each pass except the last one
#
# Validate -passdelay by checking if the run time is greater
#	 or equal to n-1 seconds times the amount of passes
#
# Source the test configuration environment
#
source ./test_config
source ./common.sh

# Pre-test set-up
initialize_test
test_dir=$XDDTEST_LOCAL_MOUNT/$TESTNAME

test_file=$test_dir/data1
touch $test_file

# Determine correct time and elapsed time

num_passes=5
pass_delay=2
correct_time=$((($num_passes-1)*$pass_delay))
xdd_cmd="$XDDTEST_XDD_EXE -target $test_file -op write -reqsize 1024 -numreqs 10 -passes $num_passes -passdelay $pass_delay"
timed_pass_output="$(2>&1 time -p $xdd_cmd)"
elapsed_time=$(echo "$timed_pass_output" |grep '^real' |awk '{print $2}')

# truncated elapsed_time
elapsed_time=$(echo $elapsed_time| cut -d '.' -f 1)

# Verify output
if [ "$elapsed_time" -ge "$correct_time" ]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test 1
fi
