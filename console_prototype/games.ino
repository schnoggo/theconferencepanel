

/* games: (defined in console)
GAME_TEAM_STEAL 0
GAME_LIGHTNING 1
GAME_ALL_PLAY 2
*/


const byte mode_instructions[11][5]  = {
{1,0,0,0,HOST},
{2,0,0,0,HOST},
{3,0,0,0,START_CLOCK},
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
  
  
void FetchGameInstruction(byte game, byte frame, byte *instruction_list) {
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




char* FetchFrameName(byte f) {
  // eventually rewrite this to use PROGMEN
  return mode_names[f];
}

void GoToFrame( byte f){
  // perform any pre-frame processing.
  current_frame = f;
  LoadGameFrame();
}
