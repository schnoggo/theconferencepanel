

/* games: (defined in console)
GAME_TEAM_STEAL 0
GAME_LIGHTNING 1
GAME_ALL_PLAY 2
*/


const byte mode_instructions[25][5]  = {
{ 1,  0,  0,  0, SYSTEM}, // 00 init
{ 2,  0,  0,  0, SYSTEM}, // 01 idle
{ 3,  0,  0,  0, HOST}, // 02 read Q
{ 0,  0,  4,  0, START_CLOCK}, //03 start clock
{200, 2, 10,  5, PLAYER}, // 04 wait for answer
{11,  9,  0,  0, HOST}, // 05 JUDGE!
{200, 2, 10,  7, PLAYER}, //06 Steal (wait for answer)
{11, 15,  0,  0, HOST}, // 07 JUDGE! (steal) NO was 13
{ 2,  0,  0,  0, HOST}, // 08 award points
{ 0,  0, 12,  0, ANIM_FAIL}, //09 
{ 0,  0, 02,  0, ANIM_TIME}, //10
{ 0,  0,  8,  0, ANIM_WIN}, //11
{ 14,  15,  0,  0, HOST}, // 12 Steal?
{ 0,  0,  2,  0, ANIM_MINOR_FAIL}, // 13 Minor Fail animation (failed steal)
{ 0,  0,   6,  0,  START_CLOCK}, // 14 start steal clock
{ 0,  0,  2,  0, RESET}, // 15 reset
{ 0,  0,  2,  0, RESET}, // 16 placeholder

//{ 18,  0,  0,  0, SYSTEM}, // 17 idle
{ 00,  0,  18,  0, RESET}, // 17 idle
{ 19,  0,  0,  0, HOST}, // 18 read Q
{ 0,  0,  20,  0, START_CLOCK}, //19 start clock
{200, 17, 23,  21, PLAYER}, // 20 wait for answer
{ 22,  23,  0,  0, HOST}, // 21 JUDGE!
{ 0,  0,  24,  0, ANIM_WIN}, //22
{ 0,  0,  17,  0, ANIM_FAIL}, //23
{17,  0,  0,  0, HOST} // 24 award points

};

 char* mode_names[]   = {
 "Initializing", //0
 "Idle", // 01
 "Read Question",//02
 "Start Timer", // 03
"Wait for Answer", //04
"Correct?", //05
"Wait For Steal", //06
"Correct?", //07
"Award Points", //08
"-WRONG-", // 09
"- TIME -", //10
"- CORRECT -", //11
"Steal?",  // 12
"- NOPE -", //13
"start steal",//14
"reset", //15
"placeholder", //16


"idle*", //17
"Read Question", //18
"Start timer", //19
"Wait for Answer", //20
"Correct?", //21
"- YAY -", //22
"-- BOO --", //23
"Award Points" //24

};
  
  
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
