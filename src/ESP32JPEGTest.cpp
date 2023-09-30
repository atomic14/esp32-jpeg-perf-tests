#include <TFT_eSPI.h>
#include <ESP32_JPEG_Library.h>
#include "ESP32JPEGTest.h"

void displayError(jpeg_error_t err)
{
  //   JPEG_ERR_OK = 0,            /* Succeeded */
  // JPEG_ERR_FAIL = -1,         /* Device error or wrong termination of input stream */
  // JPEG_ERR_MEM = -2,          /* Insufficient memory pool for the image */
  // JPEG_ERR_NO_MORE_DATA = -3, /* Input data is not enough */
  // JPEG_ERR_PAR = -4,          /* Parameter error */
  // JPEG_ERR_FMT1 = -5,         /* Data format error (may be damaged data) */
  // JPEG_ERR_FMT2 = -6,         /* Right format but not supported */
  // JPEG_ERR_FMT3 = -7          /* Not supported JPEG standard */
  switch (err)
  {
  case JPEG_ERR_OK:
    Serial.println("JPEG_ERR_OK");
    break;
  case JPEG_ERR_FAIL:
    Serial.println("JPEG_ERR_FAIL");
    break;
  case JPEG_ERR_MEM:
    Serial.println("JPEG_ERR_MEM");
    break;
  case JPEG_ERR_NO_MORE_DATA:
    Serial.println("JPEG_ERR_NO_MORE_DATA");
    break;
  case JPEG_ERR_PAR:
    Serial.println("JPEG_ERR_PAR");
    break;
  case JPEG_ERR_FMT1:
    Serial.println("JPEG_ERR_FMT1");
    break;
  case JPEG_ERR_FMT2:
    Serial.println("JPEG_ERR_FMT2");
    break;
  case JPEG_ERR_FMT3:
    Serial.println("JPEG_ERR_FMT3");
    break;
  default:
    Serial.println("Unknown error");
    break;
  }
}

uint8_t *outputBuffer = NULL;
// source code from https://github.com/esp-arduino-libs/ESP32_JPEG/blob/master/examples/DecodeTest/DecodeTest.ino
bool ESP32JPEGTest::decodeJPG(uint8_t *data, size_t length, TFT_eSPI *tft)
{
  bool success = false;
  jpeg_error_t ret = JPEG_ERR_OK;
  int inbuf_consumed = 0;

  // Generate default configuration
  jpeg_dec_config_t config = {
      .output_type = JPEG_RAW_TYPE_RGB565_BE,
      .rotate = JPEG_ROTATE_0D,
  };

  // Empty handle to jpeg_decoder
  jpeg_dec_handle_t *jpeg_dec = NULL;

  // Create jpeg_dec
  jpeg_dec = jpeg_dec_open(&config);

  // Create io_callback handle
  jpeg_dec_io_t *jpeg_io = (jpeg_dec_io_t *)calloc(1, sizeof(jpeg_dec_io_t));
  if (jpeg_io == NULL)
  {
    return JPEG_ERR_MEM;
  }

  // Create out_info handle
  jpeg_dec_header_info_t *out_info = (jpeg_dec_header_info_t *)calloc(1, sizeof(jpeg_dec_header_info_t));
  if (out_info == NULL)
  {
    return JPEG_ERR_MEM;
  }
  // Set input buffer and buffer len to io_callback
  jpeg_io->inbuf = data;
  jpeg_io->inbuf_len = length;

  // Parse jpeg picture header and get picture for user and decoder
  ret = jpeg_dec_parse_header(jpeg_dec, jpeg_io, out_info);
  if (ret < 0)
  {
    Serial.println("JPEG decode parse failed");
    displayError(ret);
    goto _exit;
  }

  if (outputBuffer == NULL)
  {
    // outputBuffer = (uint8_t *)ps_malloc(out_info->width * out_info->height * 2);
    outputBuffer = (uint8_t *)malloc(out_info->width * out_info->height * 2);

    if (outputBuffer == NULL)
    {
      Serial.println("JPEG output buffer malloc failed");
      ret = JPEG_ERR_MEM;
      goto _exit;
    }
  }

  jpeg_io->outbuf = outputBuffer;
  inbuf_consumed = jpeg_io->inbuf_len - jpeg_io->inbuf_remain;
  jpeg_io->inbuf = data + inbuf_consumed;
  jpeg_io->inbuf_len = jpeg_io->inbuf_remain;

  // Start decode jpeg raw data
  ret = jpeg_dec_process(jpeg_dec, jpeg_io);
  if (ret < 0)
  {
    Serial.println("JPEG decode process failed");
    goto _exit;
  }

_exit:
  // free(data);
  if (ret != JPEG_ERR_OK)
  {
    displayError(ret);
  }
  else
  {
    if (tft)
    {
      // now render the image
      tft->dmaWait();
      tft->setAddrWindow(0, 0, out_info->width, out_info->height);
      tft->pushPixelsDMA((uint16_t *)outputBuffer, out_info->width * out_info->height);
    }
  }
  // Decoder deinitialize
  jpeg_dec_close(jpeg_dec);
  free(out_info);
  free(jpeg_io);
  return ret == JPEG_ERR_OK;
}

bool ESP32JPEGTest::decodeJPG(uint8_t *data, size_t length, uint16_t *buffer)
{
  bool success = false;
  jpeg_error_t ret = JPEG_ERR_OK;
  int inbuf_consumed = 0;

  // Generate default configuration
  jpeg_dec_config_t config = {
      .output_type = JPEG_RAW_TYPE_RGB565_BE,
      .rotate = JPEG_ROTATE_0D,
  };

  // Empty handle to jpeg_decoder
  jpeg_dec_handle_t *jpeg_dec = NULL;

  // Create jpeg_dec
  jpeg_dec = jpeg_dec_open(&config);

  // Create io_callback handle
  jpeg_dec_io_t *jpeg_io = (jpeg_dec_io_t *)calloc(1, sizeof(jpeg_dec_io_t));
  if (jpeg_io == NULL)
  {
    return JPEG_ERR_MEM;
  }

  // Create out_info handle
  jpeg_dec_header_info_t *out_info = (jpeg_dec_header_info_t *)calloc(1, sizeof(jpeg_dec_header_info_t));
  if (out_info == NULL)
  {
    return JPEG_ERR_MEM;
  }
  // Set input buffer and buffer len to io_callback
  jpeg_io->inbuf = data;
  jpeg_io->inbuf_len = length;

  // Parse jpeg picture header and get picture for user and decoder
  ret = jpeg_dec_parse_header(jpeg_dec, jpeg_io, out_info);
  if (ret < 0)
  {
    Serial.println("JPEG decode parse failed");
    displayError(ret);
    goto _exit;
  }

  jpeg_io->outbuf = (uint8_t *) buffer;
  inbuf_consumed = jpeg_io->inbuf_len - jpeg_io->inbuf_remain;
  jpeg_io->inbuf = data + inbuf_consumed;
  jpeg_io->inbuf_len = jpeg_io->inbuf_remain;

  // Start decode jpeg raw data
  ret = jpeg_dec_process(jpeg_dec, jpeg_io);
  if (ret < 0)
  {
    Serial.println("JPEG decode process failed");
    goto _exit;
  }

_exit:
  // free(data);
  if (ret != JPEG_ERR_OK)
  {
    displayError(ret);
  }
  // Decoder deinitialize
  jpeg_dec_close(jpeg_dec);
  free(out_info);
  free(jpeg_io);
  return ret == JPEG_ERR_OK;
}