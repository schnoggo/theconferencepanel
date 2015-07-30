// include the library code:
#include <Wire.h>
#include <avr/io.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <Adafruit_NeoPixel.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

//   LCD SHIELD
// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define SERIAL_DEBUG true


// These #defines make it easy to set the backlight color
#define BACKLIGHT_RED 0x1
#define BACKLIGHT_YELLOW 0x3
#define BACKLIGHT_GREEN 0x2
#define BACKLIGHT_TEAL 0x6
#define BACKLIGHT_BLUE 0x4
#define BACKLIGHT_VIOLET 0x5
#define BACKLIGHT_WHITE 0x7



#define PLAYERS_PER_TEAM 4
#define NUMBER_OF_TEAMS 2 // should be able to support 3 teams with an Uno\


// NEO PIXEL
#define NEOPIXEL_PIN 6
#define CLOCK_RING_OFFSET 3
#define CLOCK_RING_SIZE 16

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel pixel_ring = Adafruit_NeoPixel( CLOCK_RING_SIZE +(PLAYERS_PER_TEAM*NUMBER_OF_TEAMS*2), NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);



#define TEAM1PIN 0
#define TEAM2PIN 1


// Inputs on the main game console:
#define CONSOLE_GO_PIN 4
#define CONSOLE_STOP_PIN 3
#define CONSOLE_CANCEL_PIN 5

ClickEncoder *encoder;
int16_t rotary_last, rotary_current;
ClickEncoder::Button rotary_button;



#define GO_GO 0
#define GO_STOP 1
#define GO_TIMER 2
#define GO_PLAYER 3
#define GO_TYPE 4

#define TICKDURATION 2000
#define MEMTICKDURATION 6000
#define DEBOUNCE_REPEAT 3 // increase if we start getting false hits
// 3 was selected by trial-and-error. Greater values allow spam-clicking to prevent registering of clicks.

unsigned long next_tick = 0;
unsigned long mem_tick;

char lcd_line2[17]; // leave room for the terminating zero


#define BUT1 1
#define BUT2 2
#define BUT3 4
#define BUT4 8

