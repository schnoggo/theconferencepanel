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
void Time2Neo(byte secs) {
/* show current time on ring neopixel
 inputs:
 secs - time in seconds
 
 Globals:
   GameTimer
*/
 uint32_t fore_color;
 uint32_t back_color;
 uint32_t off_color;
 off_color = pixel_ring.Color(0, 0, 0);
 if (secs >= 5){
  fore_color = pixel_ring.Color(0, 90, 0);
   back_color = pixel_ring.Color(0, 6, 0);
 } else {
   fore_color = pixel_ring.Color(172, 0, 0);
    back_color = pixel_ring.Color(8, 0, 0);
 }
  
  for(byte i=0; i<16; i++) {
    if (i >= GameTimer.duration){
      pixel_ring.setPixelColor(i, off_color);
    } else {
      if (i != secs){
        pixel_ring.setPixelColor(i, back_color);
      } else {
        pixel_ring.setPixelColor(i, fore_color);
      }
    }
  }
  pixel_ring.show();
}
