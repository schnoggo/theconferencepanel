#define CONSOLE_GO 1
#define CONSOLE_STOP 2
#define CONSOLE_CANCEL 3

// Screen types:
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

const uint8_t mode_instructions[11][5] PROGMEM = {
  {2,0,0,0,HOST},
{3,0,0,0,HOST},
{0,0,0,0,HOST},
{200,2,10,5,PLAYER},
{11,9,0,0,HOST},
{0,0,0,0,PLAYER},
{0,0,0,0,HOST},
{0,0,0,0,HOST},
{0,0,0,0,ANIM_FAIL},
{0,0,0,0,ANIM_TIME},
{0,0,0,0,ANIM_WIN}
};

 char* mode_names[] PROGMEM  ={"Initializing", "Idle", "Start Question", "Start Timer",
"Wait for Answer", "Correct?","Wait For Steal", "Callibrate: 1"};
  
  
uint8_t FetchGameInstruction(uint8_t) {
}

char* FetchFrameName(uint8_t f) {
  return mode_names[f];
}
