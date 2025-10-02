# XGestura: Gesture Recognition with ESP32

A hardware-based gesture recognition system using ESP32, MPU6050 accelerometer/gyroscope, and OLED display. Recognizes hand gestures without computer vision.

![Gesture Recognition](https://img.shields.io/badge/Project-Gesture_Recognition-blue)
![ESP32](https://img.shields.io/badge/Platform-ESP32-green)
![Arduino](https://img.shields.io/badge/Framework-Arduino-orange)

## 📋 Project Overview

This project demonstrates real-time gesture recognition using inertial sensors. The system detects various hand gestures based on motion patterns and displays the results on an OLED screen.

### Features
- ✅ Real-time orientation tracking (Pitch & Roll)
- ✅ Circular gesture detection (clockwise/counter-clockwise)
- ✅ Flick gesture detection (forward/backward/left/right)
- ✅ OLED display feedback
- ✅ No computer vision required
- ✅ Simple hardware setup

## 🛠 Hardware Requirements

| Component | Quantity | Notes |
|-----------|----------|-------|
| ESP32 DevKit V1 | 1 | Main microcontroller |
| MPU6050 Module | 1 | Accelerometer + Gyroscope |
| OLED Display (SSD1306) | 1 | 128x64 I2C display |
| Breadboard | 1 | For connections |
| Jumper Wires | Several | Male-to-Male |

## 🔌 Wiring Connections

### ESP32 to MPU6050 & OLED

| ESP32 Pin | MPU6050 Pin | OLED Pin |
|-----------|-------------|----------|
| 3.3V      | VCC         | VDD      |
| GND       | GND         | GND      |
| GPIO 22   | SCL         | SCK      |
| GPIO 21   | SDA         | SDA      |

**Note:** Both modules share the same I2C bus.

## 📦 Software Requirements

- Arduino IDE
- ESP32 Board Support
- Required Libraries:
  - `Wire.h` (I2C communication)
  - `Adafruit_GFX.h` (Graphics library)
  - `Adafruit_SSD1306.h` (OLED display)
  - `MPU6050.h` (MPU6050 sensor)
  - `MadgwickAHRS.h` (Sensor fusion)

## 🚀 Installation & Setup

### 1. Install Arduino IDE
Download and install Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Install ESP32 Board Support
1. Open Arduino IDE
2. Go to `File > Preferences`
3. Add to Additional Boards Manager URLs:

```bash
https://dl.espressif.com/dl/package_esp32_index.json
```

4. Go to `Tools > Board > Boards Manager`
5. Search for "ESP32" and install

### 3. Install Required Libraries
Open Arduino IDE and install these libraries via Library Manager:
- **Adafruit GFX Library**
- **Adafruit SSD1306**
- **MPU6050** by Electronic Cats
- **MadgwickAHRS**

### 4. Upload Code
1. Connect ESP32 to computer via USB
2. Select board: `Tools > Board > ESP32 Arduino > ESP32 Dev Module`
3. Select correct port: `Tools > Port`
4. Upload the provided code

## ✋ Supported Gestures

### 1. Circular Gesture
- **Movement:** Make smooth circular motions with the board
- **How to:** Hold board flat and move hand in circles (like stirring)
- **Detection:** "CLOCKWISE CIRCLE" or "COUNTER-CLOCKWISE" displayed

### 2. Flick Gesture  
- **Movement:** Quick tilting motions in any direction
- **How to:** Quickly tilt board forward/backward/left/right and return
- **Detection:** "FLICK FORWARD", "FLICK BACKWARD", etc. displayed

## 🎮 How to Use

1. **Power On:** Connect ESP32 to power
2. **Wait for Calibration:** OLED shows "Calibrating" (takes ~1 second)
3. **Make Gestures:** 
- For circles: Smooth circular hand motions
- For flicks: Quick tilt-and-return motions
4. **View Results:** Detected gestures appear on OLED display
5. **Cooldown:** Wait ~1 second between gestures

## 📊 Serial Output

The project also outputs debug information to Serial Monitor (115200 baud):

```bash
MPU6050 connected.
Gesture: Clockwise Circle
Gesture: Flick Forward
```

## 🔧 Code Structure

- `setup()`: Initializes sensors and display
- `loop()`: Main processing loop (40Hz)
- `detectCircularGesture()`: Circle pattern recognition
- `detectFlickGesture()`: Quick motion detection
- `updateDisplay()`: OLED screen management

## 🤝 Troubleshooting

### Common Issues

1. **OLED not displaying**
   - Check I2C address (usually 0x3C)
   - Verify wiring connections
   - Ensure 3.3V power

2. **MPU6050 not detected**
   - Check SDA/SCL connections
   - Verify sensor is working
   - Check library installation

3. **Gestures not detected**
   - Make more deliberate motions
   - Ensure calibration completes
   - Check serial monitor for errors

### Serial Monitor Messages
- `"SSD1306 allocation failed"` → OLED connection issue
- `"MPU6050 connection failed!"` → Sensor connection issue
- `"Calibrating..."` → Normal startup process

## 📁 Project Files

- `gesture_recognition.ino` - Main Arduino sketch
- `README.md` - This documentation
- Connection diagrams

## 🎯 Future Enhancements

- [ ] Add more gesture types
- [ ] Implement machine learning
- [ ] Add wireless communication
- [ ] Create gesture training mode
- [ ] Add sound feedback

## 📄 License

This project is open source. Feel free to modify and distribute.

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments
Thanks to the open-source community for their resources and libraries that made this project possible.

For further questions or issues, please reach out via the project's GitHub repository or contact the maintainer directly. Enjoy using XGestura!
