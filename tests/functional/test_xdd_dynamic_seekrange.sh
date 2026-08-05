#!/usr/bin/env bash
#
# Acceptance test for XDD
#
# Description - validates dynamic seek range calculation based on file size
#
# Verify that when no explicit -seek range is provided, xdd calculates the seek range
# dynamically based on file size / block size for regular files.
#
# Get absolute path to script
SCRIPT=${BASH_SOURCE[0]}
SCRIPTPATH=$(dirname "${SCRIPT}")

# Source the test configuration environment
source "${SCRIPTPATH}/../test_config"
source "${SCRIPTPATH}/../common.sh"

# Pre-test set-up
initialize_test
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"
log_file="$(get_log_file)"

# Test parameters
block_size=4096
file_size_bytes=1048576  # 1 MB
expected_seek_range=$((file_size_bytes / block_size))  # 256 blocks

test_file="${test_dir}/seekrange_test"

# Create a test file of known size (1 MB)
dd if=/dev/zero of="${test_file}" bs=1 count=0 seek="${file_size_bytes}" 2>/dev/null

# Verify file was created with correct size
actual_file_size=$(stat -c '%s' "${test_file}")
if [[ "${actual_file_size}" -ne "${file_size_bytes}" ]]; then
    finalize_test 1 "Failed to create test file of size ${file_size_bytes}, got ${actual_file_size}"
fi

{
    echo "Test file size: ${actual_file_size} bytes"
    echo "Block size: ${block_size}"
    echo "Expected seek range: ${expected_seek_range} blocks"
} > "${log_file}"

# Run xdd with NO explicit -seek range to trigger dynamic calculation
# The seek range is printed in target info regardless of seek mode
output=$("${XDDTEST_XDD_EXE}" -target "${test_file}" -op read -blocksize "${block_size}" -numreqs 1 2>&1)

echo "XDD Output:" >> "${log_file}"
echo "${output}" >> "${log_file}"

# Extract the seek range from stdout (in blocks)
# The output format shows "Seek Range," followed by block count on next line (with leading whitespace)
seek_range_blocks=$(echo "${output}" | grep -A 1 "Seek Range," | tail -1 | awk '{print $1}' | tr -d ',')

echo "Extracted seek range: ${seek_range_blocks}" >> "${log_file}"

if [[ -z "${seek_range_blocks}" ]]; then
    finalize_test 1 "Could not extract Seek Range from xdd output"
fi

# Verify the dynamically calculated seek range matches expected value
if [[ "${seek_range_blocks}" -eq "${expected_seek_range}" ]]; then
    echo "PASS: Seek range ${seek_range_blocks} matches expected ${expected_seek_range}" >> "${log_file}"
else
    finalize_test 1 "Seek range ${seek_range_blocks} does not match expected ${expected_seek_range}"
fi

# Test 2: Verify user-specified -seek range takes priority
user_seek_range=500
output2=$("${XDDTEST_XDD_EXE}" -target "${test_file}" -op read -blocksize "${block_size}" -numreqs 1 -seek range "${user_seek_range}" 2>&1)

{
    echo ""
    echo "Test 2 - User specified seek range:"
    echo "${output2}"
} >> "${log_file}"

seek_range_blocks2=$(echo "${output2}" | grep -A 1 "Seek Range," | tail -1 | awk '{print $1}' | tr -d ',')

echo "Extracted seek range (user specified): ${seek_range_blocks2}" >> "${log_file}"

if [[ "${seek_range_blocks2}" -eq "${user_seek_range}" ]]; then
    echo "PASS: User-specified seek range ${seek_range_blocks2} was used correctly" >> "${log_file}"
else
    finalize_test 1 "User-specified seek range ${user_seek_range} was not used, got ${seek_range_blocks2}"
fi

# Test 3: Different file size to confirm dynamic calculation
file_size_bytes2=4194304  # 4 MB
expected_seek_range2=$((file_size_bytes2 / block_size))  # 1024 blocks
test_file2="${test_dir}/seekrange_test2"

dd if=/dev/zero of="${test_file2}" bs=1 count=0 seek="${file_size_bytes2}" 2>/dev/null

output3=$("${XDDTEST_XDD_EXE}" -target "${test_file2}" -op read -blocksize "${block_size}" -numreqs 1 2>&1)

seek_range_blocks3=$(echo "${output3}" | grep -A 1 "Seek Range," | tail -1 | awk '{print $1}' | tr -d ',')

{
    echo ""
    echo "Test 3 - Different file size (4MB):"
    echo "${output3}"
    echo "Extracted seek range: ${seek_range_blocks3}"
    echo "Expected seek range: ${expected_seek_range2}"
} >> "${log_file}"

if [[ "${seek_range_blocks3}" -eq "${expected_seek_range2}" ]]; then
    echo "PASS: Seek range ${seek_range_blocks3} matches expected ${expected_seek_range2} for 4MB file" >> "${log_file}"
    finalize_test 0
else
    finalize_test 1 "Seek range ${seek_range_blocks3} does not match expected ${expected_seek_range2} for 4MB file"
fi
