/*
Globals:
buttonLines - see struct in console
NUMBER_OF_TEAMS (define) -- number of lines to poll +1 (The size of the buttonLines array)
 byte lastbutton; // which button was "down" during last poll
 byte state;     // 0:still open,  1:from open to closed 2:from closed to closed 3:still closed, 4: unknown
 unsigned int lastvalue; // last actual value from the pin
 uint8_t repeat_count; // number of times this value has been read
 unsigned long lastclosed; // time of last read that was closed
  byte pin; // which line are we reading?
  byte down_buttons;
  
player_button_thresholds - list of current values for buttons on this branch

*/

void InitAnalogButtons(){
  // 0 is console
  // 1-3 are teams
  
/* fill in the starting values for   buttonLines global structure

*/

 //  byte i; // use the global
  for (i=0; i <= NUMBER_OF_TEAMS; i++) {
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

void LockoutEarlyBuzzers() {
 // Checks to see if someone is already byzzing and locks them out.
       Serial.print("LockoutEarlyBuzzers()");
        lcd.setBacklight(BACKLIGHT_WHITE);
        uint32_t c =  NeoWheel(55);

  byte junk = PollUserButtons(true); // true = lockout any buttons already down

}

byte PollUserButtons(byte lockout) {
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
   returns player number of newly pressed button (0) for no press.
   low nybble = [player number. High nybble team number
*/
    byte retPlayer=0;
    byte retTeam;
    byte i;
    byte j;
    byte one_button;
    unsigned int team_values[NUMBER_OF_TEAMS+1]; // was v
    unsigned int v;
    
    byte possible_winners[NUMBER_OF_TEAMS * PLAYERS_PER_TEAM];
    byte winner_counter = 0;
    retTeam = 0;
    // debug stuff:
    char outnum[9];
    
    
    // if we start seeing team bias, build an array of teams here to determine
    // the order in which we test them.
    
    
    // First grab raw values for each team: 
  for (i=1; i <= NUMBER_OF_TEAMS; i++) { // channel 0 is the console - start with 1
    team_values[i]= analogRead(buttonLines[i].pin);
/*    
    Serial.print("Team ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(team_values[i]);
 */
 
  }


    
  for (i=1; i <= NUMBER_OF_TEAMS; i++) { // channel 0 is the console - start with 1
    v=team_values[i];
    buttonLines[i].down_buttons=0; // code for currently depressed buttons. 0 if nothing down.
    if (abs(v-buttonLines[i].lastvalue) <=3) {
      // repeated value:
      if (buttonLines[i].repeat_count < 254) {buttonLines[i].repeat_count++;} // count it
        if (buttonLines[i].repeat_count > DEBOUNCE_REPEAT) {
          // stable value - maybe count it as a button press
          
          if (current_console_mode==CALIBRATING_RESISTORS and (i==2)){
          // display the stable value:
            if ((millis() - calibrate_tick ) > 400) {
              sprintf(outnum, "%03d %04d", buttonLines[i].repeat_count, buttonLines[i].lastvalue);
              lcd.setCursor(7, i - 1);
              lcd.print(outnum);
              calibrate_tick = millis();
            }
          } else { // calibrating
            // not calibrating - actually return user and team:

            if (v > player_button_thresholds[0].reading){ // make sure something is pressed

              for (j=15; j>0; j--) { //step down through button thresholds
               if ( v >= player_button_thresholds[j].reading ){
                  buttonLines[i].down_buttons=player_button_thresholds[j].button_bits;
                  break; // exit the loop, since we only want the highest value
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
     
     // at this point, we may have a bit-packed array of possible players for this team.
     // that value is in  buttonLines[i].down_buttons
              
  }  
  
   // all teams have been scanned.
   // look through all pressed buttons.
   // 
   
   // we should probably handle lockouts here...
/*   
sprintf(outnum, "%04d", v);
lcd.setCursor(10, 1);
lcd.print(outnum);
*/

  // build an array of possible winners
for (i=1; i<=NUMBER_OF_TEAMS; i++) { 
    for (j=1; j<=PLAYERS_PER_TEAM; j++) {
      if (buttonLines[i].down_buttons & (1<<(j-1))) { // button was down
       if (lockout){
         DQPlayer(j,i);
        } else {
          if (PlayerEligible(j,i)){
            possible_winners[winner_counter]=(i*16) + j;
            winner_counter++;
          }
         }// lockout
      } // button down
    }
  }
// now is possible_winners array of possible return values
// winner_counter = number of elements
retPlayer = 0;
if (winner_counter > 0){
  i = random( winner_counter);
  retPlayer = possible_winners[i];
}      
  return retPlayer;
}





byte PollConsoleButtons(byte lookingfor) {
  /*
  Input:
  1 - GO button
  2 - STOP button
  3 - dial button
  
  states
    0 up
    1 down
    3 still down
    4 unknown
  
  Returns:
  1/true - yup this button is pressed
  0/false - this button is not pressed
  */
  
  
  
  byte retVal=0; // false unless we find a debounced and non-repeating button
  byte currently_down = 0;
  
  
  for (i=0; i<3; i++) {
    // read pin: different pins are read in different ways
      switch (i) {
        case 0:
          if (digitalRead(CONSOLE_GO_PIN) == LOW){ currently_down = 1;}
        break;
        
        case 1:
          if (digitalRead(CONSOLE_STOP_PIN) == LOW){ currently_down = 1;}
        break;
        
        case 3:
        // not implemented
        break;
      }
    
      switch (console_buttons[i].state){
        case 0: //up
          if (currently_down){
            // start debounce
            console_buttons[i].debounce_count = 0;
            console_buttons[i].state = 2; // bounce wait
          } else {
            console_buttons[i].seen_up = 1;
          }
        break;
                
        case 1: //down
          if (currently_down){
            console_buttons[i].seen_up = 0; // need to see it up before we count this again
            console_buttons[i].state = 3; // still down
          } else {
            console_buttons[i].state = 0; // up
            console_buttons[i].seen_up = 1;
          }
        break;
                
        case 2: // waiting for bounce to settle
          if (currently_down){
            if (console_buttons[i].debounce_count < 100){console_buttons[i].debounce_count++;}
            if (console_buttons[i].debounce_count > 3){
              console_buttons[i].state = 1; // down
            }
          } else {
            console_buttons[i].state = 0; // up
            console_buttons[i].seen_up = 1;
          }
        break;
      
        case 3: // still down
          if (currently_down){
          } else {
            console_buttons[i].state = 0; // up
            console_buttons[i].seen_up = 1;
          }
        break;
      
        case 4: //undefined state
          if (currently_down){
            console_buttons[i].seen_up = 0;
            console_buttons[i].state = 1; // down
          } else {
            console_buttons[i].state = 0; // up
            console_buttons[i].seen_up = 1;          
          }
        break;    
      }
    }
    
    // all inputs read. Check to see if the one we want is ready
    if (console_buttons[lookingfor-1].state != console_buttons[lookingfor-1].prev_state){
      if (console_buttons[lookingfor-1].state == 1){
        if (SERIAL_DEBUG){
          Serial.print("button ");
          Serial.println(lookingfor);  
        }
        retVal = 1;
      }
    }
    
    for (i=0; i<3; i++) {
      console_buttons[i].prev_state = console_buttons[i].state;
    }

    
             
                
  return retVal;
}


void InitConsoleButtons() {
  byte i;
  for (i=0; i<3; i++) { 
    console_buttons[i].state = 4; //unknown
    console_buttons[i].seen_up = 0; //have seen an up state yet
    console_buttons[i].debounce_count = 0; //clear debounce counter
    console_buttons[i].prev_state = 99;
  }
}

