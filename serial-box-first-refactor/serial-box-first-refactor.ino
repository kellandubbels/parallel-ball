//includes
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

//defines

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

boolean printDebug = true;
boolean printLineOne = true;
boolean printLineTwo = false;
String inputString = "";         // a string to hold incoming data
String lineOne = "Waiting";
String lineTwo = "Waiting";
boolean writeDisplay = true;
boolean prevField = false; boolean nextField = false; boolean prevCard = false; boolean nextCard = false;
boolean toggleDebug; boolean buttonPressed;

int offset = 0;
int displayColumns = 16;


//variables

//setup

void setup() {
  // initialize serial console:
  Serial.begin(9600);
  Serial2.begin(9600);
  // reserve 1024 bytes for the inputString:
  inputString.reserve(1024);

  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
}


//loop
void loop() {
int inputStringLength = inputString.length();
int lastColumn = offset + displayColumns;

  //scan the buttons for input
  uint8_t buttons = lcd.readButtons();
  if (buttons) {

    // rewind 12 characters
    if (buttons & BUTTON_UP) {
    prevField = true;
    }
    
    // advance 12 characters
    if (buttons & BUTTON_DOWN) {
    nextField = true;
    }
    
    // move back one card
    if (buttons & BUTTON_LEFT) {
    prevCard = true;
    }
    
    // move forward one card
    if (buttons & BUTTON_RIGHT) {
    nextCard = true;
    }

    // bonus button
    // mode switching? turn on debug 
    if (buttons & BUTTON_SELECT) {
      toggleDebug = true;
    } 
  buttonPressed = true;  
  }

  //functions

  //was there input from buttons?
  if (buttonPressed) {
   
    // rewind 16 characters
    if (prevField) {
      if ( offset > 15 ) {
        offset -= 16;
        lastColumn -= 16;
      }
    prevField = false;  
    writeDisplay = true;  
    }
    
    // advance 16 characters
    if (nextField) {
      if ( lastColumn < inputStringLength ) {
        offset += 16;
        lastColumn += 16;
      } 
    nextField = false;  
    writeDisplay = true;
    }
    
    // move back one card
    if (prevCard) {
      if ( offset > 63 ) {
        offset -= 64;
        lastColumn -= 64;
      }
    prevCard = false;
    writeDisplay = true;
    }
    
    // move forward one card
    if (nextCard) {
      if ( lastColumn < inputStringLength ) {
        offset += 64;
        lastColumn += 64;
      }
    nextCard = false;  
    writeDisplay = true;
    }

    // bonus button
    // mode switching? turn on debug 
    if (toggleDebug) {
        printDebug = !(printDebug);
        printLineTwo = !(printLineTwo);
    delay(100);    
    toggleDebug = false;
    writeDisplay = true;  
    }
    buttonPressed = false;
  }
  

  //if writeDisplay then
  if (writeDisplay) {
    lcd.clear();
  
    //if printLineOne enabled
    //print first line
    if (printLineOne) {
    lcd.setCursor(0, 0);
    lineOne = inputString.substring(offset,lastColumn);
    lcd.print(lineOne);
    }

    //if printLineTwo enabled
    //print second line
    if (printLineTwo) {
      lcd.setBacklight(TEAL);
      lcd.setCursor(0, 1);
      lineTwo = inputString.substring((offset + displayColumns),(lastColumn + displayColumns));
      lcd.print(lineTwo);
    }

    //if printDebug enabled
    //print debug text
    if (printDebug) {
      lcd.setBacklight(RED);
      lcd.setCursor(0, 1); lcd.print(inputStringLength);
      lcd.setCursor(4, 1); lcd.print(offset);
      lcd.setCursor(8, 1); lcd.print((int)(offset / 60));
      lcd.setCursor(12,1); lcd.print((offset % 60));
    //  lcd.setCursor(12,0); lcd.print(millis()/1000);
    }

  writeDisplay = false;
  }
// end of loop()
}  

//serialevent
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
   }
   } 


void serialEvent() {
  while (Serial.available()) {
    String consoleString = "";
    // get the new byte:
    char consoleChar = (char)Serial.read();
    // add it to the inputString:
    consoleString += consoleChar;
    if (consoleChar == '!'  ) {
      consoleString.remove((consoleString.length() - 1));
      Serial.print(inputString);
      Serial.print("\n");
      Serial.print(inputString.length());
      Serial.print("\n");
      }

   }
   } 




    
    
