
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



    if (digitalRead(BUTTON_PIN) == HIGH){
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

            displaymode++;
            if (displaymode>5){displaymode = 0;}
              dprintln(module_names[displaymode]);

            button_seen_up  = 0;
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
