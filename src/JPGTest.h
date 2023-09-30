#pragma once

#include <Arduino.h>

class TFT_eSPI;

class JPGTest {
public:
  // decode and optionally draw a JPEG file to the TFT screen
  virtual bool decodeJPG(uint8_t *data, size_t length, TFT_eSPI *tft) = 0;
  virtual const char *description()  = 0;
};