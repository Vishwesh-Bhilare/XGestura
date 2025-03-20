import serial
import pyautogui

# Set up serial connection (Update COM port if needed)
ser = serial.Serial('COM5', 115200)  # Change 'COM3' to your ESP32 port

# Adjust sensitivity
SENSITIVITY_X = 70  # Cursor speed (left/right)
SENSITIVITY_Y = 70  # Cursor speed (up/down)

# Get screen size
screen_width, screen_height = pyautogui.size()

while True:
    try:
        # Read a line from Serial
        line = ser.readline().decode('utf-8').strip()

        # Extract acceleration values
        if "Accel:" in line:
            parts = line.split()
            ax = int(parts[1].split("=")[1])
            ay = int(parts[2].split("=")[1])
            
            # Normalize values
            move_x = int((ax / 16384) * SENSITIVITY_X)  # MPU6050 full scale = 16384
            move_y = int((ay / 16384) * SENSITIVITY_Y)

            # Get current mouse position
            cur_x, cur_y = pyautogui.position()

            # Move mouse cursor
            new_x = min(max(cur_x + move_x, 0), screen_width)  # Keep within screen bounds
            new_y = min(max(cur_y - move_y, 0), screen_height)  # Invert Y-axis

            pyautogui.moveTo(new_x, new_y, duration=0.1)

    except Exception as e:
        print("Error:", e)
