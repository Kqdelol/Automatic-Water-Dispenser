#include <Servo.h>

const int TRIG = 9;
const int ECHO = 10;

Servo myServo;

void setup() {
    pinMode(2, INPUT_PULLUP);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    myServo.attach(6);
    myServo.write(0);
    Serial.begin(9600);
    Serial.println("Running. Press SPACE in Serial Monitor to emergency stop.");
}

int buttonStatus = 0;
bool sensing = false;
bool firstCheckPassed = false;

void emergencyStop() {
  myServo.write(0);
  sensing = false;
  firstCheckPassed = false;
  Serial.println("EMERGENCY STOP — servo returned to 0. Waiting for next button press.");
}

void loop() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == ' ') {
      emergencyStop();
    }
  }

  int pinValue = digitalRead(2);
  delay(10);
  if (buttonStatus != pinValue) {
    buttonStatus = pinValue;
    if (pinValue == 1) {
      Serial.println("Dispensing Water");
      myServo.write(90);
      delay(500);
      sensing = true;
      firstCheckPassed = false;
    }
  }

  if (sensing) {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH);
    float distance = duration * 0.034 / 2;

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance <= 4.0) {
      if (!firstCheckPassed) {
        firstCheckPassed = true;
        Serial.println("First check passed, confirming...");
      } else {
        Serial.println("Water bottle filled!");
        myServo.write(0);
        delay(500);
        sensing = false;
        firstCheckPassed = false;
      }
    } else {
      firstCheckPassed = false;  // reset if reading goes far again between checks
    }

    delay(100);
  }
}