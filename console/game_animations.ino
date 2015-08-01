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

#define TOTAL_ANIMATION_FRAMES 35
struct animframe{
  byte type; // type of sequence record
  int duration; // or animation "name" if SEQUENCE_START
  byte r;
  byte g;
  byte b;
  } animations[TOTAL_ANIMATION_FRAMES] ={
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
  {LIGHT_ALL,          5000,  255,000,000},
  {LIGHT_ALL,          5000,  000,000,000},
  {LIGHT_ALL,          5000,  255,000,000},
  {LIGHT_ALL,          5000,  000,000,000},
  {LIGHT_ALL,          5000,  255,000,000},
  {LIGHT_ALL,          0,  000,000,000},
  {SEQUENCE_END,       0,  000,000,000},
  
  {SEQUENCE_START,     ANIM_MINOR_WIN,0,0,0},
  {LIGHT_ALL,          1000,  000,255,000},
  {LIGHT_ALL,          1000,  000,000,000},
  {LIGHT_ALL,          1000,  000,255,000},
  {LIGHT_ALL,          0,  000,000,000},
  {SEQUENCE_END,       0,  000,000,000},
  
   {SEQUENCE_START,     ANIM_MINOR_FAIL,0,0,0},
  {LIGHT_BUZZING_TEAM, 1000,  255,000,000},
  {SEQUENCE_END,       0,  000,000,000},
  
  {SEQUENCE_START,      ANIM_WIN,0,0,0},
  {LIGHT_BUZZING_TEAM,300,  000,255,000},
  {LIGHT_BUZZING_TEAM,300,000,000,000},
  {LIGHT_BUZZING_TEAM,500,  000,255,000},
  {LIGHT_BUZZING_TEAM,500,000,000,000},
  {LIGHT_BUZZING_TEAM,1000,  000,255,000},
  {LIGHT_BUZZING_TEAM,1000,000,000,000},
  {SEQUENCE_END,0,  000,000,000}

};  


void PlayGameAnimation(byte animNum){
  game_animation.state = 1;
  game_animation.start_time = millis();
  game_animation.animation = animNum;
  byte i;
  game_animation.step = 0;
    for (i=0; i < TOTAL_ANIMATION_FRAMES; i++){
      if( (SEQUENCE_START == animations[i].type) && ( animNum == animations[i].duration) ){
        game_animation.step = i; // Next instruction is the beginning of the sequence
      }
    }  
}


void ServiceGameAnimation(){
  unsigned long end_time;
  switch (game_animation.state){
    case 1: // needs init
    // mostly just trigger sound here?
  game_animation.start_time = millis();
  game_animation.state = 2;
    break;
    
    case 2: // main service routine
      // load sequence frame:
        int duration = animations[game_animation.step].duration;
        uint32_t c = pixel_ring.Color(animations[game_animation.step].r, animations[game_animation.step].g, animations[game_animation.step].b);
        switch (animations[game_animation.step].type){
          
        case SEQUENCE_END:
          NeoWipe(pixel_ring.Color(0, 0, 0), 0); // Black
          game_animation.state = 0;
          GoToFrame(framecode[GO_TIMER]);        
        break;
        
        case LIGHT_BUZZING_PLAYER:
              LightOnePlayer(buzzing_player, buzzing_team, c); // player, team, color
          break;
        
         case LIGHT_BUZZING_TEAM:
            LightTeam(buzzing_team, c);
          
          
          break; 
        
        case LIGHT_ALL:
          LightTeam(1, c);
          LightTeam(2, c);
          
          break;  
        
        default:
        NeoWipe(pixel_ring.Color(0, 255, 255), 5); 
        
        
        
        }
     
    
    end_time = game_animation.start_time + duration;
    if (end_time < millis()){
      game_animation.step++;
    } 
    break; // main service routine
  }
 // CheckMemoryUse();
}

void InitGameAnimations(){
  game_animation.state = 0;
}
