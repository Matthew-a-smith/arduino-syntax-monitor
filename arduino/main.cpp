#include <LiquidCrystal.h>
#include <Arduino.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define RED_PIN     4
#define YELLOW_PIN  3
#define GREEN_PIN   2

#define MAX_ERRORS  20

String errors[MAX_ERRORS];

int errorCount = 0;
int currentError = 0;

const unsigned long displayDelay = 3000;

unsigned long lastDisplay = 0;

bool receivingErrors = false;

String fileName = "";

void setup() {

  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);

  digitalWrite(YELLOW_PIN, HIGH);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {

  // RECEIVE SERIAL DATA
  while(Serial.available()) {

    String line = Serial.readStringUntil('\n');

    line.trim();

    // ERROR STATUS
      if(line.startsWith("1")) {

      receivingErrors = true;

      errorCount = 0;
      currentError = 0;

      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Errors Found");
    }

    // NO ERRORS
    else if(line.startsWith("0|")) {

      String fileName = line.substring(2);
  
      receivingErrors = false;
      errorCount = 0;
      currentError = 0;
  
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
  
      lcd.clear();
  
      lcd.setCursor(0, 0);
      lcd.print(fileName.substring(0, 16));
  
      lcd.setCursor(0, 1);
      lcd.print("No Errors");
  }

    // EDITOR CLOSED
    else if(line.startsWith("3")) {

      receivingErrors = false;

      errorCount = 0;
      currentError = 0;

      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Editor Closed");
    }

    // RECEIVE ERROR
    else if(receivingErrors) {

      if(errorCount < MAX_ERRORS) {

        errors[errorCount] = line;
        errorCount++;
      }
    }

    Serial.println(line);
  }

  // DISPLAY ERRORS
  if(receivingErrors && errorCount > 0) {

    if(millis() - lastDisplay >= displayDelay) {

      lastDisplay = millis();

      lcd.clear();

      String error = errors[currentError];

      lcd.setCursor(0, 0);
      lcd.print(error.substring(0, 16));

      if(error.length() > 16) {

        lcd.setCursor(0, 1);
        lcd.print(error.substring(16, 32));
      }

      currentError++;

      if(currentError >= errorCount) {
        currentError = 0;
      }
    }
  }
}
