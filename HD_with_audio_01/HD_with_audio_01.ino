#include "SPI.h"
#include "Adafruit_WS2801.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"


// define and declare stuff for the LED adressable pixels:
int dataPin  = 11;    // Yellow wire on Adafruit Pixels
int clockPin = 12;    // Green wire on Adafruit Pixels
// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(50, dataPin, clockPin);
#define MAX_DISPLAY_ANGLE 9
#define MAX_DISPLAY_RADIUS 5
 
uint8_t polar[MAX_DISPLAY_ANGLE+1][MAX_DISPLAY_RADIUS+1]; // global array of coordinate->pixel conversion

int displaymode;

// Define and declare stuff for 8x8 grid display:
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix2 = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix3 = Adafruit_BicolorMatrix();


int textPos = 7;


// Declarations for the audio sampling
#define AUDIOBUFFERLEN 128
unsigned int audio_history[AUDIOBUFFERLEN]; // 16 bit samples
unsigned int audio_history_pointer = 0;
uint8_t  adc_save;                          // Default ADC mode
unsigned long last_sample_time = 0;
unsigned long sample_window_start = 0;
unsigned long sample_window_max = 0;
unsigned long sample_window_min = 1024;
unsigned long avg_audio_sample = 0;
#define CONVERT33TO5V 5/3.3;

#define AUDIO_IN_PIN 2

// Declarations for our interupt processing and background display ticklers:
static unsigned long last_interrupt_time = 0;
static unsigned long last_ribbon_update = 0;
int last_interrupt_state_0 = 0;
int seen_up_0 =0;


char* module_names[]={"Ring Meter", "VU Meter", "Zoom",
"Spinner", "Infection","Light Panel"};

void setup() {
	InitPolarCoords();
        randomSeed(analogRead(0)); // seed stream with noise from unconnected pin
	Serial.begin(9600);
      pinMode(2, INPUT);
      digitalWrite(2, HIGH);
      attachInterrupt(0, HandleInt0, CHANGE);
      
     // analogReference(EXTERNAL); // 3.3V to AREF
     analogReference(DEFAULT);
      adc_save = ADCSRA;         // Save ADC setting for restore later
       pinMode (AUDIO_IN_PIN, INPUT); // Without this analogRead always returns 0
    analogRead (AUDIO_IN_PIN) ; // do a dummy read to get the pin in the right state?

  // init rgb addressible strip & polar coordinate array:
	strip.begin();
	displaymode = 1;
	
	// Update LED contents, to start they are all 'off'
	strip.show();
	
	
	
	colorWipe(Color(0, 0, 0), 0);
  
  // init "snake" for 8x8 display:
    matrix.begin(0x70);  // I2C address of display module
    matrix2.begin(0x71);
    matrix3.begin(0x72);

  
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextSize(1);
  matrix.setTextColor(LED_GREEN);
  matrix.setRotation(3);
  matrix.clear();
  
  matrix2.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix2.setTextSize(1);
  matrix2.setTextColor(LED_GREEN);
  matrix2.setRotation(3);
  matrix2.clear();
  
  matrix3.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix3.setTextSize(1);
  matrix3.setTextColor(LED_GREEN);
  matrix3.setRotation(3);
  matrix3.clear();
  
  
  
  for(int i=0;i<AUDIOBUFFERLEN; i++){
     audio_history[i] = 0;
   }
}


void loop() {
    int i,j;
    int lastmode=displaymode;
	uint32_t clr;
	
	// going to use a tradition event loop:
	switch(displaymode){
		case 0:
		  AnimatedRings();
		break;
		
		case 1:
		  VUMeter();
		break;
		
		case 2:
		  TestRings();
		break;

		case 3:
			Spinner();
		break;
                
                case 4:
                  LavaTest();
                break;
                
                case 5:
                Panel();
                break;
	}
/*
        Serial.print("old mode: ");
        Serial.print(lastmode);
        Serial.print(", new mode: ");
        Serial.print(displaymode);
	Serial.println(" interrupted!");
*/
	lastmode=displaymode;
	// rainbowCycle(10);
//	if (random(5)==0){
  	  // displaymode=random(4);
//  displaymode++;
//  if (displaymode>5){displaymode=0;}
//	}
}

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


// Interrupt Service Routine attached to INT0 vector
void HandleInt0(){
  // displaymode is global;
   //  digitalWrite(13, !digitalRead(13));    // Toggle LED on pin 13
   int current_mode;
   unsigned long now = millis();
   
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
   current_mode = digitalRead(2);
  if (now - last_interrupt_time > 80){ // aafter debuggin pulled, drop this to 40 miliseconds
      if (current_mode == HIGH){
       if (seen_up_0 == true) {
         
         displaymode++;
         textPos = 7;
         if (displaymode>5){displaymode=0;}
        }
        seen_up_0=false;
       } else {
         seen_up_0=true;
       }
  } 
   last_interrupt_state_0 = current_mode;
   last_interrupt_time = now;

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
  
    
  if ( (now - last_ribbon_update) > 30){
    
    if (digitalRead(2) == HIGH){
      text_color = LED_RED;
    }
    
    
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
    
    
    last_ribbon_update= millis();
  }
}
    
    

