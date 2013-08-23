/*
Globals:
buttonLines - see struct in console
NUMBEROFTEAMS (define) -- number of lines to poll +1 (The size of the buttonLines array)
 byte lastbutton; // which button was "down" during last poll
 byte state;     // 0:still open,  1:from open to closed 2:from closed to closed 3:still closed, 4: inknown
 unsigned int lastvalue; // last actual value from the pin
 unsigned long lastread; // time of last read
 unsigned long lastclosed; // time of last read that was closed
  byte pin; // which line are we reading?
  


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
