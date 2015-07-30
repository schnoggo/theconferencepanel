

/* games: (defined in console)
GAME_TEAM_STEAL 0
GAME_LIGHTNING 1
GAME_ALL_PLAY 2
*/


const byte mode_instructions[16][5]  = {
{ 1,  0,  0,  0, SYSTEM}, // 00 init
{ 2,  0,  0,  0, SYSTEM}, // 01 idle
{ 3,  0,  0,  0, HOST}, // 02 read Q
{ 4,  0,  0,  0, START_CLOCK}, //03 start clock
{200, 2, 10,  5, PLAYER}, // 04 wait for answer
{11,  9,  0,  0, HOST}, // 05 JUDGE!
{200, 2, 10,  7, PLAYER}, //06 Steal (wait for answer)
{11, 13,  0,  0, HOST}, // 07 JUDGE! (steal)
{ 2,  0,  0,  0, HOST}, // 08 award points
{ 0,  0, 12,  0, ANIM_FAIL}, //09 
{ 0,  0, 02,  0, ANIM_TIME}, //10
{ 0,  0,  8,  0, ANIM_WIN}, //11
{ 15,  2,  0,  0, HOST}, // 12 Steal?
{ 1,  0,  0,  0, ANIM_MINOR_FAIL}, // 13 Minor Fail animation (failed steal)
{ 4,  0,   0,  0,  PAUSE}, // 14
{ 6,  0,   0,  0,  START_CLOCK}, // 15

};

 char* mode_names[]   = {"Initializing", "Idle", "Read Question", "Start Timer",
"Wait for Answer", "Correct?","Wait For Steal", "Correct?", "Award Points", "-WRONG-", 
"- TIME -", "- CORRECT -", "Steal?", "- NOPE -", "Callibrate: 1"};
  
  
   char* game_names[]   = {
    // 1234567890123456
      "All w Steal",
      "Lightning",
      "All play"
      };

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

char* FetchGameName(byte f) {
  // eventually rewrite this to use PROGMEN
  return game_names[f];
}

void GoToFrame( byte f){
  // perform any pre-frame processing.
  current_frame = f;
  LoadGameFrame();
}
