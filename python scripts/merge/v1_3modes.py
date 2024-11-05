import serial
import pyautogui
import time
import keyboard

# Set up serial communication (adjust the COM port as needed)
ser = serial.Serial('COM7', 9600, timeout=1)

# Sensitivity and thresholds
MOUSE_SENSITIVITY = 40
CLICK_FLICK_THRESHOLD = 6.0
DEBOUNCE_TIME = 2.0
last_click_time = 0
left_threshold = -8.0
right_threshold = 8.0
game_turn_threshold = 3.0
mode = "mouse"

def toggle_mode(new_mode):
    """Toggle between control modes."""
    global mode
    mode = new_mode
    print(f"Mode switched to {mode}")

def interpret_gesture(data):
    """Interpret gesture based on X-axis data for slide control."""
    try:
        x_value = float(data.split("Accel X:")[1].split(",")[0].strip())
        if x_value < left_threshold:
            return "previous"
        elif x_value > right_threshold:
            return "next"
    except ValueError:
        pass
    return None

def handle_mouse_control(data):
    """Handle mouse movement based on sensor data."""
    global last_click_time
    accel_x, accel_y = [float(val.split(": ")[1]) for val in data.split(", ")[:2]]
    # Move mouse based on acceleration
    pyautogui.moveRel(MOUSE_SENSITIVITY * (accel_x / abs(accel_x)) if abs(accel_x) > 2 else 0,
                      MOUSE_SENSITIVITY * (accel_y / abs(accel_y)) if abs(accel_y) > 2 else 0)

    # Flick detection for click
    current_time = time.time()
    if (current_time - last_click_time) > DEBOUNCE_TIME:
        if abs(accel_x) > CLICK_FLICK_THRESHOLD:
            pyautogui.click(button='right' if accel_x > 0 else 'left')
            last_click_time = current_time

def handle_slide_control(data):
    """Handle slide changes based on sensor data."""
    gesture = interpret_gesture(data)
    if gesture:
        pyautogui.press("right" if gesture == "next" else "left")
        print(f"{gesture.capitalize()} slide triggered")
        time.sleep(DEBOUNCE_TIME)  # Debounce

def handle_game_control(data):
    """Control steering in a racing game based on sensor data."""
    accel_x = float(data.split("Accel X:")[1].split(",")[0].strip())
    if abs(accel_x) > game_turn_threshold:
        direction = "right" if accel_x > 0 else "left"
        pyautogui.keyDown(direction)
        time.sleep(0.05 + 0.01 * abs(accel_x))  # Dynamic key press based on tilt
        pyautogui.keyUp(direction)

print("Starting... Press 'M' for mouse mode, 'S' for slide mode, or 'G' for game control mode.")

# Main loop
while True:
    # Check for mode toggle keys
    if keyboard.is_pressed('m'):
        toggle_mode("mouse")
        time.sleep(0.3)
    elif keyboard.is_pressed('s'):
        toggle_mode("slide")
        time.sleep(0.3)
    elif keyboard.is_pressed('g'):
        toggle_mode("game")
        time.sleep(0.3)

    # Read data and handle modes
    if ser.in_waiting > 0:
        data = ser.readline().decode('utf-8').strip()
        if "Accel" in data:
            if mode == "mouse":
                handle_mouse_control(data)
            elif mode == "slide":
                handle_slide_control(data)
            elif mode == "game":
                handle_game_control(data)
