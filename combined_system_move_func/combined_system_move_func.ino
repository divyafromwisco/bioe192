// Large system stepper driver
const int largeStepPin = 7;
const int largeDirPin  = 4;
const int largeEnablePin = 10;

// Small system stepper driver
const int smallStepPin = 9;
const int smallDirPin  = 8;
const int smallEnablePin = 11;


// Large system screw
const int largeStepsPerRev = 200;
const float largeLeadMmPerRev = 8.0;
const float largeStepsPerMm = largeStepsPerRev / largeLeadMmPerRev;

// Small system screw
const int smallStepsPerRev = 200;
const float smallLeadMmPerRev = 8.0;
const float smallStepsPerMm = smallStepsPerRev / smallLeadMmPerRev;


// Run Distance 

float x_large_mm = 20.0;  // distance to move large system up for puncture
float y_small_mm = 10.0;  // distance to pull plunger commented out by divya may need it however

//TO DO: Add a calculation for the distance to pull plunger based on given ml, aka multiply ml by 6 mm/ml

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

  pinMode(largeEnablePin, OUTPUT);
  pinMode(smallEnablePin, OUTPUT);

  digitalWrite(largeEnablePin, HIGH); // start disabled
  digitalWrite(smallEnablePin, HIGH); // start disabled

  Serial.println("System ready.");
  Serial.println("Enter volume from 0 to 10 ml to start sequence");
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
  digitalWrite(largeEnablePin, LOW); //changed to accomadate the enable pin
  delay(10);
  long stepsToMove = round(distance_mm * largeStepsPerMm);

  Serial.print("Large actuator moving ");
  Serial.print(direction ? "UP " : "DOWN ");
  Serial.print(distance_mm);
  Serial.println(" mm");

  moveStepper(largeStepPin, largeDirPin, direction, stepsToMove, stepDelayUs);
  digitalWrite(largeEnablePin, HIGH); // added to disable motor after moving for enable pin
  delay(10);
}


// -------------------- SMALL ACTUATOR FUNCTION --------------------

void moveLinearSmall(bool direction, float distance_mm, int stepDelayUs) {
  digitalWrite(smallEnablePin, LOW); //changed to accomadate the enable pin
  delay(10);
  long stepsToMove = round(distance_mm * smallStepsPerMm);

  Serial.print("Small actuator moving ");
  Serial.print(direction ? "UP " : "DOWN ");
  Serial.print(distance_mm);
  Serial.println(" mm");

  moveStepper(smallStepPin, smallDirPin, direction, stepsToMove, stepDelayUs);
  digitalWrite(smallEnablePin, HIGH); // added to disable motor after moving for enable pin
  delay(10);
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
  Serial.println("Enter volume from 0 to 10 ml to start again.");

  currentState = IDLE;
}


// -------------------- LOOP --------------------

void loop() {
  if (Serial.available() > 0) {

    if (currentState == IDLE) {
      float input_mL = Serial.parseFloat();

      // Clear leftover newline characters
      while (Serial.available() > 0) {
        Serial.read();
      }

      if (input_mL >= 0 && input_mL <= 10) {
        y_small_mm = input_mL * 6.0;

        Serial.print("Input volume: ");
        Serial.print(input_mL);
        Serial.println(" mL");

        Serial.print("Calculated plunger distance: ");
        Serial.print(y_small_mm);
        Serial.println(" mm");

        runStartSequence();
      } 
      else {
        Serial.println("Invalid input. Enter a value from 0 to 10 mL.");
      }
    }

    else if (currentState == WAIT_FOR_R) {
      char command = Serial.read();

      if (command == '\n' || command == '\r') {
        return;
      }

      if (command == 'r') {
        runReturnSequence();
      } 
      else {
        Serial.println("Send 'r' to move small actuator back up.");
      }
    }
  }
}