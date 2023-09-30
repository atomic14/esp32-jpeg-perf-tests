#include <TFT_eSPI.h>
#include <JPEGDEC.h>
#include "JPEGDECTest.h"

JPEGDEC jpeg;

static uint16_t *dmaBuffer[2] = {NULL, NULL};
static int dmaBufferIndex = 0;

static int maxWidth = 0;
static int maxHeight = 0;

int drawMCUs(JPEGDRAW *pDraw)
{
  TFT_eSPI *tft = (TFT_eSPI *)pDraw->pUser;
  if (tft) {
    int iCount;
    iCount = pDraw->iWidth * pDraw->iHeight; // number of pixels to draw in this call
    if (dmaBuffer[dmaBufferIndex] == NULL)
    {
      dmaBuffer[dmaBufferIndex] = (uint16_t *)malloc(iCount * 2);
    }
    maxWidth = max(maxWidth, pDraw->iWidth);
    maxHeight = max(maxHeight, pDraw->iHeight);
    memcpy(dmaBuffer[dmaBufferIndex], pDraw->pPixels, iCount * 2);
    tft->dmaWait();
    tft->setAddrWindow(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
    tft->pushPixelsDMA(dmaBuffer[dmaBufferIndex], iCount);
    dmaBufferIndex = (dmaBufferIndex + 1) % 2;
  }
  return 1;
}

bool JPEGDECTest::decodeJPG(uint8_t *data, size_t length, TFT_eSPI *tft) {
  bool success = false;
  if (jpeg.openRAM(data, length, drawMCUs))
  {
    jpeg.setPixelType(RGB565_BIG_ENDIAN);
    jpeg.setUserPointer(tft);
    if (jpeg.decode(0, 0, 0))
    {
      success = true;
    }
    jpeg.close();
  }
  // Serial.printf("JPEGDecode: maxWidth=%d, maxHeight=%d\n", maxWidth, maxHeight);
  return success;
}