
void rainbow(uint8_t wait) {
  int i, j;

  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}


void rainbowCycle(uint8_t wait) {
// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
  int i, j;
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}



// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b){
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

void UpdateSampledAudio(){
  //
  // rotates through a buffer of audio samples
  // sets the following globals:
  //  last_sample_time
  //  max_seen_volume
  //  avg_audio_sample


  unsigned long now = millis();
  unsigned int current_sample; // 16 bit
  // globals:
  //  avg_audio_sample (long)
// last_sample_time
// sample_window_start
// sample_window_max

  if ((now - last_sample_time)>8){ // don't try to read too fast
    if ((now -  sample_window_start)<64){
      // within the window
      current_sample=analogRead(AUDIO_IN_PIN);
      last_sample_time = now;
      if (current_sample<150) {current_sample=0;} // just noise
      if (current_sample<1024){ // discard values over 1023 (apparently can be noise)
        if (current_sample>sample_window_max){
          sample_window_max=current_sample;
        }
        if (current_sample<sample_window_min){
          sample_window_min=current_sample;
        }
      }

    } else {
      // start a new sample window
      avg_audio_sample = abs(sample_window_max - sample_window_min);
      avg_audio_sample = avg_audio_sample*CONVERT33TO5V; //scale the 3.3 volt to 5v (3.3 power is cleaner)
      sample_window_start = now;
      sample_window_max=0;
      sample_window_min=1024;

    }
  }
}
//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}



void BackGroundDelay(unsigned long delay_milliseconds){
  unsigned long now = millis();
  while ((now + delay_milliseconds) > millis()){
    ServiceBackground();
  }
}



void ServiceBackground(){
  unsigned long now = millis();
  int text_color = LED_GREEN;
    UpdateSampledAudio();


    #define BUTTON_PIN 2
    uint8_t button_state = 0;
    uint8_t prev_button_state = 0xFF;
    uint8_t button_seen_up = 0;
    unsigned long button_timer = 0;


    if (digitalRead(BUTTON_PIN) == HIGH){
      dprintln("PRESSED");
      button_state = 1;
    } else {
      button_state = 0;
    }
    if(button_state == prev_button_state){
      if (now - button_state_start_time > BUTTON_BOUNCE_TIME){
        // button is stable: update
        if (button_state){ // button pressed
          if (button_seen_up){
            // new press - actually do something
            dprintln('button!');
          }
        } else { // but not pressed:
          button_seen_up =  1;
        }
      }

    } else { // button state changed
      prev_button_state = button_state;
      button_state_start_time = now;

    }




  if ( (now - last_ribbon_update) > 30){



  #if defined(USE_8x8)
  matrix.clear();
  matrix.setRotation(1);
  matrix.setCursor(textPos,0);
  matrix.setTextColor(text_color);
  matrix.print(module_names[displaymode]);
  matrix.writeDisplay();

  matrix2.clear();
  matrix2.setRotation(1);
  matrix2.setCursor(textPos-8,0);
  matrix2.setTextColor(text_color);
  matrix2.print(module_names[displaymode]);
  matrix2.writeDisplay();

  matrix3.clear();
  matrix3.setRotation(1);
  matrix3.setCursor(textPos-16,0);
  matrix3.setTextColor(text_color);
  matrix3.print(module_names[displaymode]);
  matrix3.writeDisplay();

  if (textPos-- <= -90 ){textPos = 7;}
  #endif

    last_ribbon_update= millis();
  }
}


int FreeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void CheckMemoryUse(){

  if ((millis() -  MEMTICKDURATION) > mem_tick){
    mem_tick = millis();
    Serial.print("mem ");
    Serial.print(memtickle);
    Serial.print(": ");
    Serial.println(FreeRam());
    memtickle++;
  }

}
