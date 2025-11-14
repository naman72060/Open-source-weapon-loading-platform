/*
  ====================================================================
  ||    Arduino Code for 4-Actuator Control via Raspberry Pi     ||
  ====================================================================
  
  --- Actuator Layout ---
  Motor 1: Front-Left
  Motor 2: Front-Right
  Motor 3: Rear-Left
  Motor 4: Rear-Right

  --- NEW Command Mapping (DIAGONAL OPPOSITE MOVEMENTS) ---
  Joystick        -> RPi sends -> Arduino Action
  -------------------------------------------------------------
  R-Stick UP      ->    'U'    -> All Actuators UP
  R-Stick DOWN    ->    'D'    -> All Actuators DOWN
  RB              ->    'W'    -> Front (1,2) UP + Rear (3,4) DOWN
  LB              ->    'w'    -> Front (1,2) DOWN + Rear (3,4) UP
  Y Button        ->    'H'    -> Rear (3,4) UP + Front (1,2) DOWN
  A Button        ->    'h'    -> Rear (3,4) DOWN + Front (1,2) UP
  X Button        ->    'L'    -> Left (1,3) UP + Right (2,4) DOWN
  B Button        ->    'l'    -> Left (1,3) DOWN + Right (2,4) UP
  L-Stick UP      ->    'R'    -> Right (2,4) UP + Left (1,3) DOWN
  L-Stick DOWN    ->    'r'    -> Right (2,4) DOWN + Left (1,3) UP
  Button Release  ->    'S'    -> STOP ALL
*/

// Define pins for each motor driver
// Motor 1 (Front-Left)
const int dir1 = 26;
const int pwm1 = 11;
// Motor 2 (Front-Right)
const int dir2 = 36;
const int pwm2 = 8;
// Motor 3 (Rear-Left)
const int dir3 = 42;
const int pwm3 = 6;
// Motor 4 (Rear-Right)
const int dir4 = 48;
const int pwm4 = 4;

// Define a constant for motor speed (0-255)
// 255 is full speed (MAX PWM as requested)
const int actuatorSpeed = 255;

void setup() {
  // Start serial communication at 9600 baud rate
  Serial.begin(9600);
  
  // Set all control pins to OUTPUT mode
  pinMode(dir1, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(dir3, OUTPUT);
  pinMode(pwm3, OUTPUT);
  pinMode(dir4, OUTPUT);
  pinMode(pwm4, OUTPUT);
  
  // Ensure all actuators are stopped at the beginning
  stopAll();
  Serial.println("Arduino Ready. Waiting for joystick commands...");
  Serial.println("DIAGONAL OPPOSITE MODE: Front/Rear and Left/Right oppose each other!");
}

void loop() {
  // Check if there is data coming from the serial port
  if (Serial.available() > 0) {
    // Read the incoming character command
    char command = Serial.read();

    // Execute command based on the character received
    switch (command) {
      case 'U': allUp(); break;             // R-Stick UP: All Up
      case 'D': allDown(); break;           // R-Stick DOWN: All Down
      case 'W': frontWidthUp(); break;      // RB: Front(1,2)↑ Rear(3,4)↓
      case 'w': frontWidthDown(); break;    // LB: Front(1,2)↓ Rear(3,4)↑
      case 'H': rearWidthUp(); break;       // Y: Rear(3,4)↑ Front(1,2)↓
      case 'h': rearWidthDown(); break;     // A: Rear(3,4)↓ Front(1,2)↑
      case 'L': leftLengthUp(); break;      // X: Left(1,3)↑ Right(2,4)↓
      case 'l': leftLengthDown(); break;    // B: Left(1,3)↓ Right(2,4)↑
      case 'R': rightLengthUp(); break;     // L-Stick UP: Right(2,4)↑ Left(1,3)↓
      case 'r': rightLengthDown(); break;   // L-Stick DOWN: Right(2,4)↓ Left(1,3)↑
      case 'S': stopAll(); break;           // Button Release: Stop
    }
  }
}

// --- Base Actuator Functions ---
// Set direction to UP (extend) and apply speed
void actuatorUp(int dirPin, int pwmPin) {
  digitalWrite(dirPin, HIGH); // NOTE: This might be LOW on your wiring, swap if needed
  analogWrite(pwmPin, actuatorSpeed);
}

// Set direction to DOWN (retract) and apply speed
void actuatorDown(int dirPin, int pwmPin) {
  digitalWrite(dirPin, LOW); // NOTE: This might be HIGH on your wiring, swap if needed
  analogWrite(pwmPin, actuatorSpeed);
}

// Stop all actuators
void stopAll() {
  analogWrite(pwm1, 0);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 0);
  analogWrite(pwm4, 0);
}

