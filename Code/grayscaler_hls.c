#include "include/grayscaler_hls.h"

/*
** Converts 32-Bit colored image data to 8-Bit grayscale image data
** The function needs raw pixel data without any kind of compression.
*/
void grayscaler(uint32_t input[SIZE], uint8_t output[SIZE]) {
    outerLoop:for(int y = 0; y < HEIGHT; y++) {
        innerLoop:for(int x = 0; x < WIDTH; x++) {
			int index = y * WIDTH + x;
			uint32_t pixel = input[index];
			uint8_t r, g, b;

			r = pixel >> 16 & 0xFF;
			g = pixel >> 8 & 0xFF;
			b = pixel & 0xFF;

			output[index] = (30*r + 59*g + 11*b)/100;
        }
    }
}
