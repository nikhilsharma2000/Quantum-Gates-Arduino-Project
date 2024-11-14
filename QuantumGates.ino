#include <LiquidCrystal.h>
#include <RotaryEncoder.h>
//#include <stdio.h>
//#include <string.h>

// Define switch pins for classical input (LEDs)
const int switchPin1 = 6;
const int switchPin2 = 13;
const int switchPin3 = 5;

// Define LED pins
const int ledPin1 = 6;
const int ledPin2 = 13;
const int ledPin3 = 5;

// Rotary Encoder pins
const int encoderPinA = 4;
const int encoderPinB = 3;
const int buttonPin = 2;

// Initialize RotaryEncoder object
RotaryEncoder encoder(encoderPinA, encoderPinB);

// Define LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);  // Modify these pins as per your wiring

// Variables for storing classical inputs
int qubit1, qubit2, qubit3;
int selectedGate = 0;  // Gate selection (0 to 5 for X, Y, Z, H, S, T)
float randomPhase;     // Random phase between 0 and pi/2

// Gate names
const char* gates[] = {"X", "Y", "Z", "H", "S", "T"};

// Function to generate a random phase between 0 and pi/2
float generateRandomPhase() {
  return random(0, 157) / 100.0; // π/2 ≈ 1.57, scale by 100
}

// Setup function
void setup() {
  // Initialize switch and LED pins
  pinMode(switchPin1, INPUT);
  pinMode(switchPin2, INPUT);
  pinMode(switchPin3, INPUT);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  // Initialize LCD
  lcd.begin(16, 2);  // Initialize 16x2 LCD

  // Initialize serial monitor for debugging
  Serial.begin(9600);

  // Seed random generator with a unique value
  randomSeed(analogRead(A0));

  // Initialize rotary encoder
  pinMode(buttonPin, INPUT_PULLUP);  // Rotary encoder push button
}

// Function to display each section sequentially on the LCD
void displayLCDSection(int q1, int q2, int q3, const char* gate, float phase, int result) {
  
  lcd.clear();

  float total_phase=0.00;
  
  if (q1 == 1 && gate == "S"){
    total_phase = phase+1.57;
  }
  else if (q1 == 1 && gate == "T"){
    total_phase = phase+0.79;
  }
  else if(q1 == 1 && gate == "Y"){
    total_phase = -1*phase;
  }
  else if(q1 == 1 && gate == "Z"){
    total_phase == -1*phase;
  }
  else {
    total_phase = phase;
  }
  
  // Step 1: Display qubits and gate selection
  lcd.setCursor(0, 0);
  lcd.print("Q:");
  lcd.print(q1);
  lcd.print(q2);
  lcd.print(q3);
  
  
  lcd.setCursor(6, 0);
  lcd.print("G:");
  lcd.print(gate);
    // 3-second delay

  // Step 2: Display phase
  //lcd.clear();
  lcd.setCursor(10, 0);
  lcd.print("P:");
  lcd.print(phase, 2);  // Show phase with two decimal places
    // 3-second delay

  // Step 3: Display output
  //lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("O:");
  lcd.print(result, BIN);  // Output as a binary number (0 to 7)
    // 3-second delay

  lcd.setCursor(6, 1);
  lcd.print("OP:e^i");
  lcd.print(total_phase, 2);
  
}

// Quantum gate operations for 3-bit input
int applyGate(int gate, int q1, int q2, int q3, float phase) {
  //float added_phase = 0.00;
  int result = (q1 << 2) | (q2 << 1) | q3; // 3-bit input represented as integer (0-7)

  switch(gate) {
    case 0: // X Gate (NOT)
      result ^= 0b111;  // Flip all bits
      break;

    case 1: // Y Gate (Simulate as a bitwise NOT with phase shift approximation)
      result ^= 0b111;
      break;

    case 2: // Z Gate (Apply phase to all bits, but keep classical bits unchanged)
      break;

    case 3: // H Gate (Hadamard approximation)
      result = ((~q1) << 2) | ((~q2) << 1) | (~q3); // Flip all bits
      break;

    case 4: // S Gate (Simulate phase gate, no classical bit changes)
      //added_phase = 1.57;
      break;

    case 5: // T Gate (Similar to S but with different phase shift)
      //added_phase = 0.79;
      break;
  }

  
  return result;  // Return the modified 3-bit result
}

// Main loop
void loop() {
  // Read manual input from switches
int switchState1=0;
int switchState2=0;
int switchState3=0;
  switchState1 = digitalRead(switchPin1); // Read switch state
  if (switchState1 == LOW) {             // LOW if pressed with pull-up
    qubit1 = 0;         
  } else {
    qubit1 = 1;
  }
  switchState2 = digitalRead(switchPin2); // Read switch state
  if (switchState2 == LOW) {             // LOW if pressed with pull-up
    qubit2 = 0;         
  } else {
    qubit2 = 1;
  }
  switchState3 = digitalRead(switchPin3); // Read switch state
  if (switchState3 == LOW) {             // LOW if pressed with pull-up
    qubit3 = 0;         
  } else {
    qubit3 = 1;
  }

  // Update rotary encoder and calculate position
  encoder.tick();
  long newPosition = encoder.getPosition();
  if (newPosition != selectedGate) {
    selectedGate = newPosition % 6;  // Only 6 gates (0 to 5)
    if (selectedGate < 0) selectedGate += 6;  // Handle negative positions
  }

  // If rotary encoder button is pressed, generate a new random phase
  if (digitalRead(buttonPin) == LOW) {
    randomPhase = generateRandomPhase();
    delay(500);  // Debounce delay
  }

  // Apply the selected gate and phase to the 3-bit input
  int result = applyGate(selectedGate, qubit1, qubit2, qubit3, randomPhase);

  // Sequentially display inputs, selected gate, phase, and output on the LCD
  displayLCDSection(qubit1, qubit2, qubit3, gates[selectedGate], randomPhase, result);

  // Small delay for stability
  delay(200);
}
