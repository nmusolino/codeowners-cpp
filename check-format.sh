#!/bin/bash

clang_format_command="git clang-format"

commit=$1
if [[ "$commit" ]]; then
    clang_format_command="${clang_format_command} --commit ${commit}"
fi

output=$(${clang_format_command} --diff)

if [[ "$output" = "no modified files to format" || "$output" = "clang-format did not modify any files" ]]; then
    echo "$output"
    exit 0
else
    ${clang_format_command} --diff
    echo ""
    echo "Run one of the following to apply changes:"
    echo "  ${clang_format_command}"
    echo "  ${clang_format_command} --patch    # To review changes interactively"
    exit 1
fi
