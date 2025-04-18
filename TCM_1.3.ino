// === Pin assignments ===
const int potPin = A0;
const int dirPin1 = A1;  // Forward
const int dirPin2 = A2;  // Reverse

const int IN1 = 2;
const int IN2 = 3;
const int ENA1 = 9; // PWM - Timer1
const int IN3 = 4;
const int IN4 = 5;
const int ENA2 = 10; // PWM - Timer1

// === Direction states ===
enum Direction { NEUTRAL, FORWARD, REVERSE };
Direction lastDirection = NEUTRAL;
Direction currentDirection = NEUTRAL;
unsigned long directionChangeTime = 0;
const unsigned long directionDelay = 2000; // 2 seconds

// PWM writing using Timer1
void writePWM_T1(int pin, int val) {
  val = constrain(val, 0, 255);
  int duty = map(val, 0, 255, 0, ICR1);
  if (pin == 9) {
    OCR1A = duty;
  } else if (pin == 10) {
    OCR1B = duty;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Motor controller initialized.");

  pinMode(potPin, INPUT);
  pinMode(dirPin1, INPUT_PULLUP);
  pinMode(dirPin2, INPUT_PULLUP);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA1, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA2, OUTPUT);

  // Set up Timer1 for 10kHz PWM
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);
  ICR1 = 1600;
}

void loop() {
  int potValue = analogRead(potPin);
  int speed = potValue / 4;

  // Read direction switch
  bool forward = digitalRead(dirPin1) == LOW;
  bool reverse = digitalRead(dirPin2) == LOW;

  // Determine current intended direction
  if (forward) {
    currentDirection = FORWARD;
  } else if (reverse) {
    currentDirection = REVERSE;
  } else {
    currentDirection = NEUTRAL;
  }

  // Check if direction changed
  if (currentDirection != lastDirection) {
    directionChangeTime = millis();
    lastDirection = currentDirection;
    Serial.println("Direction changed - initiating delay");
  }

  // Time since direction changed
  unsigned long timeSinceChange = millis() - directionChangeTime;

  // Decide whether to activate motors
  bool enableMotors = (timeSinceChange >= directionDelay);

  // Debug output
  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print(" | PWM: ");
  Serial.print(speed);
  Serial.print(" | Direction: ");
  if (currentDirection == FORWARD) Serial.print("FORWARD");
  else if (currentDirection == REVERSE) Serial.print("REVERSE");
  else Serial.print("NEUTRAL");
  Serial.print(" | Motor Enabled: ");
  Serial.println(enableMotors ? "YES" : "WAITING");

  if (enableMotors) {
    if (currentDirection == FORWARD) {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      writePWM_T1(ENA1, speed);

      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      writePWM_T1(ENA2, speed);

    } else if (currentDirection == REVERSE) {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      writePWM_T1(ENA1, speed);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      writePWM_T1(ENA2, speed);

    } else { // Neutral
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, HIGH);
      writePWM_T1(ENA1, 0);

      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, HIGH);
      writePWM_T1(ENA2, 0);
    }
  } else {
    // Floating/off during delay
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    writePWM_T1(ENA1, 0);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    writePWM_T1(ENA2, 0);
  }

  delay(100); // update loop every 100ms
}
