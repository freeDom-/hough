/*
** Converts 32-Bit colored image data to 8-Bit grayscale image data
** The function needs and returns only pixel data without any kind of compression.
*/

#include "grayscaler.h"

uint8_t* grayscaler(uint32_t* input, int width, int height) {
    uint8_t *output = malloc(width * height * sizeof(uint8_t));

    #ifdef _OPENMP
    #pragma omp parallel for
    #endif
    // Convert image to grayscale
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
                uint8_t r, g, b;
                int index = y * width + x;

                r = input[index] >> 16 & 0xFF;
                g = input[index] >> 8 & 0xFF;
                b = input[index] & 0xFF;
                output[index] = 0.3*r + 0.59*g + 0.11*b;
        }
    }

    return output;
}
