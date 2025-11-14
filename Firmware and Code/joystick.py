import pygame
import serial
import time

# --- Configuration ---
ARDUINO_PORT = '/dev/ttyACM0' 
BAUD_RATE = 9600
JOYSTICK_THRESHOLD = 0.8

# --- Joystick Mapping ---
AXIS_MAP = {
    'L_STICK_Y': 1, # Left Stick Up/Down
    'R_STICK_Y': 3  # Right Stick Up/Down for All Up/Down
}

BUTTON_MAP = {
    'A': 0, 'B': 1, 'X': 2, 'Y': 3,
    'LB': 4, 'RB': 5, 
}

# --- Setup ---
pygame.init()
pygame.joystick.init()

if pygame.joystick.get_count() == 0:
    print("Error: No Joystick Found.")
    exit()

joystick = pygame.joystick.Joystick(0)
joystick.init()
print(f"Initialized Joystick: {joystick.get_name()}")

try:
    ser = serial.Serial(ARDUINO_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to Arduino on {ARDUINO_PORT}")
    time.sleep(2)
except serial.SerialException as e:
    print(f"Error: Could not open serial port {ARDUINO_PORT}. [{e}]")
    exit()

# --- Main Loop ---
try:
    last_command = None
    print("\n=== DIAGONAL OPPOSITE CONTROL MODE ===")
    print("🛑 SAFETY: Motors STOP immediately when controls released!")
    print("\nR-Stick UP/DOWN: All actuators move together")
    print("RB/LB: Front(1,2) vs Rear(3,4) - opposite diagonal")
    print("Y/A: Rear(3,4) vs Front(1,2) - opposite diagonal")
    print("X/B: Left(1,3) vs Right(2,4) - opposite diagonal")
    print("L-Stick UP/DOWN: Right(2,4) vs Left(1,3) - opposite diagonal")
    print("\nRunning...\n")
    
    while True:
        pygame.event.pump()
        current_command = 'S'  # Default to STOP
        
        # 1. Check Right Joystick first for All Up/Down control
        r_stick_y = joystick.get_axis(AXIS_MAP['R_STICK_Y'])
        
        if r_stick_y < -JOYSTICK_THRESHOLD:
            current_command = 'U' # All Up
        elif r_stick_y > JOYSTICK_THRESHOLD:
            current_command = 'D' # All Down
        
        # 2. Check Buttons only if the Right Stick is neutral
        if current_command == 'S':
            if joystick.get_button(BUTTON_MAP['RB']):
                current_command = 'W' # Front↑ Rear↓
            elif joystick.get_button(BUTTON_MAP['LB']):
                current_command = 'w' # Front↓ Rear↑
            elif joystick.get_button(BUTTON_MAP['Y']):
                current_command = 'H' # Rear↑ Front↓
            elif joystick.get_button(BUTTON_MAP['A']):
                current_command = 'h' # Rear↓ Front↑
            elif joystick.get_button(BUTTON_MAP['X']):
                current_command = 'L' # Left↑ Right↓
            elif joystick.get_button(BUTTON_MAP['B']):
                current_command = 'l' # Left↓ Right↑
        
        # 3. Check Left Joystick if nothing else is active
        if current_command == 'S':
            l_stick_y = joystick.get_axis(AXIS_MAP['L_STICK_Y'])
            if l_stick_y < -JOYSTICK_THRESHOLD:
                current_command = 'R' # Right↑ Left↓
            elif l_stick_y > JOYSTICK_THRESHOLD:
                current_command = 'r' # Right↓ Left↑
        
        # 4. 🛑 CRITICAL: ALWAYS send command (even if it's STOP)
        # This ensures motors stop immediately when controls are released
        if current_command != last_command:
            print(f"Sent command: {current_command}")
            ser.write(current_command.encode())
            last_command = current_command
        
        time.sleep(0.05)  # 50ms update rate

except KeyboardInterrupt:
    print("\nProgram stopped by user.")
finally:
    print("🛑 EMERGENCY STOP - Sending STOP command and closing serial port.")
    if 'ser' in locals() and ser.is_open:
        ser.write(b'S')
        ser.close()
    pygame.quit()