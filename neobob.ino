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

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF
#define BLACK  0x000000

#define PIN 13 // Datapin
#define NOLEDS 192
#define TIMEOUT_MS 1000 

enum myState {
  Start = 0,
  Head_2 = 1,
  Data = 2,
  Show = 3
};


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NOLEDS, PIN, NEO_GRB + NEO_KHZ800);
int rec, pos;
myState state;
boolean show;
char buf[6];
const int SHOWDELAY_MIRCO = (NOLEDS * 8 * 20 / 16) + 200; // 20Cycles per bit @ 16MHz & 800kHz
const char head[] = {0x41, 0x64, 0x61, 0x00, 0xC7, 0x92};
unsigned long timeoutmark = 0;

void setup()
{
  delay(1000);
  state = Start;
  show = true;
  timeoutmark = millis();
  
  strip.begin();
  // Initialize all pixels to 'off'
  colorWipe(PINK, 10);
  colorWipe(BLACK, 0);
  strip.show();
  delayMicroseconds(SHOWDELAY_MIRCO);

  // Tested and works with 500,000 baud (Arduino ProMini with FTDI & boblight@linux)
  Serial.begin(500000);

  // avoid using the loop function
  for(;;) {
    switch(state) {
      case Start:
        if(Serial.available() > 0) {
          rec = Serial.read();
          if(rec == head[0]) state = Head_2;
        }
        break;
        
      case Head_2:
        if(Serial.available() > sizeof(head) - 2) {
          Serial.readBytes(buf, sizeof(head) - 1);
          for(int p = 0; p < sizeof(head) - 1; p++) {
            if(buf[p] == head[p+1]) {
             state = Data;
             pos = 0;
            }
            else {
             state = Start;
             break; 
            }
          }
        }
        break;
  
      case Data:
        if(Serial.available() > 2) {
          Serial.readBytes(buf, 3);  
          strip.setPixelColor(pos++, buf[0], buf[1], buf[2]);
        }
  
        if(pos > NOLEDS) {
          state = Show;
        }
        break;   
      
      case Show:
        strip.show();
        delayMicroseconds(SHOWDELAY_MIRCO);
        timeoutmark = millis();
        state = Start;
        break;
    }
    
    // If we received no data for more than TIMEOUT_MS, set all pixels to black
    if(millis()  - timeoutmark > TIMEOUT_MS) {
      colorWipe(BLACK, 0);
      state = Show;
    }
  }  
}

void colorWipe(uint32_t color, int wait)
{
  for (int i=0; i < NOLEDS; i++) {
    strip.setPixelColor(i, color);
    if(wait > 0) {
      strip.show();
      delay(wait);
    }
  }
}

void loop()
{
}

