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

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NOLEDS, 6, NEO_GRB + NEO_KHZ800);
int out_r, out_g, out_b, state, rec, pos;
const int SHOWDELAY_MIRCO = (NOLEDS * 8 * 20 / 16) + 500; // 20Cycles per bit @ 16MHz & 800kHz

void setup()
{
  state = 0;

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

    case 2: // read RED
      if(Serial.peek() > -1) {
        out_r = Serial.read();
        if(out_r == 0xFF) out_r = 0xFE;
        state = 3;
      }
      break;

    case 3: // read GREEN
      if(Serial.peek() > -1) {
        out_g = Serial.read();
        if(out_g == 0xFF) out_g = 0xFE;
        state = 4;      
      }
      break;

    case 4: // read BLUE and set Pixel
      if(Serial.peek() > -1) {
        out_b = Serial.read();
        if(out_b == 0xFF) out_b = 0xFE;

        strip.setPixelColor(pos++, out_r, out_g, out_b);

        if(pos < NOLEDS) {
          state = 2;
        }
        else {
          state = 5; 
        }
      }
      break;

    case 5:
      strip.show();
      delayMicroseconds(SHOWDELAY_MIRCO);
      state = 0;
      break;
    }
  }  
}

void loop()
{
}



