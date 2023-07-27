#!/usr/bin/env bash
#
# Debug output test for XDD.
#
# Validate XDD header has worker thread NUMA cpus printed if requested
#
# Description - Just writes out to /dev/null using XDD with -debug INIT and verifies
#               that the NUMA cpus are listed if requested through the debug flag
#
# Source the test configuration environment
#
source ../test_config
source ../common.sh

initialize_test

echo "Foobar and I am here motherfucker"

xdd_cmd="${XDDTEST_XDD_EXE} -op write -reqsize 128 -numreqs 1 -targets 1 /dev/null -verbose -debug INIT"
${xdd_cmd} 2>&1 | grep "bound to NUMA node"

if [[ $? -ne 0 ]]; then
  # test failed
  finalize_test 1
fi

xdd_cmd="${XDDTEST_XDD_EXE} -op write -reqsize 128 -numreqs 1 -targets 1 /dev/null -verbose"
${xdd_cmd} 2>&1 | grep "bound to NUMA node"

if [[ $? -ne 1 ]]; then
  # test failed
  finalize_test 1
fi

# test passed
finalize_test 0
