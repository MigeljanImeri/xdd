#!/usr/bin/env bash
#
# Description - displays the target number during each output
#
# Verify -tgt by checking if target number displayed matches target number tested
#
# Source test environment
source ../test_config
source ../common.sh

# pre-test set-up
initialize_test
test_dir=$XDDTEST_LOCAL_MOUNT/$TESTNAME
mkdir -p $test_dir

test_file=$test_dir/data1
test_file2=$test_dir/data2
touch $test_file
touch $test_file2

$XDDTEST_XDD_EXE -targets 2 $test_file $test_file2 -op target 0 write -op target 1 write -reqsize 1024 -numreqs 512 -runtime 2 -hb 1 -hb tgt -hb output $test_dir/data3

# get target numbers
target=`grep -m1 tgt $test_dir/data3.T0000.csv`
target_num=$(echo $target |cut -f 4 -d ',' | cut -c 4)

target2=`grep -m1 tgt $test_dir/data3.T0001.csv`
target_num2=$(echo $target2 |cut -f 4 -d ',' | cut -c 4)

# Verify output
if [ $target_num -eq 0 ] && [ $target_num2 -eq 1 ]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test 1
fi
