#pragma once

#include "JPGTest.h"

class JPEGDECTest : public JPGTest {
  public:
    bool decodeJPG(uint8_t *data, size_t length, TFT_eSPI *tft);
    const char *description() { return "bitbank2/JPEGDEC"; };
};