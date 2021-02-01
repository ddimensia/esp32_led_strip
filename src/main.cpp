#include <Arduino.h>
#include <FastLED.h>
#include <CAN.h>

#define LED_COUNT 40
#define MID_POINT 20
#define BLEND_WIDTH 10
#define HALF_BLEND_WIDTH 5
#define LED_PIN 2

CRGB leds[LED_COUNT];
float percent = 0.0;
float direction = 0.1;
fract8 blendValues[BLEND_WIDTH];

void onReceive(int packetSize);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(LED_PIN, OUTPUT);
  if(!CAN.begin(500E3)) {
    Serial.println("Failed to start CAN!");
  } else {
    CAN.onReceive(onReceive);
  }

  FastLED.addLeds<NEOPIXEL, 12>(leds, LED_COUNT);
  for(u_int i = 0; i < BLEND_WIDTH; i++) {
    blendValues[i] = (fract8)(((float)i / BLEND_WIDTH) * 256);
  }
  Serial.println("Finished setup!");
}

void onReceive(int packetSize) {
  Serial.print("Received");
  long packetId = CAN.packetId();
  if (packetId = 1234) {
    int length = CAN.packetDlc();
    Serial.print(" size ");
    Serial.print(length);
    if (length != 2) {
      Serial.println(" unexpected!");
    } else {
      uint8_t buf[2];
      CAN.readBytes(buf, 2);
      uint16_t val = buf[0] << 8 | buf[1];
      Serial.print(" val ");
      Serial.print(val);
      percent = (val / 32678.0) - 1.0;
      Serial.print(" percent ");
      Serial.print(percent);
    }
  } else {
    Serial.print(" unknown id ");
    Serial.print(packetId);
  }
  
  Serial.println("");
}

void loop() {
  u_int value_pos = percent * (MID_POINT + HALF_BLEND_WIDTH) + MID_POINT;

  /*digitalWrite(LED_PIN, offset % 2);*/
  for(int i = 0; i < LED_COUNT; i++) {
    if (i >= value_pos + HALF_BLEND_WIDTH) {
      leds[i] = CRGB::Red;
    } else if (i <= value_pos - HALF_BLEND_WIDTH) {
      leds[i] = CRGB::Green;
    } else {
      leds[i] = blend(CRGB::Green, CRGB::Red, blendValues[i - value_pos]);
    }
  }

  FastLED.show();

  delay(20);
}