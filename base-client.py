#! /usr/bin/env python3
import serial
import sys
import io
import binascii
import datetime
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

waiting_queue = []

def serial_logger():
    global waiting_queue
    locations=['/dev/ttyACM0', '/dev/ttyACM1','/dev/ttyACM2', '/dev/ttyACM3','/dev/ttyACM4', '/dev/ttyACM5','/dev/ttyUSB0','/dev/ttyUSB1','/dev/ttyUSB2','/dev/ttyUSB3', '/dev/ttyUSB4', '/dev/ttyUSB5', '/dev/ttyUSB6', '/dev/ttyUSB7', '/dev/ttyUSB8', '/dev/ttyUSB9', '/dev/ttyUSB10','/dev/ttyS0', '/dev/ttyS1', '/dev/ttyS2', 'com2', 'com3', 'com4', 'com5', 'com6', 'com7', 'com8', 'com9', 'com10', 'com11', 'com12', 'com13', 'com14', 'com15', 'com16', 'com17', 'com18', 'com19', 'com20', 'com21', 'com1', 'end']

    for device in locations:
        try:
            ser = serial.Serial(device, 9600, timeout = 1)
            break
        except:
            if device == 'end':
                print ("Unable to find Serial Port, Please plug in cable or check cable connections.")

    print("Port", device, "has been successfully opened for serial communication.")
    count = 0

    while True:
        line = ser.readline()
        stuff = line.split(",")
        stuff[0] = int(stuff[0])
        stuff[1] = int(stuff[1])
        waiting_queue.append(stuff)

thread.start_new_thread(serial_logger, ())

fig1 = plt.figure()

data = np.random.rand(2, 1) * 100
l, = plt.plot([], [], 'r-')
plt.xlim(0, 1057)  
plt.ylim(0, 1900)
plt.xlabel('x')
plt.title('EvilCorp Personal Tracker')

def update_line(num, dsa, line):
    global data
    global waiting_queue
    if not waiting_queue:
        new = waiting_queue.pop(0)
        new = np.asarray(new)
        data = np.append(data, new, 1)
    line.set_data(data[..., :num])
    print("Current Location: X: " + str(new[0][0]) + ", Y: " + str(new[1][0]))
    return line,

line_ani = animation.FuncAnimation(fig1, update_line, 25, fargs=(data, l),
                                   interval=1000, blit=True)

plt.show()