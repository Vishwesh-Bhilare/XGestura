import serial
from pynput.keyboard import Controller, Key
import time

# Initialize serial port (adjust COM port as needed, e.g., 'COM3' for Windows)
ser = serial.Serial('COM7', 9600)  # Ensure this matches the port in Arduino IDE
keyboard = Controller()

# Delay for serial port connection
time.sleep(2)

# Function to press and release a key
def press_key(key):
    keyboard.press(key)
    time.sleep(0.2)
    keyboard.release(key)

while True:
    # Read serial data from Arduino
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        
        # Map serial commands to keypresses
        if line == 'W':
            press_key('w')
        elif line == 'A':
            press_key('a')
        elif line == 'S':
            press_key('s')
        elif line == 'D':
            press_key('d')

    time.sleep(0.1)  # Small delay for smooth execution