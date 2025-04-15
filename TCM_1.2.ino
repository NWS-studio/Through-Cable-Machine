// === Pin assignments ===
const int potPin = A0;
const int dirPin1 = A1;  // Forward
const int dirPin2 = A2;  // Reverse

const int IN1 = 2;
const int IN2 = 3;
const int ENA1 = 9; // PWM - uses Timer1
const int IN3 = 4;
const int IN4 = 5;
const int ENA2 = 10; // PWM - uses Timer1

// Function to write PWM using Timer1's ICR1 range
void writePWM_T1(int pin, int val) {
  val = constrain(val, 0, 255); // Clamp to 8-bit range
  int duty = map(val, 0, 255, 0, ICR1); // Map to new top
  if (pin == 9) {
    OCR1A = duty;
  } else if (pin == 10) {
    OCR1B = duty;
  }
}

void setup() {
  // Serial debug
  Serial.begin(9600);
  Serial.println("Motor controller initialized.");

  // Set pin modes
  pinMode(potPin, INPUT);
  pinMode(dirPin1, INPUT_PULLUP);
  pinMode(dirPin2, INPUT_PULLUP);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA1, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA2, OUTPUT);

  // === Configure Timer1 for ~10kHz PWM ===
  // Mode 14 (Fast PWM, TOP = ICR1), Clear OC1A/B on compare match
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); // no prescaler
  ICR1 = 1600; // 16MHz / (10kHz) = 1600 steps for TOP
}

void loop() {
  // Read inputs
  int potValue = analogRead(potPin);
  int speed = potValue / 4; // Map 0–1023 to 0–255
  bool forward = digitalRead(dirPin1) == LOW;
  bool reverse = digitalRead(dirPin2) == LOW;

  // Serial debug
  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print(" | PWM: ");
  Serial.print(speed);

  if (forward) {
    Serial.println(" | Direction: FORWARD");

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    writePWM_T1(ENA1, speed);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    writePWM_T1(ENA2, speed);

  } else if (reverse) {
    Serial.println(" | Direction: REVERSE");

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    writePWM_T1(ENA1, speed);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    writePWM_T1(ENA2, speed);

  } else {
    Serial.println(" | Direction: NEUTRAL");

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    writePWM_T1(ENA1, 0);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    writePWM_T1(ENA2, 0);
  }

  delay(200); // debounce + smooth output
}
