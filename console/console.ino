// include the library code:
#include <Wire.h>
#include <avr/io.h>
#include <Adafruit_RGBLCDShield.h>
#include <Adafruit_NeoPixel.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include "console.h"

//   LCD SHIELD
// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define SERIAL_DEBUG false

#define PLAYERS_PER_TEAM 4
#define NUMBER_OF_TEAMS 2 // should be able to support 3 teams with an Uno\

// NEOPIXEL
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

// Analog pins - check resistance (current) of system to detect players buttons
#define TEAM1PIN 0
#define TEAM2PIN 1

// Inputs on the main game console: (digital inputs)
#define CONSOLE_GO_PIN 4
#define CONSOLE_STOP_PIN 3
#define CONSOLE_CANCEL_PIN 5

byte console_light_pins[3] = {11, 13, 2};

#define CONSOLE_BRIGHTNESS_FLOOR 60
unsigned long console_light_timer = 0;


ClickEncoder *encoder;
int16_t rotary_last, rotary_current;
ClickEncoder::Button rotary_button;

#define TICKDURATION 2000
#define MEMTICKDURATION 6000
#define DEBOUNCE_REPEAT 3 // increase if we start getting false hits
// 3 was selected by trial-and-error. Greater values allow spam-clicking to prevent registering of clicks.

unsigned long next_tick = 0;
unsigned long mem_tick;

char lcd_line2[17]; // leave room for the terminating zero


// struct has too much overhead for this, so simple globals:
byte clock_display_state_sec = 0;
byte clock_display_state_range = 0;

// Buttons, Teams, and Players:
char playerNames[(NUMBER_OF_TEAMS * PLAYERS_PER_TEAM)][10]; // create space for player names

char player_status[NUMBER_OF_TEAMS * PLAYERS_PER_TEAM]; //signed 8 bit

byte last_player_pressed = 0;
byte buzzing_teamplayer = 0;
byte buzzing_player = 0;
byte buzzing_team = 0;
ButtonLine buttonLines[NUMBER_OF_TEAMS+1]; // 0th "team" is console

// Game Types
#define NUMBER_OF_GAME_TYPES 3
#define GAME_TEAM_STEAL 0
#define GAME_LIGHTNING 1
#define GAME_ALL_PLAY 2

byte game_start_frame[NUMBER_OF_GAME_TYPES] = {15, 17, 18};



unsigned long calibrate_tick = 0;
// set to 1 to set up mode where we can get resistor values

byte current_console_mode = 0;
byte last_console_mode = 254;
byte current_game_type = 0;
byte prev_game_type =99;
byte current_frame = 0;
byte last_game_frame = 254;
byte framecode[5]; // global array of this frame's instuctions

// animation:

typedef struct {
 char state;     // 0:not active, 1:needs init, 2: playing
 unsigned long start_time; // time of last read that was closed
  byte animation; // which line are we reading?
  int step;
  int prev_step;

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


// globals for menu operations
int menu_cursor = 0;
int menu_last_cursor = 0xFF;
int accumulated_rotary_encoder_delta = 0;
#define MENU_ROTARY_DELTA_THRESHOLD 3


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
	digitalWrite(A1, LOW); // analog - do not use internal resistor - already in the circuit
	analogRead (TEAM1PIN) ; // do a dummy read to get the pin in the right state?

  InitRotary(10, 9, 8); // only activate for certain modes
	InitGameAnimations();
	next_tick = millis() + TICKDURATION;
	InitAnalogButtons();
	InitConsoleButtons();
  PlayConsoleButtonAnimation(0, 9);
  PlayConsoleButtonAnimation(1, 18);
	current_console_mode = CONSOLE_MENU;
  current_frame = 0; // actually, game_start_frame[current_game_type];
	LoadGameFrame();
	randomSeed(analogRead(2));

	dprintln("STARTUP");
}


