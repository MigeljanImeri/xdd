#!/usr/bin/env bash
#
# Acceptance test for XDD argument parser target scoping.
#
# Validates:
# - New scoped forms via -target/-targets with undashed option stacks
# - Global options, per-target overrides, and composed nested subgrammar
# - Bad grammars are rejected cleanly (no segfault/abort) per hierarchy.md
#

SCRIPT="${BASH_SOURCE[0]}"
SCRIPTPATH=$(dirname "${SCRIPT}")

source "${SCRIPTPATH}/../test_config"
source "${SCRIPTPATH}/../common.sh"

initialize_test
test_dir="${XDDTEST_LOCAL_MOUNT}/${TESTNAME}"
log_file="$(get_log_file)"

target_a="${test_dir}/target_a.dat"
target_b="${test_dir}/target_b.dat"
target_c="${test_dir}/target_c.dat"
pattern_file="${test_dir}/pattern.bin"

touch "${target_a}" "${target_b}" "${target_c}"
printf 'pattern-seed' > "${pattern_file}"

# Exit codes from signals / abort that indicate uncaught parser errors.
is_crash_exit() {
    local rc="$1"
    [[ "${rc}" -eq 139 ]] || [[ "${rc}" -eq 134 ]] || [[ "${rc}" -eq 132 ]] || [[ "${rc}" -eq 6 ]]
}

run_case() {
    local name="$1"
    local rc
    shift

    "${XDDTEST_XDD_EXE}" "$@" >"${log_file}" 2>&1
    rc=$?
    if [[ "${rc}" -ne 0 ]]; then
        if is_crash_exit "${rc}"; then
            finalize_test 1 "${name} crashed (exit ${rc}): $* (see ${log_file})"
        fi
        finalize_test 1 "${name} failed unexpectedly (exit ${rc}): $* (see ${log_file})"
    fi
}

run_bad_case() {
    local name="$1"
    local rc
    shift

    "${XDDTEST_XDD_EXE}" "$@" >"${log_file}" 2>&1
    rc=$?

    if [[ "${rc}" -eq 0 ]]; then
        finalize_test 1 "${name} should reject bad grammar but succeeded: $* (see ${log_file})"
    fi
    if is_crash_exit "${rc}"; then
        finalize_test 1 "${name} crashed on bad grammar (exit ${rc}): $* (see ${log_file})"
    fi
}

#
# Valid grammar cases (hierarchy.md "Valid" and implemented notes)
#

# Per-target scoped atomic arguments on -target declarations.
run_case "scoped-target-atomics" \
    -op write \
    -target "${target_a}" blocksize 8k numreqs 2 \
    -target "${target_b}" blocksize 16k numreqs 4 \
    -dryrun

# -targets scoped composed arguments with nested subgrammar.
run_case "scoped-targets-composed" \
    -op write \
    -blocksize 4k \
    -numreqs 2 \
    -targets 2 "${target_a}" "${target_b}" datapattern random seek stagger 2 throttle ops 100 \
    -dryrun

# Global defaults then a single per-target override via -target.
run_case "global-then-target-override" \
    -blocksize 4096 \
    -numreqs 100 \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" blocksize 8192 \
    -dryrun

# Mix global options with per-target overrides on different targets.
run_case "mix-globals-and-overrides" \
    -blocksize 4096 \
    -numreqs 100 \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_b}" numreqs 25 \
    -dryrun

# Multiple undashed options scoped on the -targets line (applies to all listed targets).
run_case "multiple-args-on-targets-scope" \
    -targets 2 "${target_a}" "${target_b}" blocksize 2048 numreqs 10 datapattern random \
    -dryrun

# Single undashed override inside a -target block after -targets (with globals first).
run_case "single-override-in-target-block" \
    -blocksize 4096 \
    -numreqs 100 \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" blocksize 8192 \
    -dryrun

# A dashed token after scoped options terminates the -target block (global applies).
run_case "dashed-token-terminates-target-scope" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" blocksize 4096 \
    -numreqs 100 \
    -dryrun

# A dashed option after -target scoped options applies globally to all targets.
run_case "dashed-global-after-target-scope" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" -blocksize 8192 \
    -dryrun

# Undashed options after -targets filenames apply to every target in that scope.
run_case "undashed-blocksize-on-targets-scope" \
    -targets 2 "${target_a}" "${target_b}" blocksize 4096 numreqs 4 \
    -dryrun

run_case "undashed-numreqs-on-targets-scope" \
    -targets 2 "${target_a}" "${target_b}" numreqs 100 \
    -dryrun

# A dashed -targets ends the prior -target scope and begins a new target declaration.
run_case "dashed-targets-terminates-target-scope" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" -targets 1 "${target_c}" \
    -dryrun

# Composed options may appear directly on the -targets line without a -target block.
run_case "composed-on-targets-scope" \
    -targets 2 "${target_a}" "${target_b}" datapattern random \
    -dryrun

#
# Invalid grammar cases (hierarchy.md "Invalid" and stress tests)
# Each must fail with a normal error exit, not segfault/abort.
#

# Option key with no value at end of -target block.
run_bad_case "bad-dangling-target-option-key" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" blocksize

# Trailing option key missing its value inside -target block.
run_bad_case "bad-dangling-target-option-value" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" blocksize 4096 numreqs

# Deprecated legacy inline target form: -option target <#> ...
run_bad_case "bad-legacy-inline-target-form" \
    -blocksize target 0 4096 \
    -targets 1 "${target_a}"

# Deprecated legacy inline prev form: -option prev ...
run_bad_case "bad-legacy-inline-prev-form" \
    -blocksize prev 4096 \
    -targets 1 "${target_a}"

# Unknown undashed token inside -target scope.
run_bad_case "bad-unknown-scoped-option" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" notrealopt 1

# Composed option missing required argument (wholefile needs a filename).
run_bad_case "bad-composed-missing-wholefile-arg" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" datapattern wholefile

# Composed seek subgrammar missing stagger block count.
run_bad_case "bad-composed-missing-stagger-value" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" seek stagger

# Composed throttle subgrammar missing numeric value.
run_bad_case "bad-composed-missing-throttle-value" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" throttle ops

# Extra token after a complete scoped operation value.
run_bad_case "bad-composed-extra-operation-token" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" operation read write

# Nested -target meta-option inside an active -target scope.
run_bad_case "bad-meta-target-in-target-scope" \
    -targets 2 "${target_a}" "${target_b}" \
    -target "${target_a}" target "${target_c}"

# -targets count does not match number of filenames that follow.
run_bad_case "bad-targets-count-mismatch" \
    -targets 3 "${target_a}" "${target_b}" \
    -target "${target_a}" blocksize 4096

# Invalid numeric value for a scoped size option.
run_bad_case "bad-invalid-blocksize-value" \
    -targets 1 "${target_a}" \
    -target "${target_a}" blocksize notanumber

# Flag-only target option incorrectly given a trailing value token.
run_bad_case "bad-flag-option-with-extra-value" \
    -targets 1 "${target_a}" \
    -target "${target_a}" createnewfiles extra

run_bad_case "zero-values-passed" \
    -targets 1 "${target_a}" \
    -target "${target_a}" blocksize 0 nr 0