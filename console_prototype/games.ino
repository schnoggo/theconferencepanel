#define CONSOLE_GO 1
#define CONSOLE_STOP 2
#define CONSOLE_CANCEL 3

/* games: (defined in console)
GAME_TEAM_STEAL 0
GAME_LIGHTNING 1
GAME_ALL_PLAY 2
*/
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

const byte mode_instructions[11][5]  = {
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

 char* mode_names[]   = {"Initializing", "Idle", "Start Question", "Start Timer",
"Wait for Answer", "Correct?","Wait For Steal", "Callibrate: 1"};
  
  
void FetchGameInstruction(byte mode, byte frame, byte *instruction_list) {
  /*
  mode - byte - which game mode GAME_TEAM_STEAL, GAME_LIGHTNING, etc.
  frame - byte - frame which is being displayed
  instruction_list - pointer - 5 element array for the return values
  */
  byte i;
  for (i=0; i < 5; i++){
    instruction_list[i] = mode_instructions[frame][i];
  }
  
}






char* FetchFrameName(uint8_t f) {
  // eventually rewrite this to use PROGMEN
  return mode_names[f];
}
