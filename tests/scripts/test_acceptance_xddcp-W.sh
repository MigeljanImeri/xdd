#!/bin/sh -x
#
# Acceptance test for XDD.
#
# Validate the post analysis with kernel tracing flag with xddcp for the -W flag
#

#
# Source the test configuration environment
#
source ./test_config

# Perform pre-test 
echo "Beginning XDDCP Post Analysis w Kernel Tracing Test 1 . . ."
test_dir=$XDDTEST_SOURCE_MOUNT/postanalysis-W
rm -rf $test_dir
mkdir -p $test_dir
ssh $XDDTEST_E2E_DEST "rm -rf $XDDTEST_DEST_MOUNT/postanalysis-W"
ssh $XDDTEST_E2E_DEST "mkdir -p $XDDTEST_DEST_MOUNT/postanalysis-W"

source_file=$test_dir/file1
dest_file=$XDDTEST_DEST_MOUNT/postanalysis-W/file1

#
# Create the source file
#
$XDDTEST_XDD_EXE -target $source_file -op write -reqsize 4096 -mbytes 4096 -qd 4 -datapattern random >/dev/null

#
# Start a long copy
#
export PATH=$(dirname $XDDTEST_XDD_EXE):/usr/bin:$PATH
bash -x $XDDTEST_XDDCP_EXE -W 5 $source_file $XDDTEST_E2E_DEST:$dest_file &
pid=$!

wait $pid
rc=$?

test_passes=0
if [ 0 -eq $rc ]; then

    #
    # Check for existence of post analysis files with kernel tracing
    #
    test_passes=1
    xfer_files=$(ls -1 $(hostname -s)*/xfer* | wc -l)
    if [ "$xfer_files" != "18" ]; then
	test_passes=0
	echo "ERROR: Failure in Post Analysis with Kernel Tracing Test 1"
	echo "\tNumber of xfer* files is: $xfer_files, should be 18"
    fi
    xfer_files=$(ls -1 $(hostname -s)*/xfer*eps | wc -l)
    if [ "$xfer_files" != "15" ]; then
	test_passes=0
	echo "ERROR: Failure in Post Analysis with Kernel Tracing Test 1"
	echo "\tNumber of xfer*eps files is: $xfer_files, should be 15"
    fi
    xfer_files=$(ls -1 $(hostname -s)*/xferk*eps | wc -l)
    if [ "$xfer_files" != "5" ]; then
	test_passes=0
	echo "ERROR: Failure in Post Analysis with Kernel Tracing Test 1"
	echo "\tNumber of xferk*eps files is: $xfer_files, should be 5"
    fi
    xfer_files=$(ls -1 $(hostname -s)*/xferd*eps | wc -l)
    if [ "$xfer_files" != "5" ]; then
	test_passes=0
	echo "ERROR: Failure in Post Analysis with Kernel Tracing Test 1"
	echo "\tNumber of xferd*eps files is: $xfer_files, should be 5"
    fi
else
    echo "ERROR: XDDCP exited with: $rc"
fi

# Perform post-test cleanup
#rm -rf $test_dir

# Output test result
if [ "1" == "$test_passes" ]; then
  echo "Acceptance XDDCP-W: XDDCP Post Analysis w Kernel Tracing - Check: PASSED."
  exit 0
else
  echo "Acceptance XDDCP-W: XDDCP Post Analysis w Kernel Tracing - Check: FAILED."
  exit 1
fi