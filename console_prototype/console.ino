// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define BACKLIGHT_RED 0x1
#define BACKLIGHT_YELLOW 0x3
#define BACKLIGHT_GREEN 0x2
#define BACKLIGHT_TEAL 0x6
#define BACKLIGHT_BLUE 0x4
#define BACKLIGHT_VIOLET 0x5
#define BACKLIGHT_WHITE 0x7

#define TEAM1PIN 0

#define CONSOLE_GO_PIN 5
#define CONSOLE_STOP_PIN 7
#define CONSOLE_CANCEL_PIN 6

#define TICKDURATION 2000

unsigned long next_tick = 0;
int current_mode = 0;

char* playerNames[10][16]; // create space for player names
char lcd_line2[17]; // leave room for the terminating zero
char player_status[16]; //signed 8 bit

typedef struct {
 byte lastbutton; // which button was "down" during last poll
 byte state;     // 0:still open,  1:from open to closed 2:from closed to closed 3:still closed
 unsigned int lastvalue; // last actual value from the pin
 unsigned long lastread; // time of last read
 unsigned long lastclosed; // time of last read that was closed
  byte pin; // which line are we reading?

} ButtonLine;

ButtonLine buttonLines[4];

void setup() {
  byte i;
  // Debugging output
  Serial.begin(9600);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  
   pinMode (TEAM1PIN, INPUT); // Without this analogRead always returns 0
    analogRead (TEAM1PIN) ; // do a dummy read to get the pin in the right state?


next_tick = millis() + TICKDURATION;
 
 
 
    for (i=0; i < 10; i++){
   //   playerNames[i]="goober";
    }

  byte framecode[5];
 
  
  FetchGameInstruction(1,1,&framecode[0]); // pass pointer to first elemnt of our instruction array
 
}

void loop() {
  unsigned long now = millis();
  byte last_console_button = 0;
  byte new_console_button = 0;
 // int cval = PollUserButtons();
  new_console_button = PossConsoleButtons();
  if (new_console_button != last_console_button){
    
    
  }

  if (now > next_tick) {
    next_tick = millis() + TICKDURATION;
    current_mode++;
    if ( current_mode>6) {
      current_mode = 0;
    }

    DisplayModeTitle(FetchFrameName(current_mode));
    lcd.setBacklight(random(5)+1);
      unsigned int current_sample; // 16 bit
  current_sample=analogRead(TEAM1PIN);
  
  itoa( current_sample, lcd_line2, 10);
  lcd.setCursor(0, 1);
  lcd.print(lcd_line2);
  lcd.print("  ");
    
  }




}

byte PollUserButtons() {
/* 
Inputs: none (just globals)
Hardware: Uses 3 analog pins to determine which button is being pushed
Globals: UserButton{
    lastbutton: int8 value of last button pressed
    state: int8 state of last button read {
      0:still open, 
      1:from open to closed
      2:from closed to closed
      3:still closed
    }
    lastread: unsigned long time of last read
    lastclosed: unsigned long time of last read that was closed
    TEAM1PIN
    
 Outputs:
   updates UserButton
   returns player number of newly pressed button
    */
    unsigned int v;
      v=analogRead(TEAM1PIN);
    

  
  
}
byte PossConsoleButtons(){
  
  
}
void DisplayModeTitle(char* title){
  /* this version add 2K to the binary! try it without the string libraries
  // erase the line buy padding out the title with spaces
  byte strlen = title.length();
  int leftpad = (16-strlen)/2;
  String outStr = "                ";
  if ( strlen == 15 ) {
  outStr = title;
  } else if (leftpad>0) {  //center
  byte i;
    for (i=0; i <strlen; i++) {
      outStr.setCharAt(i+leftpad, title.charAt(i+leftpad-1) );
    }
  } else { // too wide
  leftpad = abs(leftpad);
    outStr = title.substring(leftpad,leftpad+16);
  }
  */
  
   // erase the line buy padding out the title with spaces
  byte titlelen = strlen(title);
  int leftpad = (16-titlelen)/2;
  //char* outStr = "                ";
  char* outStr = "----------------";
  byte i, cursor_start ;
  for (i=0; i <16; i++) {
      outStr[i] = ' ';
    }
  
  if ( titlelen == 15 ) {
     strcpy(outStr, title);
  } else if (leftpad>0) {  //center
    for (i=0; i <titlelen; i++) {
      outStr[i+leftpad] = title[i];
    }
  } else { // too wide
  leftpad = -leftpad;
  for (i=0; i <16; i++) {
      outStr[i] = title[i+leftpad];
    }
  }

  lcd.setCursor(0, 0);
  lcd.print(outStr);

}
