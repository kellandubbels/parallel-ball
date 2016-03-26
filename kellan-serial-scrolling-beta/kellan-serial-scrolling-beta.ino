/*
  Serial Event example

 When new serial data arrives, this sketch adds it to a String.
 When a newline is received, the loop prints the string and
 clears it.

 A good test for this is to try it with a GPS receiver
 that sends out NMEA 0183 sentences.

 Created 9 May 2011
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/SerialEvent

 */


// include the library code:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

 
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

boolean debug = true;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String field = "Waiting";
String fieldIncremented = "jumbotron";
boolean printLine = true;

int fieldBegin = 0;
int fieldEnd = 12;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial2.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(1024);

  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);

  // Print a message to the LCD. We track how long it takes since
  // this library has been optimized a bit and we're proud of it :)
  int time = millis();
  lcd.print("Waiting...");
  time = millis() - time;
  Serial.print("Took "); Serial.print(time); Serial.println(" ms");
  lcd.setBacklight(GREEN);


  
}

void loop() {
int inputStringLength = inputString.length();

  // print output when print flag is set:
  if (printLine) {
    lcd.clear();
    field = inputString.substring(fieldBegin,fieldEnd);
    // First row of display
    lcd.setCursor(0, 0);
    Serial.print(field);
    Serial.print("\n");
    // lcd.clear();
    lcd.print(field);

    if (debug) {
      lcd.setBacklight(RED);
      lcd.setCursor(0, 1);
      fieldIncremented = inputString.substring(fieldBegin + 12 , fieldEnd + 12);
      lcd.print(fieldIncremented);
      //more literal serial console for debugging
      //Serial.print(fieldIncremented);
      //Serial.print("\n");
    }
    if (debug == false) {
      lcd.setBacklight(VIOLET);
      lcd.setCursor(0, 1);
      lcd.print(inputStringLength);
      lcd.setCursor(4, 1);
      lcd.print(fieldBegin);
      lcd.setCursor(8, 1);
      lcd.print((int)(fieldBegin / 60));
      lcd.setCursor(12,1);
      lcd.print((fieldBegin % 60));
      lcd.setCursor(12, 0);
      lcd.print(millis()/1000);
      
      
// REMOVE THIS
      debug = false;
    }

    // clear the print flag:
      printLine = false;
  }


  // Scan the buttons and respond
  uint8_t buttons = lcd.readButtons();
  if (buttons) {

    // rewind 12 characters
    if (buttons & BUTTON_UP) {
    printLine = true;
      if ( fieldBegin > 11 ) {
        fieldBegin -= 12;
        fieldEnd -= 12;
      }
    }
    
    // advance 12 characters
    if (buttons & BUTTON_DOWN) {
    printLine = true;
      if ( fieldEnd < inputStringLength ) {
        fieldBegin += 12;
        fieldEnd += 12;
      } 
    }
    
    // move back one card
    if (buttons & BUTTON_LEFT) {
    printLine = true;
      if ( fieldBegin > 59 ) {
        fieldBegin -= 60;
        fieldEnd -= 60;
      }
    }
    
    // move forward one card
    if (buttons & BUTTON_RIGHT) {
    printLine = true;
      if ( fieldEnd < inputStringLength ) {
        fieldBegin += 60;
        fieldEnd += 60;
      }
    }

    // bonus button
    // mode switching? turn on debug 
    if (buttons & BUTTON_SELECT) {
      printLine = true;
      debug = !(debug);
      delay(100);
      } 
    }
}
  // end of main loop   
  


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent2() {
  while (Serial2.available()) {
    // get the new byte:

    char inChar = (char)Serial2.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
   
    if (inChar == '1'  ) {
      inputString.remove((inputString.length() - 1));
      if ( fieldBegin > 11 ) {
        printLine = true;
        fieldBegin -= 12;
        fieldEnd -= 12;
      }
    }
    
    if (inChar == '2' ) {
      inputString.remove((inputString.length() - 1));
      if ( fieldEnd < inputString.length() ) {
        printLine = true;
        fieldBegin += 12;
        fieldEnd += 12;
      }  
    }

    if (inChar == '3' ) {
      inputString.remove((inputString.length() - 1));
      printLine = true;
      debug = !(debug);
      delay(100);
    }
    
  }



}
