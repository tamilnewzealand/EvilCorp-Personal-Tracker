# EvilCorp-Personal-Tracker

## Usage
1. Open base-station-project and mobile-project in Simplicity Studio
2. Build and Debug mobile-project to the mobile Thunderboard Sense 2
3. Build and Debug base-station-project to the base station Thunderboard Sense 2
4. Ensure that the base station is connected to a PC via USB cable, and run the Python script base-client.py

## Notes
* Position and orientation is communicated to the base station via Bluetooth and then to the PC via serial UART
* The position and orientation data appears in the Python console as it is received as is plotted on the displayed graph
