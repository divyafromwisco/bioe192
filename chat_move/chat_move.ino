// Large system stepper driver
const int largeStepPin = 7;
const int largeDirPin  = 4;
const int largeEnablePin = 10;

// Screw parameters
const int largeStepsPerRev = 200;
const float largeLeadMmPerRev = 8.0;
const float largeStepsPerMm = largeStepsPerRev / largeLeadMmPerRev;

// Distance to move (change this)
float x_large_mm = 10.0;

// Speed (increase if vibrating)
int stepDelayUs = 5000;

// --------------------

void setup() {
  Serial.begin(9600);

  pinMode(largeStepPin, OUTPUT);
  pinMode(largeDirPin, OUTPUT);
  pinMode(largeEnablePin, OUTPUT);

  digitalWrite(largeEnablePin, HIGH); // start disabled

  delay(2000); // give time before starting

  Serial.println("Moving large actuator UP...");

  moveLargeUp(x_large_mm);

  Serial.println("Done.");
}

void loop() {
  // do nothing
}

// --------------------

void moveLargeUp(float distance_mm) {
  digitalWrite(largeEnablePin, LOW); // enable motor
  delay(10);

  long stepsToMove = round(distance_mm * largeStepsPerMm);

  // TRUE = UP (change if wrong direction)
  digitalWrite(largeDirPin, HIGH);

  delayMicroseconds(100);

  for (long i = 0; i < stepsToMove; i++) {
    digitalWrite(largeStepPin, HIGH);
    delayMicroseconds(stepDelayUs);

    digitalWrite(largeStepPin, LOW);
    delayMicroseconds(stepDelayUs);
  }

  digitalWrite(largeEnablePin, HIGH); // disable motor
}