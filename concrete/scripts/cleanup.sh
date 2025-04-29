#!/bin/bash

# Extract paths from JSON
# device_path=$(jq -r '.MasterSide.nbd_device_path' config.json)
mount_to=$(jq -r '.MasterSide.nbd_mount_on_path' config.json)

# Unmount drive dir
sudo umount "$mount_to"

# pid=$(pidof ./exe/minion_side_1_test.out)
# echo "PID: $pid"

# Remove nbd devices 
sudo modprobe -r nbd