/*
Title: Syntax-monitor.ino
Author: Matthew A. Smith
StudentID: n/a
Date: August 10th, 2026
Version: 1.6

Description:
Arduino-based syntax monitoring system that receives
syntax-checking results from a Bash script through a
USB serial connection and displays status information
using LEDs and a 16x2 LCD display.

Copyright: 2026 Matthew A. Smith
*/

/*
DOCUMENTATION

Program Purpose:
Receives syntax-checking results from the companion
Bash monitoring script and provides visual feedback
through LEDs and an LCD display.

Features:
- Serial communication
- LED status indicators
- LCD output
- Error message storage
- Error message scrolling
- File status monitoring
- Real-time updates from the Bash script

Serial Status Messages:

1            - Errors detected
0|filename   - No errors found
3            - Editor closed
END_ERRORS   - End of error transmission

LED Status:

Red LED      - Syntax errors detected
Green LED    - No syntax errors detected
Yellow LED   - Editor closed or inactive

Variables:

fileName         : String
                   Name of the file currently being checked

errorLines       : String[]
                   Stores received error messages

errorCount       : int
                   Number of stored error messages

currentError     : int
                   Current error being displayed

receivingErrors  : bool
                   Indicates that error messages are
                   currently being received

displayErrors    : bool
                   Indicates that stored errors should
                   be displayed on the LCD

Hardware Connections:

LCD Display:
RS = 13
E  = 12
D4 = 11
D5 = 10
D6 = 9
D7 = 8

LED Connections:
Red LED    = Pin 4
Yellow LED = Pin 3
Green LED  = Pin 2

Display Behavior:

- Error messages are received through the serial port.
- Errors are stored before being displayed.
- Messages scroll across the LCD one at a time.
- The display immediately stops scrolling when
  new serial data is received.
- Multiple spaces are removed from error messages
  before display.

Notes:

- Designed for a 16x2 LCD display.
- Supports up to 20 stored error messages.
- Error messages scroll horizontally.
- Intended to work with Syntax-monitor.sh.
- Uses a 9600 baud serial connection.

*/

#include <LiquidCrystal.h> // used for LCD display
#include <Arduino.h>       // provides Arduino functions, pins, and String

// LCD pins used to connect the Arduino to the display
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// pins used for the status LEDs
#define RED_PIN     4
#define YELLOW_PIN  3
#define GREEN_PIN   2

// Stores the name of the current file
String fileName = "";


void setup() {

  // Start serial communication with the computer at 9600 baud
  Serial.begin(9600);

  // Set the LED pins as outputs
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);

  // Start with the yellow LED on because the editor is not active
  digitalWrite(YELLOW_PIN, HIGH);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  // Initialize the LCD as a 16 column by 2 row display
  lcd.begin(16, 2);

  // Clear anything currently shown on the LCD
  lcd.clear();

  // Display closed by default
  lcd.setCursor(0, 0);
  lcd.print("Editor Closed");
}

// main loo[]
void loop() {

  // Stores up to 20 error messages received from the Bash script
  static String errorLines[20];

  // Number of stored errors
  static int errorCount = 0;

  // Current error being displayed on the LCD
  static int currentError = 0;

  // True while receiving error messages from serial
  static bool receivingErrors = false;

  // True when errors should be displayed on the LCD
  static bool displayErrors = false;

  // Process all incoming serial data
  while (Serial.available()) {

    // Read one line from the serial connection
    String line = Serial.readStringUntil('\n');

    // Remove leading and trailing whitespace
    line.trim();


    // Status 1 = Errors detected
    if (line.startsWith("1")) {

      // Turn on the red LED
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);

      // Clear previous errors
      errorCount = 0;
      currentError = 0;

      // Begin collecting new error messages
      receivingErrors = true;
      displayErrors = false;
    }


    // Status 0|filename = No errors found
    else if (line.startsWith("0|")) {

      // Extract filename from serial message
      fileName = line.substring(2);

      // Turn on green LED
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);

      // Display filename and status
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print(fileName.substring(0, 16));

      lcd.setCursor(0, 1);
      lcd.print("No Errors");

      // Reset error tracking
      errorCount = 0;
      currentError = 0;

      receivingErrors = false;
      displayErrors = false;
    }


    // Status 3 = Editor closed
    else if (line.startsWith("3")) {

      // Turn on yellow LED
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);

      // Display editor closed message
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Editor Closed");

      // Reset error tracking
      errorCount = 0;
      currentError = 0;

      receivingErrors = false;
      displayErrors = false;
    }


    else {

      // Marker indicating all error lines have been received
      if (line == "END_ERRORS") {

        // Begin displaying stored errors
        receivingErrors = false;
        displayErrors = true;
        currentError = 0;
      }


      // Store incoming error messages
      else if (receivingErrors && errorCount < 20) {

        errorLines[errorCount] = line;
        errorCount++;
      }
    }

  }

  // Display stored errors one at a time
  if (displayErrors && errorCount > 0) {

    // Retrieve current error
    String line = errorLines[currentError];

    // Remove leading and trailing whitespace
    line.trim();

    // Collapse repeated spaces into a single space
    while (line.indexOf("  ") != -1) {
      line.replace("  ", " ");
    }

    // Length of the current error message
    int errorLen = line.length();

    // Scroll the entire message across the LCD
    for (int position = 0; position < errorLen + 16; position++) {

      // Stop scrolling immediately if new serial data arrives
      if (Serial.available()) {
        break;
      }

      lcd.clear();

      // Bottom row of the LCD
      lcd.setCursor(0, 1);

      // Display a 16-character window of the error message
      for (int i = 0; i < 16; i++) {

        int index = position + i - 16;

        if (index >= 0 && index < errorLen) {
          lcd.print(line[index]);
        }
        else {
          lcd.print(" ");
        }
      }

      // Controls the scroll speed
      delay(300);
    }

    // Move to the next stored error
    currentError++;

    // Return to the first error when the end is reached
    if (currentError >= errorCount) {
      currentError = 0;
    }
  }
}