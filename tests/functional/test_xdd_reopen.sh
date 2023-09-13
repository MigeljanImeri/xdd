#!/usr/bin/env bash
#
# Acceptance test for XDD
#
# Description - closes and reopens file
#
# Verify -reopen by checking if (number of opens and closes during a pass + 1) equals number of opens and closes during the next incremented pass
#
# Source the test configuration environment
#
source ../test_config
source ../common.sh

# pre-test set-up
initialize_test
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"
mkdir -p "${test_dir}"

# make test file
test_file="${test_dir}/data1"
touch "${test_file}"

# create array to store number of opens and closes
declare -a sys_open
declare -a sys_close

# gather number of opens and closes during 2, 3, and 4 passes
# incrementing number of passes by n should also increment number of opens and closes by n
# used -f in strace to find child proccesses (opens and closes)

min_passes=2
max_passes=4

for ((num_passes=min_passes; num_passes<=max_passes; num_passes++)); do
      xdd_cmd="${XDDTEST_XDD_EXE} -op write -target ${test_file} -numreqs 10 -passes ${num_passes} -passdelay 1 -reopen ${test_file}"

      # shellcheck disable=SC2086
      sys_call_open=$(2>&1 1>/dev/null strace -cfq -e trace=openat ${xdd_cmd} | grep -w openat | awk '{ print $4 }')
      # shellcheck disable=SC2086
      sys_call_close=$(2>&1 1> /dev/null strace -cfq -e trace=close ${xdd_cmd} | grep -w close | awk '{ print $4 }')

      sys_open["${num_passes}"]="${sys_call_open}"
      sys_close["${num_passes}"]="${sys_call_close}"
done

pass_count=0

# Check if the first element is 1 less than the next and so on for n-1 elements
for ((i=min_passes; i<max_passes; i++)); do
      if [[ $((${sys_open[${i}]}+1)) -eq ${sys_open[$((i+1))]} ]]; then
            pass_count="$((pass_count+1))"
      fi
      if [[ $((${sys_close[${i}]}+1)) -eq ${sys_close[$((i+1))]} ]]; then
            pass_count="$((pass_count+1))"
      fi
done

correct_count="$((max_passes-min_passes))"
correct_count="$((correct_count*2))"

# verify output
if [[ "${pass_count}" -eq "${correct_count}" ]]; then
  # test passed
  finalize_test 0
else
  # test failed
  finalize_test -1 "${pass_count} != ${correct_count} so the number of open/close calls is incorrect when using flag -reopen. Currently ignoring until issue is resolved."
fi
