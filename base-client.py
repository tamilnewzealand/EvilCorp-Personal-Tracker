#! /usr/bin/env python3
import serial
import sys
import io
import binascii
import datetime
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from time import sleep
import os.path
import os
import datetime
import time

def write_to_file(x, y, o):
    if not os.path.isfile('log.csv'):
        with open('log.csv','w+') as fd:
            fd.write("Time:, X:, Y:, Orientation:" + os.linesep)
    with open('log.csv','a') as fd:
        timestep = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
        fd.write(str(timestep) + ', ' + str(x) + ', ' + str(y) + ', ' + str(o) + os.linesep)

# Serial ports
locations=['/dev/ttyACM0', '/dev/ttyACM1','/dev/ttyACM2', '/dev/ttyACM3','/dev/ttyACM4', '/dev/ttyACM5','/dev/ttyUSB0','/dev/ttyUSB1','/dev/ttyUSB2','/dev/ttyUSB3', '/dev/ttyUSB4', '/dev/ttyUSB5', '/dev/ttyUSB6', '/dev/ttyUSB7', '/dev/ttyUSB8', '/dev/ttyUSB9', '/dev/ttyUSB10','/dev/ttyS0', '/dev/ttyS1', '/dev/ttyS2', 'com2', 'com3', 'com4', 'com5', 'com6', 'com7', 'com8', 'com9', 'com10', 'com11', 'com12', 'com13', 'com14', 'com15', 'com16', 'com17', 'com18', 'com19', 'com20', 'com21', 'com1', 'end']

# Connect to base station
for device in locations:
    try:
        ser = serial.Serial(device, 115200)
        print("Port", device, "has been successfully opened for serial communication.")
        break
    except:
        if device == 'end':
            print ("Unable to find Serial Port, Please plug in cable or check cable connections.")
            sys.exit(0)

# Setup location plot
fig1 = plt.figure()
data = np.random.rand(2, 1) * 100
l, = plt.plot([], [], 'r-')
plt.axis('image')
plt.xlim(0, 1057)  
plt.ylim(0, 1900)
plt.xlabel('x')
plt.title('EvilCorp Personal Tracker')
img = plt.imread("background.png")
plt.imshow(img)

# Function to update animation with new line
def update_line(num, dsa, line):
    global data
    try:
        new_line = ser.readline()
        new_line = new_line.decode("ascii")
        new_data = new_line.split(":")
        coord = new_data[0].split(",")
        coords=[[0], [0]]
        coords[0][0] = int(coord[0])
        coords[1][0] = int(coord[1])
        new = np.asarray(coords)
        x_loc = str(new[0][0])
        y_loc = str(new[1][0])
        ore = new_data[1].strip()
        print("Current Location: X: " + x_loc + ", Y: " + y_loc + ", Orientation: " + ore)
        write_to_file(x_loc, y_loc, ore)
        data = np.append(data, new, 1)
    except:
        pass
    
    line.set_data(data[..., :num])
    return line,

# Update the plot every second
line_ani = animation.FuncAnimation(fig1, update_line, 1000, fargs=(data, l),interval=1000, blit=True)

plt.show()
