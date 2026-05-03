// -------------------- PINS --------------------

const int largeStepPin = 7;
const int largeDirPin  = 4;
const int largeEnablePin = 10;


// -------------------- SCREW SETTINGS --------------------

const int largeStepsPerRev = 200;
const float largeLeadMmPerRev = 8.0;
const float largeStepsPerMm = largeStepsPerRev / largeLeadMmPerRev;


// -------------------- SPEED --------------------

int stepDelayUs = 1000;


// -------------------- SETUP --------------------

void setup() {
  Serial.begin(9600);

  pinMode(largeStepPin, OUTPUT);
  pinMode(largeDirPin, OUTPUT);
  pinMode(largeEnablePin, OUTPUT);

  digitalWrite(largeEnablePin, HIGH); // disabled

  Serial.println("Large motor ready.");
  Serial.println("Commands:");
  Serial.println("u <mm>  → move UP");
  Serial.println("d <mm>  → move DOWN");
}


// -------------------- MOVE FUNCTION --------------------

void moveLarge(bool direction, float distance_mm) {
  digitalWrite(largeEnablePin, LOW);
  delay(10);

  long stepsToMove = round(distance_mm * largeStepsPerMm);

  digitalWrite(largeDirPin, direction ? HIGH : LOW);
  delayMicroseconds(100);

  Serial.print(direction ? "Large moving UP " : "Large moving DOWN ");
  Serial.print(distance_mm);
  Serial.println(" mm");

  for (long i = 0; i < stepsToMove; i++) {
    digitalWrite(largeStepPin, HIGH);
    delayMicroseconds(stepDelayUs);

    digitalWrite(largeStepPin, LOW);
    delayMicroseconds(stepDelayUs);
  }

  digitalWrite(largeEnablePin, HIGH);
  delay(10);
}


// -------------------- LOOP --------------------

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    char dir = input.charAt(0);
    float value = input.substring(1).toFloat();

    if (value <= 0) {
      Serial.println("Invalid distance.");
      return;
    }

    if (dir == 'u' || dir == 'U') {
      moveLarge(true, value);
    }
    else if (dir == 'd' || dir == 'D') {
      moveLarge(false, value);
    }
    else {
      Serial.println("Invalid command. Use u or d.");
    }
  }
}