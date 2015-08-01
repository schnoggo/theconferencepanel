// use the global: game_animation
/*
ANIM_TEAM_FAIL:
        ANIM_MINOR_FAIL:
        ANIM_MINOR_WIN:
        ANIM_TIME:
  */     
  
#define SEQUENCE_START 0
#define LIGHT_ALL 1
#define LIGHT_BUZZING_PLAYER 2
#define LIGHT_BUZZING_TEAM 3
#define LIGHT_OTHER_PLAYERS 4
#define LIGHT_OTHER_TEAM 5
#define CLEAR_ALL 6
#define SEQUENCE_UNDEF 7
#define SEQUENCE_END 8


struct animframe{
  byte type;
  int duration;
  byte r;
  byte g;
  byte b;
  } animations[16] ={
	{SEQUENCE_START,      ANIM_PLAYER_FAIL,0,0,0},
  {LIGHT_BUZZING_PLAYER,1000,  255,000,000},
  {LIGHT_BUZZING_PLAYER,1000,  000,000,000},
  {LIGHT_BUZZING_PLAYER,1000,  255,000,000},
  {SEQUENCE_END,0,  000,000,000},
  
  {SEQUENCE_START,      ANIM_TEAM_FAIL,0,0,0},
  {LIGHT_BUZZING_TEAM,1000,  255,000,000},
  {LIGHT_BUZZING_TEAM,1000,  000,000,000},
  {LIGHT_BUZZING_TEAM,1000,  255,000,000},
  {SEQUENCE_END,       0,  000,000,000},

  {SEQUENCE_START,     ANIM_TIME,0,0,0},
  {LIGHT_ALL,          1000,  255,000,000},
  {LIGHT_ALL,          1000,  000,000,000},
  {LIGHT_ALL,          1000,  255,000,000},
  {LIGHT_ALL,          0,  000,000,000},
  {SEQUENCE_END,       0,  000,000,000}

};  


void PlayGameAnimation(byte animNum){
  game_animation.state = 1;
  game_animation.start_time = millis();
  game_animation.animation = animNum;
  game_animation.step = 0;
}
void ServiceGameAnimation(){
  unsigned long end_time;
  int duration;
  switch (game_animation.state){
    case 1: // needs init
    // mostly just trigger sound here?\\\yte buzzing_teamplayer = 0;
// byte buzzing_player = 0;
//byte buzzing_team = 0;



  NeoWipe(pixel_ring.Color(0, 128, 0), 10); // Green
  NeoWipe(pixel_ring.Color(0, 90, 90), 20); 
  game_animation.state = 2;
    break;
    case 2: // main service routine
    
      switch(game_animation.animation){
      case ANIM_TEAM_FAIL:
        duration = 2500;
        break;
        
        default:
        duration = 1500;
      }
    
    end_time = game_animation.start_time + duration;
    if (end_time < millis()){
      NeoWipe(pixel_ring.Color(0, 0, 0), 5); // Black
    
       game_animation.state = 0;
     GoToFrame(framecode[GO_TIMER]);
    } 
    break;
  }
 // CheckMemoryUse();
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
