
char* RAM_player_names[]   = {"Veronica Belmont", "Brian Dunaway", "Scott Johnson", "Turpster",
"Tom Merrit", "Spagnolo","Player 7", "Player 8"};

// player_status
// 0 - normal
// 1 -locked out


char* FetchPlayerName(byte f) {
  Serial.println(mode_names[f]);
 // DisplayModeTitle(mode_names[f]);
  return mode_names[f];

}

void DQPlayer(byte player_num, byte team_num){
  /* removes the player from the list of players elligible to buzz in.
     ie. locks out a single player
     Inputs: player number (byte)
     Outputs: updates player_status array
     */
     
     player_status[((team_num-1) * PLAYERS_PER_TEAM) + player_num-1]=1;
}

byte PlayerEligible(byte player_num, byte team_num){
  byte retVal = 1; // assume eligible unless flagged
  if ( (player_status[((team_num-1) * PLAYERS_PER_TEAM) + player_num-1]) == 1){
    retVal=0;
  }
  return retVal;
}

void ResetPlayerList(){
  byte i;
  for (i=0; i < NUMBER_OF_TEAMS * PLAYERS_PER_TEAM; i++){
    player_status[i]=1;
  }
}
