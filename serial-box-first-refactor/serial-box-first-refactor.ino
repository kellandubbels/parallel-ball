/*
Serial box by Kellan Dubbels. March 2016.
*/

//includes
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

//defines
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// defines for RGB backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

//variables
boolean printDebug = true;       // begin with debug enabled
boolean printLineOne = true;
boolean printLineTwo = false;
String inputString = "";         // a string to hold incoming data from serial port 1
String lineOne = "Waiting";      // initialize first display line
String lineTwo = "Waiting";      // initialize second display line
boolean writeDisplay = true;     // initiate first display write
boolean prevField = false; boolean nextField = false; boolean prevCard = false; boolean nextCard = false;
boolean toggleDebug; 
boolean buttonPressed;           // interface flag set of button event
int lastRefresh = 0;
boolean staleDisplay;
int offset = 0;                  // initial offset to display data from
int displayColumns = 16;         // number of columns to display per row

int inputStringLength;


//setup

void setup() {
  // initialize serial console (this is for debugging with a computer)
  Serial.begin(9600);
  // initialize the serial input (this is the data we are interested in)
  Serial1.begin(9600);
  // reserve 1024 bytes for the inputString:
  inputString.reserve(1024);

  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
}

//main loop
void loop() {
int lastColumn = offset + displayColumns;


  // has ther been input from serial?
  if (inputString.length() > inputStringLength) {
    staleDisplay = true;
    inputStringLength = inputString.length();
  }
  
  // on staleDisplay, wait for serial to go idle, wait 300 repetitions
  if ((Serial1.available() == false) & staleDisplay ) {
    lastRefresh = lastRefresh + 1;
    // 500 is minimum value for 9600 baud to work reliably
    // 1000 is a nice balance of response and flicker
      if (lastRefresh > 300) {
        writeDisplay = true;
        lastRefresh = 0 ; 
        staleDisplay = false;  
      }
  }


  //scan the buttons for input, set flag on input
  uint8_t buttons = lcd.readButtons();
  if (buttons) {
    // rewind one field
    if (buttons & BUTTON_UP) {prevField = true;}

    // advance one field
    if (buttons & BUTTON_DOWN) {nextField = true;}

    // move back one card
    if (buttons & BUTTON_LEFT) {prevCard = true;}

    // move forward one card
    if (buttons & BUTTON_RIGHT) {nextCard = true;}

    // bonus button
    // mode switching? turn on debug 
    if (buttons & BUTTON_SELECT) {toggleDebug = true;}

  // set flag for user input 
  //buttonPressed = true;   
  //}

  //functions activated by buttons

  //was there input from buttons?
  //if (buttonPressed) {
   
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
    
    //buttonPressed = false;
  }

  //if writeDisplay then update the display based on display flags
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
      lcd.setCursor(8, 1); lcd.print((int)(offset / 64));
      lcd.setCursor(12,1); lcd.print((offset % 64));
    //  lcd.setCursor(12,0); lcd.print(millis()/1000);
    }




  
  staleDisplay = false; 
  writeDisplay = false;
  }
// end of main loop()
}  

//serialevent
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */

void serialEvent1() {
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read();
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

    
