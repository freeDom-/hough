#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "include/dimensions.h"

/*
** Converts 32-Bit colored image data to 8-Bit grayscale image data
** The function needs raw pixel data without any kind of compression.
*/
void grayscaler(uint32_t input[SIZE], uint8_t output[SIZE]) {
    outerLoop:for(int y = 0; y < HEIGHT; y++) {
        innerLoop:for(int x = 0; x < WIDTH; x++) {
                int index = y * WIDTH + x;
                uint8_t r, g, b;

                r = input[index] >> 16 & 0xFF;
                g = input[index] >> 8 & 0xFF;
                b = input[index] & 0xFF;

                output[index] = 0.3*r + 0.59*g + 0.11*b;
        }
    }
}
