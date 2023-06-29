#!/usr/bin/env bash
#
# Acceptance test for XDD
#
# Description - Truncate the file to a given length before doing I/O
#
# Validate -pretruncate by checking if the test file size is equal to the
#  truncation size
#
# Source the test configuration environment
#
source ../test_config
source ../common.sh

# Pre-test create test directory and file
initialize_test
test_dir=$XDDTEST_LOCAL_MOUNT/$TESTNAME
mkdir -p $test_dir

test_file=$test_dir/data1

# Preallocate test file
pretruncate_blocks=4096
req_size=1
block_size=1024
$XDDTEST_XDD_EXE -target $test_file -op write -reqsize $req_size -numreqs 1 -bs $block_size -pretruncate $pretruncate_blocks

pretruncate_size=$(($pretruncate_blocks * $block_size))
file_size=$(stat -c %s $test_file)

# Verify output
if [ "$file_size" -eq "$pretruncate_size" ]; then
  # test passed
  finalize_test 0
else
  # test failed
  echo "File size is $file_size, but pretruncate size was $pretruncate_size"
  finalize_test 1
fi
