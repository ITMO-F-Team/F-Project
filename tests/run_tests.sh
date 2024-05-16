#! /bin/bash

# NOTE: Run from the project root!

export TEST_SUITE_ROOT=tests/data
export COMPILER_BINARY=build/src/flang-interpreter
pytest .
