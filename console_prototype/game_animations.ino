// use the global: game_animation

void PlayGameAnimation(byte animNum){
  game_animation.state = 1;
  game_animation.start_time = millis();
  game_animation.animation = animNum;
}
void ServiceGameAnimation(){
  unsigned long end_time;
  int duration;
  switch (game_animation.state){
    case 1: // needs init
    // mostly just trigger sound here?
    break;
    case 2: // main service routine
    
    switch(game_animation.animation){
    case ANIM_FAIL:
      duration = 2500;
      break;
      
      default:
      duration = 1500;
    }
    
    end_time = game_animation.start_time + duration;
    if (end_time < millis()){
     GoToFrame(framecode[GO_TIMER]);
    } 
    break;
  }
  
}

void InitGameAnimations(){
  game_animation.state = 0;
 // commenting out other attributes since they are undefined anyway.
 // game_animation.start_time = 0;
 //  game_animation.animation = 0;
}
/*
typedef struct {
 char state;     // 0:not active, 1:needs init, 2: playing
 unsigned long start_time; // time of last read that was closed
  byte animation; // which line are we reading?

} gameanimation;
*/
