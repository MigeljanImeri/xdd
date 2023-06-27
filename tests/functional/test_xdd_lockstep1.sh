#!/usr/bin/env bash
#
# Acceptance test for XDD.
#
# Description - Do the simplest lockstep command possible
#
#
# Source the test configuration environment
#
source ./test_config
source ./common.sh

# Create the test location
initialize_test
test_dir=$XDDTEST_LOCAL_MOUNT/$TESTNAME

# A super simple lockstep
$XDDTEST_XDD_EXE -targets 2 $test_dir/foo $test_dir/foo -op target 0 write -op target 1 read \
  -reqsize 1024 -numreqs 10 -lockstep  0 1 op 1 op 1 wait complete -verbose

# Validate output
test_passes=0
correct_size=$((1024*1024*10))
file_size=$(stat -c %s $test_dir/foo)
if [ "$correct_size" == "$file_size" ]; then
    test_passes=1
else
    echo "Incorrect file size.  Size is $file_size but should be $correct_size."
fi

# Output test result
if [ "1" == "$test_passes" ]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test 1
fi
