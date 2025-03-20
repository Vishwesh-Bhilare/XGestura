import serial
import pyautogui

# Set up serial connection (Update COM port)
ser = serial.Serial('COM5', 115200)  # Change 'COM3' to your actual ESP32 port

# Sensitivity (Adjust if needed)
SENSITIVITY_X = 99
SENSITIVITY_Y = 99

# Dead zone to ignore small sensor noise
DEAD_ZONE = 300  # Ignore tiny movements to reduce jumps

while True:
    try:
        # Read a line from Serial
        line = ser.readline().decode('utf-8').strip()

        # Extract acceleration values
        if "Accel:" in line:
            parts = line.split()
            ax = int(parts[1].split("=")[1])
            ay = int(parts[2].split("=")[1])

            # Apply dead zone to remove unnecessary tiny movements
            if abs(ax) < DEAD_ZONE:
                ax = 0
            if abs(ay) < DEAD_ZONE:
                ay = 0

            # Move cursor directly
            pyautogui.moveRel(ax / 16384 * SENSITIVITY_X, -ay / 16384 * SENSITIVITY_Y)

    except Exception as e:
        print("Error:", e)
