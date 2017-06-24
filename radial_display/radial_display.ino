#include "SPI.h"
#include "Adafruit_WS2801.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

#define USE_AUDIO false
#define USE_8x8 true
#define PRDBG



// macros:
// create Serial.print macros:
#ifdef PRDBG
#define dprint(ccc) Serial.print(ccc);
#define dprintln(ccc) Serial.println(ccc);
#else
#define dprint(ccc) ""
#define dprintln(ccc) ""
#endif

// define and declare stuff for the LED adressable pixels:
// Yellow wire on Adafruit Pixels:
#define SPI_DATA_PIN 11
// Green wire on Adafruit Pixels:
#define SPI_CLOCK_PIN 12


// input button (digital input)
#define BUTTON_PIN 2
uint8_t button_state = 0;
uint8_t prev_button_state = 0xFF;
uint8_t button_seen_up = 0;
unsigned long button_state_start_time = 0;
#define BUTTON_BOUNCE_TIME 80


// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(50, SPI_DATA_PIN, SPI_CLOCK_PIN);
#define MAX_DISPLAY_ANGLE 9
#define MAX_DISPLAY_RADIUS 5

uint8_t polar[MAX_DISPLAY_ANGLE+1][MAX_DISPLAY_RADIUS+1]; // global array of coordinate->pixel conversion

int displaymode;

#ifdef USE_8x8
// Define and declare stuff for 8x8 matrix display:
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix2 = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix3 = Adafruit_BicolorMatrix();
#endif

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

// analog input for microphone:
#define AUDIO_IN_PIN 2

#define MEMTICKDURATION 6000


// Declarations for our interupt processing and background display ticklers:

static unsigned long last_ribbon_update = 0;



char* module_names[]={"Ring Meter", "VU Meter", "Zoom",
"Spinner", "Infection","Light Panel"};

unsigned long mem_tick;
unsigned int memtickle = 0;

void setup() {
	InitPolarCoords();
  randomSeed(analogRead(0)); // seed stream with noise from unconnected pin
  Serial.begin(9600);
  mem_tick = millis();
  pinMode(BUTTON_PIN, INPUT);

  // analogReference(EXTERNAL); // 3.3V to AREF
  analogReference(DEFAULT);
  adc_save = ADCSRA;         // Save ADC setting for restore later
  pinMode (AUDIO_IN_PIN, INPUT); // Without this analogRead always returns 0
  analogRead (AUDIO_IN_PIN) ; // do a dummy read to get the pin in the right state?

  // init rgb addressible strip & polar coordinate array:
  strip.begin();
  displaymode = 5;

  // Update LED contents, to start they are all 'off'
  strip.show();

  colorWipe(Color(0, 0, 0), 0);

  #if defined(USE_8x8)
  // init for 8x8 display:
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
  #endif


  for(int i=0;i<AUDIOBUFFERLEN; i++){
    audio_history[i] = 0;
  }
  dprintln("Setup Complete.");
}


void loop() {


//  CheckMemoryUse();
  int i,j;
  int lastmode=displaymode;
  uint32_t clr;

	// play animations based on display mode:
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

//  ServiceBackground();
/*
        Serial.print("old mode: ");
        Serial.print(lastmode);
        Serial.print(", new mode: ");
        Serial.print(displaymode);
	Serial.println(" interrupted!");
*/

//	lastmode=displaymode;

	// rainbowCycle(10);
//	if (random(5)==0){
  	  // displaymode=random(4);
//  displaymode++;
//  if (displaymode>5){displaymode=0;}
//	}

}
