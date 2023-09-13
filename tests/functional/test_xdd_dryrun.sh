#!/usr/bin/env bash
#
# Acceptance test for XDD.
#
# Validate the functionality of the -dryrun option to ensure it works as intended.
#
# Description - passes the dry run option to a simple xdd command 
#
# Source the test configuration environment
#
source ../test_config
source ../common.sh

# Perform pre-test
initialize_test
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"
log_file="$(get_log_file)"

# ReqSize 4096, Bytes 1MB, Targets 1, QueueDepth 4, Passes 1
data_file="${test_dir}/test"

# write a file
"${XDDTEST_XDD_EXE}" -op write -reqsize 4096 -mbytes 1 -targets 1 "${data_file}" -qd 4 -passes 1 -datapattern random

# now run dryrun
sleep_seconds=2
"${XDDTEST_XDD_EXE}" -op read  -reqsize 1 -targets 1 "${data_file}" -dryrun &
pid=$!

# sleep for 2 seconds before checking if XDD process is still running
(
echo "xdd started, pid=${pid}"
echo "sleep ${sleep_seconds}" 
) > "${log_file}"
sleep "${sleep_seconds}"

if pkill "${pid}" ; then
  # test failed
  echo "Had to kill ${pid}." >> "${log_file}"
  finalize_test 1 "Requested -dryrun option, but XDD process was still running after ${sleep_seconds}, something is causing it to hang"
else 
  # test passed
  finalize_test 0
fi
