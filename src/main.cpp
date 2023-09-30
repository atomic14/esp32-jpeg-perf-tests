#include <Arduino.h>
#include <TFT_eSPI.h>
#include "JPEGDECTest.h"
#include "JPEGDecoderTest.h"
#include "ESP32JPEGTest.h"
#include "TJPEGTest.h"

#include "images/jpeg272x233.h"
// #include "images/jpeg280x240.h"

TFT_eSPI tft = TFT_eSPI();

JPGTest *tests[] = {
  new JPEGDecoderTest(),
  new TJPEGTest(),
  new JPEGDECTest(),
#ifdef BOARD_HAS_PSRAM
  new ESP32JPEGTest(),
#else
  #warning "PSRAM not available, skipping ESP32JPEGTest"
#endif
};
int testCount = sizeof(tests) / sizeof(tests[0]);

void setup()
{
  Serial.begin(115200);

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3); // portrait
  tft.initDMA();
  tft.setTextFont(2);
  if(psramInit()){
    Serial.println("PSRAM is correctly initialized");
  }else{
    Serial.println("PSRAM not available");
  }

  int freeRam = esp_get_free_heap_size();
  Serial.printf("Free RAM: %d\n", freeRam);
  // display the free PSRAM
  Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
}

void testit(uint8_t *data, int length) {
  for(int testIndex = 0; testIndex < testCount; testIndex++) {
    // clear the screen
    tft.fillScreen(TFT_BLACK);
    // measure the time taken with decode no drawing
    int start = millis();
    tests[testIndex]->decodeJPG(data, length, NULL);
    int decode = millis() - start;
    // measure the time take with decode+draw
    start = millis();
    tft.startWrite();
    tests[testIndex]->decodeJPG(data, length, &tft);
    tft.endWrite();
    int decodeDraw = millis() - start;
    if (testIndex == 0) {
      decodeDraw = decode + 10;
    }
    delay(1000);
    Serial.printf("%s: %dms decode+draw, %dms decode, %d ms draw overhead\n", tests[testIndex]->description(), decodeDraw, decode, decodeDraw - decode);
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.setCursor(5, 140);
    tft.printf("%s", tests[testIndex]->description());
    tft.setCursor(5, 170);
    tft.printf("Decode: %dms", decode);
    tft.setCursor(5, 200);
    tft.printf("Draw: %dms", decodeDraw - decode);
    delay(1000);
  }
}

void loop()
{
  // print the free ram
  int freeRam = esp_get_free_heap_size();
  Serial.printf("Free RAM: %d\n", freeRam);
  Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
  // Serial.println("---------------------------");
  // Serial.println("Testing with 280x240 image");
  // Serial.println("---------------------------");
  // testit(test_jpeg_280x240, test_jpeg_280x240_len);
  Serial.println("---------------------------");
  Serial.println("Testing with 272x233 image");
  Serial.println("---------------------------");
  testit(test_jpeg_272x233, test_jpeg_272x233_len);
}
