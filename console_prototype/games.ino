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

const uint8_t mode_instructions[11][5]  = {
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
  
  
void FetchGameInstruction(uint8_t mode, uint8_t frame, uint8_t *instruction_list) {
  /*
  mode - byte - which game mode JUMPBALL, LIGHTNING, etc.
  frame - byte - frame which is being displayed
  instruction_list - pointer - 5 element array for the return values
  */
}
/*
void FetchFrameName(uint8_t f, char retStr[]) {
 // return mode_names[f];
 strcpy(retStr, mode_names[f]);
}
*/
char* FetchFrameName(uint8_t f) {
  Serial.println(mode_names[f]);
 // DisplayModeTitle(mode_names[f]);
  return mode_names[f];

}
