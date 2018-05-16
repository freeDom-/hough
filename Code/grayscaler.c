/*
** Converts 32-Bit colored image data to 8-Bit grayscale image data
** The function needs and returns only pixel data without any kind of compression.
*/

#include "grayscaler.h"

uint8_t* grayscaler(void* input, int width, int height) {
    uint8_t (*output)[width] = malloc(width * height * sizeof(uint8_t));
    uint32_t (*pixels)[width] = (uint32_t(*)[width]) input;

    // Convert image to grayscale
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
                uint8_t r, g, b;

                r = pixels[y][x] >> 16 & 0xFF;
                g = pixels[y][x] >> 8 & 0xFF;
                b = pixels[y][x] & 0xFF;
                output[y][x] = 0.3*r + 0.59*g + 0.11*b;
        }
    }

    return *output;
}
