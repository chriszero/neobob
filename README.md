neobob
=========================
Arduino code to control a neopixel or FastLED based RGB strip with boblight.
Configure it via the USEFASTLED #define

for FastLED see https://github.com/FastLED/FastLED

It's important to use following option in your boblight.conf 
```
[device]
allowsync off
```

Inculdes a modified Adafruit_NeoPixel for use with TM1829 stripes, remove it if you use the original stripes.
