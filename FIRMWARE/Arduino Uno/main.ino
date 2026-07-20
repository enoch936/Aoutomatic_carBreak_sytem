#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- Motor Pins (L298) ----------------
#define ENA 3
#define IN1 4
#define IN2 5
#define ENB 6
#define IN3 7
#define IN4 8

// ---------------- LEDs ----------------
#define LED_GREEN 9
#define LED_YELLOW 10
#define LED_RED 11

// ---------------- Buttons ----------------
#define BTN_STOP 12
#define BTN_UP 2
#define BTN_DOWN 13

// ---------------- Buzzer ----------------
#define BUZZER A3

// ---------------- Ultrasonic ----------------
#define TRIG A1
#define ECHO A2

// ---------------- Potentiometer ----------------
#define POT A0

int speedValue = 150;
long distance = 0;

// ---------------- Setup ----------------
void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  pinMode(BTN_STOP, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
}

// ---------------- Main Loop ----------------
void loop() {

  distance = readUltrasonic();

  int potValue = analogRead(POT);
  speedValue = map(potValue, 0, 1023, 0, 255);

  if (!digitalRead(BTN_STOP)) speedValue = 0;
  if (!digitalRead(BTN_UP)) speedValue = min(255, speedValue + 10);
  if (!digitalRead(BTN_DOWN)) speedValue = max(0, speedValue - 10);

  // -------- Behavior Logic --------
  if (distance > 30) {
    moveForward(speedValue);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
    noTone(BUZZER);
  }
  else if (distance > 10 && distance <= 30) {
    moveForward(speedValue / 2);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
    tone(BUZZER, 500);
  }
  else {
    stopMotors();
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);
    tone(BUZZER, 1000);
  }

  // -------- LCD Update --------
  lcd.setCursor(0,0);
  lcd.print("Speed:");
  lcd.print(speedValue);
  lcd.print("   ");

  lcd.setCursor(0,1);
  lcd.print("Dist:");
  lcd.print(distance);
  lcd.print("cm   ");

  delay(50);
}

// ---------------- Motor Control ----------------
void moveForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ---------------- Ultrasonic Sensor ----------------
long readUltrasonic() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 25000);
  return duration * 0.034 / 2;
}
