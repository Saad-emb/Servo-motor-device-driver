# Servo-motor-device-driver

Servo-motor-device-driver is a Linux device driver for controlling a servo motor connected to your Raspberry Pi running Raspbian.

## Building and Installing

To build and install the driver, follow these steps:

Clone the repository to your  machine
Change to the driver directory: cd <driver directory>

**Manualy:**

Install the driver: make \
Install the driver: sudo insmod \servo.ko \
Allow other users to write to the device file: sudo chmod 777 /dev/servo_motor
        
**Automaticly**
        
Execute the install.sh script : sudo ./install.sh
        

## Usage
Once the driver is installed, you can use the /dev/servo_motor device file to control the servo motor. Here's an example of how to write a desired angle to the device file:


echo "angle_value" > /dev/servo_motor
        
Example:
        
echo "90" > /dev/servo_motor       


## Uninstalling
To remove the driver from your system, 
Clean the build : make clean

