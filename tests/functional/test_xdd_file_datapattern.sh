#!/usr/bin/env bash
#
# Acceptance test for XDD.
#
# Validate the funtionality of -datapattern file/wholefile option by creating a file,
# setting the datapattern from that file and then verifying the contents match.
# Additionally, ensure that when using the wholefile option, XDD does not crash (segfault)
# if the buffer file is smaller than the requested size (reqsize)
#
# Description - Verifies XDD's -datapattern wholefile functionality.
#
# Get absolute path to script
SCRIPT="${BASH_SOURCE[0]}"
SCRIPTPATH=$(dirname "${SCRIPT}")

# Source the test configuration environment
source "${SCRIPTPATH}/../test_config"
source "${SCRIPTPATH}/../common.sh"

# Perform pre-test
initialize_test
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"
log_file="$(get_log_file)"

input_file="${test_dir}/data1.dat"
output_file="${test_dir}/data2.dat"

# Function to test XDD with file-based datapattern
# Args:
#   $1: blocksize to create the input file
#   $2: count (number of blocks to write)
#   $3: reqsize (size of each I/O request)
#   $4: numreqs (total number of I/O requests)
#   $5: queuedepth (number of concurrent requests)
test_xdd_file_datapattern() {
    local bs=$1
    local count=$2
    local reqsize=$3
    local numreqs=$4
    local qd=$5

    # Create datapattern input file using dd
    dd if=/dev/urandom of="${input_file}" bs="${bs}" count="${count}"

    # Write out file using xdd with -datapattern file
    "${XDDTEST_XDD_EXE}" -op write -reqsize "${reqsize}" -numreqs "${numreqs}" \
        -datapattern wholefile "${input_file}" -targets 1 "${output_file}" \
        -looseordering -qd "${qd}" -passes 1

    # Check if the input file is smaller than the output file
    input_size=$(stat -c%s "${input_file}")
    output_size=$(stat -c%s "${output_file}")

    # If the input_file is less than output_file
    # Append the difference to input_file
    # so when it compares both files, they have the same size
    # If the input and output files differ in size, they are guaranteed not to match.
    if ((input_size < output_size)); then
        dd if="${input_file}" of="${input_file}" \
            bs=1k \
            count=$(((output_size - input_size) / 1024)) \
            oflag=append conv=notrunc
    elif ((input_size > output_size)); then
        # Log an error if the input file is already larger in size
        echo "Error: Input file (${input_size} bytes) is larger than output file. No appending needed." >"${log_file}"
        finalize_test 1 "Failure: Input file is too large for comparison with the output file."
        rm "${input_file}" "${output_file}"
        return
    fi

    # Verify the contents of the file match
    if ! cmp "${input_file}" "${output_file}"; then
        echo "Error when comparing files ${input_file} and ${output_file} with -datapattern file" >"${log_file}"
        cmp "${input_file}" "${output_file}" >>"${log_file}"
        rm "${input_file}" "${output_file}"
        finalize_test 1 "Issue with setting datapattern from file with -datapattern file. Contents don't match."
    fi
    rm "${input_file}" "${output_file}"
}

#
# Tests below test when blocksize is the same size as the request size
#
# Verfies that XDD handles -datapattern wholefile correctly with
# Using dd: bs=4k, count=2 to create a 8KB input file
# Using XDD: reqsize=8 blocks, numreqs=1, qd=1 (one thread)
test_xdd_file_datapattern 4k 2 8 1 1

# Verifies that XDD handles -datapattern wholefile correctly with
# Using dd: bs=4k, count=2 to create a 8KB input file
# Using XDD: reqsize=4 blocks, numreqs=2m, qd=2 (two threads)
test_xdd_file_datapattern 4k 2 4 2 2

#
# Tests below test when blocksize is smaller than request size
#
# Verifies that XDD handles cases where reqsize > blocksize
# This test confirms that XDD doesn't encounter issues when the request size
# is larger than the input file size. (#38)
# Using dd: bs=6k, count=1 to create a 6KB input file.
# Using XDD: reqsize=7 blocks, numreqs=1, qd=1
test_xdd_file_datapattern 6k 1 7 1 1

# Verifies correct behavior when reqsize is not a multiple of blocksize
# This test uses two threads with loose ordering to check proper data
# distribution across threads.
# Using dd: bs=5k, count=2 to create a 10KB input file.
# Using XDD: reqsize=7 blocks, numreqs=2, qd=2 (two threads).
test_xdd_file_datapattern 5k 2 7 2 2

# Verifies correct behavior when reqsize is a multiple of blocksize
# This test also uses two threads with loose ordering for overlapping I/O operations.
# Using dd: bs=6k, count=4 to create a 24KB input file.
# Using XDD: reqsize=8 blocks, numreqs=3, qd=2.
test_xdd_file_datapattern 6k 4 8 3 2

# Verifies that XDD handles very large reqsize without segmentation faults
# Specifically tests when reqsize is significantly larger than blocksize.
# Using dd: bs=1k, count=1 to create a 1KB input file.
# Using XDD: reqsize=256 blocks (1MB), numreqs=4, qd=3.
# This ensures XDD can handle writing large data even if the input file is small.
test_xdd_file_datapattern 1k 1 256 4 3

# Test passed
finalize_test 0
