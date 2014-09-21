
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

void DQPlayer(byte playernum){
  /* removes the player from the list of players elligible to buzz in.
     ie. locks out a single player
     Inputs: player number (byte)
     Outputs: updates player_status array
     */
     
     player_status[playernum]=1;
}

void ResetPlayerList(){
  byte i;
  for (i=0; i < 16; i++){
    player_status[i]=1;
  }
}
