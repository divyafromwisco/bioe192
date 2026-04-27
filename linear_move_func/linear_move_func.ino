//small subsytem stuff
const int stepPin = 7;
const int dirPin  = 4;
const int enablePin = 10;

//large system stuff



const int stepsPerRev = 200;
const float lead_mm_per_rev = 8.0;   // 8 mm linear travel per revolution
const float stepsPerMm = stepsPerRev / lead_mm_per_rev;   // 25 steps/mm

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

// direction: true = up, false = down
// distance_mm: how far to move linearly
// stepDelayUs: controls speed
void moveLinear(bool direction, float distance_mm, int stepDelayUs) {
  long stepsToMove = round(distance_mm * stepsPerMm);

  digitalWrite(dirPin, direction ? HIGH : LOW);

  for (long i = 0; i < stepsToMove; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelayUs);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelayUs);
  }
}

void loop() {
  moveLinear(true, 20.0, 500);   // move up 10 mm
  delay(1000);

  moveLinear(false, 20.0, 500);  // move down 10 mm
  delay(1000);

  while(true);
}
