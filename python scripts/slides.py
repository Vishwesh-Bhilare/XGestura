import serial
import pyautogui
import time

# Set up the serial connection (replace 'COMX' with your actual COM port, and set the baud rate to 9600)
ser = serial.Serial('COM7', 9600, timeout=1)  # Adjust 'COMX' to your specific HC-05 port

# Thresholds for detecting left or right gestures
left_threshold = -5.0  # Adjust based on MPU6050 sensitivity and readings
right_threshold = 5.0

def interpret_gesture(data):
    """
    Interpret the gesture based on accelerometer X-axis data.
    Returns 'next' for right movement, 'previous' for left movement, or None if no significant movement.
    """
    try:
        lines = data.splitlines()  # Separate data into lines
        for line in lines:
            if "Accel X:" in line:
                # Extract X-axis acceleration value
                x_value = float(line.split("X:")[1].split(",")[0].strip())
                
                if x_value < left_threshold:
                    return "previous"
                elif x_value > right_threshold:
                    return "next"
    except ValueError:
        # Handle any parsing errors gracefully
        pass
    return None

def main():
    print("Listening for gestures...")

    while True:
        if ser.in_waiting > 0:
            # Read the data from the Bluetooth serial port
            data = ser.read(ser.in_waiting).decode('utf-8')
            
            # Interpret the gesture
            gesture = interpret_gesture(data)
            
            if gesture == "next":
                pyautogui.press("right")  # Move to next slide
                print("Next slide triggered")
                time.sleep(0.5)  # Debounce to avoid multiple triggers
            elif gesture == "previous":
                pyautogui.press("left")  # Move to previous slide
                print("Previous slide triggered")
                time.sleep(0.5)  # Debounce to avoid multiple triggers

if __name__ == "__main__":
    main()
