#!/bin/bash

# Load nbd module
sudo modprobe nbd

# Set Blocksize
echo 4 | sudo tee /sys/block/nbd0/queue/max_sectors_kb
