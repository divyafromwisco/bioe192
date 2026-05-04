// -------------------- PINS --------------------

// Large system stepper driver
const int largeStepPin = 7;
const int largeDirPin  = 4;
const int largeEnablePin = 10;

// Small system stepper driver
const int smallStepPin = 9;
const int smallDirPin  = 8;
const int smallEnablePin = 11;


// -------------------- SCREW SETTINGS --------------------

const int largeStepsPerRev = 200;
const float largeLeadMmPerRev = 8.0;
const float largeStepsPerMm = largeStepsPerRev / largeLeadMmPerRev;

const int smallStepsPerRev = 200;
const float smallLeadMmPerRev = 8.0;
const float smallStepsPerMm = smallStepsPerRev / smallLeadMmPerRev;


// -------------------- DISTANCES --------------------

float x_large_mm = 60.0;  // large motor puncture distance
float y_small_mm = 48.0;  // small motor plunger distance


// -------------------- SPEED --------------------

int largeStepDelayUs = 1000;
int smallStepDelayUs = 1000;


// -------------------- STATE --------------------

enum State {
  IDLE,
  WAIT_FOR_RESET
};

State currentState = IDLE;


// -------------------- SETUP --------------------

void setup() {
  Serial.begin(9600);

  pinMode(largeStepPin, OUTPUT);
  pinMode(largeDirPin, OUTPUT);
  pinMode(largeEnablePin, OUTPUT);

  pinMode(smallStepPin, OUTPUT);
  pinMode(smallDirPin, OUTPUT);
  pinMode(smallEnablePin, OUTPUT);

  digitalWrite(largeEnablePin, HIGH); // disabled
  digitalWrite(smallEnablePin, HIGH); // disabled

  Serial.println("System ready.");
  Serial.println("Send 's' to start sequence.");
}


// -------------------- BASIC STEPPER MOVE --------------------

void moveStepper(int stepPin, int dirPin, int enablePin, bool direction, long stepsToMove, int stepDelayUs) {
  digitalWrite(enablePin, LOW);   // enable motor
  delay(10);

  digitalWrite(dirPin, direction ? HIGH : LOW);
  delayMicroseconds(100);

  for (long i = 0; i < stepsToMove; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelayUs);

    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelayUs);
  }

  digitalWrite(enablePin, HIGH);  // disable motor
  delay(10);
}


// -------------------- LARGE MOTOR --------------------

void moveLargeUp(float distance_mm) {
  long stepsToMove = round(distance_mm * largeStepsPerMm);

  Serial.print("Large motor moving UP ");
  Serial.print(distance_mm);
  Serial.println(" mm");

  moveStepper(largeStepPin, largeDirPin, largeEnablePin, true, stepsToMove, largeStepDelayUs);
}


void moveLargeDown(float distance_mm) {
  long stepsToMove = round(distance_mm * largeStepsPerMm);

  Serial.print("Large motor moving DOWN ");
  Serial.print(distance_mm);
  Serial.println(" mm");

  moveStepper(largeStepPin, largeDirPin, largeEnablePin, false, stepsToMove, largeStepDelayUs);
}


// -------------------- SMALL MOTOR --------------------

void moveSmallDown(float distance_mm) {
  long stepsToMove = round(distance_mm * smallStepsPerMm);

  Serial.print("Small motor moving DOWN ");
  Serial.print(distance_mm);
  Serial.println(" mm");

  moveStepper(smallStepPin, smallDirPin, smallEnablePin, true, stepsToMove, smallStepDelayUs);
}


void moveSmallUp(float distance_mm) {
  long stepsToMove = round(distance_mm * smallStepsPerMm);

  Serial.print("Small motor moving UP ");
  Serial.print(distance_mm);
  Serial.println(" mm");

  moveStepper(smallStepPin, smallDirPin, smallEnablePin, false, stepsToMove, smallStepDelayUs);
}


// -------------------- MAIN SEQUENCE --------------------

void runStartSequence() {
  Serial.println("Starting sequence...");

  // 1. Large system moves up x
  moveLargeUp(x_large_mm);
  delay(500);

  // 2. Small system moves down y
  moveSmallDown(y_small_mm);
  delay(10000);

  // 3. Large system moves down x
  moveLargeDown(x_large_mm);
  delay(500);

  Serial.println("Sequence complete.");
  Serial.println("Send 'r' to reset small motor upward.");

  currentState = WAIT_FOR_RESET;
}


// -------------------- RESET SEQUENCE --------------------

void runResetSequence() {
  Serial.println("Resetting small motor...");

  // 4. Small system moves back up y
  moveSmallUp(y_small_mm);

  Serial.println("Reset complete.");
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
    else if (currentState == WAIT_FOR_RESET && command == 'r') {
      runResetSequence();
    }
    else {
      Serial.println("Invalid command.");
    }
  }
}
