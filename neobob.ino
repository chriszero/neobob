#include "Adafruit_NeoPixel.h"

/*
 * neobob
 * Copyright (C) Christian Völlinger  2013 
 * 
 * neobob is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * neobob is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define LED 13
#define LDR A0
#define LDR_MIN 600 // heller = niedriger Wert = AUS
#define LDR_MAX 850 // dunkler = höherer Wert = AN
//#define LDR_TEST 0

#define NOLEDS 30
#define TIMEOUT_MS 1000 

enum myState {
  Start = 0,
  Head_2 = 1,
  Data = 2,
  Ldr_Check = 3,
  Show = 4
};


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NOLEDS, 6, NEO_GRB + NEO_KHZ800);
int rec, pos;
myState state;
boolean show;
char buf[3];
const int SHOWDELAY_MIRCO = (NOLEDS * 8 * 20 / 16) + 200; // 20Cycles per bit @ 16MHz & 800kHz
unsigned long timeoutmark = 0;
unsigned int ldr_value = 0;

void setup()
{
  pinMode(LDR, INPUT);
  pinMode(LED, OUTPUT);
#ifdef LDR_TEST
  ldrTest();
#endif

  state = Start;
  show = true;
  timeoutmark = millis();
  
  strip.begin();
  // Initialize all pixels to 'off'
  setAllBlack();
  strip.show();
  delayMicroseconds(SHOWDELAY_MIRCO);

  // Tested and works with 500,000 baud (Arduino ProMini with FTDI & boblight@linux)
  Serial.begin(500000);

  // avoid using the loop function
  for(;;) {
    switch(state) {
      case Start:
        if(Serial.peek() > -1) {
          rec = Serial.read();
          if(rec == 0xAA) state = Head_2;
        }
        break;
      case Head_2:
        if(Serial.peek() > -1) {
          rec = Serial.read();
          if(rec == 0x55) {
            state = Data;
            pos = 0;
          }
          else {
            state = Start;
          }
        }
        break;
  
      case Data:
        if(Serial.available() > 2) {
          Serial.readBytes(buf, 3);
          if(buf[2] == 0xFF) buf[2] = 0xFE; // 0xFF is not allowed for blue !!!
  
          strip.setPixelColor(pos++, buf[0], buf[1], buf[2]);
        }
  
        if(pos > NOLEDS) {
          state = Ldr_Check;
        }
        break;   
      
      case Ldr_Check:
         ldr_value = analogRead(LDR);
         if(ldr_value < LDR_MIN) { // AUS
           show = false;
           digitalWrite(LED, LOW);
         }
         else if (ldr_value > LDR_MAX) { // AN
           show = true;
           digitalWrite(LED, HIGH);
         }
         state = Show;
        break;
  
      case Show:
        if(show == false) setAllBlack();
        strip.show();
        delayMicroseconds(SHOWDELAY_MIRCO);
        timeoutmark = millis();
        state = Start;
        break;
    }
    
    // If we received no data for more than TIMEOUT_MS, set all pixels to black
    if(millis()  - timeoutmark > TIMEOUT_MS) {
      setAllBlack();
      state = Show;
    }
  }  
}

void setAllBlack() {
 for(int i = 0; i < NOLEDS; i++) strip.setPixelColor(i, 0, 0, 0); 
}

#ifdef LDR_TEST
void ldrTest() {
  Serial.begin(9600);
  
  for(;;) {
   ldr_value = analogRead(LDR);
   Serial.print(ldr_value);
   if(ldr_value < LDR_MAX) {
     Serial.println(" OFF");
     digitalWrite(LED, LOW);
   }
   else if (ldr_value > LDR_MIN) {
     Serial.println(" ON");
     digitalWrite(LED, HIGH);
   }
   else {
     Serial.println();
   }
   delay(100); 
  }
}
#endif

void loop()
{
}




