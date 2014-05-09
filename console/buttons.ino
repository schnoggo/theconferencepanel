/*
Globals:
buttonLines - see struct in console
NUMBEROFTEAMS (define) -- number of lines to poll +1 (The size of the buttonLines array)
 byte lastbutton; // which button was "down" during last poll
 byte state;     // 0:still open,  1:from open to closed 2:from closed to closed 3:still closed, 4: unknown
 unsigned int lastvalue; // last actual value from the pin
 unsigned long lastread; // time of last read
 unsigned long lastclosed; // time of last read that was closed
  byte pin; // which line are we reading?
  
player_button_thresholds - list of current values for buttons on this branch

*/

void InitAnalogButtons(){
  // 0 is console
  // 1-3 are teams
  
/* fill in the starting values for   buttonLines global structure

*/

  byte i;
  for (i=0; i <= NUMBEROFTEAMS; i++) {
    buttonLines[i].lastbutton = 0;
    buttonLines[i].state = 4;
    buttonLines[i].lastread = 0;
    buttonLines[i].lastbutton = 0;
    }
  
  buttonLines[1].pin = TEAM1PIN;
  buttonLines[2].pin = TEAM2PIN;
}


void ClearUserButtons() {
InitAnalogButtons();

}

byte PollUserButtons() {
/* 
Inputs: none (just globals)
Hardware: Uses 3 analog pins to determine which button is being pushed
Globals: UserButton{
    lastbutton: int8 value of last button pressed
    state: int8 state of last button read {
      0:still open, 
      1:from open to closed
      2:from closed to closed
      3:still closed
    }
    lastread: unsigned long time of last read
    lastclosed: unsigned long time of last read that was closed
    TEAM1PIN
    player_button_thresholds
    
 Outputs:
   updates UserButton
   returns player number of newly pressed button
*/
    byte retPlayer=0;
    byte i;
    byte j;
    byte team;
    unsigned int v;
    team = 0;
    for (i=1; i <= NUMBEROFTEAMS; i++) { // channel 0 is the console - start with 1
      v=analogRead(buttonLines[i].pin);
      if (v > player_button_thresholds[0]){ // make sure something is pressed

        for (j=4; j>0; j--) { //step down through button thresholds
          if ( v >= player_button_thresholds[j] ){
            // debounce here
            retPlayer = (i-1)*5 + j;
            team = i;
            i = (NUMBEROFTEAMS + 1); // old-school (and dirty) method to break outer loop
            break;
          }
        }
      }
    }
    
  return retPlayer;
}
