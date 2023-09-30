#pragma once

#include "JPGTest.h"

class ESP32JPEGTest : public JPGTest {
  public:
    bool decodeJPG(uint8_t *data, size_t length, TFT_eSPI *tft);
    bool decodeJPG(uint8_t *data, size_t length, uint16_t *buffer);
    const char *description() { return "ESP32_JPEG"; };
};