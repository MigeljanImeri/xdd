#!/usr/bin/env bash
#
# Acceptance test for XDD.
#
# Validate the funtionality of -runtime option by reading test file forever untill terminated by -runtime
#
# Description - terminates XDD after a given amount of seconds have passed
#
# Source the test configuration environment
#
source ../test_config
source ../common.sh

# Perform pre-test
initialize_test
test_dir=$XDDTEST_LOCAL_MOUNT/$TESTNAME

# ReqSize 4096, Bytes 1GB, Targets 1, QueueDepth 4, Passes 1
data_file=$test_dir/data1
# write a file
$XDDTEST_XDD_EXE -op write -reqsize 4096 -mbytes    1024 -targets 1 $data_file -qd 4                -passes 1 -datapattern random
# now read forever, small random I/O  with a runtime
$XDDTEST_XDD_EXE -op read  -reqsize    1 -mbytes   16384 -targets 1 $data_file -qd 4 -runtime 10.0  -passes 1 -seek random -seek range 1024 &
pid=$!
ppid=$$
echo "xdd started, pid=$pid"
echo "sleep 30"
sleep 30

# Validate output
test_passes=1
pkill -P $ppid $pid
if [ $? -eq 0 ]; then
   test_passes=0
  echo "Had to kill $pid."
fi

# Perform post-test cleanup
# Output test result
if [ 1 -eq $test_passes ]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test 1
fi
