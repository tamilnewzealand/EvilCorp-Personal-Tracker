# COMPSYS 704 Group 7 Project Plan #
## Introduction ## 
The EvilCorp Personal Tracker project requires the relative position of a small tracking device to be calculated from sensor data. This tracking device is equipped with a wireless transmitter/receiver, and multiple inertial measurement unit (IMU) sensors. The environment for this tracking project is a room that has twenty wireless beacons at known positions. From these provided specifications, there are two main options for conducting localisation of the tracking device.

## Bluetooth ##
The first option is to use signal strength values of the Bluetooth beacons which have been placed throughout the environment. This method will involve reading the beacon messages to determine the id of the beacon which is sending it, and the signal strength to determine how far away that beacon is from the tracking device. From these two data values, the location of the tracking device can be calculated using triangulation.

## Inertial Sensors ##
The second option is to use the IMU sensors to calculate the position of the tracking device. This method will involve the gyroscope, magnetometer, and accelerometer sensors to determine how far the tracking device has travelled and in what direction, at each point in time.

## Discussion ## 
We have chosen to use the IMU sensors for localisation of the tracking device. This is because 
