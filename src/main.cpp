
#include "FastLED.h"
#include "pitches.h"
#define NUM_LEDS 1

// Data pin that led data will be written out over
#define DATA_PIN 6
#define BUZZER_PIN 2
// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8
int melody[] = {
  REST, REST, REST, NOTE_DS4, 
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_E4, NOTE_G4, NOTE_B4,   
  NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, 
  NOTE_E4, REST, REST, NOTE_DS4,
  
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_G4, NOTE_B4, NOTE_E5,
  NOTE_DS5,   
  NOTE_D5, REST, REST, NOTE_DS4, 
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_E4, NOTE_G4, NOTE_B4,   
  
  NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, 
  NOTE_E4, REST,
  REST, NOTE_E5, NOTE_D5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4,
  NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4,   
  NOTE_G4, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4
};

int durations[] = {
  2, 4, 8, 8, 
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,   
  2, 16, 16, 16, 16, 
  2, 4, 8, 4,
  
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,
  1,   
  2, 4, 8, 8, 
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,   
  
  2, 16, 16, 16, 16, 
  4, 4,
  4, 8, 8, 8, 8, 8, 8,
  16, 8, 16, 8, 16, 8, 16, 8,   
  16, 16, 16, 16, 16, 2
};
// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

void setup() {
    Serial.begin(921600);
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(2000);
    FastLED.addLeds<SM16703, DATA_PIN, BRG>(leds, NUM_LEDS);
}

// your leds.
void loop() {
   int size = sizeof(durations) / sizeof(int); 
	 for (int note = 0; note < size; note++) { 
	   //to calculate the note duration, take one second divided by the note type. 
	   //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc. 
	   int duration = 1000 / durations[note]; 
     leds[0] = CRGB::Red;
      FastLED.show();
	   tone(BUZZER_PIN, melody[note], duration); 
     leds[0] = CRGB::Blue;
      FastLED.show();
	   //to distinguish the notes, set a minimum time between them. 
	   //the note's duration + 30% seems to work well: 
	   int pauseBetweenNotes = duration * 1.30; 
	   delay(pauseBetweenNotes); 
	   //stop the tone playing: 
	   noTone(BUZZER_PIN); 
	 } 
}


