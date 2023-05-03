#!/bin/bash
#
# Acceptance test for XDD
#
# Description - starts block transfer at n number of blocks from block 0
#
# Validate -startoffset by comparing the size of a file starting from block 0 to its size starting from the nth block
#
# Source the test configuration environment
source ./test_config

# Pre-test set-up
test_name=$(basename -s sh $0)
test_name="${test_name%.*}"
test_dir=$XDDTEST_LOCAL_MOUNT/$test_name

# Make test directory and file
mkdir -p $test_dir

test_file=$test_dir/data1
touch $test_file

#ratio of req_size to start_offset must be 2:1
req_size=10
start_offset=5

$XDDTEST_XDD_EXE -target $test_file -op write -reqsize $req_size -numreqs 1 -startoffset $start_offset 


# Determine file requested transfer size and actual size
transfer_size=$(($req_size*1024))

actual_size=$(stat -c %s $test_file)

calc_actual_size=$(($actual_size-$transfer_size))

# Post test cleanup 
rm -r $test_dir

# Verify results
echo -n "Acceptance Test - $test_name : "
if [ $calc_actual_size -eq $(($transfer_size/2)) ]; then
	echo "PASSED"
	exit 0
else
	echo "FAILED"
	exit 1
fi