void loop() {

  unsigned long now = millis(); // "now" get set at beginning of every loop
  byte need2init_mode = ConsoleModeChanged();
  if (need2init_mode){
      dprint("Mode change:");
      dprintln(current_console_mode);
  }

  switch (current_console_mode) {

    case CONSOLE_MENU:
    if(need2init_mode){
     DisplayModeTitle("Main Menu");
    }

    accumulated_rotary_encoder_delta += encoder->getValue(); //signed

    if (abs(accumulated_rotary_encoder_delta) >= MENU_ROTARY_DELTA_THRESHOLD){
      // No built-in sgn() !!
      if (accumulated_rotary_encoder_delta > 0){
        menu_cursor += 1;
      } else {
        menu_cursor -= 1;
      }
      if (menu_cursor < 0 ) {
         menu_cursor = NUMBER_OF_GAME_TYPES - 1;
       } else if (menu_cursor >= NUMBER_OF_GAME_TYPES){
         menu_cursor = 0;
       }
      accumulated_rotary_encoder_delta = 0;
    }

    if (menu_cursor != menu_last_cursor) {
        menu_last_cursor = menu_cursor;
        DisplaySubMode(FetchGameName(menu_cursor));
// sub
    }

    rotary_button = encoder->getButton();

    if (rotary_button != ClickEncoder::Open){
  /*
      if (rotary_button == ClickEncoder::Pressed){lcd.print("PRESS");}
      if (rotary_button == ClickEncoder::Held){lcd.print("HOLD");}
      if (rotary_button == ClickEncoder::Released){lcd.print("REL");}
      if (rotary_button == ClickEncoder::Clicked){lcd.print("CLIK");}
      if (rotary_button == ClickEncoder::DoubleClicked){lcd.print("DBL");}
      */
    if (rotary_button == ClickEncoder::Clicked){

      current_console_mode = GAME_IN_PROGRESS;
      current_game_type = menu_cursor;
      current_frame = game_start_frame[current_game_type];
    }
}


      //current_console_mode = SELECT_GAME_MODE;
      //current_console_mode = TEST_CONSOLE_BUTTONS;


    break;


    case CALIBRATING_RESISTORS:
      if(need2init_mode){
     DisplayModeTitle("Calibrate");
   }
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
        current_frame = game_start_frame[current_game_type];
        LoadGameFrame();
      }
      if(PollConsoleButtons(2)){ //red button
        current_console_mode = GAME_IN_PROGRESS;
        current_game_type = GAME_LIGHTNING;
        current_frame = game_start_frame[current_game_type];
        LoadGameFrame();
      }
    break;

    case TEST_CONSOLE_BUTTONS:
      if(PollConsoleButtons(1)){
        lcd.setCursor(0, 0);
        lcd.print("green");
        delay(1000);
        lcd.setCursor(0, 0);
        lcd.print("     ");
        }
       if(PollConsoleButtons(2)){
        lcd.setCursor(0, 1);
        lcd.print("red");
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print("   ");
        }
    break;

    default:
      if (prev_game_type != current_game_type){
        prev_game_type = current_game_type;
        // load first frame:
        current_frame = game_start_frame[current_game_type];
        LoadGameFrame();
      }
     //  DisplayModeTitle("Default(n prog?)");
      switch (framecode[GO_TYPE]) {

       // transient frames:
        case RESET:

          GoToFrame(framecode[GO_TIMER]);

        break;

        case START_CLOCK:
        //ResetPlayerList
          StartCountdown(11); // ten seconds on the clock
          LockoutEarlyBuzzers();
          GoToFrame(framecode[GO_TIMER]);
       break;


       case HOST:
       case SYSTEM:
       // CONSOLE GO BUTTON
        if(framecode[GO_GO]>0){
          if(PollConsoleButtons(1)){
            GoToFrame(framecode[GO_GO]);
          } // PollConsoleButtons
        }

        // CONSOLE STOP BUTTON
        if(framecode[GO_STOP]>0){
          if(PollConsoleButtons(2)){
            GoToFrame(framecode[GO_STOP]);
          }
        }
       break;

       case ANIM_TEAM_FAIL:
       case ANIM_PLAYER_FAIL:
       case ANIM_MINOR_FAIL:
       case ANIM_MINOR_WIN:
       case ANIM_TIME:
        ServiceGameAnimation();

       break;

      default: // regular frames:
        // PLAYER BUZZ:
        if(framecode[GO_PLAYER]>0){ // there is a frame for player buzz-in
          buzzing_teamplayer=PollUserButtons(false);
          buzzing_player=(byte) (buzzing_teamplayer & 0x0F);
          buzzing_team=(byte)((buzzing_teamplayer & 0xF0) >> 4);

          if (buzzing_player>0){
            ClearSubMode();
            // depending on game type, we may need to lock out this player, the player team, or reset everything
            //ClearUserButtons();
            lcd.setCursor(0, 1);
            lcd.print("player");
            lcd.setCursor(10, 1);
            lcd.print("team");
            lcd.setCursor(7, 1);
            lcd.print(buzzing_player);
            lcd.setCursor(15, 1);
            lcd.print(buzzing_team);

            // Light up the buzzing player:
            LightOnePlayer(buzzing_player,buzzing_team,pixel_ring.Color(255, 255, 255));
            //  if (last_player_pressed != buzzing_player){
            GoToFrame(framecode[GO_PLAYER]);
        }
        Time2Neo(GetCountdownSeconds(),  GameTimer.duration); // update the clock since we're waiting on player
      } // looking for player buzz-in

      // CONSOLE GO BUTTON
      if(framecode[GO_GO]>0){
        if(PollConsoleButtons(1)){
          switch(framecode[GO_GO]){
          // some GO frames are virtual. So case them out
            case 200: // pause
              TogglePause();
              break;
            default:
              GoToFrame(framecode[GO_GO]);
          }
        } // PollConsoleButtons
      }

      // CONSOLE STOP BUTTON
      if(framecode[GO_STOP]>0){
        if(PollConsoleButtons(2)){
          // ClearNeoClock();
          GoToFrame(framecode[GO_STOP]);
        }
      }

      // CONSOLE TIMER event
      if(framecode[GO_TIMER]>0){
        if(CountdownExpired()>0){
          GoToFrame(framecode[GO_TIMER]);
        }
      }

    } // switch GO_TYPE
  } // the giant switch statement (current_console_mode)
  ServiceGameAnimation();
  ServiceConsoleButtonAnimations();

} // The Loop




