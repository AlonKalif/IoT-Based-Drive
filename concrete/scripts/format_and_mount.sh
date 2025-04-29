#!/bin/bash

# Extract paths from JSON
device_path=$(jq -r '.MasterSide.nbd_device_path' config.json)
mount_to=$(jq -r '.MasterSide.nbd_mount_on_path' config.json)

# Build file system 
sudo mkfs.ext2 "$device_path"

# Mount device on dir
sudo mount "$device_path" "$mount_to"

# Create a test file with r/w permissions
sudo touch "$mount_to"/afile.txt
sudo chmod 666 "$mount_to"/afile.txt