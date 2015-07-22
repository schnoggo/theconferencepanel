// -------------------------------
uint32_t NeoWheel(byte WheelPos) {
/* (From Adafruit sample code)
  Globals:
    pixel_ring Adafruit Neopixel object
    
  Inputs:
    WheelPos (byte) Position on the color wheel (0 - 255)
    The colours are a transition r - g - b - back to r

  Outputs:
    packed color value as used in Adafruit libraries
*/
  
  if(WheelPos < 85) {
   return pixel_ring.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixel_ring.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixel_ring.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// Fill the dots one after the other with a color
void NeoWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixel_ring.numPixels(); i++) {
      pixel_ring.setPixelColor(i, c);
      pixel_ring.show();
      delay(wait);
  }
}



// -------------------------------
void ClearNeoClock(){
/*


*/
   uint32_t c;
      clock_display_state_sec = 0;
      clock_display_state_range = 99;
      c =  pixel_ring.Color(0, 0, 0);
        for(uint16_t i=0; i<pixel_ring.numPixels(); i++) {
      pixel_ring.setPixelColor(i, c);
  }
      pixel_ring.show();

}


// -------------------------------
void Time2Neo(byte secs, byte range) {
/* show current time on ring neopixel
 inputs:
 secs - time in seconds
 range - how many possible seconds on the timer
 
 
 Globals:
   GameTimer
   byte clock_display_state_sec = 0;
   byte clock_display_state_range = 0;
   
   to-do
   scale display based on range
*/
 uint32_t fore_color;
 uint32_t back_color;
 uint32_t off_color;
 int8_t real_offset;
 secs = range-secs; // make it a countdown timer
  if ((clock_display_state_sec != secs) ||  (clock_display_state_range != range)){
    off_color = pixel_ring.Color(0, 0, 0);
    if (secs < 5){
      // one set of values greater than 5 secs
      fore_color = pixel_ring.Color(0, 90, 0);
      back_color = pixel_ring.Color(0, 6, 0);
    } else {
      // at 5 seconds change the clock colors
      fore_color = pixel_ring.Color(172, 0, 0);
      back_color = pixel_ring.Color(8, 0, 0);
    }
  //  back_color = pixel_ring.Color(40, 40, 40); 
    for(byte i=0; i<CLOCK_RING_SIZE; i++) {
      real_offset = NormalizeRingPos(i);
      if (i >= range){
        pixel_ring.setPixelColor(real_offset, off_color);
      } else {
        if (i != secs){
          pixel_ring.setPixelColor(real_offset, back_color);
        } else {
          pixel_ring.setPixelColor(real_offset, fore_color);
        }
      }
    }
    pixel_ring.show();
  } // don't do it if no need to update
}


void LightPlayer(uint8_t player, uint8_t team, uint32_t color, uint8_t dontshow){
/*
  player
  team
  color
  dontshow
  
  There are wto pixels per player - front and back

*/

byte pixel_num = ( (player-1)*2) + ((team-1) * (PLAYERS_PER_TEAM*2)) + 16;

//  byte i = ((player-1)*2) + ((team-1)*8) + 16; // 16 pixels on the on-board display

      pixel_ring.setPixelColor(pixel_num, color);
      pixel_ring.setPixelColor(pixel_num+1, color);
      if (dontshow == 0){
      pixel_ring.show();
      }
  
}

void LightOnePlayer(uint8_t player, uint8_t team, uint32_t color){
/* Light up just one player's lights

*/

  byte i;
  byte j;
  for(i=1; i<=PLAYERS_PER_TEAM; i++) {
    for(j=1; j<=NUMBER_OF_TEAMS; j++) {
      if((i != player ) or (j != team)){
        LightPlayer(i, j, pixel_ring.Color(0, 0, 0), 0);
      } else {
        LightPlayer(i, j, color, 0);
      }
    }
  }
   pixel_ring.show();
}




int8_t NormalizeRingPos(int8_t realPos){
  realPos = realPos + CLOCK_RING_OFFSET;
  while (realPos < 0) { realPos += 16;}
  while (realPos > 15) { realPos -= 16; }  
  return realPos;
}