void LoadGameFrame(){

  //last_game_frame = current_frame;
  FetchGameInstruction(current_game_type, current_frame, &framecode[0]); // pass pointer to first elemnt of our instruction array

  switch (framecode[GO_TYPE]) {
   case HOST:
   // read q, judge, points
    lcd.setBacklight(BACKLIGHT_RED);
    DisplayModeTitle(FetchFrameName(current_frame));
    ClearNeoClock(); //reset the clock
    delay(1000);
   break;

   case PLAYER:
   // wait for answer, steal
      lcd.setBacklight(BACKLIGHT_GREEN);
      DisplayModeTitle(FetchFrameName(current_frame));
      ClearSubMode();
   break;

   case START_CLOCK:
   // start clock
      lcd.setBacklight(BACKLIGHT_RED);
      DisplayModeTitle(FetchFrameName(current_frame));
      ClearSubMode();
   break;

   case RESET:
      ResetPlayerList();
      ClearNeoClock(); //reset the clock
      ClearSubMode(); // erase currently displaying player/team   break;

          ClearPlayerLights();



  break;

   case SYSTEM:
      lcd.setBacklight(BACKLIGHT_TEAL);
      DisplayModeTitle(FetchFrameName(current_frame));
      ResetPlayerList();
      ClearSubMode();
   break;

   case ANIM_TEAM_FAIL:
      lcd.setBacklight(BACKLIGHT_YELLOW);
      DisplayModeTitle(FetchFrameName(current_frame));
      DQTeam(buzzing_team); // last buzzing team is ineliguble
      ClearNeoClock(); //reset the clock
      ClearSubMode(); // erase currently displaying player/team

      PlayGameAnimation(ANIM_TEAM_FAIL);
     break;

    case ANIM_PLAYER_FAIL:
     lcd.setBacklight(BACKLIGHT_YELLOW);
      DisplayModeTitle(FetchFrameName(current_frame));
      DQTeam(buzzing_team); // last buzzing team is ineliguble
      ClearNeoClock(); //reset the clock
      ClearSubMode(); // erase currently displaying player/team
    PlayGameAnimation(ANIM_PLAYER_FAIL);

    break;

   case ANIM_MINOR_FAIL: // failed steal
       ResetPlayerList();
       ClearNeoClock(); //reset the clock
       ClearSubMode(); // erase currently displaying player/tea
    PlayGameAnimation(ANIM_MINOR_FAIL);

   break;

   case ANIM_TIME:
     ClearNeoClock();
      lcd.setBacklight(BACKLIGHT_YELLOW);
      DisplayModeTitle(FetchFrameName(current_frame));
      PlayGameAnimation(ANIM_TIME);
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
    dprint("mem ");
    dprint(mem_tick);
    dprint(": ");
    dprintln(FreeRam());
    mem_tick++;
  }

}

void PlayConsoleButtonAnimation(byte button_num, int8_t new_dir){
  // dir of 0 - stop animaiton and go black
  // dir positive integer - step size of ramp animation
  // dir negative integer - TBD
  console_buttons[button_num].anim_dir =  new_dir;
  //console_buttons[button_num].brightness = max(CONSOLE_BRIGHTNESS_FLOOR, CONSOLE_BRIGHTNESS_FLOOR - new_dir);
  console_buttons[button_num].brightness =  CONSOLE_BRIGHTNESS_FLOOR - new_dir;

}
void ServiceConsoleButtonAnimations(){
  // uses global values to determine button states
  // console_lights
  // console_light_timer - time of next animation event
  //  CONSOLE_LIGHT_GO_PIN, CONSOLE_LIGHT_STOP_PIN - PWM output pins for button lights
  unsigned long now = millis();
  bool dirty = false;
  int new_brightness;
  int8_t anim_dir;
  if (console_light_timer <= now){
    for (i=0; i < 2; i++){
      // figure new brightness

      new_brightness = console_buttons[i].brightness + console_buttons[i].anim_dir;
      if  (new_brightness < CONSOLE_BRIGHTNESS_FLOOR) {
          new_brightness = CONSOLE_BRIGHTNESS_FLOOR;
          console_buttons[i].anim_dir = 0 - console_buttons[i].anim_dir; // flip direction
      } else if (new_brightness>255) {
        new_brightness = 255;
        console_buttons[i].anim_dir = 0 - console_buttons[i].anim_dir; // flip direction
      }
      if (0 == console_buttons[i].anim_dir){new_brightness = 0;} //if dir is 0 turn it off

      if (new_brightness != console_buttons[i].brightness){
        console_buttons[i].brightness = new_brightness;
        analogWrite(console_buttons[i].anim_pin, console_buttons[i].brightness);
  /*
        if (0 == i){
        dprint("but  ");
        dprint(i);
        dprint(": ");
        dprintln(new_brightness);
        }
        */
      }
    } // step through buttons
    console_light_timer = now + 30;
  } // timer is up
}
