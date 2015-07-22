/*
typedef struct {
 byte state;     // 0:running,  1:expired, 2 paused,  -1:no clock
 unsigned long start_time; // when started or time paused
 unsigned long end_time; // when the clock expires, or how much remains if paused
} timer;
timer GameTimer;
*/

  
void StartCountdown(byte secs){
   unsigned long milis = secs *1000;
   GameTimer.start_time = millis(); 
  GameTimer.state=0;
  GameTimer.end_time = milis + GameTimer.start_time;
  GameTimer.duration = secs;
  if (SERIAL_DEBUG){
    Serial.println("StartCountdown");
    }

  
  
}

void PauseCountdown(){
  if (GameTimer.state == 0){ // can only pause a running timer
    unsigned long now = millis();
    unsigned long remaining = GameTimer.end_time -  now;
    GameTimer.state=2;
    GameTimer.end_time = remaining;
  }
   if (SERIAL_DEBUG){
    Serial.println("PauseCountdown");
    }
  
}

void ResumeCountdown(){
    if (GameTimer.state == 2){ // can only resume a paused timer
    unsigned long now = millis();
    unsigned long updated_endtime = now + GameTimer.end_time;
    GameTimer.state=0;
    GameTimer.end_time = updated_endtime;
  }
     if (SERIAL_DEBUG){
    Serial.println("ResumeCountdown");
    }
  
}

void TogglePause(){
  if (GameTimer.state == 2){
    ResumeCountdown();
     DisplaySubMode("      ");
  } else {
    PauseCountdown();
    DisplaySubMode("paused");
  }
}


// -------------------------------
byte CountdownExpired(){
/*
  Inputs:
    None
    
  Globals:
    GameTimer - Updates GameTimer.state
    
  Outputs:  
    1 clock has run out
    0 clock still running
    -1 no clock defined
    
*/

  byte retVal = GameTimer.state;
  unsigned long now;
  
  switch (GameTimer.state) {
    case 0: // running timer
    // see if timer has run out since last test
     now = millis();
    if (now >= GameTimer.end_time){
      retVal = 1;
      GameTimer.state = 1;
    }
    break;
    
    case 1: // already expired
    break;
    
    case 2: //paused
    retVal = 0;
    break;
    
    case -1: // no timer defined
    break;
  }
    return retVal;
}


// -------------------------------
byte GetCountdownSeconds(){
  // return current timer in seconds
  
  byte secs = 0;
  unsigned long now = millis();
  switch (GameTimer.state) {
 case 0:
  if (now >= GameTimer.end_time){
      GameTimer.state = 1;
    // secs is already default to 0
    } else {
      secs = (GameTimer.end_time - now)/1000;
    }
 break;
    case 2: // paused
    secs = (GameTimer.end_time)/1000;
    
  }
  return secs;
}
