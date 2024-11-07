import cv2
import mediapipe as mp
import pyautogui

# Initialize MediaPipe and PyAutoGUI
mp_hands = mp.solutions.hands
mp_drawing = mp.solutions.drawing_utils
hand_detector = mp_hands.Hands(max_num_hands=1, min_detection_confidence=0.7)

# Set up video capture
cap = cv2.VideoCapture(0)

# Variables for click detection and filtering
clicking = False
filtered_x, filtered_y = 0, 0  # Initialize filtered coordinates
alpha = 0.2  # Smoothing factor (between 0 and 1, lower is smoother)

def move_mouse(finger_tip_x, finger_tip_y):
    """Move mouse based on the filtered finger coordinates."""
    global filtered_x, filtered_y

    # Low-pass filter calculation
    filtered_x = alpha * finger_tip_x + (1 - alpha) * filtered_x
    filtered_y = alpha * finger_tip_y + (1 - alpha) * filtered_y

    # Convert normalized coordinates to screen coordinates
    screen_width, screen_height = pyautogui.size()
    mouse_x = int(filtered_x * screen_width)
    mouse_y = int(filtered_y * screen_height)
    pyautogui.moveTo(mouse_x, mouse_y)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Flip frame and process with MediaPipe
    frame = cv2.flip(frame, 1)
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hand_detector.process(rgb_frame)

    if result.multi_hand_landmarks:
        for hand_landmarks in result.multi_hand_landmarks:
            # Draw landmarks for visualization
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            # Get fingertip coordinates (index finger tip - landmark 8)
            index_finger_tip = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP]
            move_mouse(index_finger_tip.x, index_finger_tip.y)

            # Check for pinch gesture (index finger tip close to thumb tip)
            thumb_tip = hand_landmarks.landmark[mp_hands.HandLandmark.THUMB_TIP]
            distance = ((index_finger_tip.x - thumb_tip.x) ** 2 + (index_finger_tip.y - thumb_tip.y) ** 2) ** 0.5

            if distance < 0.05:  # Adjust threshold for pinch detection
                if not clicking:
                    pyautogui.click()  # Left click
                    clicking = True
            else:
                clicking = False

    # Show the frame (optional)
    cv2.imshow('Hand Tracking', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Clean up
cap.release()
cv2.destroyAllWindows()
