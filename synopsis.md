## Title:
## Gesture-Based Automation System for Windows Using ESP32 and Sensor-Based Motion Recognition

### Abstract:

This project implements a real-time gesture-based interface for Windows computers using sensor-based recognition, inspired by the findings in Ojeda-Castelo et al. (2022). While most prior works use vision-based or deep learning methods, our approach focuses on low-latency, sensor-driven gesture detection via the MPU-6050 IMU connected to an ESP32-WROOM module. Recognized gestures are mapped to system automation tasks—such as opening, saving, or switching files—on a Windows system using a Win32 C++ application. The design ensures contactless, intuitive, and reliable interaction, aligning with the AI-driven gesture recognition trends outlined in the reference paper.

### 1. Introduction
  The study of gesture recognition aims to create intuitive human–computer interfaces. While camera-based systems (e.g., Kinect, Leap Motion) dominate, they are computationally intensive and sensitive to lighting. The sensor-based approach proposed here leverages inertial measurement data for recognizing predefined gestures like circle, wave, and tilt, which trigger specific file-handling automations on Windows.
  According to Ojeda-Castelo et al. (2022), sensor-based gesture recognition provides high robustness and real-time efficiency—a property we exploit in this embedded–software hybrid system.

### 2. Objectives
  - Design a wearable ESP32-based motion sensing unit for gesture detection.
  - Implement gesture classification using accelerometer and gyroscope data.
  - Develop a Bluetooth communication channel to transmit gesture codes to Windows.
  - Map gestures to desktop automation commands (e.g., circle → open file, wave → save file).
  - Ensure low-latency operation and robust performance across users.

### 3. System Overview
  The proposed system consists of two main subsystems:
  - Hardware Subsystem (ESP32 + MPU-6050)
      MPU-6050 captures real-time motion data (acceleration, angular velocity).
      ESP32-WROOM reads I²C data, applies preprocessing (averaging, threshold filtering), classifies gestures, and transmits codes via Bluetooth (SPP).
      Each gesture is represented as a character command (e.g., G1, G2, …).

  - Software Subsystem (Windows Host Application):
      Win32-based listener written in C++ using the Windows API.
      Continuously reads gesture commands from the Bluetooth serial COM port.
      The gesture-event handler maps incoming codes to Windows automation functions:
      - Circle → Ctrl + O (Open File)
      - Vertical Wave → Ctrl + S (Save File)
      - Tilt Right → Alt + Tab (Switch Window)
      - Tilt Forward → Win + D (Show Desktop)
      #### Uses ShellExecute() and simulated keypress APIs (keybd_event, SendInput) for automation.

### 4. Methodology
  - Sensor Calibration: Initialize MPU-6050, collect baseline (rest) orientation, and set adaptive thresholds.
  - Gesture Detection: Track motion patterns using time-windowed averages of gyroscope and accelerometer data. Match with predefined motion templates.
  - Bluetooth Data Transmission: Send ASCII codes representing gesture IDs to the paired Windows device.
  - Windows Automation: The Win32 C++ handler decodes the received gestures and invokes mapped commands.

### 5. Expected Outcomes
  - Real-time gesture detection with minimal latency (<200 ms).
  - Seamless Windows automation without keyboard/mouse input.
  - Expandable framework to include gestures for media control or presentations.
  - Demonstrates practical realization of sensor-based gesture recognition discussed in Ojeda-Castelo et al., emphasizing AI-assisted interfaces.

### 6. Conclusion
The project integrates IoT, embedded sensing, and automation concepts to create a natural, contactless interface between humans and computers. The use of sensor-based recognition complements the AI-based advancements noted in the reference paper by prioritizing speed and reliability for real-world applications like Windows control, aligning with the Natural User Interaction (NUI) paradigm.
