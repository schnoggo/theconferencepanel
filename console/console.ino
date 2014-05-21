// include the library code:
#include <Wire.h>
#include <avr/io.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <Adafruit_NeoPixel.h>
//   LCD SHIELD
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

// NEO PIXEL
#define NEOPIXEL_PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel pixel_ring = Adafruit_NeoPixel(16+4+4, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);


#define TEAM1PIN 0
#define TEAM2PIN 1

#define CONSOLE_GO_PIN 4
#define CONSOLE_STOP_PIN 3
#define CONSOLE_CANCEL_PIN 5

#define GO_GO 0
#define GO_STOP 1
#define GO_TIMER 2
#define GO_PLAYER 3
#define GO_TYPE 4

#define TICKDURATION 2000
#define MEMTICKDURATION 6000

unsigned long next_tick = 0;
unsigned long mem_tick;

char lcd_line2[17]; // leave room for the terminating zero


#define BUT1 1
#define BUT2 2
#define BUT3 4
#define BUT4 8

struct buttonmap{
	unsigned int reading; // analog reading value
	byte button_bits; // bit-packed list of which buttons are press. see BUT1 et al.
} player_button_thresholds[17] ={
	{555,0},
	{569, BUT1},
	{582, BUT2},
	{603, BUT1 | BUT2},
	{631, BUT3},
	{650, BUT1 | BUT3},
	{673, BUT2 | BUT3},
	{694, BUT1 | BUT2 | BUT3},
	{746, BUT4},
	{773, BUT1 | BUT4},
	{804, BUT2 | BUT4},
	{836, BUT1 | BUT2 | BUT4},
	{888, BUT3 | BUT4},
	{924, BUT1 | BUT3 | BUT4},
	{974, BUT2 | BUT3 | BUT4},
	{1020, BUT1 | BUT2 | BUT3 | BUT4}
};




// Buttons, Teams, and Players:
char* playerNames[10][16]; // create space for player names

char player_status[16]; //signed 8 bit

typedef struct {
 char lastbutton; // which button was "down" during last poll (signed) -1 = no button
 byte state;     // 0:still open,  1:from open to closed 2:from closed to open 3:still closed 4: unkown
 unsigned int lastvalue; // last actual value from the pin
 uint8_t repeat_count; // number of times this value has been read
 unsigned long lastclosed; // time of last read that was closed
  byte pin; // which line are we reading?

} ButtonLine;

byte last_player_pressed = 0;
byte buzzing_player = 0;
#define NUMBEROFTEAMS 1 // should be able to support 3 teams with an Uno
ButtonLine buttonLines[NUMBEROFTEAMS+1]; // 0th "team" is console

// Game Modes
#define GAME_TEAM_STEAL 0
#define GAME_LIGHTNING 1
#define GAME_ALL_PLAY 2

// Screen types:
#define ANIM_FAIL 6
#define ANIM_GAME_OVER 9
#define ANIM_MINOR_WIN 7
#define ANIM_MINOR_FAIL 12
#define ANIM_TIME 10
#define ANIM_WIN 5
#define HOST 1
#define LIGHTNING 3
#define PAUSE 4
#define PLAYER 2
#define START_CLOCK 11
#define SYSTEM 13
#define CALIBRATING 0




unsigned long calibrate_tick = 0;
// set to 1 to set up mode where we can get resistor values

byte current_mode = 0;
byte current_frame = 0;
byte framecode[5]; // global array of this frame's instuctions

// animation:

typedef struct {
 char state;     // 0:not active, 1:needs init, 2: playing
 unsigned long start_time; // time of last read that was closed
  byte animation; // which line are we reading?

} gameanimation;

gameanimation game_animation; // make a global instance


// Timers:
typedef struct {
 byte state;     // 0:running,  1:expired, 2 paused,  -1:no clock
 unsigned long start_time; // when started or time paused
 unsigned long end_time; // when the clock expires, or how much remains if paused
 byte duration; //duration in seconds (needed for clock display)
} timer;
timer GameTimer; // global game time


void setup() {
// ------------
	byte i; // generic, global index to avoid memory thrashing
	// Debugging output
	Serial.begin(9600);
	mem_tick = millis();
	// set up the LCD's number of columns and rows: 
	lcd.begin(16, 2);

	// set up neopixel ring:
	pixel_ring.begin();
	pixel_ring.show(); // Initialize all pixels to 'off'

	pinMode(CONSOLE_GO_PIN, INPUT);
	pinMode(CONSOLE_STOP_PIN, INPUT);
	pinMode(CONSOLE_CANCEL_PIN, INPUT);
	digitalWrite(CONSOLE_GO_PIN, HIGH);
	digitalWrite(CONSOLE_STOP_PIN, HIGH);
	digitalWrite(CONSOLE_CANCEL_PIN, HIGH);

	pinMode (TEAM1PIN, INPUT); // Without this analogRead always returns 0
	pinMode (TEAM2PIN, INPUT); // Without this analogRead always returns 0
	digitalWrite(A0, LOW); // analog - do not use internal resistor
	digitalWrite(A1, LOW); // analog - do not use internal resistor - alreay in the circuit
	analogRead (TEAM1PIN) ; // do a dummy read to get the pin in the right state?

	InitGameAnimations();
	next_tick = millis() + TICKDURATION;
	InitAnalogButtons();
	current_mode = 0;
	current_frame = 0;
	
		LightPlayer(1,1,pixel_ring.Color(1, 0, 20),0);
LightPlayer(2,1,pixel_ring.Color(10, 50, 0),0);
LightPlayer(3,1,pixel_ring.Color(20, 0, 0),0);



	LoadGameFrame();  
}