// --- Mapped Movement Functions (DIAGONAL OPPOSITE MOVEMENTS) ---
void allUp() {
  actuatorUp(dir1, pwm1);
  actuatorUp(dir2, pwm2);
  actuatorUp(dir3, pwm3);
  actuatorUp(dir4, pwm4);
}

void allDown() {
  actuatorDown(dir1, pwm1);
  actuatorDown(dir2, pwm2);
  actuatorDown(dir3, pwm3);
  actuatorDown(dir4, pwm4);
}

// Front Width: Front (1,2) UP + Rear (3,4) DOWN
void frontWidthUp() {
  actuatorUp(dir1, pwm1);     // Front-Left UP
  actuatorUp(dir2, pwm2);     // Front-Right UP
  actuatorDown(dir3, pwm3);   // Rear-Left DOWN
  actuatorDown(dir4, pwm4);   // Rear-Right DOWN
}

// Front Width: Front (1,2) DOWN + Rear (3,4) UP
void frontWidthDown() {
  actuatorDown(dir1, pwm1);   // Front-Left DOWN
  actuatorDown(dir2, pwm2);   // Front-Right DOWN
  actuatorUp(dir3, pwm3);     // Rear-Left UP
  actuatorUp(dir4, pwm4);     // Rear-Right UP
}

// Rear Width: Rear (3,4) UP + Front (1,2) DOWN
void rearWidthUp() {
  actuatorUp(dir3, pwm3);     // Rear-Left UP
  actuatorUp(dir4, pwm4);     // Rear-Right UP
  actuatorDown(dir1, pwm1);   // Front-Left DOWN
  actuatorDown(dir2, pwm2);   // Front-Right DOWN
}

// Rear Width: Rear (3,4) DOWN + Front (1,2) UP
void rearWidthDown() {
  actuatorDown(dir3, pwm3);   // Rear-Left DOWN
  actuatorDown(dir4, pwm4);   // Rear-Right DOWN
  actuatorUp(dir1, pwm1);     // Front-Left UP
  actuatorUp(dir2, pwm2);     // Front-Right UP
}

// Left Length: Left (1,3) UP + Right (2,4) DOWN
void leftLengthUp() {
  actuatorUp(dir1, pwm1);     // Front-Left UP
  actuatorUp(dir3, pwm3);     // Rear-Left UP
  actuatorDown(dir2, pwm2);   // Front-Right DOWN
  actuatorDown(dir4, pwm4);   // Rear-Right DOWN
}

// Left Length: Left (1,3) DOWN + Right (2,4) UP
void leftLengthDown() {
  actuatorDown(dir1, pwm1);   // Front-Left DOWN
  actuatorDown(dir3, pwm3);   // Rear-Left DOWN
  actuatorUp(dir2, pwm2);     // Front-Right UP
  actuatorUp(dir4, pwm4);     // Rear-Right UP
}

// Right Length: Right (2,4) UP + Left (1,3) DOWN
void rightLengthUp() {
  actuatorUp(dir2, pwm2);     // Front-Right UP
  actuatorUp(dir4, pwm4);     // Rear-Right UP
  actuatorDown(dir1, pwm1);   // Front-Left DOWN
  actuatorDown(dir3, pwm3);   // Rear-Left DOWN
}

// Right Length: Right (2,4) DOWN + Left (1,3) UP
void rightLengthDown() {
  actuatorDown(dir2, pwm2);   // Front-Right DOWN
  actuatorDown(dir4, pwm4);   // Rear-Right DOWN
  actuatorUp(dir1, pwm1);     // Front-Left UP
  actuatorUp(dir3, pwm3);     // Rear-Left UP
}