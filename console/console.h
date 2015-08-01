// These #defines make it easy to set the backlight color
#define BACKLIGHT_RED 0x1
#define BACKLIGHT_YELLOW 0x3
#define BACKLIGHT_GREEN 0x2
#define BACKLIGHT_TEAL 0x6
#define BACKLIGHT_BLUE 0x4
#define BACKLIGHT_VIOLET 0x5
#define BACKLIGHT_WHITE 0x7

// the fields of a game frame:
#define GO_GO 0
#define GO_STOP 1
#define GO_TIMER 2
#define GO_PLAYER 3
#define GO_TYPE 4

// user buttons for bitmapped values:
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

typedef struct {
  char lastbutton; // which button was "down" during last poll (signed) -1 = no button
  byte state;     // 0:still open,  1:from open to closed 2:from closed to open 3:still closed 4: unkown
  unsigned int lastvalue; // last actual value from the pin
  uint8_t repeat_count; // number of times this value has been read
  unsigned long lastclosed; // time of last read that was closed
  byte pin; // which line are we reading?
  byte down_buttons; // bitmap of buttons currently down and ready to return

} ButtonLine;


// console buttons:
struct consolebuttonvals{
  byte state;
  byte seen_up;
  byte debounce_count;
  byte prev_state;
  } console_buttons[3];
  
  
// Screen types:
#define ANIM_TEAM_FAIL 6
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
#define RESET 14
#define ANIM_PLAYER_FAIL 15

// CONSOLE MODES
#define CONSOLE_MENU 0
#define SELECT_GAME_MODE 1
#define SET_SHORT_TIMER 2
#define SET_LONG_TIMER 3
#define ENTER_PLAYERS 4
#define GAME_IN_PROGRESS 5
#define CALIBRATING_RESISTORS 6
#define TEST_CONSOLE_BUTTONS 7