void loop() {
  unsigned long now = millis();
  byte last_console_button = 0;

  
  if (CALIBRATING){
  	PollUserButtons();
  } else {
  if(framecode[GO_PLAYER]>0){
    buzzing_player=PollUserButtons();
    lcd.setCursor(15, 1); // show player in lower-right
    lcd.print(buzzing_player);                                                                               
    if (buzzing_player>0){
    	// depending on game type, we made need to lock out this player, the player team, or reset everything
			ClearUserButtons();
			
			// Light up the buzzing player:
			LightOnePlayer(buzzing_player,1,pixel_ring.Color(50, 200, 200));
     //  if (last_player_pressed != buzzing_player){
      //   last_player_pressed = buzzing_player;
        GoToFrame(framecode[GO_PLAYER]);
     // }
    }
    Time2Neo(GetCountdownSeconds());
  }
  // transient frames:
   if (framecode[GO_TYPE] == START_CLOCK){
       StartCountdown(10); // ten seconds on the clock
       GoToFrame(framecode[GO_GO]);
   }
      
      
  
  if(framecode[GO_GO]>0){
    if(PollConsoleButtons(1)){
      switch(framecode[GO_TYPE]){
      case START_CLOCK:
       StartCountdown(10); // ten seconds on the clock
      break;
      
      case PLAYER:
  
      break;
      } 
     switch(framecode[GO_GO]){
       case 200:
         TogglePause();
       break;
     default:
        GoToFrame(framecode[GO_GO]);
    }
    }
  }

  
   if(framecode[GO_STOP]>0){
    if(PollConsoleButtons(2)){
      GoToFrame(framecode[GO_STOP]);
    }
  } 
  
    
   if(framecode[GO_TIMER]>0){
    if(CountdownExpired()>0){
      GoToFrame(framecode[GO_TIMER]);
    }
  }
  
  
  
//
ServiceGameAnimation();
  
}
}



byte PollConsoleButtons(byte lookingfor) {
  /*
  Input:
  1 - GO button
  2 - STOP button
  */
  byte retVal;
  // last value:
  // buttonLines[0].lastbutton
  // this will eventually use the resistor ladder model.
  // right now, we're just using digital pins
 // Serial.print("looking for:");
 // Serial.print(lookingfor);
  retVal=0;
  switch (lookingfor) {
   case 1:
   if (digitalRead(CONSOLE_GO_PIN) == LOW){ retVal=1;}
  
  break;
  case 2:
  if (digitalRead(CONSOLE_STOP_PIN) == LOW){retVal=1;}
  break;
  }

    if (retVal){ // button pressed
      if (lookingfor == buttonLines[0].lastbutton) { // have we already reported this button as pressed?
        retVal = 0;
      } else {
        buttonLines[0].lastbutton = lookingfor;
      }
    } else {
      buttonLines[0].lastbutton = -1;
    }
  return retVal;
}
void LoadGameFrame(){
  /*
  #define ANIM_FAIL 6
#define ANIM_GAME_OVER 9
#define ANIM_SINGLE_CORRECT 7
#define ANIM_SINGLE_WRONG 8
#define ANIM_TIME 10
#define ANIM_WIN 5
#define HOST 1
#define LIGHTNING 3
#define PAUSE 4
#define PLAYER 2
*/
  
  FetchGameInstruction(0,current_frame,&framecode[0]); // pass pointer to first elemnt of our instruction array
 /*
  Serial.print("Frame 0: ");
   Serial.print(framecode[0]);
    Serial.print(",  ");
 Serial.print(framecode[1]);
    Serial.print(",  ");
 Serial.println(framecode[2]);
 
 */
  switch (framecode[GO_TYPE]) {
   case HOST:
    lcd.setBacklight(BACKLIGHT_RED);
    DisplayModeTitle(FetchFrameName(current_frame));
   break;
 
 case PLAYER:
 		lcd.setBacklight(BACKLIGHT_GREEN);
    DisplayModeTitle(FetchFrameName(current_frame));
 break;
 
 case START_CLOCK:
     lcd.setBacklight(BACKLIGHT_RED);
    DisplayModeTitle(FetchFrameName(current_frame));
 
 break;
 
 case SYSTEM:
		lcd.setBacklight(BACKLIGHT_TEAL);
    DisplayModeTitle(FetchFrameName(current_frame));
 break;
 
 case ANIM_FAIL:
 		lcd.setBacklight(BACKLIGHT_YELLOW);
    DisplayModeTitle(FetchFrameName(current_frame));
    PlayGameAnimation(framecode[GO_TYPE]);
    break;
 case ANIM_TIME:
  	lcd.setBacklight(BACKLIGHT_YELLOW);
    DisplayModeTitle(FetchFrameName(current_frame));
    PlayGameAnimation(framecode[GO_TYPE]);
    break;
 case ANIM_WIN:
    lcd.setBacklight(BACKLIGHT_YELLOW);
    DisplayModeTitle(FetchFrameName(current_frame));
    PlayGameAnimation(framecode[GO_TYPE]);
    break;
    
 default: 
     lcd.setBacklight(BACKLIGHT_RED);
			DisplayModeTitle(FetchFrameName(current_frame));
 
  }

  // display the game code at the bottom of the screen
  /*
  lcd.setCursor(1, 1);
	byte i;
  for (i=0; i < 5; i++){
    lcd.print(framecode[i]);
    lcd.print(" ");
  }
  */
}


int FreeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void CheckMemoryUse(){
  if ((millis() -  MEMTICKDURATION) > mem_tick){
    mem_tick = millis();
    Serial.print("mem ");
    Serial.print(mem_tick);
    Serial.print(": ");
    Serial.println(FreeRam());
    mem_tick++;
  }
  
}
