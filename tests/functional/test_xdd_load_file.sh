#!/usr/bin/env bash
#
# Description - test xdd seek load
#
# Validate that xdd seek load is able to load a file with or without
# numreqs. It should also overwrite the blocksize if provided.
#
# Get absolute path to script
SCRIPT=${BASH_SOURCE[0]}
SCRIPTPATH=$(dirname "${SCRIPT}")

# Source the test configuration environment
source "${SCRIPTPATH}/../test_config"
source "${SCRIPTPATH}/../common.sh"

# Perform pre-test
initialize_test
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"

# Function to load a file with optional numreqs and blocksize
seek_load_file() {
  local load_file=$1
  local numreqs=$2
  local blocksize=$3
  local cmd=("${XDDTEST_XDD_EXE}" -seek load "${load_file}" -target /dev/null)

  # Add parameters only if they're non-empty
  if [[ -n "${numreqs}" ]]; then
    cmd+=(-numreqs "${numreqs}")
  fi
  if [[ -n "${blocksize}" ]]; then
    cmd+=(-blocksize "${blocksize}")
  fi

  # Execute the constructed command array
  "${cmd[@]}"
}

# Function to verify the output
verify_output() {
  local output=$1
  local expected_numreqs=$2
  local expected_blocksize=$3

  # Extract the numreqs and blocksize from the output
  local numreqs
  numreqs=$(echo "${output}" | grep -oP 'Number of Operations, \K[0-9]+')
  local blocksize
  blocksize=$(echo "${output}" | grep -oP 'Blocksize in bytes, \K[0-9]+')

  # Verify the numreqs and blocksize with the expected values
  if [[ "${numreqs}" != "${expected_numreqs}" ]]; then
    finalize_test 1 "Numreqs verification failed: expected ${expected_numreqs}, got ${numreqs}"
  fi

  if [[ "${blocksize}" != "${expected_blocksize}" ]]; then
    finalize_test 1 "Blocksize verification failed: expected ${expected_blocksize}, got ${blocksize}"
  fi
}

# Test 1: Default blocksize with numreqs 10
seek_file="${test_dir}/fool.dat"
load_file="${seek_file}.T0.txt"

# Create a file with numreqs 10 using default blocksize
"${XDDTEST_XDD_EXE}" -seek save "${seek_file}" -target /dev/null -numreqs 10 -op write

# Test cases for default blocksize
# 1. Load the file with
#    no numreqs, no blocksize
output=$(seek_load_file "${load_file}")
verify_output "${output}" 10 4096

# 2. Load the file with
#       numreqs < seek file lines, no blocksize
output=$(seek_load_file "${load_file}" 5 "")
verify_output "${output}" 5 4096

# 3. Load the file with
#    no numreqs, blocksize
output=$(seek_load_file "${load_file}" "" 8192)
verify_output "${output}" 10 4096

# 4. Load the file with
#       numreq > seek file lines, no blocksize
output=$(seek_load_file "${load_file}" 20 "")
verify_output "${output}" 10 4096

# Clean up
rm -f "${seek_file}" "${load_file}"

# Test 2: blocksize 8192 with numreqs 5
# Create a file with numreqs 5, custom blocksize
"${XDDTEST_XDD_EXE}" -seek save "${seek_file}" -target /dev/null -numreqs 5 -blocksize 8192 -op write

# Test cases for custom blocksize
# 1. Load the file with
#    no numreqs, no blocksize
output=$(seek_load_file "${load_file}")
verify_output "${output}" 5 8192

# 2. Load the file with 
#       numreqs, no blocksize
output=$(seek_load_file "${load_file}" 3 "")
verify_output "${output}" 3 8192

# 3. Load the file with
#    no numreqs, blocksize
output=$(seek_load_file "${load_file}" "" 4096)
verify_output "${output}" 5 8192

# 4. Load the file with
#      numreqs > seek file lines, no blocksize
output=$(seek_load_file "${load_file}" 10 "")
verify_output "${output}" 5 8192

# Clean up
rm -f "${seek_file}" "${load_file}"

# Output test result
finalize_test 0