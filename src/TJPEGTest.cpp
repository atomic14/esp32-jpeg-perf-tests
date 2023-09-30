#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "TJPEGTest.h"

// library design needs global variables...
static TFT_eSPI *_tft = NULL;

static uint16_t *dmaBuffer[2] = {NULL, NULL};
static int dmaBufferIndex = 0;

uint16_t maxWidth;
uint16_t maxHeight;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  maxWidth = max(maxWidth, w);
  maxHeight = max(maxHeight, h);
  // copy the data to the dma buffer
  if (dmaBuffer[dmaBufferIndex] == NULL)
  {
    dmaBuffer[dmaBufferIndex] = (uint16_t *)malloc(w * h * 2);
  }
  memcpy(dmaBuffer[dmaBufferIndex], bitmap, w * h * 2);
  dmaBufferIndex = (dmaBufferIndex + 1) % 2;
  if (_tft) {
    _tft->dmaWait();
    _tft->setAddrWindow(x, y, w, h);
    _tft->pushPixelsDMA(dmaBuffer[dmaBufferIndex], w * h);
  }
  return 1;
}

TJPEGTest::TJPEGTest() {
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
  TJpgDec.setSwapBytes(true);
}


bool TJPEGTest::decodeJPG(uint8_t *data, size_t length, TFT_eSPI *tft) {
  _tft=tft;
  JRESULT res = TJpgDec.drawJpg(0, 0, data, length);
  // Serial.printf("TJpgDec: maxWidth=%d, maxHeight=%d\n", maxWidth, maxHeight);
  return res == JDR_OK;
}
