// Pin assignments
const int potPin = A0;
const int dirPin1 = A1;  // Forward
const int dirPin2 = A2;  // Reverse

const int IN1 = 2;
const int IN2 = 3;
const int ENA1 = 9; // PWM

const int IN3 = 4;
const int IN4 = 5;
const int ENA2 = 10; // PWM

void setup() {
  pinMode(potPin, INPUT);
  pinMode(dirPin1, INPUT_PULLUP);
  pinMode(dirPin2, INPUT_PULLUP);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA1, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA2, OUTPUT);

  Serial.begin(9600);
  Serial.println("Motor controller initialized.");
}

void loop() {
  int potValue = analogRead(potPin);
  int speed = potValue / 4; // Map 0–1023 to 0–255

  bool forward = digitalRead(dirPin1) == LOW;
  bool reverse = digitalRead(dirPin2) == LOW;

  Serial.print("Potentiometer: ");
  Serial.print(potValue);
  Serial.print(" | Speed PWM: ");
  Serial.print(speed);

  if (forward) {
    Serial.println(" | Direction: FORWARD");

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA1, speed);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA2, speed);

  } else if (reverse) {
    Serial.println(" | Direction: REVERSE");

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA1, speed);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA2, speed);

  } else {
    Serial.println(" | Direction: NEUTRAL");

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA1, 0); // Float or stop

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA2, 0);
  }

  delay(200);
}
