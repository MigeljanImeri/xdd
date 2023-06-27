#!/usr/bin/env bash
#
# Description - synchronizes write ops after each pass, flushing all data to the physical media
#
# Test -syncwrite by checking if number of passes equals the number of fdatasyncs
#
# Source the test configuration environment
#
source ./test_config
source ./common.sh

#skip test on non-Linux platforms
if [ `uname` != "Linux" ]; then
  # test passed
  finalize_test -1
fi

# Pre-test set-up
initialize_test
test_dir=$XDDTEST_LOCAL_MOUNT/$TESTNAME

test_file=$test_dir/data1
touch $test_file

num_passes=10

# get the amount of fdatasyncs
xdd_cmd="$XDDTEST_XDD_EXE -target $test_file -op write -numreqs 10 -passes $num_passes -syncwrite"
sys_call=$(2>&1 strace -cfq -e trace=fdatasync $xdd_cmd |grep "fdatasync")
sync_num=$(echo $sys_call |cut -f 4 -d ' ')

# Verify output
if [ $sync_num -eq $num_passes ]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test 1
fi
