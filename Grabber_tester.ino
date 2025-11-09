#include <Servo.h>

// ===== Pins =====
const int SERVO_A_PIN = 9;   // first servo
const int SERVO_B_PIN = 10;  // second servo

const int FLEX1_PIN = A0;  // Servo A CCW
const int FLEX2_PIN = A1;  // Servo A CW
const int FLEX3_PIN = A2;  // Servo B CCW
const int FLEX4_PIN = A3;  // Servo B CW

// ===== Servos & angles =====
Servo servoA, servoB;

float angleA = 90;              // start centered
float angleB = 90;
const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;

// ===== Calibration (per sensor) =====
// Update these if your sensors differ. Earlier you measured ~360..500.
int FLEX1_MIN = 360, FLEX1_MAX = 500; // A0 (A CCW)
int FLEX2_MIN = 360, FLEX2_MAX = 500; // A1 (A CW)
int FLEX3_MIN = 360, FLEX3_MAX = 500; // A2 (B CCW)
int FLEX4_MIN = 360, FLEX4_MAX = 500; // A3 (B CW)

// ===== Behavior tuning =====
const float THRESH_PCT = 30.0;   // ignore bends below this %
const float SPEED_SCALE = 0.05;  // deg per loop per 1% bend (e.g., 100% => 5°/loop)
const unsigned long LOOP_DELAY_MS = 20;

// ---- helpers ----
float bendPercent(int raw, int mn, int mx) {
  float pct = map(raw, mn, mx, 0, 100);
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  return pct;
}

void setup() {
  Serial.begin(9600);

  servoA.attach(SERVO_A_PIN);
  servoB.attach(SERVO_B_PIN);

  servoA.write(angleA);
  servoB.write(angleB);

  Serial.println(F("==== 4 Flex Sensors -> 2 Servos (proportional speed) ===="));
  Serial.println(F("A0=A CCW, A1=A CW, A2=B CCW, A3=B CW"));
}

void loop() {
  // --- Read raw values ---
  int r1 = analogRead(FLEX1_PIN);
  int r2 = analogRead(FLEX2_PIN);
  int r3 = analogRead(FLEX3_PIN);
  int r4 = analogRead(FLEX4_PIN);

  // --- Map to 0..100% ---
  float b1 = bendPercent(r1, FLEX1_MIN, FLEX1_MAX);
  float b2 = bendPercent(r2, FLEX2_MIN, FLEX2_MAX);
  float b3 = bendPercent(r3, FLEX3_MIN, FLEX3_MAX);
  float b4 = bendPercent(r4, FLEX4_MIN, FLEX4_MAX);

  // ===== Servo A logic (same style as before: “winner” direction) =====
  if (b1 > THRESH_PCT && b1 > b2) {
    angleA -= (b1 * SPEED_SCALE);   // CCW
  } else if (b2 > THRESH_PCT && b2 > b1) {
    angleA += (b2 * SPEED_SCALE);   // CW
  }
  angleA = constrain(angleA, MIN_ANGLE, MAX_ANGLE);
  servoA.write((int)angleA);

  // ===== Servo B logic =====
  if (b3 > THRESH_PCT && b3 > b4) {
    angleB -= (b3 * SPEED_SCALE);   // CCW
  } else if (b4 > THRESH_PCT && b4 > b3) {
    angleB += (b4 * SPEED_SCALE);   // CW
  }
  angleB = constrain(angleB, MIN_ANGLE, MAX_ANGLE);
  servoB.write((int)angleB);

  // --- Serial debug ---
  Serial.print("F1(A-CCW): "); Serial.print(b1,1); Serial.print("%  ");
  Serial.print("F2(A- CW): "); Serial.print(b2,1); Serial.print("%  ");
  Serial.print("| A: ");       Serial.print((int)angleA);

  Serial.print("   ||   ");

  Serial.print("F3(B-CCW): "); Serial.print(b3,1); Serial.print("%  ");
  Serial.print("F4(B- CW): "); Serial.print(b4,1); Serial.print("%  ");
  Serial.print("| B: ");       Serial.println((int)angleB);

  delay(LOOP_DELAY_MS);
}
