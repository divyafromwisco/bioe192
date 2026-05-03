// -------------------- PINS --------------------

const int smallStepPin = 9;
const int smallDirPin  = 8;
const int smallEnablePin = 11;


// -------------------- SCREW SETTINGS --------------------

const int smallStepsPerRev = 200;
const float smallLeadMmPerRev = 8.0;
const float smallStepsPerMm = smallStepsPerRev / smallLeadMmPerRev;


// -------------------- SPEED --------------------

int stepDelayUs = 1000;


// -------------------- SETUP --------------------

void setup() {
  Serial.begin(9600);

  pinMode(smallStepPin, OUTPUT);
  pinMode(smallDirPin, OUTPUT);
  pinMode(smallEnablePin, OUTPUT);

  digitalWrite(smallEnablePin, HIGH); // disabled

  Serial.println("Ready.");
  Serial.println("Commands:");
  Serial.println("u <mm>  → move UP");
  Serial.println("d <mm>  → move DOWN");
}


// -------------------- MOVE FUNCTION --------------------

void moveSmall(bool direction, float distance_mm) {
  digitalWrite(smallEnablePin, LOW);
  delay(10);

  long stepsToMove = round(distance_mm * smallStepsPerMm);

  digitalWrite(smallDirPin, direction ? HIGH : LOW);
  delayMicroseconds(100);

  Serial.print(direction ? "Moving UP " : "Moving DOWN ");
  Serial.print(distance_mm);
  Serial.println(" mm");

  for (long i = 0; i < stepsToMove; i++) {
    digitalWrite(smallStepPin, HIGH);
    delayMicroseconds(stepDelayUs);

    digitalWrite(smallStepPin, LOW);
    delayMicroseconds(stepDelayUs);
  }

  digitalWrite(smallEnablePin, HIGH);
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
      moveSmall(false, value);
    }
    else if (dir == 'd' || dir == 'D') {
      moveSmall(true, value);
    }
    else {
      Serial.println("Invalid command. Use u or d.");
    }
  }
}
