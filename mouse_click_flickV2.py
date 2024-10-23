import serial
import pyautogui
import time

# Adjust the COM port and baud rate as per your setup
ser = serial.Serial('COM7', 9600)  # Update COM port if needed
ser.flush()

# Sensitivity settings for mouse movement
MOUSE_SENSITIVITY = 55

# Flick detection settings for clicking
CLICK_FLICK_THRESHOLD = 3.0  # Change in acceleration for a flick
DEBOUNCE_TIME = 1  # Debounce time to prevent multiple clicks
last_click_time = 0  # Timestamp of the last click

# Store previous acceleration values for flick detection
prev_accel_x, prev_accel_y, prev_accel_z = 0, 0, 0

while True:
    try:
        if ser.in_waiting > 0:
            # Read the serial data and decode it
            line = ser.readline().decode('utf-8').strip()

            # Detect mouse movement using Accel data
            if "Accel" in line:
                accel_data = line.split(", ")

                # Example input: "Accel X: 0.12, Y: -9.81, Z: 0.56"
                accel_x = float(accel_data[0].split(": ")[1])
                accel_y = float(accel_data[1].split(": ")[1])
                accel_z = float(accel_data[2].split(": ")[1])

                # Move the mouse based on X and Y acceleration
                if accel_x > 2:
                    pyautogui.moveRel(MOUSE_SENSITIVITY, 0)  # Move right
                elif accel_x < -2:
                    pyautogui.moveRel(-MOUSE_SENSITIVITY, 0)  # Move left

                if accel_y > 2:
                    pyautogui.moveRel(0, MOUSE_SENSITIVITY)  # Move down
                elif accel_y < -2:
                    pyautogui.moveRel(0, -MOUSE_SENSITIVITY)  # Move up

                # Flick detection for clicking
                current_time = time.time()
                if (current_time - last_click_time) > DEBOUNCE_TIME:
                    # Check for a sudden change in acceleration for a flick gesture
                    delta_x = abs(accel_x - prev_accel_x)
                    delta_y = abs(accel_y - prev_accel_y)

                    # Detect a flick on the X-axis (left or right flick) or Y-axis (up or down flick)
                    if delta_x > CLICK_FLICK_THRESHOLD:
                        pyautogui.click(button='right')  # right-click the mouse
                        last_click_time = current_time  # Update the last click timestamp
                        print(f"Click detected with flick (ΔX: {delta_x}")

                    if delta_y > CLICK_FLICK_THRESHOLD:
                        pyautogui.click(button='left')  # left-click the mouse
                        last_click_time = current_time  # Update the last click timestamp
                        print(f"Click detected with flick (ΔY: {delta_y})")

                # Update previous acceleration values for the next loop
                prev_accel_x = accel_x
                prev_accel_y = accel_y
                prev_accel_z = accel_z

    except Exception as e:
        print(f"Error: {e}")
        ser.close()
        break
