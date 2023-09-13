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

("${XDDTEST_XDD_EXE}" -op write -reqsize 128 -numreqs 1 -targets 1 /dev/null -verbose -debug INIT 2>&1 | grep "bound to NUMA node") || finalize_test 1 "XDD output is missing NUMA node pinning info"

("${XDDTEST_XDD_EXE}" -op write -reqsize 128 -numreqs 1 -targets 1 /dev/null -verbose 2>&1 | grep "bound to NUMA node") && finalize_test 1 "XDD output should not have NUMA node pinning info because -debug INIT was not used"

# test passed
finalize_test 0