struct buttonmap{
	unsigned int reading; // analog reading value
	byte button_bits; // bit-packed list of which buttons are pressed. see BUT1 et al.
} player_button_thresholds[17] ={
	{555,0}, // current on pin, byte with bits set representing pressed buttons
	{569, BUT1},
	{586, BUT2},
	{603, BUT1 | BUT2},
	{630, BUT3},
	{650, BUT1 | BUT3},
	{672, BUT2 | BUT3},
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



// struct has too much overhead for this, so simple globals:
byte clock_display_state_sec = 0;
byte clock_display_state_range = 0;

// Buttons, Teams, and Players:
char playerNames[(NUMBER_OF_TEAMS * PLAYERS_PER_TEAM)][10]; // create space for player names

char player_status[NUMBER_OF_TEAMS * PLAYERS_PER_TEAM]; //signed 8 bit

typedef struct {
  char lastbutton; // which button was "down" during last poll (signed) -1 = no button
  byte state;     // 0:still open,  1:from open to closed 2:from closed to open 3:still closed 4: unkown
  unsigned int lastvalue; // last actual value from the pin
  uint8_t repeat_count; // number of times this value has been read
  unsigned long lastclosed; // time of last read that was closed
  byte pin; // which line are we reading?
  byte down_buttons; // bitmap of buttons currently down and ready to return

} ButtonLine;



byte last_player_pressed = 0;
byte buzzing_teamplayer = 0;
byte buzzing_player = 0;
byte buzzing_team = 0;
ButtonLine buttonLines[NUMBER_OF_TEAMS+1]; // 0th "team" is console

// Game Types
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

// CONSOLE MODES
#define CONSOLE_MENU 0
#define SELECT_GAME_MODE 1
#define SET_SHORT_TIMER 2
#define SET_LONG_TIMER 3
#define ENTER_PLAYERS 4
#define GAME_IN_PROGRESS 5
#define CALIBRATING_RESISTORS 6



unsigned long calibrate_tick = 0;
// set to 1 to set up mode where we can get resistor values

byte current_console_mode = 0;
byte last_console_mode = 254;
byte current_game_type = 0;
byte current_frame = 0;
byte last_game_frame = 254;
byte framecode[5]; // global array of this frame's instuctions

byte current_button_list[4]; // do this global to avoid cycles to allocate every test

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





byte i; // generic, global index to avoid memory thrashing

	
void setup() {
// ------------

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

 // InitRotary(11, 9, 8); // only activate for certain modes
	InitGameAnimations();
	next_tick = millis() + TICKDURATION;
	InitAnalogButtons();
	ClearConsoleButtons();
	current_console_mode = CONSOLE_MENU;
	current_frame = 0;
	/*
  LightPlayer(1,1,pixel_ring.Color(1, 0, 100),0);
  LightPlayer(2,1,pixel_ring.Color(30, 100, 0),0);
  LightPlayer(3,1,pixel_ring.Color(255, 0, 0),0);
    LightPlayer(4,1,pixel_ring.Color(0, 255, 2g5),0);


 LightPlayer(1,1,pixel_ring.Color(0, 0, 20),0);
  LightPlayer(2,1,pixel_ring.Color(0, 20, 0),0);
  LightPlayer(3,1,pixel_ring.Color(20, 0, 0),0);
    LightPlayer(4,1,pixel_ring.Color(20, 0, 20),0);
    
 LightPlayer(1,2,pixel_ring.Color(0, 0, 20),0);
  LightPlayer(2,2,pixel_ring.Color(0, 20, 0),0);
  LightPlayer(3,2,pixel_ring.Color(20, 0, 0),0);
    LightPlayer(4,2,pixel_ring.Color(20, 0, 20),0);
    */
    
	LoadGameFrame();  
	randomSeed(analogRead(2));
	
	Serial.println("STARTUP");
	
}




void loop() {
  
  unsigned long now = millis(); // "now" get set at beginning of every loop
  byte need2init_mode = ConsoleModeChanged();
  if (need2init_mode){
    if (SERIAL_DEBUG){
      Serial.print("Mode change:");
      Serial.println(current_console_mode);
    }
  } 
    
  switch (current_console_mode) {

    case CONSOLE_MENU:
     DisplayModeTitle("Main Menu");
    
    /*
    rotary_current += encoder->getValue();
  
    if (rotary_current != rotary_last) {
      rotary_last = rotary_current;
      Serial.print("Encoder Value: ");
      Serial.println(rotary_current);
      lcd.setCursor(0, 1);
      lcd.print("        ");
      lcd.setCursor(0, 1);

    }
  
    rotary_button = encoder->getButton();
   
    if (rotary_button != ClickEncoder::Open){
     lcd.setCursor(8, 1);
      lcd.print("        ");
      lcd.setCursor(8, 1);
      if (rotary_button == ClickEncoder::Pressed){lcd.print("PRESS");}
      if (rotary_button == ClickEncoder::Held){lcd.print("HOLD");}
      if (rotary_button == ClickEncoder::Released){lcd.print("REL");}
      if (rotary_button == ClickEncoder::Clicked){lcd.print("CLIK");}
      if (rotary_button == ClickEncoder::DoubleClicked){lcd.print("DBL");}
}
*/
  current_console_mode = SELECT_GAME_MODE;
    
    break;
   
    
    case CALIBRATING_RESISTORS:
     DisplayModeTitle("Calibrate");
      PollUserButtons(false);
    break;
    
    
    case SELECT_GAME_MODE:
      if (need2init_mode){
        lcd.setBacklight(BACKLIGHT_TEAL);
        DisplayModeTitle("Select Game Type");
      }

  // just have 2 modes right now: 
  // green button: all buzz with steal, 
  // red button: all buz, not steal
       if(PollConsoleButtons(1)){
        current_console_mode = GAME_IN_PROGRESS;
        current_game_type = GAME_TEAM_STEAL;

      ClearConsoleButtons(); // we've responded to the button push. Get ready for next button
       } else {
          if(PollConsoleButtons(2)){ //red button   
          
          current_console_mode = GAME_IN_PROGRESS;
          current_game_type = GAME_LIGHTNING;
         }
       
       }
       
    break;


    default: 
     //  DisplayModeTitle("Default(n prog?)");
      if(framecode[GO_PLAYER]>0){
        buzzing_teamplayer=PollUserButtons(false);
        buzzing_player=(byte) (buzzing_teamplayer & 0x0F);
        buzzing_team=(byte)((buzzing_teamplayer & 0xF0) >> 4);
        //lcd.setCursor(15, 1); // show player in lower-right
                                                                              
        if (buzzing_player>0){
          // depending on game type, we may need to lock out this player, the player team, or reset everything
          ClearUserButtons();
        lcd.setCursor(0, 1);
        lcd.print("player");
        lcd.setCursor(10, 1);
        lcd.print("team");
        lcd.setCursor(7, 1);
        lcd.print(buzzing_player);                                                                               
        lcd.setCursor(15, 1);
        lcd.print(buzzing_team); 

          // Light up the buzzing player:
         // LightOnePlayer(buzzing_player,buzzing_team,pixel_ring.Color(50, 200, 200));
          LightOnePlayer(buzzing_player,buzzing_team,pixel_ring.Color(255, 255, 255));
          //  if (last_player_pressed != buzzing_player){
          //   last_player_pressed = buzzing_player;
          GoToFrame(framecode[GO_PLAYER]);
        // }
        }
        Time2Neo(GetCountdownSeconds(),  GameTimer.duration);
      }
      // transient frames:
      if (framecode[GO_TYPE] == START_CLOCK){
      //ResetPlayerList
        StartCountdown(10); // ten seconds on the clock 
        LockoutEarlyBuzzers();
        GoToFrame(framecode[GO_GO]);
        

      }

      if(framecode[GO_GO]>0){
        if(PollConsoleButtons(1)){
          ClearConsoleButtons(); // we've responded to the button push. Get ready for next button
          switch(framecode[GO_TYPE]){
            case START_CLOCK:
              StartCountdown(10); // ten seconds on the clock
              ClearSubMode();
              break;

            case PLAYER:
            ClearSubMode();
              break;
              
          } 
          switch(framecode[GO_GO]){
            case 200:
              TogglePause();
              break;
            default:
              GoToFrame(framecode[GO_GO]);
          }
        } // PollConsoleButtons (check for pause)
      }


      if(framecode[GO_STOP]>0){
        if(PollConsoleButtons(2)){
         ClearConsoleButtons(); // we've responded to the button push. Get ready for next button
          ClearNeoClock();
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

    
  } // the giant switch statement
  

} // The Loop

void ClearConsoleButtons() {
/*
    buttonLines[0].lastbutton = 0;
    buttonLines[0].state = 4;
    buttonLines[0].lastvalue = 9999;
    buttonLines[0].repeat_count = 0;
    buttonLines[0].down_buttons = 0;
*/
     buttonLines[0].lastvalue = 0;
    buttonLines[0].state = 4; // unknown
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
  //last_game_frame = current_frame;
  FetchGameInstruction(current_game_type, current_frame, &framecode[0]); // pass pointer to first elemnt of our instruction array
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
   // read q, judge, points
    lcd.setBacklight(BACKLIGHT_RED);
    DisplayModeTitle(FetchFrameName(current_frame));
   break;
 
   case PLAYER:
   // wait for answer, steal
      lcd.setBacklight(BACKLIGHT_GREEN);
      DisplayModeTitle(FetchFrameName(current_frame));
   break;
 
   case START_CLOCK:
   // start clock
       lcd.setBacklight(BACKLIGHT_RED);
      DisplayModeTitle(FetchFrameName(current_frame));
      LockoutEarlyBuzzers();
      ClearSubMode();
   break;
 
   case SYSTEM:
      lcd.setBacklight(BACKLIGHT_TEAL);
      DisplayModeTitle(FetchFrameName(current_frame));
      ResetPlayerList();
   break;
 
   case ANIM_FAIL:
      lcd.setBacklight(BACKLIGHT_YELLOW);
      DisplayModeTitle(FetchFrameName(current_frame));
      DQTeam(buzzing_team); // last buzzing team is ineliguble
      ClearNeoClock(); //reset the clock
      ClearSubMode(); // erase currently displaying player/team

      PlayGameAnimation(framecode[GO_TYPE]);
      break;
   
   case ANIM_MINOR_FAIL: // failed steal
       ResetPlayerList();
       ClearNeoClock(); //reset the clock
       ClearSubMode(); // erase currently displaying player/tea
   
   
   case ANIM_TIME:
      lcd.setBacklight(BACKLIGHT_YELLOW);
      DisplayModeTitle(FetchFrameName(current_frame));
      PlayGameAnimation(framecode[GO_TYPE]);
      ClearNeoClock();
      break;
   case ANIM_WIN:
      lcd.setBacklight(BACKLIGHT_YELLOW);
      DisplayModeTitle(FetchFrameName(current_frame));
      ResetPlayerList();
      PlayGameAnimation(framecode[GO_TYPE]);
      break;
    
   default: 
       lcd.setBacklight(BACKLIGHT_RED);
        DisplayModeTitle(FetchFrameName(current_frame));
 
  }

  lcd.setCursor(0,0);
    lcd.print(framecode[GO_TYPE]);

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


byte ConsoleModeChanged(){
/* --------------------
  Inputs: none
  Global: 
    current_console_mode, last_console_mode (byte) current global mode
    Timer1 - interrupt timer object
    
  Return: (bool/byte)
    true - mode has changed
    false - mode has not changed
    
  Note: Also enables/disables interrupt-driven timers if necessary
*/

  byte retVal  = false;  
  if (last_console_mode != current_console_mode){ // need to init new mode
    if (last_console_mode == CONSOLE_MENU){ // add ORs here for modes that use timer
      DisableRotaryScanner();
    }

    if (current_console_mode == CONSOLE_MENU){
      EnableRotaryScanner();
    }

    last_console_mode = current_console_mode;
    retVal = true;
  }    
  return retVal;
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



