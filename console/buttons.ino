/*
Globals:
buttonLines - see struct in console
NUMBEROFTEAMS (define) -- number of lines to poll +1 (The size of the buttonLines array)
 byte lastbutton; // which button was "down" during last poll
 byte state;     // 0:still open,  1:from open to closed 2:from closed to closed 3:still closed, 4: unknown
 unsigned int lastvalue; // last actual value from the pin
 uint8_t repeat_count; // number of times this value has been read
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
    buttonLines[i].lastvalue = 9999;
    buttonLines[i].lastbutton = 0;
    buttonLines[i].repeat_count = 0;
    }
  
  buttonLines[1].pin = TEAM1PIN;
  buttonLines[2].pin = TEAM2PIN;
  calibrate_tick = 0;
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
    byte retTeam;
    byte i;
    byte j;

    unsigned int v;
    retTeam = 0;
    // debug stuff:
    char outnum[9];
    
    for (i=1; i <= NUMBEROFTEAMS; i++) { // channel 0 is the console - start with 1
      v=analogRead(buttonLines[i].pin);
      if (abs(v-buttonLines[i].lastvalue) <=3) {
        // repeated value:
        if (buttonLines[i].repeat_count < 254) {buttonLines[i].repeat_count++;} // count it
          if (buttonLines[i].repeat_count > 8) {
            // stable value - maybe count it as a button press

            if (CALIBRATING){
            // display the stable value:
              if ((millis() - calibrate_tick ) > 400) {
                sprintf(outnum, "%03d %04d", buttonLines[i].repeat_count, buttonLines[i].lastvalue);
                lcd.setCursor(7, i - 1);
                lcd.print(outnum);
                calibrate_tick = millis();
              }
            } else { // calibrating
              // not calibrating - actually return user and team:

              if (v > player_button_thresholds[0]){ // make sure something is pressed
                for (j=4; j>0; j--) { //step down through button thresholds
                  if ( v >= player_button_thresholds[j] ){
                    retPlayer = (i-1)*5 + j;
                    retTeam = i;
                    i = (NUMBEROFTEAMS + 1); // old-school (and dirty) method to break outer loop
                    break;

                  }
                } // step through thresholds
              } // quick check for ANY press
          } // calibrate or real button
        } // stable value

      } else {
        // changing value. Rest counter and take new value
        buttonLines[i].lastvalue = v;
        buttonLines[i].repeat_count = 0;
      }
    } // step through input pins    
  return retPlayer;
}
