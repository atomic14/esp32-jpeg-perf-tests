#pragma once

#include "JPGTest.h"

class TJPEGTest : public JPGTest {
  public:
    TJPEGTest();
    bool decodeJPG(uint8_t *data, size_t length, TFT_eSPI *tft);
    bool decodeJPG(uint8_t *data, size_t length, uint16_t *buffer) { return false;};
    const char *description() { return "Bodmer/TJpg_Decoder"; };
};