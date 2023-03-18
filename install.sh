#!/bin/bash

# Build and install the driver
make
sudo insmod servo.ko

# Set the appropriate permissions on the device file
sudo chmod 777 /dev/servo_motor



