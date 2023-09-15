#!/usr/bin/env bash

set -e

# shellcheck disable=SC2046
shellcheck -s bash -e SC1091 $(find "$@" -name "*.sh" | sort)
