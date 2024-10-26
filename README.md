# XGestura

## Overview

XGestura is an innovative project that transforms hand movements into digital inputs using an Arduino-mounted glove equipped with an MPU6050 motion sensor and an HC-05 Bluetooth module. This allows users to control mouse movements and keyboard actions simply by moving their hands, creating a seamless interaction experience for various applications.

## Features

- **Two Modes of Operation**:
  - **Mouse Control**: Maps glove movements to mouse movements.
  - **Keyboard Control**: Maps sensor inputs to WASD keys for game control.
  
- **Wireless Communication**: Utilizes Bluetooth for real-time data transmission.

## How It Works

### Components

- **Arduino**: Acts as the main microcontroller, processing sensor data.
- **MPU6050**: A 6-axis motion sensor that captures the orientation and movement of the glove.
- **HC-05 Bluetooth Module**: Enables wireless communication with a connected device (like a computer).

### Mode 1: Mouse Control

1. **Sensor Input**: The MPU6050 captures the tilt and orientation of the glove.
2. **Data Processing**: The Arduino processes this data and converts it into corresponding mouse movements.
3. **Output**: The movement is transmitted via the HC-05 Bluetooth module to a computer, where a Python script translates the data into mouse actions.

### Mode 2: Keyboard Control

1. **Sensor Input**: Similar to the mouse control mode, the MPU6050 captures glove movements.
2. **Data Mapping**: The Arduino processes the data to map specific movements to WASD key presses.
3. **Output**: The HC-05 sends this data to the computer, where a different Python script interprets the movements as keyboard inputs.

## Working

### MPU6050

The MPU6050 is a 6-axis motion sensor that combines a 3-axis gyroscope and a 3-axis accelerometer. Here’s how it works:

1. **Accelerometer**: Measures acceleration along the X, Y, and Z axes. The values represent the force exerted on the sensor due to movement and gravity, expressed in g (gravitational units).
   - **X-axis**: Measures side-to-side movements.
   - **Y-axis**: Measures forward and backward movements.
   - **Z-axis**: Measures vertical movements.

   When you tilt your hand, the accelerometer captures changes in these values, providing data on the angle and direction of movement.

2. **Gyroscope**: Measures the rate of rotation around each axis (X, Y, and Z), providing angular velocity. This data helps in determining the orientation of the glove over time.

3. **Data Fusion**: By combining data from both sensors using algorithms (like a complementary or Kalman filter), the Arduino calculates the glove's orientation and movement accurately.

### HC-05 Bluetooth Module

The HC-05 is a Bluetooth module that allows for wireless communication between devices. Here’s how it works in the context of XGestura:

1. **Communication Protocol**: The HC-05 uses the Serial Port Profile (SPP) to establish a connection with a computer or other Bluetooth-enabled devices. This protocol allows for data transmission similar to a serial cable.

2. **Pairing**: Before data can be sent, the HC-05 must be paired with the computer. This usually involves entering a PIN (default is often "1234" or "0000").

3. **Data Transmission**: Once paired, the HC-05 transmits the processed data from the Arduino (mouse or keyboard commands) wirelessly to the computer. The computer, running a Python script, interprets these signals to perform actions like moving the cursor or pressing keys.

## Applications

- **Gaming**: Control game characters with natural hand movements, enhancing the gaming experience.
- **Accessibility**: Provide an alternative input method for individuals with mobility impairments.
- **Virtual Reality**: Enhance VR environments by translating real-world movements into virtual actions.
- **Robotics**: Use hand gestures to control robotic systems wirelessly.

## Concept

The idea behind XGestura is to bridge the gap between physical actions and digital responses. By leveraging motion sensing technology, we can create a more intuitive way to interact with computers, games, and virtual environments. This project demonstrates the potential of combining hardware and software to innovate user interfaces.

## Getting Started

### Prerequisites

- Arduino board (e.g., Arduino Uno)
- MPU6050 sensor
- HC-05 Bluetooth module
- Jumper wires
- Python (with PyBluez library for Bluetooth communication)

### Installation

#### You'll need:

Python libraries: pyserial to communicate with the HC-05 over Bluetooth and pyautogui to control the keyboard.
Install dependencies with:
```
pip install pyserial pyautogui
```

1. **Hardware Setup**: 
   - Connect the MPU6050 to the Arduino using I2C.
   - Connect the HC-05 to the Arduino for Bluetooth communication.

2. **Arduino Code**: Upload the provided Arduino sketch to configure the MPU6050 and HC-05.

3. **Python Environment**: Install the required libraries using pip:
   ```
   pip install pybluez
   ```
Run the Script: Execute the desired Python script to start interacting with your computer.

## Contributing
Feel free to contribute to the project by opening issues or submitting pull requests. Your feedback and improvements are always welcome!

## License
This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments
Thanks to the open-source community for their resources and libraries that made this project possible.

For further questions or issues, please reach out via the project's GitHub repository or contact the maintainer directly. Enjoy using XGestura!
