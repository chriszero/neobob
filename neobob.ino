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

#define NOLEDS 30
#define TIMEOUT_MS 1000 

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NOLEDS, 6, NEO_GRB + NEO_KHZ800);
int state, rec, pos;
char buf[3];
const int SHOWDELAY_MIRCO = (NOLEDS * 8 * 20 / 16) + 200; // 20Cycles per bit @ 16MHz & 800kHz
unsigned long timeoutmark = 0;

void setup()
{
  state = 0;
  timeoutmark = millis();
  
  strip.begin();
  // Initialize all pixels to 'off'
  for(int i = 0; i < NOLEDS; i++) strip.setPixelColor(i, 0, 0, 0);
  strip.show();
  delayMicroseconds(SHOWDELAY_MIRCO);

  // Tested and works with 500,000 baud (Arduino ProMini with FTDI & boblight@linux)
  Serial.begin(500000);

  // avoid using the loop function
  for(;;) {
    switch(state) {
    case 0:
      if(Serial.peek() > -1) {
        rec = Serial.read();
        if(rec == 0xAA) state = 1;
      }
      break;
    case 1:
      if(Serial.peek() > -1) {
        rec = Serial.read();
        if(rec == 0x55) {
          state = 2;
          pos = 0;
        }
        else {
          state = 0;
        }
      }
      break;

    case 2:
      if(Serial.available() > 2) {
        Serial.readBytes(buf, 3);
        if(buf[2] == 0xFF) buf[2] = 0xFE; // 0xFF is not allowed for blue !!!

        strip.setPixelColor(pos++, buf[0], buf[1], buf[2]);
      }

      if(pos > NOLEDS) {
        state = 3;
      }
      break;   

    case 3:
      strip.show();
      delayMicroseconds(SHOWDELAY_MIRCO);
      timeoutmark = millis();
      state = 0;
      break;
    }
    
    // If we received no data for more than TIMEOUT_MS, set all pixels to black
    if(millis()  - timeoutmark > TIMEOUT_MS) {
      for(int i = 0; i < NOLEDS; i++) strip.setPixelColor(i, 0, 0, 0);
      // show via 'state 3'
      state = 3;
    }
  }  
}

void loop()
{
}




