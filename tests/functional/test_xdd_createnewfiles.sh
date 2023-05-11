#!/bin/sh
#
# Acceptance test for XDD.
#
# Validate the output results of -createnewfiles on 1GB files
#
# Description - creates target file for each pass in an XDD run
#
# Source the test configuration environment
#
source ./test_config
source ./common.sh


# Create the test location
initialize_test
data_file=$XDDTEST_LOCAL_MOUNT/$TESTNAME/test

# ReqSize 4096, Bytes 1GiB, Targets 1, QueueDepth 4, Passes 4
$XDDTEST_XDD_EXE -op write -reqsize 4096 -mbytes 1024 -targets 1 $data_file -qd 4 -createnewfiles -passes 4 -datapattern random 

# Validate output
test_passes=1
correct_size=1073741824

data_files="$data_file.00000001 $data_file.00000002 $data_file.00000003 $data_file.00000004"
for f in $data_files; do 
  file_size=$(stat -c %s $f)
  if [ "$correct_size" != "$file_size" ]; then
    test_passes=0
    echo "Incorrect file size for $f.  Size is $file_size but should be $correct_size."
  fi
done

# Output test result
if [ "1" == "$test_passes" ]; then
  # test passed 
  finalize_test 0
else
  # test failed  
  finalize_test 1
fi
