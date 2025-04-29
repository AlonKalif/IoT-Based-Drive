#!/bin/bash

# File path passed from minion's main
file_path="$1"

if [ -f "$file_path" ]; then
    # Remove old memory file if it exist
    rm -f "$file_path"
fi

# Create new memory file filled with 8MB of zeroes
dd if=/dev/zero of="$file_path" bs=1M count=8

# Give it read & write permissions
chmod 666 "$file_path"



