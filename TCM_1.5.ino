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

int currentSpeed = 0;
unsigned long lastDirChange = 0;
int lastDirection = 0; // 0 = neutral, 1 = forward, -1 = reverse

// Function to write PWM using Timer1
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
  Serial.println("Motor controller with ramp-up initialized.");

  pinMode(potPin, INPUT);
  pinMode(dirPin1, INPUT_PULLUP);
  pinMode(dirPin2, INPUT_PULLUP);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA1, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA2, OUTPUT);

  // Timer1 @ ~10 kHz PWM (ICR1 = 1600)
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); // no prescaler
  ICR1 = 1600;
}

void loop() {
  int potValue = analogRead(potPin);
  int targetSpeed = potValue / 4;

  bool forward = digitalRead(dirPin1) == LOW;
  bool reverse = digitalRead(dirPin2) == LOW;

  int direction = 0;
  if (forward) direction = 1;
  else if (reverse) direction = -1;

  // Detect direction change, enforce 1 second pause
  if (direction != lastDirection) {
    lastDirChange = millis();
    lastDirection = direction;
    currentSpeed = 0;
  }

  if (millis() - lastDirChange < 1000) {
    direction = 0; // Hold neutral for 1 second after change
  }

  // Ramp speed up/down toward target
  if (currentSpeed < targetSpeed) {
    currentSpeed++;
  } else if (currentSpeed > targetSpeed) {
    currentSpeed--;
  }

  // Debug output
  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print(" | PWM: ");
  Serial.print(currentSpeed);
  Serial.print(" | Direction: ");
  if (direction == 1) Serial.println("FORWARD");
  else if (direction == -1) Serial.println("REVERSE");
  else Serial.println("NEUTRAL");

  // Set direction + PWM
  if (direction == 1) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    writePWM_T1(ENA1, currentSpeed);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    writePWM_T1(ENA2, currentSpeed);

  } else if (direction == -1) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    writePWM_T1(ENA1, currentSpeed);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    writePWM_T1(ENA2, currentSpeed);

  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    writePWM_T1(ENA1, 0);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    writePWM_T1(ENA2, 0);
  }

  delay(2); // Small delay for ramp timing (~2ms per step)
}
