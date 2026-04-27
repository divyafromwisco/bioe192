// -------------------- PIN DEFINITIONS --------------------

// Large linear actuator stepper driver
const int largeStepPin = 7;
const int largeDirPin  = 4;

// Small linear actuator stepper driver
const int smallStepPin = 9;
const int smallDirPin  = 8;


// -------------------- MOTOR SETTINGS --------------------

// Large actuator screw
const int largeStepsPerRev = 200;
const float largeLeadMmPerRev = 8.0;
const float largeStepsPerMm = largeStepsPerRev / largeLeadMmPerRev;

// Small actuator screw
const int smallStepsPerRev = 200;
const float smallLeadMmPerRev = 8.0;
const float smallStepsPerMm = smallStepsPerRev / smallLeadMmPerRev;


// -------------------- MOTION DISTANCES --------------------

float x_large_mm = 20.0;  // large actuator distance
float y_small_mm = 10.0;  // small actuator distance

int largeStepDelayUs = 500;
int smallStepDelayUs = 500;


// -------------------- STATE MACHINE --------------------

enum State {
  IDLE,
  WAIT_FOR_R
};

State currentState = IDLE;


// -------------------- SETUP --------------------

void setup() {
  Serial.begin(9600);

  pinMode(largeStepPin, OUTPUT);
  pinMode(largeDirPin, OUTPUT);

  pinMode(smallStepPin, OUTPUT);
  pinMode(smallDirPin, OUTPUT);

  Serial.println("System ready.");
  Serial.println("Send 's' to start sequence.");
}


// -------------------- BASIC MOVE FUNCTION --------------------

void moveStepper(int stepPin, int dirPin, bool direction, long stepsToMove, int stepDelayUs) {
  digitalWrite(dirPin, direction ? HIGH : LOW);

  delayMicroseconds(10); // small setup time for direction pin

  for (long i = 0; i < stepsToMove; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelayUs);

    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelayUs);
  }
}


// -------------------- LARGE ACTUATOR FUNCTION --------------------

void moveLinearLarge(bool direction, float distance_mm, int stepDelayUs) {
  long stepsToMove = round(distance_mm * largeStepsPerMm);

  Serial.print("Large actuator moving ");
  Serial.print(direction ? "UP " : "DOWN ");
  Serial.print(distance_mm);
  Serial.println(" mm");

  moveStepper(largeStepPin, largeDirPin, direction, stepsToMove, stepDelayUs);
}


// -------------------- SMALL ACTUATOR FUNCTION --------------------

void moveLinearSmall(bool direction, float distance_mm, int stepDelayUs) {
  long stepsToMove = round(distance_mm * smallStepsPerMm);

  Serial.print("Small actuator moving ");
  Serial.print(direction ? "UP " : "DOWN ");
  Serial.print(distance_mm);
  Serial.println(" mm");

  moveStepper(smallStepPin, smallDirPin, direction, stepsToMove, stepDelayUs);
}


// -------------------- MAIN SEQUENCE --------------------

void runStartSequence() {
  Serial.println("Starting sequence...");

  // 1. Large system moves up x
  moveLinearLarge(true, x_large_mm, largeStepDelayUs);
  delay(500);

  // 2. Small system moves down y
  moveLinearSmall(false, y_small_mm, smallStepDelayUs);
  delay(500);

  // 3. Large system moves down x
  moveLinearLarge(false, x_large_mm, largeStepDelayUs);
  delay(500);

  Serial.println("Sequence complete.");
  Serial.println("Send 'r' to move small actuator back up.");

  currentState = WAIT_FOR_R;
}


// -------------------- RESET SMALL ACTUATOR --------------------

void runReturnSequence() {
  Serial.println("Returning small actuator...");

  // 4. Small system moves back up y
  moveLinearSmall(true, y_small_mm, smallStepDelayUs);

  Serial.println("Small actuator returned.");
  Serial.println("Send 's' to start again.");

  currentState = IDLE;
}


// -------------------- LOOP --------------------

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (command == '\n' || command == '\r') {
      return;
    }

    if (currentState == IDLE && command == 's') {
      runStartSequence();
    }

    else if (currentState == WAIT_FOR_R && command == 'r') {
      runReturnSequence();
    }

    else {
      Serial.println("Invalid command for current state.");
    }
  }
}