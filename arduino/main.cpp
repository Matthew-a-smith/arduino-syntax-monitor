#include <Arduino.h>

#define RED_PIN     4
#define GREEN_PIN   7
#define YELLOW_PIN  2

void setup() {
  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);

  // Default state
  digitalWrite(YELLOW_PIN, HIGH);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
}

void loop() {

  while (Serial.available()) {

    char c = Serial.read();

    if (c == '1') {
      // Error
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
    }
    else if (c == '0') {
      // No errors
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
    }
    else if (c == '3') {
      // Default / Nano closed / waiting
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
    }

    Serial.print(c);
  }
}