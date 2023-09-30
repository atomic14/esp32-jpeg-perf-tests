#include <TFT_eSPI.h>
#include <JPEGDecoder.h>
#include "JPEGDecoderTest.h"

#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

static uint16_t *dmaBuffer[2] = {NULL, NULL};
static int dmaBufferIndex = 0;


bool JPEGDecoderTest::decodeJPG(uint8_t *data, size_t length, TFT_eSPI *tft) {
  uint16_t maxWidth;
  uint16_t maxHeight;
  int xpos = 0;
  int ypos = 0;
  int result = JpegDec.decodeArray(data, length);
  if (result == 1) {
    // retrieve infomration about the image
    uint16_t *pImg;
    uint16_t mcu_w = JpegDec.MCUWidth;
    uint16_t mcu_h = JpegDec.MCUHeight;
    uint32_t max_x = JpegDec.width;
    uint32_t max_y = JpegDec.height;

    maxWidth = max(maxWidth, mcu_w);
    maxHeight = max(maxHeight, mcu_h);

    // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
    // Typically these MCUs are 16x16 pixel blocks
    // Determine the width and height of the right and bottom edge image blocks
    uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
    uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

    // save the current image block size
    uint32_t win_w = mcu_w;
    uint32_t win_h = mcu_h;

    // save the coordinate of the right and bottom edges to assist image cropping
    // to the screen size
    max_x += xpos;
    max_y += ypos;

    // read each MCU block until there are no more
    while (JpegDec.readSwappedBytes()) {
	  
      // save a pointer to the image block
      pImg = JpegDec.pImage ;

      // calculate where the image block should be drawn on the screen
      int mcu_x = JpegDec.MCUx * mcu_w + xpos;  // Calculate coordinates of top left corner of current MCU
      int mcu_y = JpegDec.MCUy * mcu_h + ypos;

      // check if the image block size needs to be changed for the right edge
      if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
      else win_w = min_w;

      // check if the image block size needs to be changed for the bottom edge
      if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
      else win_h = min_h;

      // copy pixels into a contiguous block
      if (win_w != mcu_w)
      {
        uint16_t *cImg;
        int p = 0;
        cImg = pImg + win_w;
        for (int h = 1; h < win_h; h++)
        {
          p += mcu_w;
          for (int w = 0; w < win_w; w++)
          {
            *cImg = *(pImg + w + p);
            cImg++;
          }
        }
      }

      // draw image MCU block only if it will fit on the screen
      if (tft) {
        if (dmaBuffer[dmaBufferIndex] == NULL)
        {
          dmaBuffer[dmaBufferIndex] = (uint16_t *)malloc(win_w * win_h * 2);
        }
        memcpy(dmaBuffer[dmaBufferIndex], pImg, win_w * win_h * 2);
        if (( mcu_x + win_w ) <= tft->width() && ( mcu_y + win_h ) <= tft->height())
        {
          tft->dmaWait();
          tft->setAddrWindow(
            mcu_x, mcu_y, 
            win_w, win_h
          );
          tft->pushPixelsDMA(
            dmaBuffer[dmaBufferIndex],
            win_w * win_h
          );
        }
        else if ( (mcu_y + win_h) >= tft->height()) JpegDec.abort(); // Image has run off bottom of screen so abort decoding
      }
    }
    // Serial.printf("JPEGDecode: maxWidth=%d, maxHeight=%d\n", maxWidth, maxHeight);
    return true;
  }
  return false;
}