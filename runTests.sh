#!/usr/bin/env bash
###############################################################################
# runTests.sh
# ===========
#
# Description:           Script to run all test cases.
#
###############################################################################

# Relative path to test case directory
TEST_DIR="TestCases"

# Set to nonzero value to remove output files after each test.
CLEAN_UP=1

# Print a message to stderr
log_stderr()
{
    echo "$@" 1>&2;
}

# Allow running specified test only
if [ "$#" == '1' ]; then
    # Run specific test case
    TEST_CASES="test$1"
    log_stderr "Running test $TEST_CASES"
else
    # Get list of test cases
    TEST_CASES="$(ls -1 "$TEST_DIR" | grep -Po '^\w+(?=\.input)')"
    log_stderr "Running all tests"
fi

# Set input file separator to newline
IFS=$'\n'

for TEST_NAME in $TEST_CASES; do
    log_stderr "================ START TEST $TEST_NAME ================"

    # Run game using test input and store
    ./scrabble < "$TEST_DIR/$TEST_NAME.input" > "$TEST_DIR/$TEST_NAME.gameout"

    # Diff output
    diff --color=always -w "$TEST_DIR/$TEST_NAME.output" "$TEST_DIR/$TEST_NAME.gameout"

    # Store return value
    RESULT_1=$?
    RESULT_2=0

    # Remove output file
    if [ "$CLEAN_UP" != '0' ]; then
        rm "$TEST_DIR/$TEST_NAME.gameout"
    fi

    # Diff save file, if test includes it
    if [ -e "$TEST_DIR/$TEST_NAME.expsave" ]; then
        log_stderr "================ Save file ================"

        # Move save file into test case directory (save does not allow /)
        mv "$TEST_NAME.actualsave" "$TEST_DIR/$TEST_NAME.actualsave"

        # Diff save files
        diff --color=always -w -y "$TEST_DIR/$TEST_NAME.expsave" "$TEST_DIR/$TEST_NAME.actualsave"

        # Set return value for second part of the test
        RESULT_2=$?

        # Remove save file
        if [ $CLEAN_UP != '0' ]; then
            rm "$TEST_DIR/$TEST_NAME.actualsave"
        fi

        # Print newline
        log_stderr ""
    fi

    if [ "$RESULT_1" ==  "0" ] && [ "$RESULT_2" == "0" ]; then
        OUTCOME="PASS"
    else
        OUTCOME="FAIL"
    fi

    log_stderr "================ END TEST $TEST_NAME: $OUTCOME ================"
done

# Reset input file separator
IFS=

# vim: set ts=4 sw=4 tw=79 fdm=manual ff=unix fenc=utf-8 et :
