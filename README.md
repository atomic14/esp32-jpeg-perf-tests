[![Build Firmware](https://github.com/atomic14/esp32-jpeg-perf-tests/actions/workflows/build_firmware.yml/badge.svg)](https://github.com/atomic14/esp32-jpeg-perf-tests/actions/workflows/build_firmware.yml)

# ESP32 JPEG Performance Test
[![Demo Video](https://img.youtube.com/vi/OyoN1ypBSrQ/0.jpg)](https://www.youtube.com/watch?v=OyoN1ypBSrQ)

You can see a video explanation here [here](https://www.youtube.com/watch?v=OyoN1ypBSrQ).

I received an issue on my esp32-tv project: [Decoding JPEG with SIMD](https://github.com/atomic14/esp32-tv/issues/3) along with a link to some [demo code](https://github.com/esp-arduino-libs/ESP32_JPEG/blob/master/examples/DecodeTest/DecodeTest.ino).

So I thought I should do a bit of a comparison.

Turns out the suggested library is pretty fast - it even beats the amazing [JPEGDEC Library]() which is already unbelievably fast.

Here's the results running on an ESP32-S3 with PSRAM.

| Library                | Decode (ms) | Decode+Draw (ms) | Draw Overhead (ms) |
|------------------------|-------------|------------------|---------------|
| Bodmer/JPEGDecoder     | 108         | 118              | 10            |
| Bodmer/TJpg_Decoder    | 55          | 64               | 9             |
| ~~bitbank2/JPEGDEC~~       | ~~32~~          | ~~38~~               | ~~6~~             |
| bitbank2/JPEGDEC (New!)       | 23          | 29               | 6             |
| ESP32_JPEG             | 20          | 37               | 17            |

As you can see it is very fast - 20ms to decode the image.

NOTE - there's a new improved version of the bitbank2/JPEGDEC library which takes advantage of SIMD instructions on the ESP32S3. This now decodes JPEGS in just 23ms!

You might be wondering why the total draw+decode time does not look particularly good. There's a good explanation in the video and it comes down to using DMA to draw the image. With the other options we can overlap sending blocks of pixels to the screen with decoding the next block of pixels. With this library we have to wait for the entire image to be decoded before we can start sending it to the screen.

With the esp32-tv project this may not matter that much - we can display the current frame of the video while the next frame is being decoded. For displaying a single image it's still not terrible. We can do the decode very quickly and then the CPU is free to do other stuff while the display is being updated.

I've done some very adhoc tests and we can get almost 40fps! Which is pretty amazing.
